// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../WREnvDoorMountedVehicleBase.h"
#include "WRElevator.generated.h"

/**
 *
 */
UCLASS()
class WR_API AWRElevator : public AWREnvDoorMountedVehicleBase
{
	GENERATED_BODY()

public:
	AWRElevator();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void MakeDebugString(TArray<FString>& OUT OutString);
	virtual int32 GetStationDoorIndex(const uint8 IN InStation) override;			// override AWREnvDoorMountedVehicleBase
	FORCEINLINE virtual bool IsUsingOutterDoor() override { return true; }			// override AWREnvDoorMountedVehicleBase	
	FORCEINLINE virtual bool IsUseOverlapEventControl() override { return true; }	// override AWREnvVehicleBase

public:
	void OnSpawnComplete(const struct pt_Game_Actor::Inf_ElevatorAppear_G2U& IN InInform);	
		
};
