// Fill out your copyright notice in the Description page of Project Settings.


#include "WRWidgetComponentBase.h"
#include "UI/Base/WRUIPanel.h"
#include "Core/WRGameInstance.h"
#include "Actor/Character/WRCharacter.h"
#include "Manager/WRCharacterManager.h"
#include "Camera/CameraComponent.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Utility/WRActorUtility.h"
#include "UnrealMathUtility.h"
#include "Manager/WRInputSystemManager.h"
#include "ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/WRConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Component/Custom/WRStereoLayerComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Component/Custom/WRTestStereoLayerComponent.h"
#include "Component/Custom/WRUIStereoLayerComponent.h"

DECLARE_STATS_GROUP(TEXT("NameTag"), STATGROUP_NameTag, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("NameTag_UpdateCamera"), STAT_NameTag_UpdateCamera, STATGROUP_NameTag);
DECLARE_CYCLE_STAT(TEXT("NameTagComponent_ALL"), STAT_NameTag_ALL, STATGROUP_NameTag);
DECLARE_CYCLE_STAT(TEXT("NameTagComponent_Engine"), STAT_NameTag_Engine, STATGROUP_NameTag);

UWRWidgetComponentBase::UWRWidgetComponentBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostUpdateWork;

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetDrawAtDesiredSize(true);

	InitMaterial();
}

void UWRWidgetComponentBase::BeginPlay()
{
	Super::BeginPlay();

}

void UWRWidgetComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (WRStereoLayerComponent)
	{
		WRStereoLayerComponent->SetTexture(nullptr);
		WRStereoLayerComponent->SetVisibility(false);
		WRStereoLayerComponent->SetActive(false);
		if (WRStereoLayerComponent->IsRegistered())
			WRStereoLayerComponent->UnregisterComponent();
		WRStereoLayerComponent->RemoveFromRoot();
		WRStereoLayerComponent->DestroyComponent();
		WRStereoLayerComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UWRWidgetComponentBase::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_NameTag_ALL);

	if(bCyberUI)
		Tick_CyberAni(DeltaTime);

	if (bAttachToCamera)
		UpdateLocationByCamera(DeltaTime);
	else if (bAttachToActor && GetOwnerActor() != nullptr)
		UpdateLocationByActor();
	
	if (OffsetScaleForFixedSize != 0.f)
		UpdateScaleForFixedSize(DeltaTime);
	
	if (bBillboard)
		UpdateRotationByBilboard();
	
	SCOPE_CYCLE_COUNTER(STAT_NameTag_Engine);
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	

	Tick_StereoLayer(DeltaTime);	
}

void UWRWidgetComponentBase::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	if (bNewActive == false )
	{
		if(bAddedToScreen)
			RemoveWidgetFromScreen();
		
		if (WRStereoLayerComponent)
		{
			WRStereoLayerComponent->SetTexture(nullptr);
			WRStereoLayerComponent->SetVisibility(false);
			WRStereoLayerComponent->SetActive(false);
			if (WRStereoLayerComponent->IsRegistered())
				WRStereoLayerComponent->UnregisterComponent();
			WRStereoLayerComponent->RemoveFromRoot();
			WRStereoLayerComponent->DestroyComponent();
			WRStereoLayerComponent = nullptr;
		}
	}
}

void UWRWidgetComponentBase::SetWidget(UUserWidget* InWidget)
{
	Super::SetWidget(InWidget);

	InitCyberMaterial();
}

void UWRWidgetComponentBase::SetFixedTransform(const FTransform& IN InTransform)
{
	OffsetTransform = InTransform;
	OffsetTransform.SetScale3D(OffsetTransform.GetScale3D() * 0.2f);
	SetRelativeTransform(OffsetTransform);
}

void UWRWidgetComponentBase::SetOffsetTransform(const FTransform& IN InTransform)
{
	OffsetTransform = InTransform;
	OffsetTransform.SetScale3D(OffsetTransform.GetScale3D() * 0.2f);
}

