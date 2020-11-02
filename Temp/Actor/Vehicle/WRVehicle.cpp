// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVehicle.h"

#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "UnrealString.h"
#include "Struct/WRUIStruct.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "../Animation/WRVehicleAnimInstance.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

#include "../Character/WRCharacter.h"

#include "Component/Character/WRActorComponentSFX.h"
#include "Component/Character/WRActorComponentVFX.h"
#include "Component/Vehicle/WRVehicleComponentManageMaterial.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SpotLightComponent.h"

#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"

#include "Enum/EWRActorComponent.h"
#include "Enum/EWRUIEnum.h"

#include "Define/WRPathDefine.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

#include "Network/IMMessageSender.h"

#include "Manager/WRVehicleManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRSFXManager.h"

#include "WidgetComponent/WRWidgetComponentBase.h"

#include "WRVehicleWheelRear.h"
#include "WRVehicleWheelFront.h"

#include "Table/WRCustomTextTable.h"
#include "Table/Base/WRTableManager.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WRUIUtility.h"
#include "Utility/WRHMDUtility.h"

#include "UI/Panel/WRUIPanel_BoardingRequest.h"
#include "UI/Panel/WRUIPanel_InputIcon.h"

#include "Localization/WRLocalizationProcessor.h"

#ifndef HMD_MODULE_INCLUDED
#define HMD_MODULE_INCLUDED 0
#endif
// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#endif // HMD_MODULE_INCLUDED
#include "WheeledVehicleMovementComponent.h"
#include "WRVehicleMovementComponent4W.h"
#include "Engine/EngineTypes.h"
#include "Manager/WRSpawnSimulationManager.h"
#include "Table/WRVehicleTable.h"
#include "Manager/WRAutoPilotManager.h"
#include "GameFramework/Volume.h"
#include "Manager/WRNoticeManager.h"
#include "../Character/WRCharacterPlayer.h"
#include "VehicleWheel.h"

const FName AWRVehicle::LookUpBinding("LookUp");
const FName AWRVehicle::LookRightBinding("LookRight");
const FName AWRVehicle::EngineAudioRPM("RPM");

#define LOCTEXT_NAMESPACE "VehiclePawn"

// For Cpp
DEFINE_LOG_CATEGORY(WRVehicle)

AWRVehicle::AWRVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UWRVehicleMovementComponent4W>(VehicleMovementComponentName))
{
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	check(Vehicle4W->WheelSetups.Num() == 4);

	bIsLowFriction = false;
	bInReverseGear = false;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

AWRVehicle::AWRVehicle()
{
	UWRVehicleMovementComponent4W* Vehicle4W = CastChecked<UWRVehicleMovementComponent4W>(GetVehicleMovement());
	check(Vehicle4W->WheelSetups.Num() == 4);

	bIsLowFriction = false;
	bInReverseGear = false;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}


AWRVehicle::~AWRVehicle()
{
}

void AWRVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForwardCar", this, &AWRVehicle::MoveForwardCar);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWRVehicle::MoveRight);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AWRVehicle::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AWRVehicle::OnHandbrakeReleased);


	WRInputSystemManager::Get()->SetUpInputBind(PlayerInputComponent);


	//test
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AWRVehicle::TurnRight);			// [ 2019-8-21 : kyu ] <-, ->
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AWRVehicle::OnToggleCamera);
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AWRVehicle::OnResetVR);
}

void AWRVehicle::OutsideWorldBounds()
{
	PopupForOutofBound();

	WRVehicleManager::Get()->RemoveVehicle(this);
	Super::OutsideWorldBounds();
}

void AWRVehicle::FellOutOfWorld(const class UDamageType& dmgType)
{
	PopupForOutofBound();

	WRVehicleManager::Get()->RemoveVehicle(this);
	Super::FellOutOfWorld(dmgType);
}

void AWRVehicle::PopupForOutofBound()
{
	if (WRHMDUtility::IsDisplayEnable() == true)
		return;

	FVector CurrentLocation = GetActorLocation();
	CurrentLocation.Z += 1000.0f;
	SetActorLocation(CurrentLocation);

	if (bOutsideWorldBounds == false)
	{
		bOutsideWorldBounds = true;

		FVector ActorLocation = GetActorLocation();

		FString MSG = FString::Format(TEXT("Name : {0}\n")TEXT("X,Y : {2}, {3}"),
			{ GetName(), ActorLocation.X, ActorLocation.Y });

		FText Description = FText::FromString(MSG);
		FText Title = FText::FromString("PendingKill");
		FMessageDialog::Open(EAppMsgType::Ok, Description, &Title);
	}
}


bool AWRVehicle::SetLocation(const FVector& NewLocation, bool bModifyZ, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	FVector ModifiedPosition = NewLocation;
	if (bModifyZ)
	{
		WRActorUtility::ModifyZHeightForClient(this, NewLocation, ModifiedPosition.Z);
	}

	return SetActorLocation(ModifiedPosition, bSweep, OutSweepHitResult, Teleport);
}

bool AWRVehicle::SetRotation(FRotator NewRotation, ETeleportType Teleport /*= ETeleportType::None*/)
{
	return SetActorRotation(NewRotation, Teleport);
}

bool AWRVehicle::SetLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bModifyZ, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	FVector ModifiedPosition = NewLocation;
	if (bModifyZ)
	{
		WRActorUtility::ModifyZHeightForClient(this, NewLocation, ModifiedPosition.Z);
	}

	return SetActorLocationAndRotation(ModifiedPosition, NewRotation, bSweep, OutSweepHitResult, Teleport);
}

bool AWRVehicle::SetActorLocation(const FVector& NewLocation, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	return Super::SetActorLocation(NewLocation, bSweep, OutSweepHitResult, Teleport);
}

bool AWRVehicle::SetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	return Super::SetActorLocationAndRotation(NewLocation, NewRotation, bSweep, OutSweepHitResult, Teleport);
}

