// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentVR.h"
#include "GameFramework/PlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include "IXRTrackingSystem.h"
#include "Manager/WRConsoleManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Manager/WRInputSystemManager.h"
#include "Actor/Character/WRCharacterMine.h"

#include "ModuleManager.h"
#include "AIController.h"
#include "Table/WRPlayerTable.h"
#include "Table/Base/WRTableManager.h"
// #include "../../Steam/SteamVR/Source/SteamVR/Public/ISteamVRPlugin.h"
// #include "OpenVR/OpenVRv1_5_17/headers/openvr.h"


// For Cpp
DEFINE_LOG_CATEGORY(WRActorComponentVR)

void UWRActorComponentVR::OnCreateComponent()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
	{
		UE_LOG(WRActorComponentVR, Error, TEXT("UWRActorComponentVR::OnCreateComponent - OwnerCharacter is null"));
		return;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APawn* PossessedPawn = PlayerController->GetPawn();
	// [ 2019-7-18 : magedoga ] Possess to playercontroller, If not possess
	if (PossessedPawn == nullptr || OwnerCharacter != Cast<AWRCharacter>(PossessedPawn))
	{
		PlayerController->Possess(OwnerCharacter);

		if (OwnerCharacter->GetAIController())
		{
			OwnerCharacter->GetAIController()->SetPawn(OwnerCharacter);
		}
	}

// 	FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(OwnerCharacter->GetCharacterTableID());
// 	if (PlayerTable != nullptr)
// 	{
// 		USceneComponent* CameraParentComponent = Cast<USceneComponent>(OwnerCharacter->GetDefaultSubobjectByName(FName("CameraParent")));
// 		if (CameraParentComponent != nullptr)
// 		{
// 			//float Height = WRConsoleManager::Get()->GetFloat(EWRConsole::VR_StartHeight);
// 			CameraParentComponent->SetRelativeLocation(PlayerTable->HMDCameraOffset);
// 		}
// 	}
	


//#if STEAMVR_SUPPORTED_PLATFORMS
// 	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == true)
// 	{
// 		FName HMDDeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
// 		UE_LOG(WRActorComponentVR, Display, TEXT("UWRActorComponentVR::OnCreateComponent - HMD Device Name [%s]"), *(HMDDeviceName.ToString()));
// 		ISteamVRPlugin* VRSystem = &FModuleManager::LoadModuleChecked< ISteamVRPlugin >("SteamVR");
// 		if (VRSystem != nullptr)
// 		{
// 			FString DriverID;
// 			vr::TrackedPropertyError Error;
// 			TArray<char> Buffer;
// 			Buffer.AddUninitialized(vr::k_unMaxPropertyStringSize);
// 
// 			int Size = VRSystem->GetVRSystem()->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, Buffer.GetData(), Buffer.Num(), &Error);
// 			if (Error == vr::TrackedProp_BufferTooSmall)
// 			{
// 				Buffer.AddUninitialized(Size - Buffer.Num());
// 				Size = VRSystem->GetVRSystem()->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, Buffer.GetData(), Buffer.Num(), &Error);
// 			}
// 
// 			if (Error == vr::TrackedProp_Success)
// 			{
// 				DriverID = UTF8_TO_TCHAR(Buffer.GetData());
// 			}
// 			else
// 			{
// 				DriverID = UTF8_TO_TCHAR(VRSystem->GetVRSystem()->GetPropErrorNameFromEnum(Error));
// 			}
// 
// 			UE_LOG(WRActorComponentVR, Display, TEXT("UWRActorComponentVR::OnCreateComponent - Driver ID [%s]"), *DriverID);
// 			if (DriverID.ToLower().Contains("huaweicv1pro") == true)
// 			{
// 				IConsoleVariable* FoundConsole = IConsoleManager::Get().FindConsoleVariable(TEXT("r.defaultfeature.antialiasing"));
// 				if (FoundConsole != nullptr)
// 				{
// 					FoundConsole->Set(2);
// 				}
// 			}
// 		}
// 	}
// #else
// 	FName HMDDeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
// 	UE_LOG(WRActorComponentVR, Display, TEXT("UWRActorComponentVR::OnCreateComponent - HMD Device Name [%s]"), *(HMDDeviceName.ToString()));
// #endif // STEAMVR_SUPPORTED_PLATFORMS

// 
// #if WITH_EDITOR
// 	// [ 2019-8-26 : magedoga ] TODO (When Changed HMD Display Enable State)
// 	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false && GetWorld()->WorldType == EWorldType::PIE)
// 	{
// 		UCameraComponent* CameraComponent = Cast<UCameraComponent>(OwnerCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
// 		if (CameraComponent != nullptr)
// 		{
// 			CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 88.0f));
// 
// 			// [ 2019-8-20 : kyu ] HMD Off 시 키매핑에 의한 Yaw 회전으로 수정을 위해 초기 셋팅 추가
// 			FRotator OwnerRotation = OwnerCharacter->GetActorRotation();
// 			CameraComponent->SetWorldRotation(OwnerRotation);
// 		}
// 	}
// #endif 
// 
// 	// [ 2019-7-11 : magedoga ] Setting tracking origin by platform (Oculus, Steam(VIVE), PSVR)
// 	//FName HMDDeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
// 	//UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
// 	//if (HMDDeviceName == FName("PSVR"))
// 	{
// 		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
// 	}

	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(OwnerCharacter->GetActorRotation().Yaw);
}

