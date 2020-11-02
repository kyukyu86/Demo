// Fill out your copyright notice in the Description page of Project Settings.
#include "WRPreviewStudio.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"


#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LightComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"

#include "Math/UnrealMathUtility.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Manager/WRAsyncLoadUObjectManager.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Utility/WRActorSpawnUtility.h"

#include "UI/Slot/WRUISlot_Preview.h"

#include "Table/WRCharacterPresetTable.h"
#include "Table/Base/WRTableManager.h"

#include "../Character/WRCharacter.h"
#include "../Character/WRCharacterPlayer.h"




AWRPreviewStudio::AWRPreviewStudio()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* CustomRootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	if (CustomRootComponent)
	{
		CustomRootComponent->SetupAttachment(GetRootComponent());
		RootComponent = CustomRootComponent;
	}

	MeshParent = CreateDefaultSubobject<USceneComponent>("MeshParent");
	if (MeshParent)
	{
		MeshParent->SetupAttachment(GetRootComponent());
	}
	
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>("Scene");
	if (SceneComponent)
	{
		SceneComponent->SetupAttachment(GetRootComponent());

		if (SceneCapture == nullptr)
		{
			//USceneCaptureComponent2D* CaptureComponent = NewObject<USceneCaptureComponent2D>();
			//SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
			//SceneCapture->CaptureStereoPass = EStereoscopicPass::eSSP_FULL;//LEFT_EYE; //??
			//SceneCapture->bCaptureOnMovement = false;
			SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
			if (SceneCapture)
			{
				SceneCapture->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
				SceneCapture->SetRelativeLocation(FVector(0.f, 100.f, 0.f));
				SceneCapture->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
				SceneCapture->bCaptureEveryFrame = true;
				SceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
				//SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
				SceneCapture->FOVAngle = 45.f;
				FString TargetTexturePath = TEXT("/Game/Asset/BluePrint/Character/Preview/RT2D_Preview");
				SceneCapture->TextureTarget = LoadObject<UTextureRenderTarget2D>(NULL, *TargetTexturePath);
				SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

				SrcZoomRelativeLoc = SceneCapture->GetRelativeLocation();
			}
			//SceneCapture->RegisterComponent();

// 			const FName TargetName = MakeUniqueObjectName(this, UTextureRenderTarget2D::StaticClass(), TEXT("RT2D_Preview"));
// 			SceneCapture->TextureTarget = NewObject<UTextureRenderTarget2D>(this, TargetName);
// 			SceneCapture->TextureTarget->SizeX = 256;
// 			SceneCapture->TextureTarget->SizeY = 512;

// 			const FName TargetName = MakeUniqueObjectName(this, UMarerial::StaticClass(), TEXT("RT2D_Preview"));
// 			UKismetMaterialLibrary::CreateDynamicMaterialInstance()
// 			SceneCapture->TextureTarget = NewObject<UTextureRenderTarget2D>(this, TargetName);

			//SceneCapture->RegisterComponentWithWorld(GWorld);
			//SceneCapture->UpdateContent();
		}		
	}


	if (StaticMeshComponent == nullptr)
		StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	if (StaticMeshComponent)
	{
		StaticMeshComponent->AlwaysLoadOnClient = true;
		StaticMeshComponent->bOwnerNoSee = false;
		StaticMeshComponent->bCastDynamicShadow = true;
		StaticMeshComponent->bAffectDynamicIndirectLighting = true;
		StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		StaticMeshComponent->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
		StaticMeshComponent->SetRelativeLocation(FVector::ZeroVector);
		static FName MeshCollisionProfileName(TEXT("NoCollision"));
		StaticMeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
		StaticMeshComponent->SetGenerateOverlapEvents(false);
		StaticMeshComponent->SetCanEverAffectNavigation(false);

		StaticMeshComponent->SetEnableGravity(false);
	}


	if (SkeletalMeshComponent == nullptr)
		SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->AlwaysLoadOnClient = true;
		SkeletalMeshComponent->AlwaysLoadOnServer = true;
		SkeletalMeshComponent->bOwnerNoSee = false;
		SkeletalMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		SkeletalMeshComponent->bCastDynamicShadow = true;
		SkeletalMeshComponent->bAffectDynamicIndirectLighting = true;
		SkeletalMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		SkeletalMeshComponent->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
		SkeletalMeshComponent->SetRelativeLocation(FVector::ZeroVector);
		static FName MeshCollisionProfileName(TEXT("NoCollision"));
		SkeletalMeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
		SkeletalMeshComponent->SetGenerateOverlapEvents(false);
		SkeletalMeshComponent->SetCanEverAffectNavigation(false);

		SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		SkeletalMeshComponent->SetEnableGravity(false);
	}

	if (ChildActorComp == nullptr)
		ChildActorComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor"));

	if (ChildActorComp)
	{
		ChildActorComp->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
		ChildActorComp->SetRelativeLocation(FVector::ZeroVector);

// 		StaticMeshComponent->AlwaysLoadOnClient = true;
// 		StaticMeshComponent->bOwnerNoSee = false;
// 		StaticMeshComponent->bCastDynamicShadow = true;
// 		StaticMeshComponent->bAffectDynamicIndirectLighting = true;
// 		StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
// 		static FName MeshCollisionProfileName(TEXT("NoCollision"));
// 		StaticMeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
// 		StaticMeshComponent->SetGenerateOverlapEvents(false);
// 		StaticMeshComponent->SetCanEverAffectNavigation(false);
// 
// 		StaticMeshComponent->SetEnableGravity(false);
	}
}