void AWRVehicle::OnBeginOverlapVehicleSeatPosition(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString LeftS;
	FString RightS;
	OverlappedComp->ComponentTags[1].ToString().Split("_", &LeftS, &RightS);
	int32 SeatIndex = FCString::Atoi(*RightS);
	if (0 == SeatIndex)
	{
		//운전석일때
		//내 소유 차량이 아니면 아무 처리도 하지 않는다
		if (OwnerIsMe() == false)
		{
			return;
		}
	}
	else
	{
		//운전석 이외일때
		//내 소유의 차량이 아니면
		if (OwnerIsMe() == false)
		{
			//운전석에 누가 타 있는지 체크
			if (PassengerInfo.Num() != 0)
			{
				if (INVALID_AID == PassengerInfo[0])
					return;
			}
		}
	}

	if (2 == OverlappedComp->ComponentTags.Num())
	{
		WRVehicleManager::Get()->SetMyVehicleInfoToBoarding(OverlappedComp->ComponentTags[1].ToString(), GetVehicleAID());
	}
	else
	{
		FString MSG = FString::Format(TEXT("OnBeginOverlapVehicleSeatPosition: {0}"), {OverlappedComp->ComponentTags.Num()} );
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Invalid tag"));

		return;
	}

	AWRCharacter* OtherCharacter = Cast<AWRCharacter>(Other);
	if (OtherCharacter && OtherCharacter->IsMe())
	{
		int32 remainder = SeatIndex % 2;
		FString SocketName = "RideButtonUI_";
		if (remainder == 0)	//왼쪽 좌석
		{
			SocketName += "L";
		}
		else if (remainder == 1)	//오른쪽 좌석
		{
			SocketName += "R";
		}

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(*SocketName));
				UWRUIPanel_InputIcon* Panel_InputIcon = Cast<UWRUIPanel_InputIcon>(InCombinedPanelData.UIPanel);
				if (Panel_InputIcon)
				{
					Panel_InputIcon->SetInputKey(EWRInput::Triangle);
					FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(25);
					if (CustomTextTable)
					{
						FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
						Panel_InputIcon->SetContent(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey));
					}
				}
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::InputIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate, this);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}	
}

void AWRVehicle::OnEndOverlapVehicleSeatPosition(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWRCharacter* OtherCharacter = Cast<AWRCharacter>(Other);
	if (OtherCharacter && OtherCharacter->IsMe())
	{
		WRVehicleManager::Get()->SetMyVehicleInfoToBoarding("", INVALID_AID);

		WRUIManager::Get()->CloseUI(EWRUIPanelKind::InputIcon);
	}
}

void AWRVehicle::OnComponentHitPhysics(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AWRVehicle::OnBeginOverlapVehicleForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (this == Other)
		return;

	AVolume* Volume = Cast<AVolume>(Other);
	if (nullptr != Volume)
		return;

	WRSpawnSimulationManager::Get()->OnBeginOverlapSimulationActor(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	SetMaterialParameterValue(TEXT("Selected"), 1.0f);
}

void AWRVehicle::OnEndOverlapVehicleForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (this == Other)
		return;

	AVolume* Volume = Cast<AVolume>(Other);
	if (nullptr != Volume)
		return;

	WRSpawnSimulationManager::Get()->OnEndOverlapSimulationActor(OverlappedComp, Other, OtherComp, OtherBodyIndex);

	SetMaterialParameterValue(TEXT("Selected"), 0.0f);
}

void AWRVehicle::OnBeginOverlapAutoPilotSensor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PassengerInfo.Num() == 0 || PassengerInfo[0] == INVALID_AID)
		return;

	if (Other == this)
		return;

	if (Other->GetParentActor() == this)
		return;

	WRAutoPilotManager::Get()->OnBeginOverlapAutoPilot(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWRVehicle::OnEndOverlapAutoPilotSensor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PassengerInfo.Num() == 0 || PassengerInfo[0] == INVALID_AID)
		return;

	if (Other == this)
		return;

	if (Other->GetParentActor() == this)
		return;

	WRAutoPilotManager::Get()->OnEndOverlapAutoPilot(OverlappedComp, Other, OtherComp, OtherBodyIndex);
}

//---------------------------------------------------------------------------
//------------------운전자가 나 자신일때만 업데이트 됨-------------------------
void AWRVehicle::MoveForwardCar(float Val)
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		//vr로 할때는 InputEventToControllVehicle의 r2에서 처리 한다
		return;
	}
	else
	{
		//이건 컴터에서 키보드로 할때 동작
		//조이패드 입력은 InputEventToControllVehicle 에서 처리 함
		if (WRAutoPilotManager::Get()->IsActiveAutoPilot())
		{
			//악셀입력이 들어오면 자동주행 도 정지 시킴
			if (Val != 0.0f)
			{
				WRAutoPilotManager::Get()->EndAutoPilot(this);
			}
		}
		else
		{
			RequestAccelator(Val);
		}
	}
}

