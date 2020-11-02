// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Enum/EWRCharacter.h"
#include "WRVehicleAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRVehicleAnimInstance : public UVehicleAnimInstance
{
	GENERATED_BODY()

public:
	void SetSteering(float InValue);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVehicle")
	float SteeringValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVehicle")
	bool bOpenLeftSide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVehicle")
	bool bOpenRightSide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVehicle")
	bool bOpenRoof = false;
};

UCLASS()
class WR_API UWRSteeringWheelAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Steering Wheel State", meta = (AllowPrivateAccess = "true"))
	EWRSteeringWheelState SWheelState = EWRSteeringWheelState::ManualIdle;
}; 