void AWRPreviewStudio::UpdateLightChannel(class UMeshComponent* IN InMeshComp)
{
	if (InMeshComp)
	{		
		InMeshComp->LightingChannels.bChannel0 = LightChannel == 0;
		InMeshComp->LightingChannels.bChannel1 = LightChannel == 1;
		InMeshComp->LightingChannels.bChannel2 = LightChannel == 2;
		InMeshComp->MarkRenderStateDirty();
	}
}

void AWRPreviewStudio::UpdateLightChannel(AActor* IN InActor, EWREquipPart InPart)
{
	AWRCharacter* CustomCharacter = Cast<AWRCharacter>(InActor);
	if (CustomCharacter == nullptr)
		return;

	UWRActorComponentEquipmentLook* EquipLookComponent = CustomCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLookComponent)
	{
		UMeshComponent* EquippedComponent = EquipLookComponent->GetMesh(InPart);
		if (EquippedComponent)
		{
			UpdateLightChannel(EquippedComponent);
		}
	}
}

void AWRPreviewStudio::BeginPlay()
{
	Super::BeginPlay();	

	SetActorLocation(FVector(100000.f, 0.f, 0.f));	
}

void AWRPreviewStudio::BeginDestroy()
{
	auto DestroyComponentLambda = [&](USceneComponent* InSceneComponent)
	{
		if (InSceneComponent->IsValidLowLevel())
		{
			InSceneComponent->DestroyComponent();
			InSceneComponent = nullptr;
		}
	};

// 	DestroyComponentLambda(SkeletalMeshComponent);
// 	DestroyComponentLambda(StaticMeshComponent);
// 	DestroyComponentLambda(ChildActorComp);
// 	DestroyComponentLambda(MeshParent);
// 	DestroyComponentLambda(SceneCapture);

	Super::BeginDestroy();
}

void AWRPreviewStudio::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotate(DeltaTime);
	UpdateZoom(DeltaTime);
}

bool AWRPreviewStudio::OnInputStart(const FVector2D InCurrentLocation)
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	bRotate = true;
	InputStartLocation = InCurrentLocation;

	return true;
}