void AWRVehicle::MoveRight(float Val)
{
	if (WRAutoPilotManager::Get()->IsActiveAutoPilot())
	{
		if (Val != 0.0f)
		{
			WRAutoPilotManager::Get()->EndAutoPilot(this);
		}
	}
	else
	{
		RequestSteering(Val);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void AWRVehicle::OnHandbrakePressed()
{
	if (GetWorld()->WorldType == EWorldType::PIE &&
		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		return;
	}

	RequestBeginBrake();
	WRAutoPilotManager::Get()->EndAutoPilot(this);
}

void AWRVehicle::OnHandbrakeReleased()
{
	if (GetWorld()->WorldType == EWorldType::PIE &&
		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		return;
	}

	RequestEndBrake();
}

void AWRVehicle::TickExecuteThrottleAndSteering(float InDelta)
{
	//핸들 업데이트
	GetVehicleMovementComponent()->SetSteeringInput(ReceivedSteeringValue);
	UWRVehicleAnimInstance* VehicleAnimInstance = Cast<UWRVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	if (VehicleAnimInstance)
	{
		for (int32 Idx = 0; Idx < GetVehicleMovement()->Wheels.Num(); ++Idx)
		{
			UVehicleWheel* FrontWheel = Cast<UVehicleWheel>(GetVehicleMovement()->Wheels[Idx]);
			if (nullptr == FrontWheel)
				continue;

			VehicleAnimInstance->SetSteering(FrontWheel->GetSteerAngle() * 2);
			break;
		}
	}
	
	if (DriverIsMe())
	{
		//속도 업데이트
		GetVehicleMovementComponent()->SetThrottleInput(ReceivedAccelationValue);
	}
	else
	{
		if (IsSomeoneDriving() == false)
			return;

		//위치값 세팅
		if (ReceivedAccLocation != FVector::ZeroVector)
		{
			static float InterpSpeed = 1.5f;

			//float InterpSpeed = UKismetMathLibrary::Abs(ReceivedSpeed * 0.036f * 0.2f);
			FVector InterpVec = UKismetMathLibrary::VInterpTo(GetActorLocation(), ReceivedAccLocation, InDelta, InterpSpeed);
			SetLocation(InterpVec, true);
		}

		//회전값 세팅
		if (ReceivedAccRotation != FRotator::ZeroRotator ||
			GetActorRotation().Equals(ReceivedAccRotation, BIG_NUMBER) == false)
		{
			static float InterpRotSpeed = 2.0f;
			FRotator InterpRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), ReceivedAccRotation, InDelta, InterpRotSpeed);
			SetRotation(InterpRotation);
		}
	}

	//StartSmoke(ReceivedAccelationValue);
	SteerSmoke(ReceivedAccelationValue);

	PlayEngineSound();
}

void AWRVehicle::TickRequestThrottleAndSteering(float InDelta)
{
	if (DriverIsMe() == false)
	{
		return;
	}

	if (GetWRVehicleMovement()->GetThrottleInput() == 0.0f &&
		RequestedAccelationValue == 0.0f &&
		GetWRVehicleMovement()->GetForwardSpeed() != 0.0f)
	{
		int32 aa = 0;
	}

	//속도
	ElapsedTime2RequestAccelation += InDelta;
	if (VEHICLE_TICK_TIME_TO_SYNC <= ElapsedTime2RequestAccelation)
	{
		float Speed = GetWRVehicleMovement()->GetForwardSpeed();
		if (RequestedAccelationValue != 0.0f)
		{
			//엑셀이 입력되고 있는 상태에서는 해당 입력값을 서버에 노티
			ElapsedTime2RequestAccelation = 0.0f;
			IMMessageSender::RequestAcceleration(this, RequestedAccelationValue, Speed);
		}
		else
		{
			//그이외에는 현재 엔진에서 계산되고 잇는 현재 input값을 이용
			//또는 현재 입력이 없는 상태로 차가 계속 이동중이면 서버에 노티
			if (GetWRVehicleMovement()->GetThrottleInput() != 0.0f ||
				Speed != 0.0f)
			{
				ElapsedTime2RequestAccelation = 0.0f;
				IMMessageSender::RequestAcceleration(this, GetWRVehicleMovement()->GetThrottleInput(), Speed);
			}
		}
	}

/*	//steering 은 moveRight에서 업뎃 돌면서 처리 되므로 
	//굳이 tick에서 입력처리 하지 않는다
	//차량의 회전은 현 차량의 회전값으로 처리 하므로 굳이 틱마다 보낼 필요가 없다

	//스티어링
	ElapsedTime2RequestSteering += InDelta;
	if (VEHICLE_TICK_TIME_TO_SYNC <= ElapsedTime2RequestSteering)
	{
		if (RequestedSteeringValue != 0.0f)
		{
			//스티어링이 입력되고 잇는중
			ElapsedTime2RequestSteering = 0.0f;
			IMMessageSender::RequestSteering(this, RequestedSteeringValue);
 		}
		else
		{
			//엔진에서 계산된 여분의 스티어링입력값
			if (GetWRVehicleMovement()->GetSteeringInput() != 0.0f)
			{
				ElapsedTime2RequestSteering = 0.0f;
				IMMessageSender::RequestSteering(this, GetWRVehicleMovement()->GetSteeringInput());
			}
		}
	}
*/
}

void AWRVehicle::TurnRight(float InScale)
{
#if WITH_EDITOR
	// [ 2019-8-20 : kyu ] HMD Off 시 키매핑에 의한 Yaw 회전 수정 적용 추가
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false && GetWorld()->WorldType == EWorldType::PIE)
	{
		UCameraComponent* CameraComponent = Cast<UCameraComponent>(this->GetComponentByClass(UCameraComponent::StaticClass()));
		if (CameraComponent != nullptr)
		{
			FRotator OwnerRotation = CameraComponent->GetComponentRotation();
			OwnerRotation.Yaw = this->GetActorRotation().Yaw;
			CameraComponent->SetWorldRotation(OwnerRotation);
		}
	}
#endif // WITH_EDITOR
}

void AWRVehicle::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}

void AWRVehicle::EnableIncarView(const bool bState)
{
	if (bState != bInCarCameraActive)
	{
		bInCarCameraActive = bState;

		if (bState == true)
		{
			OnResetVR();
		}
		else
		{
		}
	}
}

