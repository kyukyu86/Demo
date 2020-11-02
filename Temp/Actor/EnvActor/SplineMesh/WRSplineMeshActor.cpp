// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSplineMeshActor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

void AWRSplineMeshActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	this->InitializeSplineMesh();
}

void AWRSplineMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

void AWRSplineMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWRSplineMeshActor::BeginDestroy()
{
	Super::BeginDestroy();
}

void AWRSplineMeshActor::InitializeSplineMesh()
{
	this->ClearAllSplineMesh();

	InRootSplineComponent = Cast<USplineComponent>(this->GetRootComponent());
	if (InRootSplineComponent == nullptr)
		return;
	InRootSplineComponent->SetMobility(EComponentMobility::Movable);

	int32 NumOfSplinePoint = InRootSplineComponent->GetNumberOfSplinePoints();

	for (int32 InIndex = 0; InIndex < NumOfSplinePoint - 1; InIndex++)
	{
		FVector StartPos, StartTangent, EndPos, EndTangent;
		InRootSplineComponent->GetLocationAndTangentAtSplinePoint(InIndex, StartPos, StartTangent, ESplineCoordinateSpace::Local);
		InRootSplineComponent->GetLocationAndTangentAtSplinePoint(InIndex + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

		USplineMeshComponent* InSplineMeshComponent = NewObject<USplineMeshComponent>(InRootSplineComponent);
		if (InSplineMeshComponent == nullptr)
			continue;

		InSplineMeshComponent->SetWorldTransform(this->GetActorTransform());

		InSplineMeshComponent->SetMobility(EComponentMobility::Movable);
		InSplineMeshComponent->WeldTo(InRootSplineComponent);

		InSplineMeshComponent->SetCollisionProfileName(FName("WREnvObject"));

		InSplineMeshComponent->SetStaticMesh(InStaticMesh);
		InSplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InSplineMeshComponent->SetCastShadow(false);
		InSplineMeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);

		InSplineMeshComponent->SetTranslucentSortPriority(TranslucencySortPriority);

		InSplineMeshComponent->RegisterComponent();
		InSplineMeshComps.Add(InSplineMeshComponent);

		USplineMeshComponent* InPhysicsMeshComponent = NewObject<USplineMeshComponent>(InRootSplineComponent);
		if (InSplineMeshComponent == nullptr)
			continue;

		InPhysicsMeshComponent->SetWorldTransform(this->GetActorTransform());

		InPhysicsMeshComponent->SetMobility(EComponentMobility::Movable);
		InPhysicsMeshComponent->WeldTo(InRootSplineComponent);

		InPhysicsMeshComponent->SetCollisionProfileName(FName("WREnvObject"));

		InPhysicsMeshComponent->SetStaticMesh(InPhysicsMesh);
		InPhysicsMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		InPhysicsMeshComponent->SetCastShadow(false);
		InPhysicsMeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);

		InPhysicsMeshComponent->SetTranslucentSortPriority(TranslucencySortPriority);

		InPhysicsMeshComponent->SetVisibility(DrawPhysicsMesh);

		InPhysicsMeshComponent->RegisterComponent();
		InSplineMeshComps.Add(InPhysicsMeshComponent);
	}
}

void AWRSplineMeshActor::ClearAllSplineMesh()
{
	for (USplineMeshComponent* It : InSplineMeshComps)
	{
		if (It == nullptr)
			return;

		if (It->IsRegistered() == true)
			It->UnregisterComponent();

		It->RemoveFromRoot();
		It->DestroyComponent();
	}
	InSplineMeshComps.Empty();
}
