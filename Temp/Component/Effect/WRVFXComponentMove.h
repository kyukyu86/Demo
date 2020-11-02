// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Enum/EWRVFX.h"

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRVFXComponentMove.generated.h"

USTRUCT(BlueprintType)
struct FWRMoves
{
	GENERATED_BODY()

public:
	float Tick(float InDeltaTime);

public:
	void Reset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	float StartVelocity = 1000.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	float FinishVelocity = 1000.0F;

	// [ 2019-8-16 : animal14 ] StartVelocity에서 FinishVelocity까지 도달하는데 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	float Acceleration = 1.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WRMoveComponent")
	float DeltaTime = 0.0F;
};

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRVFXComponentMove : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetActive(bool bNewActive, bool bReset = false) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#else // WITH_EDITOR
#endif // WITH_EDITOR

public:
	void Attach() {
		IsAttaching = true;
	}
	void Stop() {
		IsMoving = false;
	}

	void SetAdditionalAngle(const FRotator& InAngle) {
		AdditionalAngle = InAngle;
	}

private:
	void LoadTable();

	void Initialize4Spawner();
	//void Initialize4Target();
	void Initialize4SpawnerToTarget();
	void Initialize4TraceTarget();

	void Tick4Transform(float InDeltaTime);
	void Tick4CCD(const FVector& InCurrentDirection, const float InCurrentSpeed, const float InDeltaTime);

	FVector GetDirection(float InDeltaTime);
	float GetSpeed();

	void Initialize4Parabola(const FVector& InFrom, const FVector& InTo);
	FVector GetParabolaDirection(const float InCurrentTime, const float InPreviousTime);

#if UE_EDITOR
	void Debug(const FVector& InCurrentDirection, const float InCurrentSpeed, const float InDeltaTime);
#else // UE_EDITOR
#endif // UE_EDITOR

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	EWRVFXMove Type = EWRVFXMove::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	bool IsCCD = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	uint32 bOverrideMovements : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent", meta = (EditCondition = "!bOverrideMovements"))
	FWRMoves Movements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMoveComponent")
	bool IsDebug = false;

private:
	DECLARE_DELEGATE(WRInitialize)
	TArray<WRInitialize> Initializes;

	bool IsMoving = true;
	bool IsAttaching = false;

	FVector Direction = FVector::ZeroVector;
	FVector CalculatedParabolaDirection = FVector::ZeroVector;

	float SplineRatio = 1.0F;
	float DistanceRatio = 1.0F;

	float Radius = 0.0F;

	FRotator AdditionalAngle = FRotator::ZeroRotator;
};