void AWRVehicle::Tick(float Delta)
{
	Super::Tick(Delta);

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;

	// Update phsyics material
	UpdatePhysicsMaterial();

	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Update throttleInput and Steering
	TickExecuteThrottleAndSteering(Delta);

	// Request Acc and Steering 
	TickRequestThrottleAndSteering(Delta);

	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->XRSystem.IsValid() == true) && ((GEngine->XRSystem->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (bHMDActive == false)
	{
		if ((InputComponent) && (bInCarCameraActive == true))
		{
			// 			FRotator HeadRotation = InternalCamera->GetRelativeRotation();
			// 			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			// 			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			// 			InternalCamera->SetRelativeRotation(HeadRotation);
		}
	}

	// Pass the engine RPM to the sound component
// 	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
// 	EngineSoundComponent->SetFloatParameter(EngineAudioRPM, GetVehicleMovement()->GetEngineRotationSpeed()*RPMToAudioScale);
}

void AWRVehicle::BeginPlay()
{
	Super::BeginPlay();

	bool bWantInCar = false;
	// First disable both speed/gear displays 
	bInCarCameraActive = false;

	// Enable in car view if HMD is attached
#if HMD_MODULE_INCLUDED
	bWantInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED

	EnableIncarView(bWantInCar);
	// Start an engine sound playing
//	EngineSoundComponent->Play();
	
	// 시트 포지션용 콜리전 박스
	TArray<UActorComponent*> BoxComponents = GetComponentsByTag(UBoxComponent::StaticClass(), "SeatPosition");
	for (int32 Idx = 0 ; Idx < BoxComponents.Num(); ++Idx)
	{
		UBoxComponent* BoxComp = Cast<UBoxComponent>(BoxComponents[Idx]);
		if (BoxComp)
		{
			BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AWRVehicle::OnBeginOverlapVehicleSeatPosition);
			BoxComp->OnComponentEndOverlap.AddDynamic(this, &AWRVehicle::OnEndOverlapVehicleSeatPosition);
		}
	}

	// AutoPilot 용 콜리전 박스
	TArray<UActorComponent*> SensorComponents = GetComponentsByTag(UBoxComponent::StaticClass(), "Sensor");
	for (int32 Idx = 0; Idx < SensorComponents.Num(); ++Idx)
	{
		UBoxComponent* SensorComp = Cast<UBoxComponent>(SensorComponents[Idx]);
		if (SensorComp)
		{
			SensorComp->OnComponentBeginOverlap.AddDynamic(this, &AWRVehicle::OnBeginOverlapAutoPilotSensor);
			SensorComp->OnComponentEndOverlap.AddDynamic(this, &AWRVehicle::OnEndOverlapAutoPilotSensor);
		}
	}


	// 테스트
	GetMesh()->OnComponentHit.AddDynamic(this, &AWRVehicle::OnComponentHitPhysics);

// 	GetMovementComponent()->bConstrainToPlane = true;
// 	GetMovementComponent()->bSnapToPlaneAtStart = true;
	SetVehicleSimulatePhysics(false);
	ECollisionEnabled::Type aa = GetMesh()->GetCollisionEnabled();
	FName bb = GetMesh()->GetCollisionProfileName();
	ECollisionChannel cc = GetMesh()->GetCollisionObjectType();
}

void AWRVehicle::RegistOverlapCollision()
{
	TArray<UActorComponent*> BoxComponents = GetComponentsByTag(UBoxComponent::StaticClass(), "SeatPosition");
	for (int32 Idx = 0 ; Idx < BoxComponents.Num(); ++Idx)
	{
		UBoxComponent* BoxComp = Cast<UBoxComponent>(BoxComponents[Idx]);
		if (BoxComp)
		{
			BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AWRVehicle::OnBeginOverlapVehicleSeatPosition);
			BoxComp->OnComponentEndOverlap.AddDynamic(this, &AWRVehicle::OnEndOverlapVehicleSeatPosition);
		}
	}
}

void AWRVehicle::OnPostSpawn()
{
	BuildComponent();

	UWRActorComponentSFX* ActorComponentSFX = GetComponent<UWRActorComponentSFX>();
	if (ActorComponentSFX)
	{
		UAudioComponent* AudioComponent = ActorComponentSFX->AddSFX(EWRSFXActor::Engine, SOUND_PATH_4_VEHICLE_ENGINE, GetRootComponent());
		if (AudioComponent)
		{
			float CurrentSpeed = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036F;
			AudioComponent->SetVolumeMultiplier(CurrentSpeed);
		}
	}

	TArray<UStaticMeshComponent*> StaticComponents;
	GetComponents<UStaticMeshComponent>(StaticComponents);

	for (auto& StaticComponent : StaticComponents)
	{
		FString Name = StaticComponent->GetName();
		if (Name.Contains("RPM"))
		{
			RPMComponent = StaticComponent;
		}
		if (Name.Contains("Speed"))
		{
			KPHComponent = StaticComponent;
		}
	}

	InitVehicleState();
}

void AWRVehicle::OnPreDestroy()
{
	for (int32 Idx = 0; Idx < PassengerInfo.Num(); ++Idx)
	{
		AWRCharacter* Passenger = WRCharacterManager::Get()->FindbyAID(PassengerInfo[Idx]);
		if (Passenger)
		{
			Passenger->SetSkeletalMeshComponentToFABRIK();

			Passenger->SetBoardingVehicleAID(INVALID_AID);
		}
	}

	if (DestoryHandle.ExecuteIfBound(this))
	{
		DestoryHandle.Unbind();
	}

	RemoveComponentAll();
}

void AWRVehicle::BuildComponent()
{
	CreateComponent<UWRActorComponentVFX>(EWRCharacterComponent::VFX);
	CreateComponent<UWRActorComponentSFX>(EWRCharacterComponent::SFX);
	CreateComponent<UWRVehicleComponentManageMaterial>(EWRCharacterComponent::ManageMaterial);
}

void AWRVehicle::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void AWRVehicle::SetupInCarHUD()
{

}

void AWRVehicle::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		if (bIsLowFriction == true)
		{
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			bIsLowFriction = false;
		}
		else
		{
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			bIsLowFriction = true;
		}
	}
}

void AWRVehicle::OpenLeftDoor(int32 InSeatIndex, bool InOpen)
{
	if (nullptr == GetMesh())
		return;

	UWRVehicleAnimInstance* AnimInstance = Cast<UWRVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->bOpenLeftSide = InOpen;
}

void AWRVehicle::OpenRightDoor(int32 InSeatIndex, bool InOpen)
{
	if (nullptr == GetMesh())
		return;

	UWRVehicleAnimInstance* AnimInstance = Cast<UWRVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->bOpenRightSide = InOpen;
}

void AWRVehicle::UpdateHUDStrings()	
{
	if (PassengerInfo.Num() == 0 || PassengerInfo[0] == INVALID_AID)
		return;

	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);
	int32 Gear = GetVehicleMovement()->GetCurrentGear();

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));

	//UE_LOG(VehicleManager, Display, TEXT("[AWRVehicle::UpdateHUDStrings] %d"), KPH_int);

	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}

	SetMaterialParameterValue("RotateValue", GetCurrentRPM() / GetVehicleMovement()->MaxEngineRPM, RPMComponent);
	SetMaterialParameterValue("RotateValue", GetCurrentKPH() / 200.0F, KPHComponent);
}

