// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPreviewActor.h"
#include "Components/MeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"

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
}

void ADMPreviewActor::BeginPlay()
{
	Super::BeginPlay();	
	
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
	float MaxRatio = FMath::Min3(RatioX, RatioY, RatioZ);

	FVector CurrentMeshScale = MeshComponent->GetComponentScale();
	FVector ModifiedMeshScale = CurrentMeshScale * MaxRatio;
	MeshComponent->SetWorldScale3D(ModifiedMeshScale);

	// Modify Location
	FVector CurrentMeshLocation = MeshComponent->GetComponentLocation();
	UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtend, SphereRadius);
	FVector ModifiedMeshLocationOffset = CurrentMeshLocation - Origin;
	MeshComponent->AddLocalOffset(ModifiedMeshLocationOffset);
}