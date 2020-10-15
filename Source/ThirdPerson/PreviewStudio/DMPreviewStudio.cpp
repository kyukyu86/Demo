// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPreviewStudio.h"

#include "Math/UnrealMathUtility.h"

#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LightComponent.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../UI/Slot/DMUISlot_Preview.h"
#include "../Manager/DMAsyncLoadManager.h"
#include "../Util/DMActorUtil.h"




ADMPreviewStudio::ADMPreviewStudio()
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

void ADMPreviewStudio::UpdateLightChannel(class UMeshComponent* IN InMeshComp)
{
	if (InMeshComp)
	{
		InMeshComp->LightingChannels.bChannel0 = LightChannel == 0;
		InMeshComp->LightingChannels.bChannel1 = LightChannel == 1;
		InMeshComp->LightingChannels.bChannel2 = LightChannel == 2;
		InMeshComp->MarkRenderStateDirty();
	}
}

void ADMPreviewStudio::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(100000.f, 0.f, 0.f));
}

void ADMPreviewStudio::BeginDestroy()
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

void ADMPreviewStudio::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotate(DeltaTime);
	UpdateZoom(DeltaTime);
}

bool ADMPreviewStudio::OnInputStart(const FVector2D InCurrentLocation)
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	bRotate = true;
	InputStartLocation = InCurrentLocation;

	return true;
}

bool ADMPreviewStudio::OnInputMove(const FVector2D InCurrentLocation)
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

bool ADMPreviewStudio::OnInputEnd()
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	if (bRotate == false)
		return false;

	bRotate = false;
	InputStartLocation = FVector2D::ZeroVector;

	return true;
}

bool ADMPreviewStudio::OnRotateStart(const bool InLeft)
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	bRotate = true;
	bAuto = true;
	bLeft = InLeft;

	return true;
}

bool ADMPreviewStudio::OnRotateEnd()
{
	if (PreviewInfo.GetPreviewData().UseRotate == false)
		return false;

	if (bRotate == false)
		return false;

	bRotate = false;
	bAuto = false;

	return true;
}

bool ADMPreviewStudio::OnZoomIn()
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

bool ADMPreviewStudio::OnZoomOut()
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

bool ADMPreviewStudio::OnZoomEnd()
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

void ADMPreviewStudio::SetPreviewTarget(const FDMPreviewInfo& IN InPreivewInfo)
{
	PreviewInfo = InPreivewInfo;

	switch (InPreivewInfo.Type)
	{
	case EDMPreviewType::Static:
	{
		// Target Async Lambda
		auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
		{
			strAsyncKey.Empty();

			UStaticMesh* CastedObject = Cast<UStaticMesh>(InObject);
			if (CastedObject == nullptr)
				return;

			SetStaticMesh(CastedObject);
		});

		strAsyncKey = DMAsyncLoadManager::Get()->AsyncLoadAsset(InPreivewInfo.BPPath, AsyncCreateComplete);
	}
	break;

	case EDMPreviewType::Skeletal:
	{
		// Target Async Lambda
		auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
		{
			strAsyncKey.Empty();

			USkeletalMesh* CastedObject = Cast<USkeletalMesh>(InObject);
			if (CastedObject == nullptr)
				return;

			SetSkeletalMesh(CastedObject, nullptr);
		});

		strAsyncKey = DMAsyncLoadManager::Get()->AsyncLoadAsset(InPreivewInfo.BPPath, AsyncCreateComplete);
	}
	break;

	case EDMPreviewType::DefaultActor:
	{
		// Target Async Lambda
 		auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
 		{
			strAsyncKey.Empty();
 
 			SetDefaultActor((UClass*)InObject, nullptr);
 		});
 
 		// [ 2020-6-9 : kyu ] 경로는 _C로 변환 후 LoadAsset해야 정상적으로 BP가 불러와진다.
 		FString ResolvedFullPath = DMActorUtil::ResolvePath(InPreivewInfo.BPPath);
 		strAsyncKey = DMAsyncLoadManager::Get()->AsyncLoadAsset(ResolvedFullPath, AsyncCreateComplete);
	}
	break;

	case EDMPreviewType::CustomActor:
	{
		// Target Async Lambda
 		auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
 		{
			strAsyncKey.Empty();
 
 			AActor* CastedObject = Cast<AActor>(InObject);
 			if (CastedObject == nullptr)
 				return;
 
 			SetCustomActor(CastedObject, nullptr);
 		});
 
 		// [ 2020-6-9 : kyu ] Spawn으로 하고 ChildActor에 넣으면 SpawnedActor가 앞에 남는다
 		strAsyncKey = DMAsyncLoadManager::Get()->AsyncSpawnActor(InPreivewInfo.BPPath, AsyncCreateComplete);
	}
	break;
	}
}

