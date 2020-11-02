// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvDoorMountedVehicleBase.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRTransportTable.h"
#include "Manager/WRSFXManager.h"
#include "Utility/WRActorUtility.h"
#include "Components/ShapeComponent.h"

AWREnvDoorMountedVehicleBase::AWREnvDoorMountedVehicleBase()
{

}

AWREnvDoorMountedVehicleBase::AWREnvDoorMountedVehicleBase(EWREnvType InEnvType) : AWREnvVehicleBase(InEnvType)
{

}

void AWREnvDoorMountedVehicleBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ChildActors;
	this->GetAllChildActors(ChildActors);
	if (ChildActors.Num() != 0)
	{
		for (auto& Door : ChildActors)
		{
			// Set Child Door (Not Outter Door)
			AWREnvSlideDoor* CastedDoor = Cast<AWREnvSlideDoor>(Door);
			if (CastedDoor != nullptr)
			{
				CastedDoor->SetOwnerVehicle(this);
				DoorList.Add(CastedDoor);
			}
		}
	}
}

void AWREnvDoorMountedVehicleBase::BeginDestroy()
{
	OutterDoorList.Empty();

	Super::BeginDestroy();
}

void AWREnvDoorMountedVehicleBase::Tick(float DeltaTime)
{
	AWREnvVehicleBase::Tick(DeltaTime);

}

void AWREnvDoorMountedVehicleBase::MakeDebugString(TArray<FString>& OUT OutString)
{
	AWREnvVehicleBase::MakeDebugString(OutString);

	// Overlap Event State
	bool bOverlapEvent = false;
	UPrimitiveComponent* SceneToPrimitive = Cast<UPrimitiveComponent>(GetRootComponent());
	if (SceneToPrimitive != nullptr)
		bOverlapEvent = SceneToPrimitive->GetGenerateOverlapEvents();
	OutString.Add(FString::Printf(TEXT("Overlap State : %d"), bOverlapEvent));

	// Overlap Component Count
	OutString.Add(FString::Printf(TEXT("Recognition Overlaped Components : %d"), FOR_DEBUG_RecognitionOverlapedComponentNum));

	// Overlap OutterDoor Count
	OutString.Add(FString::Printf(TEXT("Recognition Overlaped OutterDoors : %d"), FOR_DEBUG_RecognitionOverlapedOuterDoorNum));

	// OutterDoor Count
	OutString.Add(FString::Printf(TEXT("Outter Doors : %d"), OutterDoorList.Num()));
}

void AWREnvDoorMountedVehicleBase::OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform)
{
	EWRVehicleState::en PrevState = eState;

	AWREnvVehicleBase::OnChangeState(InInform);

	// [ 2020-4-2 : kyu ] 공용화. 엘리베이터 등이 Move일때는 OverlapEvent를 꺼서 최적화. Move가 아닐때는 OverlapEvent를 켜서 OutterDoor를 업데이트 및 동작 동기화를 한다.
	// 이에 따라 Overlap 체크부분을 SlideDoor기반에서 Vehicle기반으로 수정함
	if (eState == EWRVehicleState::Open)
	{
		if (IsUsingOutterDoor())
		{
			UpdateOutterDoorList();	// [ 2020-4-3 : kyu ] 열릴 때 업데이트
		}		
	}
	else if(eState == EWRVehicleState::Move)
	{
		if (IsUsingOutterDoor())
		{
			ClearOutterDoorList(); // [ 2020-4-3 : kyu ] 이동시 외부문 해제
		}
	}

	CheckSound(PrevState, eState);

	switch (eState)
	{
	case EWRVehicleState::Open:
	{
		// CurrStation에 해당하는 문을 연다
		OpenDoor(GetStationDoorIndex(CurrStation));
	}
	break;
	case EWRVehicleState::Close:
	{
		// CurrStation에 해당하는 문을 닫는다
		CloseDoor(GetStationDoorIndex(CurrStation));
	}
	break;
	}
}

void AWREnvDoorMountedVehicleBase::OnSpawnComplete(const FWREnvVehicleSpawnParams& IN InParams)
{
	AWREnvVehicleBase::OnSpawnComplete(InParams);

}

void AWREnvDoorMountedVehicleBase::OpenDoor(const int32 IN InDoorIndex)
{
	bOpened = true;

	// Elevator Door
	for (auto& Door : DoorList)
	{
		if (Door->IsValidLowLevel() && Door->GetPositionIndex() == InDoorIndex)
		{
			Door->OpenDoor(); // 동일 Index 문이 여러개 일 수 있다.
		}
	}

	// Elevator Outter Door
	for (auto& OutterDoor : OutterDoorList)
	{
		if (OutterDoor->IsValidLowLevel() && OutterDoor->GetPositionIndex() == InDoorIndex)
		{
			OutterDoor->OpenDoor();
		}
	}	
}

void AWREnvDoorMountedVehicleBase::CloseDoor(const int32 IN InDoorIndex)
{
	bOpened = false;

	// Elevator Door
	for (auto& Door : DoorList)
	{
		if (Door->IsValidLowLevel() && Door->GetPositionIndex() == InDoorIndex)
		{
			Door->CloseDoor();
		}
	}

	// Elevator Outter Door
	for (auto& OutterDoor : OutterDoorList)
	{
		if (OutterDoor->IsValidLowLevel() && OutterDoor->GetPositionIndex() == InDoorIndex)
		{
			OutterDoor->CloseDoor();
		}
	}
}

