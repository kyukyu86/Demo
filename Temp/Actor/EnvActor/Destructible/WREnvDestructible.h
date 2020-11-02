// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "WREnvDestructible.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWREnvDestructible : public AWREnvActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AWREnvDestructible();

	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	class UWRDestructibleComponent* pDestructibleComp = nullptr;
	class UCapsuleComponent* pCapsuleCollisionComp = nullptr;

	int32 DestructibleTID = -1;

public:
	UFUNCTION()
	void OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTouchedFracture(const FVector& HitPosition, const FVector& HitDirection);
};