void UWRWidgetComponentBase::SetBillboard(const bool IN bInBillboard)
{
	bBillboard = bInBillboard;
}

void UWRWidgetComponentBase::SetOffsetScaleForFixedSize(const float IN InOffsetScaleForFixedSize)
{
	OffsetScaleForFixedSize = InOffsetScaleForFixedSize;
}

void UWRWidgetComponentBase::SetOwnerActor(const AActor* const IN InOwnerActor)
{
	if (InOwnerActor == nullptr)
		return;

	OwnerActor = InOwnerActor;
}

void UWRWidgetComponentBase::SetBitFlag(const EWRUIWidgetCreationBitFlag IN InWidgetComponentFlag)
{
	WidgetComponentFlag = InWidgetComponentFlag;

	if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::Billboard)
	{
		SetBillboard(true);
	}
	if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::ManualRedraw)
	{
		bManuallyRedraw = true;
	}
	if ((InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCamera)
		|| (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCameraAll))
	{
		SetAttachToCamera(true);

		TransformForCameraAll = OffsetTransform;

		if ((WidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCameraAll))
		{
			FRotator OffsetRotator = TransformForCameraAll.GetRotation().Rotator();
			OffsetRotator.Yaw = (OffsetRotator.Yaw - 180) * -1;

			TransformForCameraAll.SetRotation(OffsetRotator.GetInverse().Quaternion());
		}
	}
	else if (GetOwnerActor() != nullptr)
	{
		if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCharacter)
		{
			bAttachToActor = true;
		}
		else if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCharacterAndRotateToMe)
		{
			bAttachToActor = true;
			bRotateToMe = true;
		}
		else if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCharacterAndRotateAxisZToMe)
		{
			bAttachToActor = true;
			bRotateAxisZToMe = true;
		}
		else
		{
			if (OffsetTransform.Equals(FTransform::Identity))
				SetFixedTransform(GetOwnerActor()->GetTransform());
			else
				SetFixedTransform(OffsetTransform * GetOwnerActor()->GetTransform());
		}
	}
	if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::IgnoreDepth)
	{
		SetBlendMode(EWidgetBlendMode::Transparent);

		// CreateStereo Flag 가 있으면 스테레오 레이어용 Material (Widget Component Hidden Material 을 셋팅한다)
		if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::CreateStereoLayer)
		{
			SetMaterialAsHidden();
		}
		else if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::CreateWidgetComponent)
		{
			SetMaterialByBlendMode(EWidgetBlendMode::Transparent);
		}

		InitCyberMaterial();

		bIgnore = true;

		if (WRStereoLayerComponent)
			WRStereoLayerComponent->SetSupportsDepth(0);	// 1 = true, 0 = false
	}

	if (InWidgetComponentFlag & EWRUIWidgetCreationBitFlag::WidgetInteractionable)
	{
		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	// Stereo Lyaer Setting
	if (WRStereoLayerComponent != nullptr)
	{
		if (WidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCameraAll)
			WRStereoLayerComponent->SetStereoLayerType(EStereoLayerType::SLT_FaceLocked);
		else
			WRStereoLayerComponent->SetStereoLayerType(EStereoLayerType::SLT_WorldLocked);
	}
}

void UWRWidgetComponentBase::SetAttachSocketName(const FName& IN InAttachSoccketName)
{
	AttachSocketName = InAttachSoccketName;
}

void UWRWidgetComponentBase::SetAttachToCamera(const bool IN bInAttachToCamera)
{
	bAttachToCamera = bInAttachToCamera;

	if (bAttachToCamera)
	{
		AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		if (MyChar == nullptr)
			return;

		UCameraComponent* CamComponet = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
		if (CamComponet == nullptr)
			return;

		FTransform TransformTemp = CamComponet->GetComponentTransform();
		BeforeYaw = FRotator(TransformTemp.GetRotation()).Yaw;
		SetRelativeLocation(TransformTemp.GetLocation());
	}
}

void UWRWidgetComponentBase::SetMaterialAsHidden()
{
	SetMaterial(0, MaterialHideWidgetComponent);
}

