// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRStaticMeshAbandonedThing.h"
#include "Utility/WRActorUtility.h"

#include "EngineUtils.h"
#include "Manager/WRSFXManager.h"
#include "Table/WRItemTable.h"
#include "Table/Base/WRTableManager.h"
#include "Manager/WRConsoleManager.h"


void AWRStaticMeshAbandonedThing::BeginPlay()
{
	Super::BeginPlay();

	if (RootComponent)
	{
		UPrimitiveComponent* PrimitiveRootComponent = Cast<UPrimitiveComponent>(RootComponent);
		if (PrimitiveRootComponent)
		{
			PrimitiveRootComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}	
}

void AWRStaticMeshAbandonedThing::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	float fSize = NormalImpulse.Size();
	float fStandard = WRConsoleManager::Get()->GetFloat(EWRConsole::AbandonedThingImpactSound);
	if (fSize >= fStandard)
	{
		FWRTemporaryItemTable* AbandonedTemporaryItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, ItemTID);
		if (AbandonedTemporaryItemTable != nullptr)
		{
			if (AbandonedTemporaryItemTable->DropSound.IsEmpty() == false)
			{
				WRSFXManager::Get()->SpawnAtLocation(AbandonedTemporaryItemTable->DropSound, HitLocation);
			}
		}
	}
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void AWRStaticMeshAbandonedThing::FinishMaintainTimer()
{
	WRActorUtility::ClearTimer(MaintainTimerHandle);
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Destroy();
}

void AWRStaticMeshAbandonedThing::SetMaintainTimer(float IN InTimer)
{
	WRActorUtility::SetTimer(MaintainTimerHandle, FTimerDelegate::CreateUObject(this, &AWRStaticMeshAbandonedThing::FinishMaintainTimer), InTimer, false);
}

void AWRStaticMeshAbandonedThing::SetPhysics(FVector IN InDirection, float IN InForce /* = 0.f */)
{
	UStaticMeshComponent* MeshComponent = GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetCollisionProfileName(FName("PhysicsActor"));
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetNotifyRigidBodyCollision(true);

		// Impulse
		if (InForce != 0.f)
		{
			MeshComponent->AddImpulse(InDirection * InForce, NAME_None, true);
		}		
	}
}
