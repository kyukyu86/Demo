// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPreviewActorDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"

ADMPreviewActorDynamic::ADMPreviewActorDynamic() : Super()
{
	SkeletalMeshComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
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
}

void ADMPreviewActorDynamic::BeginPlay()
{
	Super::BeginPlay();

}

void ADMPreviewActorDynamic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADMPreviewActorDynamic::SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim)
{
	if (SkeletalMeshComponent == nullptr)
		return;

	SkeletalMeshComponent->SetSkeletalMesh(InMesh);
	SkeletalMeshComponent->PlayAnimation(InAnim, true);

	UpdateScale();
}
