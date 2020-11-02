// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRFloatingBoard.h"

#include "Network/WRProtocolHeaders.h"
#include "Table/WRTransportTable.h"
#include "Table/Base/WRTableManager.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Components/ShapeComponent.h"
#include "Manager/WRUIManager.h"


AWRFloatingBoard::AWRFloatingBoard() : AWREnvVehicleBase(EWREnvType::FloatingBoard)
{

}

void AWRFloatingBoard::MakeDebugString(TArray<FString>& OUT OutString)
{
	AWREnvVehicleBase::MakeDebugString(OutString);

}

void AWRFloatingBoard::Tick(float DeltaTime)
{
	AWREnvVehicleBase::Tick(DeltaTime);

	FWRDebugUIData data;
	data.Flag = EWRDebugUIBitFlag::Floating;
	data.Actor = this;
	data.Offset = FVector(0, 0, 0);
	MakeDebugString(data.ArrayContents);
	WRUIManager::Get()->UpdateDebugUI(data);
}

void AWRFloatingBoard::OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform)
{
	AWREnvVehicleBase::OnChangeState(InInform);

	switch (eState)
	{
	case EWRVehicleState::Move:
	{
		// + 애매하게 걸친 플레이어 밀어주기

		//WRActorUtility::SetGenerateOverlapEventsbyActor(true, this, true);

		for (auto& RecognitionArea : RecognitionList)
		{
			//RecognitionArea->UpdateOverlaps();

			TArray<AActor*> Actors;
			RecognitionArea->GetOverlappingActors(Actors, AWRCharacterPlayer::StaticClass());
			if (Actors.Num() != 0)
			{
				for (auto& Actor : Actors)
				{
					AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(Actor);
					if (CastedPlayer != nullptr)
					{
						if (IsAlreadyPassenger(CastedPlayer->GetActorID()))
							continue;

						FVector vDirection = Actor->GetActorLocation() - this->GetActorLocation();
						vDirection.Normalize();
						vDirection.Z = 0;
						CastedPlayer->AddActorWorldOffset(vDirection * PushOutValue);
					}
				}
			}
		}		

		//WRActorUtility::SetGenerateOverlapEventsbyActor(false, this, true);
	}
	break;
	}
}

void AWRFloatingBoard::OnSpawnComplete(const struct pt_Game_Actor::Inf_FloatingBoardAppear_G2U& IN InInform)
{
	FWREnvVehicleSpawnParams Params;
	Params.TableID		= InInform.floatingBoardTID;
	Params.AID			= InInform.aid;
	Params.State		= (EWRVehicleState::en)InInform.vehicleState;
	Params.CurrStation	= InInform.curStation;
	Params.NextStation	= InInform.nextStation;
	Params.StartTime	= InInform.startTime;
	Params.PassengerList.assign(InInform.lstPassengers.begin(), InInform.lstPassengers.end());
	Params.Location		= FVector(InInform.posX, InInform.posY, InInform.posZ);
	Params.Rotation		= FRotator(0.0f, InInform.angle, 0.0f);

	AWREnvVehicleBase::OnSpawnComplete(Params);

	FWRFloatingBoardTable* pTable = WRTableManager::Get()->FindRow<FWRFloatingBoardTable>(InInform.floatingBoardTID);
	if (pTable != nullptr)
	{
		SetMoveSpeed(pTable->MoveSpeed);
	}
}