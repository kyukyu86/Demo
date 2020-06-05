// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPreviewActorStatic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ADMPreviewActorStatic::ADMPreviewActorStatic() : Super()
{
	StaticMeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	if (StaticMeshComponent)
	{
		StaticMeshComponent->AlwaysLoadOnClient = true;
		StaticMeshComponent->bOwnerNoSee = false;
		StaticMeshComponent->bCastDynamicShadow = true;
		StaticMeshComponent->bAffectDynamicIndirectLighting = true;
		StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		StaticMeshComponent->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
		static FName MeshCollisionProfileName(TEXT("PreviewActor"));
		StaticMeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
		StaticMeshComponent->SetGenerateOverlapEvents(false);
		StaticMeshComponent->SetCanEverAffectNavigation(false);
	}
}

void ADMPreviewActorStatic::BeginPlay()
{
	Super::BeginPlay();

}

void ADMPreviewActorStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADMPreviewActorStatic::SetStaticMesh(class UStaticMesh* InMesh)
{
	if (StaticMeshComponent == nullptr)
		return;

	StaticMeshComponent->SetStaticMesh(InMesh);

	UpdateScale();
}