void UWRWidgetComponentBase::SetMaterialByBlendMode(const EWidgetBlendMode IN InBlendMode)
{
	if (InBlendMode == EWidgetBlendMode::Transparent)
	{
		if (bIsTwoSided)
			SetMaterial(0, TranslucentMaterial);
		else
			SetMaterial(0, TranslucentMaterial_OneSided);
	}
	else if (InBlendMode == EWidgetBlendMode::Opaque)
	{
		if (bIsTwoSided)
			SetMaterial(0, OpaqueMaterial);
		else
			SetMaterial(0, OpaqueMaterial_OneSided);
	}
	else if (InBlendMode == EWidgetBlendMode::Masked)
	{
		if (bIsTwoSided)
			SetMaterial(0, MaskedMaterial);
		else
			SetMaterial(0, MaskedMaterial_OneSided);
	}
}

void UWRWidgetComponentBase::StartCyberAni()
{
	if (bCyberUI == false)
		return;

	UMaterialInstanceDynamic*  DynamicMat = GetMaterialInstance();
	if (DynamicMat == nullptr)
		return;

	CyberUIOpacityTime = 1.f;
	DynamicMat->SetScalarParameterValue("UIMainOpacity", 0);
}

void UWRWidgetComponentBase::UseStereoLayer(const EStereoLayerShape IN StereoLayerShape)
{
	CreateStereoLayerComponent(StereoLayerShape);
}

