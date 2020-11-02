// Fill out your copyright notice in the Description page of Project Settings.


#include "WRPreviewActorCustomActor.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ChildActorComponent.h"

AWRPreviewActorCustomActor::AWRPreviewActorCustomActor() : Super()
{
	/*
	ChildActorComp = CreateOptionalDefaultSubobject<UChildActorComponent>(TEXT("ChildActor"));
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
	*/
}

void AWRPreviewActorCustomActor::BeginPlay()
{
	Super::BeginPlay();

}

void AWRPreviewActorCustomActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWRPreviewActorCustomActor::SetCustomActor(class AActor* InActor, class UAnimationAsset* InAnim)
{
	ChildActorComp->SetChildActorClass(InActor->GetClass());
	//ChildActorComp->CreateChildActor();
	AActor* pChildActor = ChildActorComp->GetChildActor();
	if (pChildActor == nullptr)
		return;

	if (InAnim)
	{
		TArray<USkeletalMeshComponent*> SkeletalMeshComps;
		pChildActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
		if (SkeletalMeshComps.Num() > 0)
		{
			if (SkeletalMeshComps[0]->IsValidLowLevel())
			{
				SkeletalMeshComps[0]->SetAnimationMode(EAnimationMode::AnimationSingleNode);
				SkeletalMeshComps[0]->SetEnableGravity(false);
				SkeletalMeshComps[0]->PlayAnimation(InAnim, true);
			}
		}

// 		ACharacter* CastedActor = Cast<ACharacter>(pChildActor);
// 		if (CastedActor)
// 		{
// 			USkeletalMeshComponent* SkeletalMeshComp = CastedActor->GetMesh();
// 			if (SkeletalMeshComp)
// 			{
// 				SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
//				SkeletalMeshComp->SetEnableGravity(false);
// 				SkeletalMeshComp->PlayAnimation(InAnim, true);
// 			}
// 		}

	}

	UpdateScale();
}

void AWRPreviewActorCustomActor::SetAnimation()
{

}

void AWRPreviewActorCustomActor::SetData()
{

}

class UMeshComponent* AWRPreviewActorCustomActor::GetMeshComponent()
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
		return Cast<USkeletalMeshComponent>(SkeletalMeshComps[0]);
	}

	TArray<UStaticMeshComponent*> StaticMeshComps;
	pChildActor->GetComponents<UStaticMeshComponent>(StaticMeshComps);
	if (StaticMeshComps.Num() > 0)
	{
		return Cast<UStaticMeshComponent>(StaticMeshComps[0]);
	}

	return nullptr;
}