void UWRActorComponentVR::OnDestroyComponent()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		AWRCharacter* PossessedCharacter = Cast<AWRCharacter>(PlayerController->GetPawn());
		AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(GetOwner());
		if (OwnerCharacter == PossessedCharacter)
		{
			PlayerController->UnPossess();
		}
	}
}

void UWRActorComponentVR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	   
	//AActor* Owner = GetOwner();
	AWRCharacter* Owner = GetOwnerCharacter();
	if (Owner->GetBoardingVehicle())
		return;

	FRotator frot = GetOwnerCharacter()->GetActorRotation();
	FTransform fTran = GetOwnerCharacter()->GetActorTransform();
	FRotator frotTran = fTran.GetRotation().Rotator();


	//====================================================================================
	// [ 2019-7-16 : magedoga ] HMD Transform
// 	FVector VRTrackingLocation;
// 	FRotator VRTrackingRotation;
// 
// 	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(VRTrackingRotation, VRTrackingLocation);
// 	FTransform TrackingToWorld = UHeadMountedDisplayFunctionLibrary::GetTrackingToWorldTransform(this);
// 	FVector VRLocation = TrackingToWorld.TransformPosition(VRTrackingLocation);
// 	FRotator VRRotation = TrackingToWorld.TransformRotation(VRTrackingRotation.Quaternion()).Rotator();

	UCameraComponent* CameraComponent = Cast<UCameraComponent>(Owner->GetComponentByClass(UCameraComponent::StaticClass()));
	FVector VRLocation = CameraComponent->GetComponentLocation();
	FRotator VRRotation = CameraComponent->GetComponentRotation();

// 	FVector VRLocation = WRInputSystemManager::Get()->GetHMDLocation();
// 	FRotator VRRotation = WRInputSystemManager::Get()->GetHMDRotation();
	//====================================================================================


	// [ 2020-3-4 : kyu ] 카메라 회전에 대한 케릭터 Body회전 락
	if (WRInputSystemManager::Get()->IsRotationLock())
		return;


#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
	//USceneComponent* MeshParentComponent = Owner->GetMeshParentComponent();
	//if (MeshParentComponent != nullptr)
	//{
	//	MeshParentComponent->SetWorldRotation(VRRotation);
	//}
#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_

#if !UE_BUILD_SHIPPING
	VRLocation.Z += 20.0f;
	int32 ShowDebugDrawLine = WRConsoleManager::Get()->GetInt(EWRConsole::VR_DebugDrawLine_Axis);
	//====================================================================================
	// [ 2019-7-16 : magedoga ] Pawn
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector OwnerForward = Owner->GetActorForwardVector();