bool AWRPreviewStudio::OnInputMove(const FVector2D InCurrentLocation)
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	if (bRotate == false)
		return false;

	UMeshComponent* MeshComponent = GetMeshComponent();
	if (MeshComponent == nullptr)
		return false;

	// Diff Rotation
	FVector2D DiffLocation = InCurrentLocation - InputStartLocation;
	FRotator AddRotator = FRotator::ZeroRotator;
	AddRotator.Yaw = DiffLocation.X * -1.f;
	MeshParent->AddWorldRotation(AddRotator);

	InputStartLocation = InCurrentLocation;

	return true;
}

bool AWRPreviewStudio::OnInputEnd()
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	if (bRotate == false)
		return false;

	bRotate = false;
	InputStartLocation = FVector2D::ZeroVector;

	return true;
}

bool AWRPreviewStudio::OnRotateStart(const bool InLeft)
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	bRotate = true;
	bAuto = true;
	bLeft = InLeft;

	return true;
}

bool AWRPreviewStudio::OnRotateEnd()
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	if (bRotate == false)
		return false;

	bRotate = false;
	bAuto = false;

	return true;
}

bool AWRPreviewStudio::OnZoomIn()
{
	if (PreviewInfo.GetPreviewData().UseZoom == false)
		return false;

// 	if (bZoomIn == false)
// 	{
// 		FVector vZoomDirection = GetZoomDirection();
// 	}

	bZoomIn = true;
	bZoomOut = false;

	return true;
}

bool AWRPreviewStudio::OnZoomOut()
{
	if (PreviewInfo.GetPreviewData().UseZoom == false)
		return false;

// 	if (bZoomOut == false)
// 	{
// 		FVector vZoomDirection = GetZoomDirection();
// 	}

	bZoomIn = false;
	bZoomOut = true;

	return true;
}

bool AWRPreviewStudio::OnZoomEnd()
{
	if (PreviewInfo.GetPreviewData().UseZoom == false)
		return false;

	if (bZoomIn != false && bZoomOut != false)
		return false;

//	ResetZoomDirection();

	bZoomIn = false;
	bZoomOut = false;

	return true;
}

void AWRPreviewStudio::SetPreviewTarget(const FWRPreviewInfo& IN InPreivewInfo)
{
	PreviewInfo = InPreivewInfo;

	switch (InPreivewInfo.Type)
	{
	case EWRPreviewType::Static:
	{
		// Target Async Lambda
		auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
		{
			strAsyncKey.Empty();

			UStaticMesh* CastedObject = Cast<UStaticMesh>(InObject);
			if (CastedObject == nullptr)
				return;

			SetStaticMesh(CastedObject);		
		});

		strAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InPreivewInfo.BPPath, AsyncCreateComplete);
	}
	break;

	case EWRPreviewType::Skeletal:
	{
		// Target Async Lambda
		auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
		{
			strAsyncKey.Empty();

			USkeletalMesh* CastedObject = Cast<USkeletalMesh>(InObject);
			if (CastedObject == nullptr)
				return;

			SetSkeletalMesh(CastedObject, nullptr);
		});

		strAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InPreivewInfo.BPPath, AsyncCreateComplete);
	}
	break;

	case EWRPreviewType::DefaultActor:
	{
		// Target Async Lambda
		auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
		{
			strAsyncKey.Empty();

			SetDefaultActor((UClass*)InObject, nullptr);
		});

		// [ 2020-6-9 : kyu ] 경로는 _C로 변환 후 LoadAsset해야 정상적으로 BP가 불러와진다.
		FString ResolvedFullPath = WRActorSpawnUtility::ResolvePath(InPreivewInfo.BPPath);
		strAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(ResolvedFullPath, AsyncCreateComplete);
	}
	break;

	case EWRPreviewType::CustomActor:
	{
		// Target Async Lambda
		auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
		{
			strAsyncKey.Empty();

 			AActor* CastedObject = Cast<AActor>(InObject);
 			if (CastedObject == nullptr)
 				return;
 
 			SetCustomActor(CastedObject, nullptr);			
		});

		FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InPreivewInfo.GetPreviewCustomActorData().PresetTID);
		if (PresetTable == nullptr)
			return;

		// 외부에서 가져온 PresetTID에서 BP경로를 가져온다
		PreviewInfo.BPPath = PresetTable->PresetBlueprint;

		// [ 2020-6-9 : kyu ] Spawn으로 하고 ChildActor에 넣으면 SpawnedActor가 앞에 남는다
		strAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(PreviewInfo.BPPath, AsyncCreateComplete);
	}
	break;
	}
}