void AWREnvDoorMountedVehicleBase::AddOutterDoor(AWREnvSlideDoor* IN InOutterDoor)
{
	OutterDoorList.AddUnique(InOutterDoor);
}

void AWREnvDoorMountedVehicleBase::RemoveOutterDoor(AWREnvSlideDoor* IN InOutterDoor)
{
	OutterDoorList.Remove(InOutterDoor);
}

int32 AWREnvDoorMountedVehicleBase::GetStationDoorIndex(const uint8 IN InStation)
{
	return 1; // Default
}

void AWREnvDoorMountedVehicleBase::UpdateOutterDoorList()
{
	FOR_DEBUG_RecognitionOverlapedComponentNum = 0;
	FOR_DEBUG_RecognitionOverlapedOuterDoorNum = 0;
	OutterDoorList.Empty();

	for (auto& Area : RecognitionList)
	{		
		TArray<UPrimitiveComponent*> Components;
		Area->GetOverlappingComponents(Components);
		if(Components.Num() == 0)
			continue;

		FOR_DEBUG_RecognitionOverlapedComponentNum += Components.Num();
		for (auto& Comp : Components)
		{
			AActor* pOwner = Comp->GetOwner();
			if(pOwner == nullptr)
				continue;

			AWREnvSlideDoor* CastedDoor = Cast<AWREnvSlideDoor>(pOwner);
			if (CastedDoor != nullptr && CastedDoor->IsOutterDoor())
			{
				if (OutterDoorList.Contains(CastedDoor) == false)
				{
					++FOR_DEBUG_RecognitionOverlapedOuterDoorNum;
					CastedDoor->SetOwnerVehicle(this);
					OutterDoorList.Add(CastedDoor);
				}
			}
		}
	}
}

void AWREnvDoorMountedVehicleBase::ClearOutterDoorList()
{
	FOR_DEBUG_RecognitionOverlapedComponentNum = 0;
	FOR_DEBUG_RecognitionOverlapedOuterDoorNum = 0;
	for (auto& OutterDoor : OutterDoorList)
	{
		if (OutterDoor->IsValidLowLevel())
		{
			OutterDoor->SetOwnerVehicle(nullptr);
		}
	}
	OutterDoorList.Empty();
}

void AWREnvDoorMountedVehicleBase::CheckSound(const EWRVehicleState::en IN InPrevState, const EWRVehicleState::en IN InCurrState)
{
	if (InPrevState == EWRVehicleState::Close && InCurrState == EWRVehicleState::Move)
	{
		// Start & Move
		PlaySound(EWRVehicleSound::Start);
		PlaySound(EWRVehicleSound::Move); // Looping On
	}
	else if (InPrevState == EWRVehicleState::Move && InCurrState == EWRVehicleState::Stop)
	{
		// Arrival
		StopSound(EWRVehicleSound::Move); // Looping Off
		PlaySound(EWRVehicleSound::Arrival);
	}
}

void AWREnvDoorMountedVehicleBase::PlaySound(const EWRVehicleSound IN InSound)
{
	switch (InSound)
	{
	case EWRVehicleSound::Start:
	{
		if (StartSound.IsNull() == false)
		{
			WRSFXManager::Get()->SpawnAttached(StartSound.LoadSynchronous(), this->GetRootComponent());
		}
	}
	break;
	case EWRVehicleSound::Arrival:
	{
		if (ArrivalSound.IsNull() == false)
		{
			WRSFXManager::Get()->SpawnAttached(ArrivalSound.LoadSynchronous(), this->GetRootComponent());
		}
	}
	break;
	case EWRVehicleSound::Move:
	{
		if (pAudioComp_Move != nullptr && pAudioComp_Move->IsValidLowLevel())
		{
			WRSFXManager::Get()->Remove(pAudioComp_Move);
			pAudioComp_Move = nullptr;
		}
		if (MoveSound.IsNull() == false)
		{
			pAudioComp_Move = WRSFXManager::Get()->SpawnAttached(MoveSound.LoadSynchronous(), this->GetRootComponent());
		}
	}
	break;
	}
}

void AWREnvDoorMountedVehicleBase::StopSound(const EWRVehicleSound IN InSound)
{
	switch (InSound)
	{
	case EWRVehicleSound::Start:
	case EWRVehicleSound::Arrival:
	{

	}
	break;
	case EWRVehicleSound::Move:
	{
		if (pAudioComp_Move != nullptr && pAudioComp_Move->IsValidLowLevel())
		{
			WRSFXManager::Get()->Remove(pAudioComp_Move);
			pAudioComp_Move = nullptr;
		}
	}
	break;
	}
}




// void AWREnvDoorMountedVehicleBase::UpdateOutterDoor()
// {
// 	OutterDoorList.Empty();
// 
// 	TArray<AActor*> OverlapedActors;
// 	GetOverlappingActors(OverlapedActors, AWREnvSlideDoor::StaticClass());
// 	if (OverlapedActors.Num() != 0)
// 	{
// 		for (auto& OutterDoor : OverlapedActors)
// 		{
// 			AWREnvSlideDoor* CastedOutterDoor = Cast<AWREnvSlideDoor>(OutterDoor);
// 			if (CastedOutterDoor != nullptr && CastedOutterDoor->IsOutterDoor())
// 			{
// 				CastedOutterDoor->SetOwnerVehicle(this);
// 				OutterDoorList.Add(CastedOutterDoor);
// 			}
// 		}		
// 	}
// }