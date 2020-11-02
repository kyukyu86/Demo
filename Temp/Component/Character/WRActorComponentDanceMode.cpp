// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentDanceMode.h"

#include "Enum/EWRCharacter.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterManager.h"

#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacter.h"

#include "Table/WRSocialTable.h"
#include "Table/Base/WRTableManager.h"

#include "UI/Panel/WRUIPanel_DanceMode.h"

#include "Define/WRPathDefine.h"
#include "Utility/WRUIUtility.h"
#include "WRActorComponentFSM.h"
#include "Network/IMMessageSender.h"
#include "../../Table/WRPlayerTable.h"
#include "../../Network/Share/Define/Define_BluePrint.h"
#include "../../Utility/WRActorUtility.h"


void UWRActorComponentDanceMode::OnCreateComponent()
{
	Super::OnCreateComponent();


}

void UWRActorComponentDanceMode::OnDestroyComponent()
{
	Super::OnDestroyComponent();
}

void UWRActorComponentDanceMode::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (CurDanceStep != EWRDanceStep::None)
	//{
	//	PointTime += DeltaTime;
	//	if (PointTime > 0.5f)
	//	{
	//		PointTime = 0.f;
	//		IncreaseDancePoint(10);
	//	}
	//}
}

void UWRActorComponentDanceMode::StartDanceMode(const WRTableID IN InDanceGroupTID, const bool IN bInRetry /*= false*/)
{
	const FWRDanceGroupTable* DanceGroupTable = WRTableManager::Get()->FindRow<FWRDanceGroupTable>(InDanceGroupTID);
	if (DanceGroupTable == nullptr)
		return;

	if (DanceGroupTable->DanceTIDs.Num() == 0)
		return;

	CurDanceGroupTable = DanceGroupTable;
	CurIndex = 0;

	AWRCharacterPlayer* PlayerCharacter = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->SetUsingSyncHMDHead(false);
	}	

	SetDanceStep(CurDanceGroupTable->DanceTIDs[CurIndex]);
}

void UWRActorComponentDanceMode::StartDanceMode(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID)
{
	const FWRDanceGroupTable* DanceGroupTable = WRTableManager::Get()->FindRow<FWRDanceGroupTable>(InDanceGroupTID);
	if (DanceGroupTable == nullptr)
		return;

	if (DanceGroupTable->DanceTIDs.Num() == 0)
		return;

	CurIndex = DanceGroupTable->DanceTIDs.Find(InDanceTID);
	if (CurIndex == INDEX_NONE)
	{
		CurIndex = 0;
		return;
	}	

	CurDanceGroupTable = DanceGroupTable;

	AWRCharacterPlayer* PlayerCharacter = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->SetUsingSyncHMDHead(false);
	}

	SetDanceStep(CurDanceGroupTable->DanceTIDs[CurIndex]);
}

void UWRActorComponentDanceMode::StopDanceMode(const bool IN bInStopFSM /*= true*/)
{
	if(bInStopFSM)
		GetOwnerCharacter()->DetermineFSMSignal(EWRFSM::Idle);
	CurDanceGroupTable = nullptr;
	CurDanceTable = nullptr;

	AWRCharacterPlayer* PlayerCharacter = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->SetUsingSyncHMDHead(true);
	}
}

bool UWRActorComponentDanceMode::IsDanceModeOn() const
{
	if (CurDanceGroupTable == nullptr)
		return false;

	return true;
}

void UWRActorComponentDanceMode::OnInformChangeDanceStepReceived(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID)
{
	if (InDanceGroupTID == 0 || InDanceTID == 0)
	{
		StopDanceMode();
	}
	else
	{
		if (GetOwnerCharacter()->GetCurrentFSMType() == EWRFSM::Dance)
		{
			SetDanceStep(InDanceTID);
		}
		else
		{
			StartDanceMode(InDanceGroupTID);
		}	
	}
}

void UWRActorComponentDanceMode::SetDanceStep(const WRTableID IN InDanceTID)
{
	FWRDanceTable* DanceTable = WRTableManager::Get()->FindRow<FWRDanceTable>(InDanceTID);
	if (DanceTable == nullptr)
	{
		StopDanceMode();
		return;
	}

	CurDanceTable = DanceTable;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::Dance;
	
	FSMInfo.MontageTID = DanceTable->BaseSubMontageTID;
	FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(DanceTable->BaseSubMontageName);
	
	
	if (OwnerCharacter->GetCurrentFSMType() == EWRFSM::Dance)
	{
		OwnerCharacter->PlayAnimJumpToSection(EWRFSM::Dance, FSMInfo.JumpToSectionName);
	}
	else
	{
		OwnerCharacter->DetermineFSMSignal(FSMInfo);
	}	

	if(CurDanceTable->StartSound.IsNone() == false)
		WRSFXManager::Get()->SpawnAttached(WRUIUtility::ConvertFNameToFString(CurDanceTable->StartSound), OwnerCharacter->GetRootComponent());

	
	if (CurDanceTable->StartFX.IsNone() == false)
	{
		WRVFXManager::Get()->Spawn(WRUIUtility::ConvertFNameToFString(CurDanceTable->StartFX), OwnerCharacter, nullptr);
	}
}