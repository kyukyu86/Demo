// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRStaticMeshActor.h"
#include "WRStaticMeshAbandonedThing.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRStaticMeshAbandonedThing : public AWRStaticMeshActor
{
	GENERATED_BODY()

private:
	FTimerHandle MaintainTimerHandle;
	WRTableID ItemTID = INVALID_TABLE_TID;

protected:
	virtual void BeginPlay() override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

public:
	// Info
	FORCEINLINE void SetItemTID(const WRTableID IN InItemTID) { ItemTID = InItemTID; }
	// Maintain Timer
	void SetMaintainTimer(float IN InTimer);
	void FinishMaintainTimer();
	// Physics
	void SetPhysics(FVector IN InDirection, float IN InForce = 0.f);

};