void AWRVehicle::SetPassengerInfo(int32 InIndex, AID_t InAID)
{
	//PassengerInfo[InIndex] = InAID;
}

void AWRVehicle::MakePassengerInfo(WRTableID InTID, std::vector<AID_t> InPassengerInfo)
{
	FWRVehicleTable* VehicleTable = WRTableManager::Get()->FindRow<FWRVehicleTable>(InTID);
	if (VehicleTable)
	{
		for (int32 Idx = 0; Idx < VehicleTable->MaxRideNum; ++Idx)
		{
			PassengerInfo.Add(INVALID_AID);
		}

		for (int32 Idx = 0; Idx < InPassengerInfo.size(); ++Idx)
		{
			PassengerInfo[Idx] = InPassengerInfo[Idx];
		}
	}
}

UWRVehicleMovementComponent4W* AWRVehicle::GetWRVehicleMovement()
{
	return Cast<UWRVehicleMovementComponent4W>(GetMovementComponent());
}

void AWRVehicle::SwitchCamera()
{
	UCameraComponent* OuterCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("OuterCamera"));
	UCameraComponent* InnerCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("Camera_0"));
	if (nullptr == OuterCamera || nullptr == InnerCamera)
	{
		return;
	}

	if (InnerCamera->IsActive())
	{
		InnerCamera->Deactivate();
		OuterCamera->Activate(true);
	}
	else
	{
		OuterCamera->Deactivate();
		InnerCamera->Activate(true);
	}	
}

void AWRVehicle::InitVehicleState()
{
	RequestAccelator(0.0f);
	RequestEndBrake();

	TArray<UActorComponent*> AutoPilotIndicator = GetComponentsByTag(UActorComponent::StaticClass(), "ToggleAutoPilot");
	for (int32 Idx = 0; Idx < AutoPilotIndicator.Num(); ++Idx)
	{
		USkeletalMeshComponent* SkeltalMeshComponent = Cast<USkeletalMeshComponent>(AutoPilotIndicator[Idx]);
		if (SkeltalMeshComponent)
		{
			UWRSteeringWheelAnimInstance* SteeringAnimInstance = Cast<UWRSteeringWheelAnimInstance>(SkeltalMeshComponent->GetAnimInstance());
			if (SteeringAnimInstance)
			{
				SteeringAnimInstance->SWheelState = EWRSteeringWheelState::ManualIdle;
				SetMaterialParameterValue_Internal("AutoSpawn", 1.0f, SkeltalMeshComponent);
			}
		}

		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(AutoPilotIndicator[Idx]);
		if (StaticMeshComponent)
		{
			SetMaterialParameterValue_Internal("AutoSpawn", 0.0f, StaticMeshComponent);
		}
	}
}

void AWRVehicle::ToggleAutoPilot(bool bToggle)
{
	if (DriverIsMe() == false)
		return;

	ToggleAutoPilotIndicator(bToggle);

	AWRCharacter* Driver = WRCharacterManager::Get()->FindbyAID(PassengerInfo[0]);
	if (Driver)
	{
		WRFSMInfo FSMInfo;
		FSMInfo.JumpToSectionName = WREnumUtility::EnumToString("EWRFSM", EWRFSM::DrivingIdle);
		if (bToggle)
		{
			FSMInfo.JumpToSectionName += "R";
			Driver->SetSkeletalMeshComponentToFABRIK();
		}
		else
		{
			FSMInfo.JumpToSectionName += "L";
			Driver->SetSkeletalMeshComponentToFABRIK(GetMesh());
		}

		FSMInfo.FSMType = EWRFSM::DrivingIdle;
		FSMInfo.bImmediateUpdate = true;
		Driver->DetermineFSMSignal(FSMInfo);
	}
}

void AWRVehicle::ToggleAutoPilotIndicator(bool bToggle)
{
	TArray<UActorComponent*> AutoPilotIndicator = GetComponentsByTag(UActorComponent::StaticClass(), "ToggleAutoPilot");
	for (int32 Idx = 0; Idx < AutoPilotIndicator.Num(); ++Idx)
	{
		USkeletalMeshComponent* SkeltalMeshComponent = Cast<USkeletalMeshComponent>(AutoPilotIndicator[Idx]);
		if (SkeltalMeshComponent)
		{
			UWRSteeringWheelAnimInstance* SteeringAnimInstance = Cast<UWRSteeringWheelAnimInstance>(SkeltalMeshComponent->GetAnimInstance());
			if (SteeringAnimInstance)
			{
				if (bToggle)
				{
					SteeringAnimInstance->SWheelState = EWRSteeringWheelState::StartAutoPilot;
					SetMaterialParameterValue_Internal("AutoSpawn", 1.0f, SkeltalMeshComponent);
				}
				else
				{
					SteeringAnimInstance->SWheelState = EWRSteeringWheelState::EndAutoPilot;
					SetMaterialParameterValue_Internal("AutoSpawn", 0.0f, SkeltalMeshComponent);
				}
			}
		}

		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(AutoPilotIndicator[Idx]);
		if (StaticMeshComponent)
		{
			if (bToggle)
			{
				SetMaterialParameterValue_Internal("AutoSpawn", 1.0f, StaticMeshComponent);
			}
			else
			{
				SetMaterialParameterValue_Internal("AutoSpawn", 0.0f, StaticMeshComponent);
			}
		}
	}
}

void AWRVehicle::PlayEngineSound()
{
	UWRActorComponentSFX* ActorComponentSFX = GetComponent<UWRActorComponentSFX>();
	if (ActorComponentSFX)
	{
		UAudioComponent* AudioComponent = ActorComponentSFX->GetSFX(EWRSFXActor::Engine);
		if (AudioComponent)
		{
			float MaxVolume = WRConsoleManager::Get()->GetFloat(EWRConsole::VehicleMaxVolume);
			float CurrentSpeed = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036F;

			AudioComponent->SetVolumeMultiplier(FMath::Min(CurrentSpeed * 0.05F, MaxVolume));
		}
	}
}

