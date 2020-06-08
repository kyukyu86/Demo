// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPreviewActor.h"
#include "Components/MeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMaterialLibrary.h"

ADMPreviewActor::ADMPreviewActor()
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
				FString TargetTexturePath = TEXT("/Game/StarterContent/Blueprints/Assets/Actor/PreviewActor/RT2D_Preview.RT2D_Preview");
				SceneCapture->TextureTarget = LoadObject<UTextureRenderTarget2D>(NULL, *TargetTexturePath);
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
}

void ADMPreviewActor::BeginPlay()
{
	Super::BeginPlay();	
	
// 	if (SceneCapture->IsValidLowLevel())
// 	{
// 		SceneCapture->DestroyComponent();
// 		SceneCapture = nullptr;
// 	}
}

void ADMPreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADMPreviewActor::SetMesh(const EDMPreviewType InType, UObject* InObject, class UAnimationAsset* InAnim /* = nullptr */)
{
	switch (InType)
	{
	case EDMPreviewType::Static:
	{
		UStaticMesh* CastedObject = Cast<UStaticMesh>(InObject);
		if (CastedObject == nullptr)
			return;

		SetStaticMesh(CastedObject);
	}
	break;
	case EDMPreviewType::Dynamic:
	{
		USkeletalMesh* CastedObject = Cast<USkeletalMesh>(InObject);
		if (CastedObject == nullptr)
			return;

		SetSkeletalMesh(CastedObject, InAnim);
	}
	break;
	case EDMPreviewType::CustomActor:
	{
		AActor* CastedObject = Cast<AActor>(InObject);
		if (CastedObject == nullptr)
			return;

		SetCustomActor(CastedObject, InAnim);
	}
	break;
	}
}

void ADMPreviewActor::OnInputStart(const FVector InCurrentLocation)
{
	bRotate = true;
	InputStartLocation = InCurrentLocation;
}

void ADMPreviewActor::OnInputMove(const FVector InCurrentLocation)
{
	if (bRotate == false)
		return;

	UMeshComponent* MeshComponent = GetMeshComponent();
	if (MeshComponent == nullptr)
		return;

	// Diff Rotation
	FVector DiffLocation = InCurrentLocation - InputStartLocation;
	FRotator AddRotator = FRotator::ZeroRotator;
	AddRotator.Yaw = DiffLocation.X * -1.f;
	MeshParent->AddWorldRotation(AddRotator);

	InputStartLocation = InCurrentLocation;
}

void ADMPreviewActor::OnInputEnd()
{
	bRotate = false;
	InputStartLocation = FVector::ZeroVector;
}

void ADMPreviewActor::UpdateScale()
{	
	UMeshComponent* MeshComponent = GetMeshComponent();
	if (MeshComponent == nullptr)
		return;

	// Set Current Extend Mesh
	FVector Origin;
	FVector BoxExtend;
	float SphereRadius;
	UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtend, SphereRadius);
	CurrentExtendMesh = BoxExtend;

	// Modify Mesh Scale
	float RatioX = DesiredExtendMesh.X / CurrentExtendMesh.X;
	float RatioY = DesiredExtendMesh.Y / CurrentExtendMesh.Y;
	float RatioZ = DesiredExtendMesh.Z / CurrentExtendMesh.Z;
	float MulScale = FMath::Min3(RatioX, RatioY, RatioZ);

	FVector CurrentMeshScale = MeshComponent->GetComponentScale();
	FVector ModifiedMeshScale = CurrentMeshScale * MulScale;
	MeshComponent->SetWorldScale3D(ModifiedMeshScale);

	// Modify Location
	FVector CurrentMeshLocation = MeshComponent->GetComponentLocation();
	UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtend, SphereRadius);
	FVector ModifiedMeshLocationOffset = CurrentMeshLocation - Origin;
	MeshComponent->AddLocalOffset(ModifiedMeshLocationOffset);
}