void ADMPreviewStudio::OnPreviewTargetSetted()
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

	// Rotate Init
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

void ADMPreviewStudio::SetStaticMesh(class UStaticMesh* InMesh)
{
	if (StaticMeshComponent == nullptr)
		return;

	StaticMeshComponent->SetStaticMesh(InMesh);

// 	StaticMeshComponent->LightingChannels.bChannel0 = false;
// 	StaticMeshComponent->LightingChannels.bChannel1 = false;
// 	StaticMeshComponent->LightingChannels.bChannel2 = true;
// 	StaticMeshComponent->MarkRenderStateDirty();

	UpdateScale();
	OnPreviewTargetSetted();

	SceneCapture->ClearShowOnlyComponents();
	SceneCapture->ShowOnlyComponent(StaticMeshComponent);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();
}

void ADMPreviewStudio::SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim)
{
	if (SkeletalMeshComponent == nullptr)
		return;

	SkeletalMeshComponent->SetSkeletalMesh(InMesh);
	SkeletalMeshComponent->PlayAnimation(InAnim, true);

	UpdateScale();
	OnPreviewTargetSetted();

	SceneCapture->ClearShowOnlyComponents();
	SceneCapture->ShowOnlyComponent(SkeletalMeshComponent);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();
}

void ADMPreviewStudio::SetCustomActor(class AActor* InActor, class UAnimationAsset* InAnim)
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

	UpdateScale();
	OnPreviewTargetSetted();

	SceneCapture->ShowOnlyActors.Empty();
	SceneCapture->ShowOnlyActors.Add(CustomActor);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();

	// + 외형정보 설정

	const FDMPreviewCustomActorData& CustomActorData = PreviewInfo.GetPreviewCustomActorData();

	// 모두 설정완료 후 완료됨 알림
	PreviewInfo.PreviewLoadCompletedDelegate.ExecuteIfBound(CustomActor);
}

void ADMPreviewStudio::SetDefaultActor(class UClass* InActorClass, class UAnimationAsset* InAnim)
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

AActor* ADMPreviewStudio::GetDefaultActor()
{
	if (ChildActorComp == nullptr)
		return nullptr;

	return ChildActorComp->GetChildActor();
}

AActor* ADMPreviewStudio::GetCustomActor()
{
	return CustomActor;
}

void ADMPreviewStudio::AddActorInShowList(AActor* IN InActor)
{
	SceneCapture->ShowOnlyActors.Add(InActor);
	SceneCapture->UpdateContent();
	SceneCapture->CaptureScene();
}

void ADMPreviewStudio::UpdateScale()
{
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

void ADMPreviewStudio::UpdateRotate(const float IN InDeltaTime)
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

void ADMPreviewStudio::UpdateZoom(const float IN InDeltaTime)
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

FVector ADMPreviewStudio::GetSrcCameraOffset()
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

FVector ADMPreviewStudio::GetZoomDirection()
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

void ADMPreviewStudio::ResetZoomDirection()
{
	ZoomDirection = FVector::ZeroVector;
}

class UMeshComponent* ADMPreviewStudio::GetMeshComponent()
{
	switch (PreviewInfo.Type)
	{
	case EDMPreviewType::Static:
	{
		return StaticMeshComponent;
	}
	break;

	case EDMPreviewType::Skeletal:
	{
		return SkeletalMeshComponent;
	}
	break;

	case EDMPreviewType::DefaultActor:
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

	case EDMPreviewType::CustomActor:
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

void ADMPreviewStudio::Release()
{
	if (strAsyncKey.IsEmpty() == false)
	{
		DMAsyncLoadManager::Get()->CancelAsyncLoad(strAsyncKey);
	}
	if (CustomActor != nullptr && CustomActor->IsValidLowLevel())
	{
		CustomActor->RemoveFromRoot();

// 		AWRCharacter* CastedCustomActor = Cast<AWRCharacter>(CustomActor);
// 		if (CastedCustomActor)
// 		{
// 			CastedCustomActor->OnPreDestroy();
// 		}

		CustomActor->Destroy();
		CustomActor = nullptr;
	}
}