void AWRVehicle::SetVehicleSimulatePhysics(bool bFlag)
{
	if (nullptr == GetMesh())
		return;

	GetMesh()->SetSimulatePhysics(bFlag);

	if (bFlag)	//시뮬레이션 되는 차는 기본 브레이크 값을 준다.
	{
		GetWRVehicleMovement()->SetIdleBrakeInput(10.0f);
		GetWRVehicleMovement()->SetStopThreshold(10.0f);
	}
	else
	{
		GetWRVehicleMovement()->SetIdleBrakeInput(0.0f);
		GetWRVehicleMovement()->SetStopThreshold(0.0f);
	}
}

bool AWRVehicle::OwnerIsMe()
{
// 	AWRCharacter* OwnerCharacter = WRCharacterManager::Get()->FindbyAID(VehicleOwnerAID);
// 	if (nullptr == OwnerCharacter)
// 		return false;
// 
// 	return OwnerCharacter->IsMe();

	if (WRCharacterManager::Get()->GetMyAID() == VehicleOwnerAID)
		return true;

	return false;
}

bool AWRVehicle::DriverIsMe()
{
	if (PassengerInfo.Num() == 0)
		return false;

	if (WRCharacterManager::Get()->GetMyAID() == PassengerInfo[0])
		return true;

	return false;
}

bool AWRVehicle::IsSomeoneDriving()
{
	if (PassengerInfo.Num() == 0)
		return false;

	if (PassengerInfo[0] != INVALID_AID)
		return true;

	return false;
}

int32 AWRVehicle::GetSeatIndex(AID_t InPassengerAID)
{
	for (int32 Idx = 0; Idx < PassengerInfo.Num(); ++Idx)
	{
		if (PassengerInfo[Idx] == InPassengerAID)
		{
			return Idx;
		}
	}

	return INVALID_SEAT_INDEX;
}

void AWRVehicle::SetVehicleAndOwnerAID(AID_t InAID, AID_t InOwnerAID)
{
	VehicleAID = InAID;
	VehicleOwnerAID = InOwnerAID;
}

AID_t AWRVehicle::GetVehicleAID()
{
	return VehicleAID;
}

AID_t AWRVehicle::GetVehicleOwnerAID()
{
	return VehicleOwnerAID;
}

void AWRVehicle::OnReceivedLetSomeoneBoardingTimeOver(AID_t InAID, AID_t InVehicleAID, int8 InSeatIndex)
{
	IMMessageSender::ReportAgreeToBoard(InAID, InSeatIndex, InVehicleAID, false);
}

float AWRVehicle::GetCurrentRPM()
{
	return FMath::Abs(GetVehicleMovement()->GetForwardSpeed());
}

float AWRVehicle::GetCurrentKPH()
{
	return FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036F;
}

void AWRVehicle::OnInfGetOnVehicle(const pt_Game_Vehicle::Inf_GetOnVehicle_G2U& IN InPacket)
{
	if (PassengerInfo.Num() <= InPacket.seatIndex)
	{
		FString MSG = FString::Format(TEXT("OnInfGetOnVehicle : {0}"), { InPacket.seatIndex });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Out of range seat index"));

		return;
	}

	//순서 지켜야 함
	//1. 차량에 탑승인원정보 세팅
	PassengerInfo[InPacket.seatIndex] = InPacket.actorID;

	AWRCharacter* Passenger = WRCharacterManager::Get()->FindbyAID(InPacket.actorID);
	if (Passenger)
	{
		//탑승자에게 탑승한 차량 아이디 세팅
		Passenger->SetBoardingVehicleAID(InPacket.vehicleAID);

		//승차 시작
		TArray<WRFSMInfo> SequenceFSM;
		if (Passenger->IsInBattle())
		{
			//전투 상태면 칼 넣는 애니 실행
			WRFSMInfo FSMInfo(EWRFSM::Disarm);
			SequenceFSM.Add(FSMInfo);
		}

		//탑승정보 세팅
		WRFSMInfo FSMInfo(EWRFSM::EnterVehicle);
		SequenceFSM.Add(FSMInfo);
		Passenger->RegistSequenceFSMSignal(SequenceFSM, true);
	}
	else
	{
		WRCharacterManager::Get()->ModifyBoardingVehicleInfo(InPacket.actorID, GetVehicleAID(), InPacket.seatIndex);
	}
}

void AWRVehicle::OnInfGetOffVehicle(const pt_Game_Vehicle::Inf_GetOffVehicle_G2U& IN InPacket)
{
	AWRCharacter* Passenger = WRCharacterManager::Get()->FindbyAID(InPacket.actorID);
	if (Passenger)
	{
		//순서 유지 할것
		//1.하차 
		// aid 와 어느쪽 시트인지 정보 넘겨야함
		WRFSMInfo FSMInfo(EWRFSM::ExitVehicle);
		EWRVehicleSeatSide SeatSide = WRVehicleManager::Get()->GetSeatingSide(Passenger->GetActorID());
		FString strAID = FString::Format(TEXT("{0}"), { InPacket.vehicleAID });
		FSMInfo.DataString.Add(strAID);
		FSMInfo.DataString.Add(FString::FromInt((int32)SeatSide));
		FSMInfo.DataString.Add(FString::FromInt(InPacket.seatIndex));
		//fsm 실행
		Passenger->DetermineFSMSignal(FSMInfo);

		//2.탑승정보 초기화
		Passenger->SetBoardingVehicleAID(INVALID_AID);

		//몇번째 시트에 타고잇는지 정보도 삭제 해야 함
		if (InPacket.seatIndex != INVALID_SEAT_INDEX && InPacket.seatIndex < PassengerInfo.Num())
		{
			PassengerInfo[InPacket.seatIndex] = INVALID_AID;
		}
	}
	else
	{
		WRCharacterManager::Get()->ModifyBoardingVehicleInfo(InPacket.actorID, INVALID_AID, INVALID_SEAT_INDEX);
	}
}

