// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Network/WRProtocolHeaders.h"
#include "WREnvVehicleBase.generated.h"



struct FWREnvVehicleSpawnParams
{
	FWREnvVehicleSpawnParams()
		: TableID(INVALID_TABLE_TID)
		, AID(INVALID_AID)
		, State(EWRVehicleState::INVALID)
		, CurrStation(0)
		, NextStation(0)
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
	{
		StartTime.Clear();
	}
	WRTableID TableID;
	AID_t AID;
	EWRVehicleState::en State;
	std::list<AID_t> PassengerList;
	uint8 CurrStation;
	uint8 NextStation;
	stSmallDateTime StartTime;
	FVector Location;
	FRotator Rotation;
};


UCLASS()
class WR_API AWREnvVehicleBase : public AWREnvActor
{
	GENERATED_BODY()

public:
	AWREnvVehicleBase();
	AWREnvVehicleBase(EWREnvType InEnvType);

private:
	void UpdateLocation(float DeltaTime);
	void UpdatePassengersCulling(float DeltaTime);
	void UpdatePassengerDynamicMovementLocation(float DeltaTime, FVector InLocationToModify);

protected:
	virtual void BeginPlay() override;		
	virtual void MakeDebugString(TArray<FString>& OUT OutString);
	FORCEINLINE void SetMoveSpeed(const float IN InMoveSpeed) { fMoveSpeed = InMoveSpeed; }	// from Table Data

	UFUNCTION()
	virtual void OnBeginOverlap_RecognitionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnEndOverlap_RecognitionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE virtual bool IsVehicle() { return true; }					// 탑승물인가
	FORCEINLINE virtual bool IsUseOverlapEventControl() { return false; }	// OverlapEvent 제어를 할 것인가? (최적화)
	FORCEINLINE EWRVehicleState::en GetState() { return eState; }			// 상태
	FORCEINLINE uint8 GetCurrStation() { return CurrStation; }
	FORCEINLINE uint8 GetNextStation() { return NextStation; }
	FORCEINLINE float GetMoveSpeed() { return fMoveSpeed; }
	FVector GetMoveVector();
	FVector GetSrcLocation();
	FVector GetDstLocation();

	bool IsAlreadyPassenger(const AID_t IN InCharAID);

	void OnSpawnComplete(const FWREnvVehicleSpawnParams& IN InParams);
	void OnUpdatePassengersLocation();

	// Packet
	virtual void OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform);
	void OnMoveToPos(const struct pt_Game_User::Inf_ActorMoveToPos_G2U& IN InInform);
	void OnMoveStop(const struct pt_Game_User::Inf_ActorMoveStop_G2U& IN InInform);
	virtual void AddPassenger(const AID_t IN InAID);
	virtual void RemovePassenger(const AID_t IN InAID);

	UFUNCTION(BlueprintCallable)
	void TestMove(FVector InDstLocation);

protected:
	TArray<class UShapeComponent*> RecognitionList;

	EWRVehicleState::en eState = EWRVehicleState::INVALID;
	uint8 CurrStation = 0;
	uint8 NextStation = 0;
	stSmallDateTime StartTime;
	TArray<AID_t> Passengers;

	bool bMove = false;
	float fMoveSpeed = 300.f;	// cm/s
	float fElapsedTime = 0.f;
	FVector vSrcLocation;
	FVector vDstLocation;

	FVector vInitializeLocation;

	// for Culling
	float CullingRefreshTime = 0.f;
};