void AWRPreviewStudio::OnPreviewTargetSetted()
{
	if (SceneCapture)
	{
		FVector2D CustomSceneSize = PreviewInfo.GetPreviewData().SceneSize;

		// Set
		if (SceneCapture->TextureTarget->SizeX != CustomSceneSize.X || SceneCapture->TextureTarget->SizeY != CustomSceneSize.Y)
		{
			SceneCapture->TextureTarget->SizeX = CustomSceneSize.X;
			SceneCapture->TextureTarget->SizeY = CustomSceneSize.Y;
			SceneCapture->TextureTarget->UpdateResourceImmediate();
		}

		// SceneCapture Location Init
		SceneCapture->SetRelativeLocation(GetSrcCameraOffset());

// 		SceneCapture->ShowOnlyActors.Empty();
// 		SceneCapture->ShowOnlyActors.Add(this);
// 		SceneCapture->UpdateContent();
// 		SceneCapture->CaptureScene();
	}

	// Transform Offset
	UMeshComponent* MeshComponent = GetMeshComponent();
	if (MeshComponent)
	{
		if (PreviewInfo.GetPreviewData().ActorLocationOffset.IsZero() == false)
		{
			MeshComponent->AddWorldOffset(PreviewInfo.GetPreviewData().ActorLocationOffset);
		}
		if (PreviewInfo.GetPreviewData().ActorRotationOffset.IsZero() == false)
		{
			MeshComponent->AddWorldRotation(PreviewInfo.GetPreviewData().ActorRotationOffset);
		}		
	}
}

void AWRPreviewStudio::SetStaticMesh(class UStaticMesh* InMesh)
{
	if (StaticMeshComponent == nullptr)
		return;

	StaticMeshComponent->SetStaticMesh(InMesh);

	UpdateScale();
	// [ 2020-9-10 : kyu ] 라이트 채널 변경
	UpdateLightChannel(StaticMeshComponent);
	OnPreviewTargetSetted();

	SceneCapture->ClearShowOnlyComponents();
	SceneCapture->ShowOnlyComponent(StaticMeshComponent);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();
}

void AWRPreviewStudio::SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim)
{
	if (SkeletalMeshComponent == nullptr)
		return;

	SkeletalMeshComponent->SetSkeletalMesh(InMesh);
	SkeletalMeshComponent->PlayAnimation(InAnim, true);
		
	UpdateScale();
	// [ 2020-9-10 : kyu ] 라이트 채널 변경
	UpdateLightChannel(SkeletalMeshComponent);
	OnPreviewTargetSetted();

	SceneCapture->ClearShowOnlyComponents();
	SceneCapture->ShowOnlyComponent(SkeletalMeshComponent);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();
}