void AWRVehicle::OnInfBeginBrake(const pt_Game_Vehicle::Inf_BeginBrake_G2U& IN InPacket)
{
	//GetVehicleMovementComponent()->SetHandbrakeInput(true);
	GetVehicleMovementComponent()->SetBrakeInput(true);
}

void AWRVehicle::OnInfEndBrake(const pt_Game_Vehicle::Inf_EndBrake_G2U& IN InPacket)
{
	//GetVehicleMovementComponent()->SetHandbrakeInput(false);
	GetVehicleMovementComponent()->SetBrakeInput(false);
}

void AWRVehicle::OnInfAcceleration(const pt_Game_Vehicle::Inf_Acceleration_G2U& IN InPacket)
{
	ReceivedAccelationValue = InPacket.InputPower;

	ReceivedAccLocation.X = InPacket.posX;
	ReceivedAccLocation.Y = InPacket.posY;
	ReceivedAccLocation.Z = InPacket.posZ;

	ReceivedSpeed = InPacket.Speed;

	ReceivedAccRotation.Pitch = InPacket.Pitch;
	ReceivedAccRotation.Yaw = InPacket.Yaw;
	ReceivedAccRotation.Roll = InPacket.Roll;
}

void AWRVehicle::OnInfSteering(const pt_Game_Vehicle::Inf_Steering_G2U& IN InPacket)
{
	ReceivedSteeringValue = InPacket.steering;
/*
	GetVehicleMovementComponent()->SetSteeringInput(InPacket.steering);
	UWRVehicleAnimInstance* VehicleAnimInstance = Cast<UWRVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	if (VehicleAnimInstance)

	{

		for (int32 Idx = 0; Idx < GetVehicleMovement()->Wheels.Num(); ++Idx)

		{

			UVehicleWheel* FrontWheel = Cast<UVehicleWheel>(GetVehicleMovement()->Wheels[Idx]);
			if (nullptr == FrontWheel)

				continue;;

			VehicleAnimInstance->SetSteering(FrontWheel->GetSteerAngle() * 2);
			break;
		}
	}
*/
}

void AWRVehicle::OnCmdAgreeToBoard(const pt_Game_Vehicle::Cmd_AgreeToBoard_G2U& IN InPacket)
{
	AWRCharacter* VehicleOwner = WRCharacterManager::Get()->FindbyAID(VehicleOwnerAID);
	if (VehicleOwner)
	{
		//UI
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
				UWRUIPanel_BoardingRequest* Panel = Cast<UWRUIPanel_BoardingRequest>(InCombinedPanelData.UIPanel);
				if (Panel)
				{
					FWROnFinishedBoardingRequestTime BoardingTimeDelegate;
					BoardingTimeDelegate.BindUObject(this, &AWRVehicle::OnReceivedLetSomeoneBoardingTimeOver);
					Panel->SetBoardingType(EWRBoardingRequestType::Driver, BoardingTimeDelegate, InPacket.passengerAID, InPacket.vehicleAID, InPacket.seatIndex);
				}
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::BoardingRequest, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
	else
	{
		IMMessageSender::ReportAgreeToBoard(InPacket.passengerAID, InPacket.seatIndex, InPacket.vehicleAID, false);
	}
}

void AWRVehicle::OnInfHorning(const pt_Game_Vehicle::Inf_BeHorning_G2U& IN InPacket)
{
	if (0 < PassengerInfo.Num())
	{
		WRFSMInfo FSMInfo(EWRFSM::VehicleInteraction);
		FSMInfo.JumpToSectionName = "VehicleHorn";
		AWRCharacter* pDriver = WRCharacterManager::Get()->FindbyAID(PassengerInfo[0]);
		if (pDriver)
		{
			pDriver->DetermineFSMSignal(FSMInfo);
		}

		// [ 2020-7-7 : animal14 ] 경적 소리
		WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_VEHICLE_HORN, GetActorLocation());
	}
}

void AWRVehicle::OnInfUpdateExterior(const pt_Game_Vehicle::Inf_UpdateExterior_G2U& IN InPacket)
{
	if (nullptr == GetMesh())
		return;

	//루프 처리
	UWRVehicleAnimInstance* AnimInstance = Cast<UWRVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->bOpenRoof = InPacket.Exterior.foldingTop;
	}

	//라이트 처리
	USpotLightComponent* HeadLight = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
	if (HeadLight)
	{
		HeadLight->Activate(InPacket.Exterior.light);
	}

	switch (InPacket.Exterior.turnIndicator)
	{
	case stVehicleExteriorInfo::TurnIndicator::LEFT:
	{
		USpotLightComponent* RightTurnSignal = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
		if (RightTurnSignal)
		{
			RightTurnSignal->Activate(false);
		}

		USpotLightComponent* LeftTurnSignal = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
		if (LeftTurnSignal)
		{
			LeftTurnSignal->Activate(true);
		}

		WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_VEHICLE_BLINK, GetActorLocation());
	}
	break;
	case stVehicleExteriorInfo::TurnIndicator::RIGHT:
	{
		USpotLightComponent* RightTurnSignal = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
		if (RightTurnSignal)
		{
			RightTurnSignal->Activate(true);
		}

		USpotLightComponent* LeftTurnSignal = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
		if (LeftTurnSignal)
		{
			LeftTurnSignal->Activate(false);
		}

		WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_VEHICLE_BLINK, GetActorLocation());
	}
	break;
	default:
	{
		USpotLightComponent* RightTurnSignal = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
		if (RightTurnSignal)
		{
			RightTurnSignal->Activate(false);
		}

		USpotLightComponent* LeftTurnSignal = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
		if (LeftTurnSignal)
		{
			LeftTurnSignal->Activate(false);
		}
	}
	break;
	}
}

void AWRVehicle::OnInfCancelOnBoard(const pt_Game_Vehicle::Inf_CancelOnBoard_G2U& IN InPacket)
{
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::BoardingRequest);
}

void AWRVehicle::OnInfSyncPhysicForce()
{
	if (GetMesh() == nullptr)
		return;

	//GetMesh()->AddForceAtLocation();
}

