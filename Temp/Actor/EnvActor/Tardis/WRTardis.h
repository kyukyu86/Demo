// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../WREnvDoorMountedVehicleBase.h"
#include "WRTardis.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRTardis : public AWREnvDoorMountedVehicleBase
{
	GENERATED_BODY()
	
public:
	AWRTardis();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void MakeDebugString(TArray<FString>& OUT OutString);
	virtual int32 GetStationDoorIndex(const uint8 IN InStation) override;	// override AWREnvDoorMountedVehicleBase

	UFUNCTION()
	void OnBeginOverlap_CheckArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap_CheckArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform) override;
	virtual void AddPassenger(const AID_t IN InAID) override;			// override AWREnvVehicleBase
	virtual void RemovePassenger(const AID_t IN InAID) override;		// override AWREnvVehicleBase
	void OnSpawnComplete(const struct pt_Game_Actor::Inf_TardisAppear_G2U& IN InInform);
	void OnGroupWarpTimerCount(const uint8 IN InReadyCountSecond, const bool IN InBegin);
	void OnGroupWarpStop();

private:
	bool IsEmptyCheckAllArea();
	void UpdateDoorState();
	void OnDoorAction(const bool IN InOpen);

	void OnStandby();
	void OnDeparture();

private:
	bool bWaitingToDepart = false;

	TArray<class UShapeComponent*> CheckAreas;
	TArray<class UWRWidgetComponentBase*> TimerWidgets;
};
