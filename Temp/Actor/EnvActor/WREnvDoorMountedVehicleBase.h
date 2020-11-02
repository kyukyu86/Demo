// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvVehicleBase.h"
#include "Door/WREnvSlideDoor.h"
#include "WREnvDoorMountedVehicleBase.generated.h"


enum class EWRVehicleSound : uint8
{
	None,
	Start,
	Arrival,
	Move,
};


UCLASS()
class WR_API AWREnvDoorMountedVehicleBase : public AWREnvVehicleBase
{
	GENERATED_BODY()

public:
	AWREnvDoorMountedVehicleBase();
	AWREnvDoorMountedVehicleBase(EWREnvType InEnvType);

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;
	virtual void MakeDebugString(TArray<FString>& OUT OutString);
	virtual int32 GetStationDoorIndex(const uint8 IN InStation);	// Based This
	FORCEINLINE virtual bool IsUsingOutterDoor() { return false; }	// Based This
	FORCEINLINE bool IsDoorOpened() { return bOpened; }

public:
	virtual void OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform) override;
	void OnSpawnComplete(const FWREnvVehicleSpawnParams& IN InParams);
	// Door
	void OpenDoor(const int32 IN InDoorIndex);
	void CloseDoor(const int32 IN InDoorIndex);
	// Outter Door
	void AddOutterDoor(AWREnvSlideDoor* IN InOutterDoor);
	void RemoveOutterDoor(AWREnvSlideDoor* IN InOutterDoor);

private:
	// Outter Door
	void UpdateOutterDoorList();
	void ClearOutterDoorList();
	// Sound
	void CheckSound(const EWRVehicleState::en IN InPrevState, const EWRVehicleState::en IN InCurrState);
	void PlaySound(const EWRVehicleSound IN InSound);
	void StopSound(const EWRVehicleSound IN InSound);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		bool UseOutterDoor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<class USoundBase> StartSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<class USoundBase> ArrivalSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<class USoundBase> MoveSound;

protected:
	bool bOpened = false;
	// Door
	TArray<AWREnvSlideDoor*> DoorList;
	// Outter Door
	TArray<AWREnvSlideDoor*> OutterDoorList;
	// Sound
	class UAudioComponent* pAudioComp_Move = nullptr;

	// for Debug
	int32 FOR_DEBUG_RecognitionOverlapedComponentNum = 0;
	int32 FOR_DEBUG_RecognitionOverlapedOuterDoorNum = 0;
	
};