bool AWRVehicle::RequestAccelator(float InValue, bool bResetBrake)
{
	if (InValue != RequestedAccelationValue)
	{
		if (bResetBrake)
		{
			if (GetWRVehicleMovement()->GetHandBrakeInput())
			{
				IMMessageSender::RequestEndBrake(this);
			}
		}

		RequestedAccelationValue = InValue;
		float Speed = GetWRVehicleMovement()->GetForwardSpeed();
		IMMessageSender::RequestAcceleration(this, RequestedAccelationValue, Speed);

		ElapsedTime2RequestAccelation = 0.0f;
		return true;
	}

	return false;
}

bool AWRVehicle::RequestSteering(float InValue)
{
	if (InValue != RequestedSteeringValue)
	{
		RequestedSteeringValue = InValue;
		IMMessageSender::RequestSteering(this, RequestedSteeringValue);

		ElapsedTime2RequestSteering = 0.0f;
		return true;
	}

	return false;
}

void AWRVehicle::RequestBeginBrake()
{
	IMMessageSender::RequestBeginBrake(this);
}

void AWRVehicle::RequestEndBrake()
{
	IMMessageSender::RequestEndBrake(this);
}

void AWRVehicle::RequestToggleRoof()
{
	UWRVehicleAnimInstance* AnimInstance = Cast<UWRVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	stVehicleExteriorInfo ExInfo;
	ExInfo.foldingTop = !AnimInstance->bOpenRoof;
	IMMessageSender::RequestUpdateExterior(this, ExInfo);
}

void AWRVehicle::RequestToggleLight()
{
	UWRVehicleAnimInstance* AnimInstance = Cast<UWRVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;
	
	USpotLightComponent* HeadLight = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
	if (nullptr == HeadLight)
		return;

	stVehicleExteriorInfo ExInfo;
	ExInfo.light = !HeadLight->IsActive();
	IMMessageSender::RequestUpdateExterior(this, ExInfo);
}

void AWRVehicle::RequestTurnSignal(bool IsLeft)
{
	//왼쪽 아니면 오른쪽이다
	stVehicleExteriorInfo ExInfo;
	if (IsLeft)
	{
		//ExInfo.turnIndicator
	}
	else
	{

	}
}

void AWRVehicle::RemoveVehicle()
{
	UWRVehicleComponentManageMaterial* VehicleComponentManageMaterial = GetComponent<UWRVehicleComponentManageMaterial>();
	if (VehicleComponentManageMaterial == nullptr)
	{
		return;
	}

	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::VehicleSpawnDirection);
	VehicleComponentManageMaterial->Remove(EWRManageMaterial::Spawn, Duration);

	VehicleComponentManageMaterial->OnComplete(EWRManageMaterial::Spawn).AddUObject(this, &AWRVehicle::OnCompleteDissolve);

	WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_VEHICLE_SUMMON, GetActorLocation());
}

void AWRVehicle::OnCompleteDissolve()
{
	UWRVehicleComponentManageMaterial* VehicleComponentManageMaterial = GetComponent<UWRVehicleComponentManageMaterial>();
	if (VehicleComponentManageMaterial == nullptr)
	{
		return;
	}

	VehicleComponentManageMaterial->OnComplete(EWRManageMaterial::Spawn).RemoveAll(this);

	WRVehicleManager::Get()->RemoveVehicle(this);
}

void AWRVehicle::SetMaterialParameterValue_Internal(FName InParameterName, float InValue, class UMeshComponent* InMeshComponent /*= nullptr*/)
{
	WRACTOR_SETMATERIAL_PARAMETER_VALUE(InParameterName, float, InValue, SetScalarParameterValue, (InMeshComponent == nullptr) ? GetMesh() : InMeshComponent);
}

void AWRVehicle::SetMaterialParameterValue_Internal(FName InParameterName, FLinearColor InValue, class UMeshComponent* InMeshComponent /*= nullptr*/)
{
	WRACTOR_SETMATERIAL_PARAMETER_VALUE(InParameterName, FLinearColor, InValue, SetVectorParameterValue, (InMeshComponent == nullptr) ? GetMesh() : InMeshComponent);
}

void AWRVehicle::StartSmoke(float InValue)
{
	if (InValue)
	{
		if (IsSpawnedStartSmoke)
		{
			return;
		}

		WRVFXManager::Get()->Spawn(EFFECT_PATH_4_SMOKE, this, nullptr);

		IsSpawnedStartSmoke = true;
	}
	else
	{
		int32 CurrentSpeed = FMath::FloorToInt(GetCurrentKPH());
		if (CurrentSpeed)
		{
			return;
		}

		IsSpawnedStartSmoke = false;
	}
}

void AWRVehicle::SteerSmoke(float InValue)
{
	auto CalculateAngle = [&]() {
		for (auto& Wheels : GetVehicleMovement()->Wheels)
		{
			UVehicleWheel* FrontWheels = Cast<UVehicleWheel>(Wheels);
			if (FrontWheels == nullptr)
			{
				continue;
			}

			float Angle = FrontWheels->GetSteerAngle() * 2.0F;
			if (Angle > 15.0F || Angle < -15.0F)
			{
				return true;
			}
		}
		return false;
	};

	if (InValue)
	{
		if (IsSpawnedSteerSmoke)
		{
			return;
		}

		int32 CurrentSpeed = FMath::FloorToInt(GetCurrentKPH());
		if (CurrentSpeed > 49 && CalculateAngle())
		{
			WRVFXManager::Get()->Spawn(EFFECT_PATH_4_SMOKE, this, nullptr);

			IsSpawnedSteerSmoke = true;
		}
	}
	else
	{
		IsSpawnedSteerSmoke = false;
	}
}

void AWRVehicle::SetVehicleInSpawnVolume(bool bInVehicleSpawnArea)
{
	bVehicleSpawnArea = bInVehicleSpawnArea;

	//WRSpawnSimulationManager::Get()->UpdateActorLocationInVolume_deprecated(this, bVehicleSpawnArea);
}

#undef LOCTEXT_NAMESPACE