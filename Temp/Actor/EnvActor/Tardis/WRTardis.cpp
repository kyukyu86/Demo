// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRTardis.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRTransportTable.h"
#include "Components/ShapeComponent.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "WidgetComponent/WRWidgetComponentBase.h"
#include "Table/WRCustomTextTable.h"
#include "UI/Panel/WRUIPanel_TardisTimer.h"

#include "Localization/WRLocalizationProcessor.h"

#define DEF_TARDIS_CUSTOM_TEXT_TABLE_TID_STANDBY			5
#define DEF_TARDIS_CUSTOM_TEXT_TABLE_TID_WAITINGTODEPART	6
#define DEF_TARDIS_CUSTOM_TEXT_TABLE_TID_DEPARTURE			7


AWRTardis::AWRTardis() : AWREnvDoorMountedVehicleBase(EWREnvType::Tardis)
{

}

void AWRTardis::BeginPlay()
{
	AWREnvDoorMountedVehicleBase::BeginPlay();

	TArray<UActorComponent*> CheckComponents = this->GetComponentsByTag(UShapeComponent::StaticClass(), "CheckArea");
	if (CheckComponents.Num() != 0)
	{
		for (auto& Comp : CheckComponents)
		{
			UShapeComponent* CastedComp = Cast<UShapeComponent>(Comp);
			if (CastedComp != nullptr)
			{
				CastedComp->OnComponentBeginOverlap.AddDynamic(this, &AWRTardis::OnBeginOverlap_CheckArea);
				CastedComp->OnComponentEndOverlap.AddDynamic(this, &AWRTardis::OnEndOverlap_CheckArea);
				CheckAreas.Add(CastedComp);
			}
		}
	}
	else
	{
		ensureMsgf(false, TEXT("CheckArea is not setted"));
	}

	TInlineComponentArray<UWRWidgetComponentBase*> WidgetComponents;
	this->GetComponents<UWRWidgetComponentBase>(WidgetComponents);

	if (WidgetComponents.Num() != 0)
	{
		for (auto& Comp : WidgetComponents)
		{
			UWRWidgetComponentBase* CastedComp = Cast<UWRWidgetComponentBase>(Comp);
			if (CastedComp == nullptr)
				continue;
						
			UWRUIPanel_TardisTimer* pTimer = Cast<UWRUIPanel_TardisTimer>(Comp->GetUserWidgetObject());
			if (pTimer == nullptr)
				continue;

			pTimer->SetBelongToBP();
// 			pTimer->ManualConstruct();			
// 			pTimer->SetContents("Stand by");
// 			pTimer->SetStandby();
			TimerWidgets.Add(CastedComp);
		}
	}
	else
	{
		ensureMsgf(false, TEXT("WidgetComponent is not setted"));
	}
}

void AWRTardis::Tick(float DeltaTime)
{
	AWREnvDoorMountedVehicleBase::Tick(DeltaTime);

	FWRDebugUIData data;
	data.Flag = EWRDebugUIBitFlag::Tardis;
	data.Actor = this;	
	data.Offset = FVector(0, 0, 100);
	MakeDebugString(data.ArrayContents);
	WRUIManager::Get()->UpdateDebugUI(data);
}

void AWRTardis::OnBeginOverlap_CheckArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Other == nullptr)
		return;

	AWRCharacter* pChar = Cast<AWRCharacter>(Other);
	if (pChar == nullptr || pChar->IsPlayer() == false)
		return;

	UpdateDoorState();
}

void AWRTardis::OnEndOverlap_CheckArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedComp == nullptr)
		return;

	TArray<AActor*> Actors;
	OverlappedComp->GetOverlappingActors(Actors, AWRCharacterPlayer::StaticClass());
	if (Actors.Num() > 0)
		return;

	UpdateDoorState();
}

void AWRTardis::OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform)
{	
	// Fake 처리가 있어서 AWREnvDoorMountedVehicleBase를 거치지 않는다
	AWREnvVehicleBase::OnChangeState(InInform);

	UpdateDoorState();

	// 실제 상태변화에 따른 처리
	switch (eState)
	{
	case EWRVehicleState::Open:
	{
		// + 오픈 후 잔여 탑승자(원래 없어져야할) 삭제처리? 해제? 내쫒음?
	}
	break;
	case EWRVehicleState::Close:
	{
		bWaitingToDepart = false;
		// + 탑승자가 아닌 사람들 내쫒기

		// + Start Group Warp (Ref "InInform.lstPassengers")
	}
	break;
	}
}

void AWRTardis::AddPassenger(const AID_t IN InAID)
{
	AWREnvVehicleBase::AddPassenger(InAID);
}

void AWRTardis::RemovePassenger(const AID_t IN InAID)
{
	AWREnvVehicleBase::RemovePassenger(InAID);

	// [ 2020-4-7 : kyu ] Waiting To Depart 중 모두 하차시 초기화
	if (Passengers.Num() == 0)
	{
		bWaitingToDepart = false;
		UpdateDoorState();
	}
}

