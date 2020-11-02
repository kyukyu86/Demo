// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRCharacterObjectBase.h"

#include "Components/StaticMeshComponent.h"

AWRCharacterObjectBase::AWRCharacterObjectBase()
{

}

AWRCharacterObjectBase::AWRCharacterObjectBase(const FObjectInitializer& ObjectInitializer)
	: AWRCharacter(ObjectInitializer)
{
	USceneComponent* pMeshParent = GetMeshParentComponent();
	if (pMeshParent == nullptr)
		return;

	StaticMesh = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	if (StaticMesh)
	{
		StaticMesh->AlwaysLoadOnClient = true;
		StaticMesh->bOwnerNoSee = false;
		StaticMesh->bCastDynamicShadow = true;
		StaticMesh->bAffectDynamicIndirectLighting = true;
		StaticMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		StaticMesh->SetupAttachment(pMeshParent);
		static FName MeshCollisionProfileName(TEXT("WRIOCollision"));
		StaticMesh->SetCollisionProfileName(MeshCollisionProfileName);
		StaticMesh->SetGenerateOverlapEvents(false);
		StaticMesh->SetCanEverAffectNavigation(false);
	}
}

class UMeshComponent* AWRCharacterObjectBase::GetCurrentMesh() const
{
	if (GetMesh()->SkeletalMesh)
	{
		return GetMesh();
	}
	else if (StaticMesh->GetStaticMesh())
	{
		return StaticMesh;
	}
	else
	{
		return nullptr;
	}
}

void AWRCharacterObjectBase::BeginPlay()
{
	Super::BeginPlay();
}