void AWRPreviewStudio::SetCustomActor(class AActor* InActor, class UAnimationAsset* InAnim)
{
	if (InActor == nullptr)
		return;

	CustomActor = InActor;
	CustomActor->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
	CustomActor->SetActorRelativeLocation(FVector::ZeroVector);

	TArray<USkeletalMeshComponent*> SkeletalMeshComps;
	CustomActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
	if (SkeletalMeshComps.Num() > 0)
	{
		if (SkeletalMeshComps[0]->IsValidLowLevel())
		{				
			SkeletalMeshComps[0]->SetEnableGravity(false);

			if (InAnim)
			{
				SkeletalMeshComps[0]->SetAnimationMode(EAnimationMode::AnimationSingleNode);
				SkeletalMeshComps[0]->PlayAnimation(InAnim, true);
			}
		}
	}

	ACharacter* CastedCharacter = Cast<ACharacter>(CustomActor);
	if (CastedCharacter)
	{
		CastedCharacter->GetCharacterMovement()->GravityScale = 0.f;
		CastedCharacter->GetCapsuleComponent()->SetEnableGravity(false);		
	}	

	//====================================================================================
	// [ 2020-8-5 : kyu ] 외형데이터 추가
	AWRCharacter* CustomCharacter = Cast<AWRCharacter>(CustomActor);
	if (CustomCharacter == nullptr)
		return;

	const FWRPreviewCustomActorData& CustomActorData = PreviewInfo.GetPreviewCustomActorData();

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(CustomActorData.PresetTID);
	if (PresetTable == nullptr)
		return;

	CustomCharacter->CreateComponent<UWRActorComponentEquipmentLook>(EWRCharacterComponent::EquipmentLook);
	UWRActorComponentEquipmentLook* EquipLook = CustomCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLook)
	{
		for (int32 i = 0; i < (int32)EWREquipPart::Max; ++i)
		{
			EquipLook->Unequip((EWREquipPart)i);
		}

		for (auto& PresetDefaultPart : PresetTable->PresetDefaultParts)
		{
			// [ 2020-9-14 : kyu ] 기본외형 장착시에도 LightChannel 변경대응하기
			EquipLook->Equip(PresetDefaultPart.Value, NAME_None, FWREquipmentComplete::CreateUObject(this, &AWRPreviewStudio::UpdateLightChannel));
		}
	}

	CustomCharacter->SetCharacterTableID(PresetTable->CharTableID);
	CustomCharacter->OnPostSpawn();

	if (CustomCharacter->IsPlayer())
	{
		// Player일 경우 머리 회전 끔
		AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(CustomCharacter);
		if (CastedPlayer)
		{
			CastedPlayer->SetUsingSyncHMDHead(false);
		}
	}

	// Base Equip Look
	UWRActorComponentEquipmentLook* EquipLookComponent = CustomCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLookComponent)
	{
		// CustomActor의 경우 Mesh셋업 시 Async인지라 별도 완료 델리게이트를 받아야한다.
		auto EquipCompleteDelegate = FWREquipmentComplete::CreateLambda([=](AActor* InActor, EWREquipPart InPart)
		{
			bIsCompleted = true;
			UpdateScale();
			OnPreviewTargetSetted();

			SceneCapture->ShowOnlyActors.Empty();
			SceneCapture->ShowOnlyActors.Add(CustomActor);
			SceneCapture->UpdateContent();
			SceneCapture->CaptureScene();

			// Costume
			// [ 2020-9-14 : kyu ] EquipPart가 비동기로드라서 비동기로드가 완료 시 일일이 호출하여 LightChannel을 변경해준다.
			CustomCharacter->SetEquippedCostume(CustomActorData.CostumeList, FWREquipmentComplete::CreateUObject(this, &AWRPreviewStudio::UpdateLightChannel));

			// Weapon
			CustomCharacter->SetEquippedWeaponItemTID(CustomActorData.LeftWeaponTID, CustomActorData.RightWeaponTID, CustomActorData.TemporaryTID);

			// 커스텀액터의 프리뷰가 셋팅 완료됨을 알려야한다
			PreviewInfo.PreviewLoadCompletedDelegate.ExecuteIfBound(CustomActor);

			// [ 2020-9-10 : kyu ] 라이트 채널 변경
			TArray<USkeletalMeshComponent*> SkeletalMeshComps;
			CustomActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
			for (auto skeletalcomp : SkeletalMeshComps)
			{
				UpdateLightChannel(skeletalcomp);
			}

			// 커스터마이징 헤어 적용(다른파츠 추가 변경 필요)
			if (CustomActorData.CustomizeInfo.customize.empty() == false)
			{
				WRTableID EquipLookTID = std::stoi(CustomActorData.CustomizeInfo.customize);
				EquipLook->Equip(EquipLookTID, NAME_None, nullptr);
			}
		});
		EquipLookComponent->Equip(PresetTable->NormalEquipmentLookTID, NAME_None, EquipCompleteDelegate);
	}
}

