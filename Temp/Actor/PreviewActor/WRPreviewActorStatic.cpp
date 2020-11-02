// Fill out your copyright notice in the Description page of Project Settings.


#include "WRPreviewActorStatic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AWRPreviewActorStatic::AWRPreviewActorStatic() : Super()
{
	/*
	StaticMeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	if (StaticMeshComponent)
	{
		StaticMeshComponent->AlwaysLoadOnClient = true;
		StaticMeshComponent->bOwnerNoSee = false;
		StaticMeshComponent->bCastDynamicShadow = true;
		StaticMeshComponent->bAffectDynamicIndirectLighting = true;
		StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		StaticMeshComponent->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
		static FName MeshCollisionProfileName(TEXT("NoCollision"));
		StaticMeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
		StaticMeshComponent->SetGenerateOverlapEvents(false);
		StaticMeshComponent->SetCanEverAffectNavigation(false);

		StaticMeshComponent->SetEnableGravity(false);
	}
	*/
}

void AWRPreviewActorStatic::BeginPlay()
{
	Super::BeginPlay();

}

class UMeshComponent* AWRPreviewActorStatic::GetMeshComponent()
{
	return StaticMeshComponent;
}

void AWRPreviewActorStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWRPreviewActorStatic::SetStaticMesh(class UStaticMesh* InMesh)
{
	if (StaticMeshComponent == nullptr)
		return;

	StaticMeshComponent->SetStaticMesh(InMesh);

	UpdateScale();
}
