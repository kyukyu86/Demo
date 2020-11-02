// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "ENUM/EWRCharacter.h"
#include <Components/SplineComponent.h>
#include "Define/WRTypeDefine.h"
#include "WRActorComponentDynamicMovement.generated.h"

struct FWRDynamicMovementInfo
{
	WRTableID DynamicMovementTID = 0;
	FVector StartingLocation = FVector::ZeroVector;
	FVector DestLocation = FVector::ZeroVector;
	bool bSweep = true;;
	float ElapsedTime = 0.0f;

	void Reset()
	{
		DynamicMovementTID = 0;
		StartingLocation = FVector::ZeroVector;
		DestLocation = FVector::ZeroVector;
		bSweep = true;
		ElapsedTime = 0.0f;
	}

	bool IsValid()
	{
		return (DynamicMovementTID != 0) ? true : false;
	}

	float UpdateElapsedTime(float DeltaTime)
	{
		ElapsedTime -= DeltaTime;
		if (ElapsedTime < 0.0f)
		{
			ElapsedTime = 0.0f;

			//이거는 생각을 좀 해보자
			Reset();
		}

		return ElapsedTime;
	}
};


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentDynamicMovement : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void OnCreateComponent()	override;
	virtual void OnDestroyComponent()	override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void CreateSplineComponent();
	void DestroySplineComponent();

	void TickCalculateMovement(float DeltaTime);
	bool DetermineDynamicMovement(WRTableID InTableID, AWRCharacter* InChar, bool bSweep = true);
	bool InvokeDynamicMovement(WRTableID InTableID, FVector InDestLocation, bool bSweep = true);

	void OnDynamicMoveFinished();
	FVector GetSplineDestLocation();
	void SyncDynamicMoveLocation();
	bool IsValidDynamicMovement();

	void UpdateLocationOnVehicle(FVector InLocationToModify);

public:
	UFUNCTION(BlueprintCallable)
	void OnHit(UPrimitiveComponent* HitComponent, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);

private:

public:
	bool btest = false;
	USplineComponent* SplineComponent;

private:
	FWRDynamicMovementInfo DynamicMovementInfo;
	FTimerHandle DynamicMovementTimer;

};