void AWRPreviewStudio::AddActorInShowList(AActor* IN InActor)
{
	SceneCapture->ShowOnlyActors.Add(InActor);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();
}

void AWRPreviewStudio::SetDefaultActor(class UClass* InActorClass, class UAnimationAsset* InAnim)
{
	if (ChildActorComp == nullptr)
		return;

	ChildActorComp->SetChildActorClass(InActorClass);
	//ChildActorComp->CreateChildActor();
	AActor* pChildActor = ChildActorComp->GetChildActor();
	if (pChildActor == nullptr)
		return;

	// Location Offset
	if (PreviewInfo.GetPreviewData().ActorLocationOffset.IsZero() == false)
	{
		ChildActorComp->SetRelativeLocation(PreviewInfo.GetPreviewData().ActorLocationOffset);
	}
	// Rotation Offset
	if (PreviewInfo.GetPreviewData().ActorRotationOffset.IsZero() == false)
	{
		ChildActorComp->SetRelativeRotation(PreviewInfo.GetPreviewData().ActorRotationOffset);
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshComps;
	pChildActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
	if (SkeletalMeshComps.Num() > 0)
	{
		if (SkeletalMeshComps[0]->IsValidLowLevel())
		{
			SkeletalMeshComps[0]->SetEnableGravity(false);

			if (InAnim)
			{
				SkeletalMeshComps[0]->SetAnimationMode(EAnimationMode::AnimationSingleNode);
				SkeletalMeshComps[0]->PlayAnimation(InAnim, true);
			}

			// [ 2020-9-10 : kyu ] 라이트 채널 변경
			UpdateLightChannel(SkeletalMeshComps[0]);
		}
	}

	ACharacter* CastedCharacter = Cast<ACharacter>(pChildActor);
	if (CastedCharacter)
	{
		CastedCharacter->GetCharacterMovement()->GravityScale = 0.f;
		CastedCharacter->GetCapsuleComponent()->SetEnableGravity(false);
	}

	UpdateScale();
	OnPreviewTargetSetted();

	SceneCapture->ShowOnlyActors.Empty();
	SceneCapture->ShowOnlyActors.Add(pChildActor);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();
}

AActor* AWRPreviewStudio::GetDefaultActor()
{
	if (ChildActorComp == nullptr)
		return nullptr;

	return ChildActorComp->GetChildActor();
}

AActor* AWRPreviewStudio::GetCustomActor()
{
	return CustomActor;
}

void AWRPreviewStudio::UpdateScale()
{	
	if (PreviewInfo.AutoScaling == false)
		return;

	UMeshComponent* MeshComponent = GetMeshComponent();
	if (MeshComponent == nullptr)
		return;

	// Set Current Extend Mesh
	FVector PreMeshLocation = MeshComponent->GetComponentLocation();
	FVector PreOrigin;
	//FVector Origin;
	FVector BoxExtend;
	float SphereRadius;
	UKismetSystemLibrary::GetComponentBounds(MeshComponent, PreOrigin, BoxExtend, SphereRadius);
	CurrentExtendMesh = BoxExtend;

	FVector CurrentMeshScale = MeshComponent->GetComponentScale();
	// Modify Mesh Scale
	float RatioX = DesiredExtendMesh.X / CurrentExtendMesh.X; //(CurrentExtendMesh.X * CurrentMeshScale.X);	// 이건 스케일 먹이면 안될듯
	float RatioY = DesiredExtendMesh.Y / CurrentExtendMesh.Y; //(CurrentExtendMesh.Y * CurrentMeshScale.Y);
	float RatioZ = DesiredExtendMesh.Z / CurrentExtendMesh.Z; //(CurrentExtendMesh.Z * CurrentMeshScale.Z);
	float MulScale = FMath::Min3(RatioX, RatioY, RatioZ);

	FVector ModifiedMeshScale = CurrentMeshScale * MulScale;
	MeshComponent->SetWorldScale3D(ModifiedMeshScale);

	// Modify Location
	FVector CurrentMeshLocation = MeshComponent->GetComponentLocation();
	FVector Origin;
	UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtend, SphereRadius);

	MeshComponent->SetRelativeLocation(FVector::ZeroVector);

	FVector ModifiedMeshLocationOffset = CurrentMeshLocation - Origin;
	//FVector ModifiedMeshLocationOffset = PreOrigin - Origin;	// 이건 값이 맞지 않음
	MeshComponent->AddLocalOffset(ModifiedMeshLocationOffset);
}

