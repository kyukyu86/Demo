// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRElevator.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRTransportTable.h"
#include "Manager/WRSFXManager.h"
#include "Utility/WRActorUtility.h"
#include "Manager/WRUIManager.h"
#include "Components/ShapeComponent.h"


AWRElevator::AWRElevator() : AWREnvDoorMountedVehicleBase(EWREnvType::Elevator)
{

}

void AWRElevator::MakeDebugString(TArray<FString>& OUT OutString)
{
	AWREnvDoorMountedVehicleBase::MakeDebugString(OutString);

}

void AWRElevator::OnSpawnComplete(const struct pt_Game_Actor::Inf_ElevatorAppear_G2U& IN InInform)
{
	FWREnvVehicleSpawnParams Params;
	Params.TableID		= InInform.elevatorTID;
	Params.AID			= InInform.aid;
	Params.State		= (EWRVehicleState::en)InInform.vehicleState;
	Params.CurrStation	= InInform.curStation;
	Params.NextStation	= InInform.nextStation;
	Params.StartTime	= InInform.startTime;
	Params.PassengerList.assign(InInform.lstPassengers.begin(), InInform.lstPassengers.end());
	Params.Location		= FVector(InInform.posX, InInform.posY, InInform.posZ);
	Params.Rotation		= FRotator(0.0f, InInform.angle, 0.0f);

	AWREnvDoorMountedVehicleBase::OnSpawnComplete(Params);

	FWRElevatorTable* pTable = WRTableManager::Get()->FindRow<FWRElevatorTable>(InInform.elevatorTID);
	if (pTable != nullptr)
	{
		SetMoveSpeed(pTable->MoveSpeed);
	}
}

void AWRElevator::Tick(float DeltaTime)
{
	AWREnvDoorMountedVehicleBase::Tick(DeltaTime);

	FWRDebugUIData data;
	data.Flag = EWRDebugUIBitFlag::EnvElevator;
	data.Actor = this;	
	data.Offset = FVector(0, 0, 100);
	MakeDebugString(data.ArrayContents);
	WRUIManager::Get()->UpdateDebugUI(data);
}

int32 AWRElevator::GetStationDoorIndex(const uint8 IN InStation)
{
	FWRElevatorTable* FoundTable = WRTableManager::Get()->FindRow<FWRElevatorTable>(GetTID());
	if (FoundTable == nullptr)
		return -1;

	switch (InStation)
	{
	case 1:
	{
		return FoundTable->DoorIndex1;
	}break;
	case 2:
	{
		return FoundTable->DoorIndex2;
	}break;
	}

	return -1;
}