// 	//====================================================================================
// 	// [ 2019-7-16 : magedoga ] HMD
// 	FVector VRTrackingLocation;
// 	FRotator VRTrackingRotation;
// 
// 	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(VRTrackingRotation, VRTrackingLocation);
// 	FTransform TrackingToWorld = UHeadMountedDisplayFunctionLibrary::GetTrackingToWorldTransform(this);                          
// 	FVector VRLocation = TrackingToWorld.TransformPosition(VRTrackingLocation);
// 	VRLocation.Z += 20.0f;
// 	FRotator VRRotation = VRTrackingRotation;


	if (ShowDebugDrawLine > 0 && UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// [ 2019-7-19 : magedoga ] Show Debug Line of HMD
		UKismetSystemLibrary::DrawDebugLine(this, VRLocation, VRLocation + (VRRotation.Quaternion().GetForwardVector() * 1000.0f), FLinearColor::Yellow, 0.0f, 5.0f);
	}

	if (ShowDebugDrawLine > 1)
	{
		// [ 2019-7-19 : magedoga ] Show Debug Line of Pawn
		UKismetSystemLibrary::DrawDebugLine(this, OwnerLocation, OwnerLocation + (OwnerForward * 1000.0f), FLinearColor::Green, 0.0f, 5.0f);
	}

	if (ShowDebugDrawLine > 2)
	{
		// [ 2019-7-19 : magedoga ] Show Debug Line of Camera
		
		{
			FVector CamLocation = CameraComponent->GetComponentLocation();
			FVector CamForward = CameraComponent->GetForwardVector();

			CamLocation.Z -= 20.0f;

			UKismetSystemLibrary::DrawDebugLine(this, CamLocation, CamLocation + (CamForward * 1000.0f), FLinearColor::Red, 0.0f, 5.0f);
		}

	}
#endif // UE_BUILD_SHIPPING
}

void UWRActorComponentVR::AddRotationYaw(const float InDeltaYaw, const float InScale)
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == true)
	{
		FRotator HMDOrientation = GEngine->XRSystem->GetBaseOrientation().Rotator();
		HMDOrientation.Yaw += InDeltaYaw * -InScale;
		GEngine->XRSystem->SetBaseOrientation(HMDOrientation.Quaternion());
	}
}

void UWRActorComponentVR::SetBaseOrientationRotator(const FRotator& InRotator)
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == false)
		return;

	GEngine->XRSystem->SetBaseOrientation(InRotator.Quaternion());
}

FRotator UWRActorComponentVR::GetBaseOrientationRotator() const
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == false)
		return FRotator::ZeroRotator;

	FRotator HMDOrientation = GEngine->XRSystem->GetBaseOrientation().Rotator();
	return HMDOrientation;
}

FRotator UWRActorComponentVR::GetHMDRelativeRotator(const bool bInUseOnlyInputSystem /*= false*/) const
{
	FRotator OwnerRotator = GetOwner()->GetActorRotation();
	FRotator HMDRotator = WRInputSystemManager::Get()->GetHMDRotation();

	if (bInUseOnlyInputSystem == true)
		return HMDRotator;

	return (HMDRotator - OwnerRotator).Clamp();
}

void UWRActorComponentVR::ResetOrientation(const float InYaw, const bool bInUseOnlyInputSystem /*= false*/)
{
// 	float ClampAxisYaw = FRotator::NormalizeAxis(FRotator::ClampAxis());
 	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(InYaw, EOrientPositionSelector::Orientation);

	float HMDRelativeYaw = FRotator::NormalizeAxis(this->GetHMDRelativeRotator(bInUseOnlyInputSystem).Yaw);
	this->AddRotationYaw(HMDRelativeYaw, 1.0f);
}