void AWRPreviewStudio::UpdateRotate(const float IN InDeltaTime)
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return;

	if (bAuto == false)
		return;

	if (bRotate == false)
		return;

	UMeshComponent* MeshComponent = GetMeshComponent();
	if (MeshComponent == nullptr)
		return;

	FRotator AddRotator = FRotator::ZeroRotator;
	if (bLeft)
	{
		AddRotator.Yaw = PreviewInfo.GetPreviewData().ActorRotationValue;
	}
	else
	{
		AddRotator.Yaw = PreviewInfo.GetPreviewData().ActorRotationValue * -1.f;
	}

	// [ 2020-6-10 : kyu ] MeshComponent가 아닌 Parent로 써봄
	MeshParent->AddWorldRotation(AddRotator);
}

void AWRPreviewStudio::UpdateZoom(const float IN InDeltaTime)
{
	if (bZoomIn)
	{
		if (AccumulatedZoom >= PreviewInfo.GetPreviewData().ZoomMax)
			return;
			   
		AccumulatedZoom += 1.f;
		
		FVector vZoomDirection = GetZoomDirection();
		FVector vDstLocation = GetSrcCameraOffset() + (vZoomDirection * AccumulatedZoom);
		SceneCapture->SetRelativeLocation(vDstLocation);
	}
	else if (bZoomOut)
	{
		if (AccumulatedZoom <= 0.f)
			return;
		
		AccumulatedZoom -= 1.f;

		if (AccumulatedZoom <= 0.f)
		{
			AccumulatedZoom = 0.f;
			SceneCapture->SetRelativeLocation(GetSrcCameraOffset());			
		}
		else
		{
			FVector vZoomDirection = GetZoomDirection();
			FVector vDstLocation = GetSrcCameraOffset() + (vZoomDirection * AccumulatedZoom);
			SceneCapture->SetRelativeLocation(vDstLocation);
		}
	}
}

FVector AWRPreviewStudio::GetSrcCameraOffset()
{	
	FVector vResult = SrcZoomRelativeLoc + PreviewInfo.GetPreviewData().CameraOffset;

	FName CameraOffsetPivotSocket = PreviewInfo.GetPreviewData().CameraOffsetPivotSocket;
	if (CameraOffsetPivotSocket.IsNone() == false)
	{
		USkeletalMeshComponent* CastedMeshComponent = Cast<USkeletalMeshComponent>(GetMeshComponent());
		if (CastedMeshComponent != nullptr)
		{
			FTransform SocketTransform = CastedMeshComponent->GetSocketTransform(CameraOffsetPivotSocket, ERelativeTransformSpace::RTS_Actor);
			FVector vSocketLocation = SocketTransform.GetLocation();

			vResult += vSocketLocation;
		}
	}

	return vResult;
}