void UWRWidgetComponentBase::InitMaterial()
{
	// Translucent material instances
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterial_Finder(TEXT("/Game/Asset/UI/Material/XR_Widget3DPassThrough_IgnoreDepth_TranslucentMaterial_TwoSided"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterial_OneSided_Finder(TEXT("/Game/Asset/UI/Material/XR_Widget3DPassThrough_IgnoreDepth_TranslucentMaterial_OneSided"));
	TranslucentMaterial = TranslucentMaterial_Finder.Object;
	TranslucentMaterial_OneSided = TranslucentMaterial_OneSided_Finder.Object;

	// Opaque material instances
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpaqueMaterial_Finder(TEXT("/Game/Asset/UI/Material/XR_Widget3DPassThrough_OpaqueMaterial_TwoSided"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpaqueMaterial_OneSided_Finder(TEXT("/Game/Asset/UI/Material/XR_Widget3DPassThrough_OpaqueMaterial_OneSided"));
	OpaqueMaterial = OpaqueMaterial_Finder.Object;
	OpaqueMaterial_OneSided = OpaqueMaterial_OneSided_Finder.Object;

	// Masked material instances
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaskedMaterial_Finder(TEXT("/Game/Asset/UI/Material/XR_Widget3DPassThrough_MaskedMaterial_TwoSided"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaskedMaterial_OneSided_Finder(TEXT("/Game/Asset/UI/Material/XR_Widget3DPassThrough_MaskedMaterial_OneSided"));
	MaskedMaterial = MaskedMaterial_Finder.Object;
	MaskedMaterial_OneSided = MaskedMaterial_OneSided_Finder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialHideWidgetComponent_Finder(TEXT("/Game/Asset/UI/Material/MaterialHideWidgetComponent"));
	MaterialHideWidgetComponent = MaterialHideWidgetComponent_Finder.Object;
}

void UWRWidgetComponentBase::Tick_CyberAni(const float IN InDeltaTime)
{
	if (bCyberUI == false)
		return;

	if (CyberUIOpacityTime <= 0)
		return;

	CyberUIOpacityTime -= InDeltaTime * WRConsoleManager::Get()->GetFloat(EWRConsole::UI_CyberAniSpeed);
	if (CyberUIOpacityTime < 0)
		CyberUIOpacityTime = 0.f;

	GetMaterialInstance()->SetScalarParameterValue("UIMainOpacity", 1 - CyberUIOpacityTime);
}

void UWRWidgetComponentBase::Tick_StereoLayer(const float IN InDeltaTime)
{
	if (WRStereoLayerComponent == nullptr)
		return;

	if ((WidgetComponentFlag & EWRUIWidgetCreationBitFlag::CreateStereoLayer) == false)
		return;

	FTransform WidgetComponentTranform = GetRelativeTransform();
	if ((WidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCameraAll))
	{
		WidgetComponentTranform = TransformForCameraAll;
	}
	else
	{
		WidgetComponentTranform.SetRotation(WidgetComponentTranform.GetRotation() * FQuat(WidgetComponentTranform.GetRotation().GetUpVector(), PI));
	}

	WRStereoLayerComponent->SetRelativeTransform(WidgetComponentTranform);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////if (WRConsoleManager::IsValid() && WRConsoleManager::Get()->GetBool(EWRConsole::TestYSY) && WRStereoLayerComponent)
	//if (WRConsoleManager::IsValid() && (WidgetComponentFlag & EWRUIWidgetCreationBitFlag::CreateStereoLayer) && WRStereoLayerComponent)
	//{
	//	//WRStereoLayerComponent->SetVisibility(true);
	//	FTransform WidgetComponentTranform = GetRelativeTransform();
	//	if ((WidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCameraAll))
	//	{
	//		WidgetComponentTranform = TransformForCameraAll;
	//		FVector ff = WidgetComponentTranform.GetLocation();
	//		WidgetComponentTranform.SetLocation(ff);
	//	}
	//	else
	//	{
	//		FVector ff = WidgetComponentTranform.GetLocation();

	//		// [ 2020-8-12 : vuvle ] 동시에 두개 띄울때 겹치지 않아 보이게 만들기 위한 Test Code
	//		//ff.Z += 50;
	//		WidgetComponentTranform.SetLocation(ff);
	//		WidgetComponentTranform.SetRotation(WidgetComponentTranform.GetRotation() * FQuat(WidgetComponentTranform.GetRotation().GetUpVector(), PI));

	//		if (WRStereoLayerComponent->GetStereoLayerShape() == EStereoLayerShape::SLSH_CylinderLayer)
	//		{
	//			WidgetComponentTranform.SetScale3D(FVector(2.0f, 2.0f, 2.0f));
	//		}
	//		//WidgetComponentTranform.SetRotation(WidgetComponentTranform.GetRotation());
	//	}

	//	//		WidgetComponentTranform.SetLocation                         (WidgetComponentTranform.GetLocation() - GetCameraLocation());
	//	WRStereoLayerComponent->SetRelativeTransform(WidgetComponentTranform);

	//	if (WRStereoLayerComponent->GetStereoLayerShape() == EStereoLayerShape::SLSH_CylinderLayer)
	//	{
	//		UWRUIPanel* CastedPanel = Cast<UWRUIPanel>(GetUserWidgetObject());
	//		WRStereoLayerComponent->SetCylinderRadius(180 * CurrentDrawSize.X / (CastedPanel->GetCylinderArcAngle() * PI));
	//		WRStereoLayerComponent->SetCylinderOverlayArc(CurrentDrawSize.X);
	//		WRStereoLayerComponent->SetCylinderHeight(CurrentDrawSize.Y);
	//		WRStereoLayerComponent->SetTexture(Cast<UTexture>(GetRenderTarget()));
	//	}
	//	else if (WRStereoLayerComponent->GetStereoLayerShape() == EStereoLayerShape::SLSH_QuadLayer)
	//	{
	//		WRStereoLayerComponent->SetTexture(Cast<UTexture>(GetRenderTarget()));
	//		WRStereoLayerComponent->SetQuadSize(CurrentDrawSize);
	//	}

	//	//WRStereoLayerComponent->MarkTextureForUpdate();

	//	/*if (WRStereoLayerComponent)
	//		WRStereoLayerComponent->SetVisibility(false);*/

	//		// 		if(WidgetComponentFlag & EWRUIWidgetCreationBitFlag::CreateStereoLayer)
	//		// 		{
	//		// 			SetMaterialAsHidden();
	//		// 		}
	//}
}

void UWRWidgetComponentBase::UpdateLocationByActor()
{
	if (GetOwnerActor()->IsValidLowLevel() == false)
		return;

	FTransform TransformTemp(FTransform::Identity);

	if(AttachSocketName.IsNone())
		TransformTemp = GetOwnerActor()->GetTransform();
	else
		TransformTemp = WRActorUtility::GetSocketTransform(GetOwnerActor(), AttachSocketName);
	
	TransformTemp.SetScale3D(FVector::OneVector);

	if (OffsetTransform.Equals(FTransform::Identity) == false)
	{
		if (bRotateAxisZToMe)
		{
			AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
			if (MyChar == nullptr)    
				return;

			FTransform ConvertedOffsetTransform = OffsetTransform;

			// 원래 상대위치
			FVector SrcRelativeLocation = ConvertedOffsetTransform.GetLocation();
			FVector DstRelativeLocation = SrcRelativeLocation;
			SrcRelativeLocation.Z = 0.0f;

			// 대상에서 내게 오는 벡터
			FVector OwnerLocation = TransformTemp.GetLocation();
			OwnerLocation.Z = 0.0f;
			FVector MainCharLocation = MyChar->GetActorLocation();
			MainCharLocation.Z = 0.0f;
			FVector ToMainCharVector = MainCharLocation - OwnerLocation;

			// 1. 대상을 원래 상대위치에서 내 각도만큼 돌림 (대상에서 내쪽의 각도로)
			float fDegrees = WRActorUtility::GetDegreesBetween2Vectors(GetOwnerActor()->GetActorForwardVector(), ToMainCharVector);
			DstRelativeLocation = DstRelativeLocation.RotateAngleAxis(fDegrees, FVector(0, 0, 1));

			// 2. 대상을 원래 Offset 각도만큼 되돌림 (원래 전방에서 오프셋으로)
// 			fDegrees = WRActorUtility::GetDegreesBetween2Vectors(FVector::ForwardVector, SrcRelativeLocation);
// 			fDegrees *= -1.0f;
// 			DstRelativeLocation = DstRelativeLocation.RotateAngleAxis(fDegrees, FVector(0, 0, 1));

			ConvertedOffsetTransform.SetLocation(DstRelativeLocation);
			TransformTemp = ConvertedOffsetTransform * TransformTemp;
		}
		else if (bRotateToMe)
		{
			AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
			if (MyChar == nullptr)
				return;

			FTransform ConvertedOffsetTransform = OffsetTransform;
			FVector SrcRelativeLocation = ConvertedOffsetTransform.GetLocation();
			float fDist = SrcRelativeLocation.Size();

			// 대상에서 내게 오는 벡터
			FVector OwnerLocation = TransformTemp.GetLocation();
			FVector MainCharLocation = MyChar->GetActorLocation();
			FVector ToMainCharVector = MainCharLocation - OwnerLocation;
			ToMainCharVector.Normalize();

			ToMainCharVector *= fDist;

			OwnerLocation += ToMainCharVector;
			TransformTemp.SetLocation(OwnerLocation);
			TransformTemp.SetScale3D(OffsetTransform.GetScale3D());
		}
		else
		{
			TransformTemp = OffsetTransform * TransformTemp;
		}
	}
	
	if (GetRelativeLocation() != TransformTemp.GetLocation())
	{
		SetRelativeLocation(TransformTemp.GetLocation());
	}
	if (GetRelativeRotation().Equals(TransformTemp.GetRotation().Rotator()) == false)
	{
		SetRelativeRotation(TransformTemp.GetRotation());
	}
	if (GetRelativeScale3D().Equals(TransformTemp.GetScale3D()) == false)
	{
		SetRelativeScale3D(TransformTemp.GetScale3D());
	}	
}

void UWRWidgetComponentBase::UpdateLocationByCamera(const float IN InDeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_NameTag_UpdateCamera);

	FTransform TransformTemp(FTransform::Identity);
	TransformTemp.SetLocation(GetCameraLocation());

	FRotator RotatorTemp(GetCameraRotation());
	TransformTemp.SetRotation(RotatorTemp.Quaternion());

	if(WidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCamera)
		RotatorTemp.Pitch = 0;		// 상하로는 안움직이도록 함

	if (WidgetComponentFlag & EWRUIWidgetCreationBitFlag::CreateStereoLayer)
	{
		if (WRConsoleManager::Get()->GetBool(EWRConsole::TestYSY2) == false)
		{
			RotatorTemp.Roll = 0;
		}
	}
	else
	{
		RotatorTemp.Roll = 0;
	}	
	
	if (WidgetComponentFlag & EWRUIWidgetCreationBitFlag::SmoothMoving)
		BeforeYaw = RotatorTemp.Yaw = SmoothMoving(InDeltaTime, RotatorTemp.Yaw);		// 고개를 돌렸을때 스무스하게 따라다니게 하는 기능 

	TransformTemp.SetRotation(FQuat(RotatorTemp));

	if (OffsetTransform.Equals(FTransform::Identity) == false)
	{
		if(WidgetComponentFlag & EWRUIWidgetCreationBitFlag::CreateStereoLayer)
		{
			/*TransformForCameraAll = OffsetTransform;

			if ((WidgetComponentFlag & EWRUIWidgetCreationBitFlag::AttachToCameraAll))
			{					
				FRotator OffsetRotator = TransformForCameraAll.GetRotation().Rotator();
				OffsetRotator.Yaw = (OffsetRotator.Yaw - 180) * -1;
					
				TransformForCameraAll.SetRotation(OffsetRotator.GetInverse().Quaternion());
			}*/

			RotatorTemp.Roll = 0;
			TransformTemp.SetRotation(FQuat(RotatorTemp));
		}
		
		TransformTemp = OffsetTransform * TransformTemp;
	}

	if (GetRelativeLocation() != TransformTemp.GetLocation())
	{
		SetRelativeLocation(TransformTemp.GetLocation());
	}
	if (GetRelativeRotation().Equals(TransformTemp.GetRotation().Rotator()) == false)
	{
		SetRelativeRotation(TransformTemp.GetRotation());
	}
	if (GetRelativeScale3D().Equals(TransformTemp.GetScale3D()) == false)
	{
		SetRelativeScale3D(TransformTemp.GetScale3D());
	}
}

void UWRWidgetComponentBase::UpdateScaleForFixedSize(const float IN InDeltaTime)
{
	FVector CameraLocation = GetCameraLocation();
	FTransform TransformTemp = GetRelativeTransform();
	float DistTemp = FVector::Dist(TransformTemp.GetLocation(), CameraLocation);
	float ScaleTemp = 0.f;

	ScaleTemp = DistTemp * OffsetScaleForFixedSize * (0.5 + (DistTemp > 2000 ? 0 : 2000 - DistTemp) / 4000);

	if(bAttachToActor)
		SetRelativeScale3D(TransformTemp.GetScale3D() * ScaleTemp);
	else
		SetRelativeScale3D(OffsetTransform.GetScale3D() * ScaleTemp);
}

void UWRWidgetComponentBase::UpdateRotationByBilboard()
{
	if (GetWorld()->WorldType != EWorldType::PIE && GetWorld()->WorldType != EWorldType::Game)
		return;

	FVector Dir = GetCameraLocation() - GetRelativeLocation();
	Dir.Z = 0.f;
	FRotator WidgetRotation = Dir.Rotation();
	

	SetRelativeRotation(FQuat(WidgetRotation));
}

void UWRWidgetComponentBase::OnRenderTargetSizeChanged(const FVector2D& IN InRenderTargetSize)
{
	if (WRStereoLayerComponent)
	{
		if (WRStereoLayerComponent->GetStereoLayerShape() == EStereoLayerShape::SLSH_CylinderLayer)
		{
			UWRUIPanel* CastedPanel = Cast<UWRUIPanel>(GetUserWidgetObject());
			if (CastedPanel != nullptr)
			{
				WRStereoLayerComponent->SetCylinderRadius(180 * InRenderTargetSize.X / (CastedPanel->GetCylinderArcAngle() * PI));
			}			

			WRStereoLayerComponent->SetCylinderOverlayArc(InRenderTargetSize.X);
			WRStereoLayerComponent->SetCylinderHeight(InRenderTargetSize.Y);
		}
		else if (WRStereoLayerComponent->GetStereoLayerShape() == EStereoLayerShape::SLSH_QuadLayer)
		{
			WRStereoLayerComponent->SetQuadSize(InRenderTargetSize);
		}
	}	
}

FVector UWRWidgetComponentBase::CalculateGoAToB(const float IN InDeltaTime, const FVector& IN InAPosition, const FVector& IN InBPosition)
{
	if (InAPosition == InBPosition)
		return InAPosition;

	FVector Dir = InBPosition - InAPosition;
	FVector NormalDir = Dir;
	NormalDir.Normalize();

	FVector DestPosition;
	if (Dir.Size() > 200)
		DestPosition = InAPosition + NormalDir * InDeltaTime * Dir.Size() * 0.3;
	else
		DestPosition = InAPosition + NormalDir * InDeltaTime * 60;
	
	if ((DestPosition - InAPosition).Size() > Dir.Size())
		DestPosition = InBPosition;

	return DestPosition;
}

float UWRWidgetComponentBase::SmoothMoving(const float IN InDeltaTime, float IN InCameraYaw)
{
	float returnYaw = InCameraYaw;
	if (BeforeYaw != InCameraYaw)
	{
		float Speed;
		if (FMath::Abs(InCameraYaw - BeforeYaw) < 180)
		{
			Speed = InCameraYaw - BeforeYaw;
		}
		else if (InCameraYaw > BeforeYaw)
		{
			InCameraYaw -= 360;
			Speed = InCameraYaw - BeforeYaw;
		}
		else
		{
			InCameraYaw += 360;
			Speed = InCameraYaw - BeforeYaw;
		}


		if (FMath::Abs(Speed) < 20)
			Speed = Speed / FMath::Abs(Speed) * 20;

		returnYaw = BeforeYaw + Speed * 1.6 * InDeltaTime;

		if (Speed > 0)
		{
			if (returnYaw > InCameraYaw)
				returnYaw = InCameraYaw;
		}
		else
		{
			if (returnYaw < InCameraYaw)
				returnYaw = InCameraYaw;
		}
	}
	
	return returnYaw;
}

FVector UWRWidgetComponentBase::GetCameraLocation()
{
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
 	return PlayerController->PlayerCameraManager->GetCameraLocation();
	
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar == nullptr)
		return FVector::ZeroVector;

	UCameraComponent* CameraComponent = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CameraComponent != nullptr)
	{
		return CameraComponent->GetComponentLocation();
	}

	return FVector::ZeroVector;
}

FRotator UWRWidgetComponentBase::GetCameraRotation()
{
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	return PlayerController->PlayerCameraManager->GetCameraRotation();

	AWRCharacterMine* MyChar =  WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar == nullptr)
		return FRotator::ZeroRotator;

	UCameraComponent* CameraComponent = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CameraComponent != nullptr)
	{
		return CameraComponent->GetComponentRotation();
	}

	return FRotator::ZeroRotator;
}

void UWRWidgetComponentBase::InitCyberMaterial()
{
	UWRUIPanel* Panel = Cast<UWRUIPanel>(Widget);
	UMaterialInstanceDynamic*  DynamicMat = GetMaterialInstance();

	if (Panel && DynamicMat)
	{
		bCyberUI = Panel->IsCyberUI();
		DynamicMat->SetScalarParameterValue("BaseMain", (bCyberUI ? 0 : 1));
		if (bCyberUI)
		{	
			DynamicMat->SetScalarParameterValue("UIRandom", float(FMath::Rand() % 101) / 100.f);
			bool bUseCyberAni = Panel->IsUseCyberAni();
			if (bUseCyberAni)
			{
				CyberUIOpacityTime = 1.f;
				DynamicMat->SetScalarParameterValue("UIMainOpacity", 0);
			}
			else
			{
				CyberUIOpacityTime = 0.f;
				DynamicMat->SetScalarParameterValue("UIMainOpacity", 1);
			}
		}
	}
}

void UWRWidgetComponentBase::CreateStereoLayerComponent(const EStereoLayerShape IN InStereoLayerShape/* = EStereoLayerShape::SLSH_QuadLayer*/)
{
	if (WRStereoLayerComponent != nullptr)
		return;

	WRStereoLayerComponent = NewObject<UWRUIStereoLayerComponent>();
	if (WRStereoLayerComponent == nullptr)
		return;

	if(WRStereoLayerComponent->IsRegistered() == false)
	{
		UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();
		if (GameInstance == nullptr)
			return;

		UWorld* World = GameInstance->GetWorld();
		if (World == nullptr)
			return;

		WRStereoLayerComponent->RegisterComponentWithWorld(World);
	}

	WRStereoLayerComponent->SetStereoLayerShape(InStereoLayerShape);
	WRStereoLayerComponent->AddToRoot();
	WRStereoLayerComponent->SetActive(true);
	WRStereoLayerComponent->bLiveTexture = true;
	
	UWRUIPanel* CastedPanel = Cast<UWRUIPanel>(GetUserWidgetObject());
	if (CastedPanel != nullptr)
	{
		WRStereoLayerComponent->SetPanelKind(CastedPanel->GetPanelKind());
	}
}

void UWRWidgetComponentBase::DrawRenderTarget(const float IN InDeltaTime)
{
	if (WidgetRenderer == nullptr)
		return;

	DrawWidgetToRenderTarget(InDeltaTime);
}

void UWRWidgetComponentBase::UpdateRenderTarget(FIntPoint DesiredRenderTargetSize)
{
	bool bWidgetRenderStateDirty = false;
	bool bClearColorChanged = false;

	FLinearColor ActualBackgroundColor = BackgroundColor;
	switch (BlendMode)
	{
	case EWidgetBlendMode::Opaque:
		ActualBackgroundColor.A = 1.0f;
		break;
	case EWidgetBlendMode::Masked:
		ActualBackgroundColor.A = 0.0f;
		break;
	}

	if (DesiredRenderTargetSize.X != 0 && DesiredRenderTargetSize.Y != 0)
	{
		if (RenderTarget == nullptr)
		{
			RenderTarget = NewObject<UTextureRenderTarget2D>(this);
			RenderTarget->ClearColor = ActualBackgroundColor;

			bClearColorChanged = bWidgetRenderStateDirty = true;

			RenderTarget->InitCustomFormat(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y, PF_B8G8R8A8, false);

			if (MaterialInstance)
			{
				MaterialInstance->SetTextureParameterValue("SlateUI", RenderTarget);
			}

			if (WRStereoLayerComponent)
			{
				WRStereoLayerComponent->SetTexture(Cast<UTexture>(GetRenderTarget()));
				OnRenderTargetSizeChanged(FVector2D(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y));
			}
		}
		else
		{
			// Update the format
			if (RenderTarget->SizeX != DesiredRenderTargetSize.X || RenderTarget->SizeY != DesiredRenderTargetSize.Y)
			{
				RenderTarget->InitCustomFormat(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y, PF_B8G8R8A8, false);
				bWidgetRenderStateDirty = true;

				OnRenderTargetSizeChanged(FVector2D(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y));
			}

			// Update the clear color
			if (RenderTarget->ClearColor != ActualBackgroundColor)
			{
				RenderTarget->ClearColor = ActualBackgroundColor;
				bClearColorChanged = bWidgetRenderStateDirty = true;
			}

			if (bWidgetRenderStateDirty)
			{
				RenderTarget->UpdateResourceImmediate();
			}
		}
	}

	if (RenderTarget)
	{
		// If the clear color of the render target changed, update the BackColor of the material to match
		if (bClearColorChanged && MaterialInstance)
		{
			MaterialInstance->SetVectorParameterValue("BackColor", RenderTarget->ClearColor);
		}

		if (bWidgetRenderStateDirty)
		{
			MarkRenderStateDirty();
		}
	}
}
