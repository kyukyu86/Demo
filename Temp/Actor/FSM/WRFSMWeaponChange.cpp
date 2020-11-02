#include "WRFSMWeaponChange.h"
#include "Table/WRWeaponTable.h"
#include "Table/Base/WRTableManager.h"
#include "../Character/WRCharacterMine.h"
#include "Enum/EWRCharacter.h"


WRFSMWeaponChange::WRFSMWeaponChange()
	:WRFSMBase(EWRFSM::WeaponChange, EWRFSM::Idle, true)
{
}


WRFSMWeaponChange::~WRFSMWeaponChange()
{
}

void WRFSMWeaponChange::Begin(const WRFSMInfo& FSMInfo)
{
	if (FSMInfo.VoidPtr == nullptr)
		return;

	WRTableID TableID = INVALID_TABLE_TID;
	ChangeEquipmentType = *(static_cast<EWRChangeEquipment*>(FSMInfo.VoidPtr));
	switch (ChangeEquipmentType)
	{
	case EWRChangeEquipment::MainWeapon:
	{
		TableID = FSMInfo.OwnerCharacter->GetRightWeaponTID();
	}
	break;
	case EWRChangeEquipment::SubWeapon:
	{
		TableID = FSMInfo.OwnerCharacter->GetLeftWeaponTID();
	}
	break;
	case EWRChangeEquipment::Temporary:
	{
		TableID = FSMInfo.OwnerCharacter->GetTemporaryTID();
	}
	break;
	default:
	{
		return;
	}
	break;
	}

	FWRWeaponTable* pWeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(TableID);
	if (pWeaponTable == nullptr)
		return;

	FString SectionName = TEXT("Equip");
	if (FSMInfo.JumpToSectionName != "")
	{
		SectionName = FSMInfo.JumpToSectionName;
	}

	int32 MontageID = pWeaponTable->BaseMontageTID;
	AWRCharacterMine* pMe = Cast<AWRCharacterMine>(FSMInfo.OwnerCharacter);
	if (pMe && pMe->IsShowing3rdCharacter())
	{
		MontageID = pWeaponTable->BaseSubMontageTID;
	}

	ProcessAnimation(FSMInfo.OwnerCharacter, MontageID, SectionName);
}

void WRFSMWeaponChange::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMWeaponChange::End(const WRFSMInfo& InReservedFSM)
{
	AWRCharacterPlayer* Character = Cast<AWRCharacterPlayer>(InReservedFSM.OwnerCharacter);

	if (Character)
	{
		Character->ChangeWeaponWithoutAnimation(ChangeEquipmentType);
	}
}

void WRFSMWeaponChange::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMWeaponChange::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMWeaponChange::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}

EWRFSM WRFSMWeaponChange::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	if (InChar->IsInBattle() || InChar->GetTemporaryTID() != INVALID_TABLE_TID)
	{
		return EWRFSM::Idle;
	}
	else
	{
		return EWRFSM::Disarm;
	}
}