FVector AWRPreviewStudio::GetZoomDirection()
{
// 	if (ZoomDirection.IsZero() == false)
// 		return ZoomDirection;

	FVector vZoomDirection = FVector::ZeroVector;
	if (PreviewInfo.GetPreviewData().ZoomSocket.IsNone())
	{
		vZoomDirection = SceneCapture->GetForwardVector().GetSafeNormal();
	}
	else
	{
		UMeshComponent* MeshComponent = GetMeshComponent();
		if (MeshComponent->DoesSocketExist(PreviewInfo.GetPreviewData().ZoomSocket))
		{
			FVector vSocketLocation = MeshComponent->GetSocketLocation(PreviewInfo.GetPreviewData().ZoomSocket);
			FVector vSceneCaptureCompLoc = SceneCapture->GetComponentLocation();
			vZoomDirection = vSocketLocation - vSceneCaptureCompLoc;
			vZoomDirection.Normalize();
		}
		else
		{
			vZoomDirection = SceneCapture->GetForwardVector().GetSafeNormal();
		}
	}
	return vZoomDirection;
}

void AWRPreviewStudio::ResetZoomDirection()
{
	ZoomDirection = FVector::ZeroVector;
}

class UMeshComponent* AWRPreviewStudio::GetMeshComponent()
{
	switch (PreviewInfo.Type)
	{
	case EWRPreviewType::Static:
	{
		return StaticMeshComponent;
	}
	break;

	case EWRPreviewType::Skeletal:
	{
		return SkeletalMeshComponent;
	}
	break;

	case EWRPreviewType::DefaultActor:
	{
		if (ChildActorComp == nullptr)
			return nullptr;

		AActor* pChildActor = ChildActorComp->GetChildActor();
		if (pChildActor == nullptr)
			return nullptr;

		TArray<USkeletalMeshComponent*> SkeletalMeshComps;
		pChildActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
		if (SkeletalMeshComps.Num() > 0)
		{
			if (SkeletalMeshComps[0]->SkeletalMesh->IsValidLowLevel())
				return SkeletalMeshComps[0];
		}

		TArray<UStaticMeshComponent*> StaticMeshComps;
		pChildActor->GetComponents<UStaticMeshComponent>(StaticMeshComps);
		if (StaticMeshComps.Num() > 0)
		{
			if (StaticMeshComps[0]->GetStaticMesh()->IsValidLowLevel())
				return StaticMeshComps[0];
		}
	}
	break;

	case EWRPreviewType::CustomActor:
	{
		if (CustomActor == nullptr)
			return nullptr;

		TArray<USkeletalMeshComponent*> SkeletalMeshComps;
		CustomActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
		if (SkeletalMeshComps.Num() > 0)
		{
			if (SkeletalMeshComps[0]->SkeletalMesh->IsValidLowLevel())
				return SkeletalMeshComps[0];
		}

		TArray<UStaticMeshComponent*> StaticMeshComps;
		CustomActor->GetComponents<UStaticMeshComponent>(StaticMeshComps);
		if (StaticMeshComps.Num() > 0)
		{
			if (StaticMeshComps[0]->GetStaticMesh()->IsValidLowLevel())
				return StaticMeshComps[0];
		}
	}
	break;
	}

	return nullptr;
}

void AWRPreviewStudio::Release()
{
	if (strAsyncKey.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(strAsyncKey);
	}
	if (CustomActor != nullptr && CustomActor->IsValidLowLevel())
	{
		CustomActor->RemoveFromRoot();
		
		AWRCharacter* CastedCustomActor = Cast<AWRCharacter>(CustomActor);
		if (CastedCustomActor)
		{
			CastedCustomActor->OnPreDestroy();
		}

		CustomActor->Destroy();
		CustomActor = nullptr;
	}
}

