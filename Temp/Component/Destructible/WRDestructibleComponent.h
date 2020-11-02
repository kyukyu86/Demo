// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleComponent.h"
#include "Enum/EWREnvObject.h"
#include "WRDestructibleComponent.generated.h"

/**
 *
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRDestructibleComponent : public UDestructibleComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "WRDestructibleInfo")
	EWRDestructibleHitPositionType HitType = EWRDestructibleHitPositionType::Static;
	UPROPERTY(EditAnywhere, Category = "WRDestructibleInfo")
	EWRDestructibleImpulseType ImpulseType = EWRDestructibleImpulseType::Radial;
	UPROPERTY(EditAnywhere, Category = "WRDestructibleInfo")
	float ImpulseStrength = 0.f;
	UPROPERTY(EditAnywhere, Category = "WRDestructibleInfo")
	float ImpulsePower = 0.f;
	UPROPERTY(EditAnywhere, Category = "WRDestructibleInfo")
		float ImpulseRadius = 0.f;

private:
	class UArrowComponent* InPhysicsDirection = nullptr;
	FHitResult* InHitResult = nullptr;


	TArray<class UMaterialInstanceDynamic*> MIDs;

	float DissolveDuration = 0.f;
	float DissolveDeltaTime = 0.f;

	FVector PreviousLocation = FVector::ZeroVector;

	float AccDeltaTime = 0.0F;

	bool IsBeginDestruction = false;
public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	void SetEnableTick(bool IsTickEnable);

	FVector GetImpulseStartLocation();
	FVector GetImpulseDirection();

	void DestructionByRadial();
	void DestructionByDirection();

	void InitializePhysicsDirection();


	void CreateDynamicMaterials();

	void SetDynamicMaterialValue(float InDeltaTime);

	void ProcessRemoveDisplay(float InDeltaTime);

	void MakeRemoveDisplay();

	void CompleteDissolve();
public:
	UFUNCTION(BlueprintCallable, Category = "Level Sequence")
	void BeginDestruction();


};