void AWRTardis::OnSpawnComplete(const struct pt_Game_Actor::Inf_TardisAppear_G2U& IN InInform)
{
	FWREnvVehicleSpawnParams Params;
	Params.TableID		= InInform.tardisTID;
	Params.AID			= InInform.aid;
	Params.State		= (EWRVehicleState::en)InInform.vehicleState;
	Params.PassengerList.assign(InInform.lstPassengers.begin(), InInform.lstPassengers.end());
	Params.Location		= FVector(InInform.posX, InInform.posY, InInform.posZ);
	Params.Rotation		= FRotator(0.0f, InInform.angle, 0.0f);

	AWREnvDoorMountedVehicleBase::OnSpawnComplete(Params);

	FWRTardisTable* pTable = WRTableManager::Get()->FindRow<FWRTardisTable>(InInform.tardisTID);
	if (pTable != nullptr)
	{
		UpdateDoorState();
	}
}

void AWRTardis::OnStandby()
{
	for (UWRWidgetComponentBase* Comp : TimerWidgets)
	{
		UWRUIPanel_TardisTimer* pTimer = Cast<UWRUIPanel_TardisTimer>(Comp->GetUserWidgetObject());
		if (pTimer == nullptr)
			continue;

		pTimer->SetStandby();
		FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_TARDIS_CUSTOM_TEXT_TABLE_TID_STANDBY);
		if (pTextTable != nullptr)
			pTimer->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable,pTextTable->Localkey.ToString()).ToString());
		else
			pTimer->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_StandbyKey").ToString());
	}
}

void AWRTardis::OnDeparture()
{
	for (UWRWidgetComponentBase* Comp : TimerWidgets)
	{
		UWRUIPanel_TardisTimer* pTimer = Cast<UWRUIPanel_TardisTimer>(Comp->GetUserWidgetObject());
		if (pTimer == nullptr)
			continue;

		pTimer->SetTimerStop();
		FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_TARDIS_CUSTOM_TEXT_TABLE_TID_DEPARTURE);
		if (pTextTable != nullptr)
			pTimer->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable,pTextTable->Localkey.ToString()).ToString());
		else
			pTimer->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_DepartureKey").ToString());
	}
}

void AWRTardis::MakeDebugString(TArray<FString>& OUT OutString)
{
	// Departure State
	OutString.Add(FString::Printf(TEXT("Waiting To Depart : %d"), bWaitingToDepart));

	// Check Area
	TArray<AActor*> Actors;
	for (UShapeComponent* Comp : CheckAreas)
	{
		Comp->GetOverlappingActors(Actors, AWRCharacterPlayer::StaticClass());
	}
	OutString.Add(FString::Printf(TEXT("CheckArea : %d"), Actors.Num()));
}

void AWRTardis::OnGroupWarpTimerCount(const uint8 IN InReadyCountSecond, const bool IN InBegin)
{
	bWaitingToDepart = true;

	for (UWRWidgetComponentBase* Comp : TimerWidgets)
	{
		UWRUIPanel_TardisTimer* pTimer = Cast<UWRUIPanel_TardisTimer>(Comp->GetUserWidgetObject());
		if(pTimer == nullptr)
			continue;

		pTimer->SetTimerInfo(EWRTimerDirection::Decrease, InReadyCountSecond);
		FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_TARDIS_CUSTOM_TEXT_TABLE_TID_WAITINGTODEPART);
		if (pTextTable != nullptr)
			pTimer->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable,pTextTable->Localkey.ToString()).ToString());
		else
			pTimer->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_WaitingToDepartKey").ToString());
	}
}

void AWRTardis::OnGroupWarpStop()
{
	bWaitingToDepart = false;

	for (UWRWidgetComponentBase* Comp : TimerWidgets)
	{
		UWRUIPanel_TardisTimer* pTimer = Cast<UWRUIPanel_TardisTimer>(Comp->GetUserWidgetObject());
		if (pTimer == nullptr)
			continue;

		pTimer->SetTimerStop();
	}
}

bool AWRTardis::IsEmptyCheckAllArea()
{
	for (UShapeComponent* Comp : CheckAreas)
	{
		TArray<AActor*> Actors;
		Comp->GetOverlappingActors(Actors, AWRCharacterPlayer::StaticClass());
		if (Actors.Num() > 0)
			return false;
	}
	return true;
}

void AWRTardis::UpdateDoorState()
{
	// 원래 상태가 우선임
	switch (eState)
	{
	case EWRVehicleState::Open:
	case EWRVehicleState::Wait:
	{
		// 출발 대기중에는 열기
		if (bWaitingToDepart)
		{
			OnDoorAction(true);
		}
		// 모두 개방중에, 체크영역에도 사람이 없고, 탑승자도 없으면 닫음
		else if (IsEmptyCheckAllArea() && Passengers.Num() == 0)
		{
			OnDoorAction(false);
		}
		else
		{
			OnDoorAction(true);
		}
	}
	break;
	case EWRVehicleState::Close:
	{
		// 원래가 닫힌거면 닫힌것임
		OnDoorAction(false);
	}
	break;
	}
	
	// Door UI -> 실제 상태변화에 따른 처리
	switch (eState)
	{
	case EWRVehicleState::Open:
	case EWRVehicleState::Wait:
	{
		OnStandby();
	}
	break;
	case EWRVehicleState::Close:
	{
		OnDeparture();
	}
	break;
	}
}

void AWRTardis::OnDoorAction(const bool IN InOpen)
{
	if(IsDoorOpened() == false && InOpen)
	{
		OpenDoor(GetStationDoorIndex(CurrStation));
	}
	else if(IsDoorOpened() && InOpen == false)
	{
		CloseDoor(GetStationDoorIndex(CurrStation));
	}
}

int32 AWRTardis::GetStationDoorIndex(const uint8 IN InStation)
{
	return 1; // 별도 설정된 데이터 따로 없음
}