void UWRActorComponentVR::SetCameraOffset(const EWRCameraOffset IN InType, const FVector IN InOffset /*= FVector::ZeroVector*/)
{
	AWRCharacterMine* pMine = Cast<AWRCharacterMine>(GetOwnerCharacter());
	if (pMine == nullptr)
		return;

	USceneComponent* CameraParent = Cast<USceneComponent>(pMine->GetDefaultSubobjectByName(FName("CameraParent")));
	if (CameraParent->IsValidLowLevel() == false)
		return;

	FVector ResultOffset = FVector::ZeroVector;
	FVector RelativeLocation = CameraParent->GetRelativeTransform().GetTranslation();

	switch (InType)
	{
	case EWRCameraOffset::None:
	{
		ResultOffset = RelativeLocation - BeforeCameraOffset;
		BeforeCameraOffset = FVector::ZeroVector;

	}break;
	case EWRCameraOffset::Add:
	{
		BeforeCameraOffset += InOffset;
		ResultOffset = RelativeLocation + InOffset;
	}break;
	case EWRCameraOffset::New:
	{
		// 일단 이전 오프셋만큼 빼고
		ResultOffset = RelativeLocation - BeforeCameraOffset;
		// 새로운 오프셋만큼 추가
		BeforeCameraOffset = InOffset;
		ResultOffset = ResultOffset + InOffset;
	}break;
	case EWRCameraOffset::Replace:
	{		
		// 일단 이전 오프셋만큼 빼고
		ResultOffset = RelativeLocation - BeforeCameraOffset;
		// 이전 오프셋 초기화
		BeforeCameraOffset = FVector::ZeroVector;
		ResultOffset = InOffset;
	}break;
	}

	CameraParent->SetRelativeLocation(ResultOffset);

// 	auto SetCameraParentOffset = [&](const FVector IN _offset, const bool _Add)
// 	{
// 		TArray<UActorComponent*> ActorComponents;
// 		pMine->GetComponents(USceneComponent::StaticClass(), ActorComponents);
// 		for (int32 i = 0; i < ActorComponents.Num(); ++i)
// 		{
// 			if (ActorComponents[i]->GetName().Contains("CameraParent") == false)
// 				continue;
// 
// 			USceneComponent* CameraParent = Cast<USceneComponent>(ActorComponents[i]);
// 			FTransform RelativeTR = CameraParent->GetRelativeTransform();
// 			FVector CalculatedLocation = _Add == true ? RelativeTR.GetTranslation() + _offset : RelativeTR.GetTranslation() - _offset;
// 			CameraParent->SetRelativeLocation(CalculatedLocation);
// 		}
// 	};
// 
// 	switch (InType)
// 	{
// 	case EWRCameraOffset::None:
// 	{
// 		// 이전 오프셋만큼 뺌
// 		SetCameraParentOffset(BeforeCameraOffset, false);
// 		// 초기화
// 		BeforeCameraOffset = FVector::ZeroVector;
// 	}
// 	break;
// 	case EWRCameraOffset::Add:
// 	{
// 		// 이전 오프셋 유지한채로 추가
// 		BeforeCameraOffset += InOffset;
// 		SetCameraParentOffset(InOffset, true);
// 	}
// 	break;
// 	case EWRCameraOffset::New:
// 	{
// 		// 일단 이전 오프셋만큼 빼고
// 		SetCameraParentOffset(BeforeCameraOffset, false);
// 
// 		// 새로운 오프셋만큼 추가
// 		BeforeCameraOffset = InOffset;
// 		SetCameraParentOffset(InOffset, true);
// 	}
// 	break;
// 	}
}

void UWRActorComponentVR::InitCameraOffsetByPlayerTable()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
	{
		UE_LOG(WRActorComponentVR, Error, TEXT("UWRActorComponentVR::InitCameraOffsetByPlayerTable - OwnerCharacter is null"));
		return;
	}

	FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(OwnerCharacter->GetCharacterTableID());
	if (PlayerTable != nullptr)
	{
		UE_LOG(WRActorComponentVR, Display, TEXT("UWRActorComponentVR::InitCameraOffsetByPlayerTable"));
		this->SetCameraOffset(EWRCameraOffset::Replace, PlayerTable->HMDCameraOffset);
	}
}
