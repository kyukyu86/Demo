// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSkeletalMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/CapsuleComponent.h"


AWRSkeletalMeshActor::AWRSkeletalMeshActor(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComponent = GetSkeletalMeshComponent();
	if (MeshComponent == nullptr)
	{
		return;
	}

	MeshComponent->SetMobility(EComponentMobility::Movable);

	UCapsuleComponent* CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	RootComponent = CapsuleComponent;

	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AWRSkeletalMeshActor::SetSkeletalMesh(class USkeletalMesh* NewMesh, bool bReinitPose /*= true*/)
{
	USkeletalMeshComponent* MeshComponent = GetSkeletalMeshComponent();
	if (MeshComponent == nullptr)
	{
		return;
	}

	MeshComponent->SetSkeletalMesh(NewMesh, bReinitPose);
}