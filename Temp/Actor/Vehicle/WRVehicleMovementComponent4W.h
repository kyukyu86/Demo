// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "WRVehicleMovementComponent4W.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRVehicleMovementComponent4W : public UWheeledVehicleMovementComponent4W
{
	GENERATED_BODY()
	
public:
	virtual void UpdateDrag(float DeltaTime) override;

public:
	void ClearAllInputValue();

	float GetSteeringInput();
	float GetThrottleInput();

	float GetRowSteeringInput();
	float GetRowThrottleInput();

	bool GetHandBrakeInput();

	int32 GetSpeedByKM();

	void SetIdleBrakeInput(float InValue);
	void SetStopThreshold(float InValue);

};
