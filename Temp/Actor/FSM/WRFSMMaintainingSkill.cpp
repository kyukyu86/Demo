#include "WRFSMMaintainingSkill.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "../Character/WRCharacterPlayer.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "../Character/WRCharacterMine.h"



WRFSMMaintainingSkill::WRFSMMaintainingSkill()
	:WRFSMAttack(EWRFSM::MaintainingSkill, EWRFSM::MaintainingSkill)
{

}

WRFSMMaintainingSkill::~WRFSMMaintainingSkill()
{

}

void WRFSMMaintainingSkill::Begin(const WRFSMInfo& FSMInfo)
{
	WRFSMAttack::Begin(FSMInfo);

	FWRSkillTable* pSkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(FSMInfo.SkillTableID, FSMInfo.OwnerCharacter->GetSkillTableFileName());
	if (pSkillTable == nullptr)
		return;
	
	switch (pSkillTable->SkillType)
	{
	case EWRSkillType::Hook:
	{
		if (FSMInfo.OwnerCharacter->IsMe())
		{
			AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(FSMInfo.OwnerCharacter);
			if (CastedMine != nullptr)
			{
				CastedMine->SetUsingSyncHMDHead(false);
				CastedMine->SetHMDRotationToUpdate(FRotator::ZeroRotator);
				CastedMine->SetEnableSyncHMDTick(false);
			}
		}
	}
	break;
	}
}

void WRFSMMaintainingSkill::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	WRFSMAttack::Tick(OwnerCharacter, DeltaTime);

}

void WRFSMMaintainingSkill::End(const WRFSMInfo& InReservedFSM)
{
	switch (InReservedFSM.PreviousFSMType)
	{
	case EWRFSM::MaintainingSkill:
	{
		FWRSkillTable* pSkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InReservedFSM.PreviousSkillTableID, InReservedFSM.OwnerCharacter->GetSkillTableFileName());
		if (pSkillTable != nullptr && pSkillTable->SkillType == EWRSkillType::Hook)
		{
			AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(InReservedFSM.OwnerCharacter);
			if (CastedPlayer != nullptr)
			{
				CastedPlayer->CutOffHookChain(true);
			}

			if (InReservedFSM.OwnerCharacter->IsMe())
			{
				AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(InReservedFSM.OwnerCharacter);
				if (CastedMine != nullptr)
				{
					CastedMine->SetUsingSyncHMDHead(true);
					CastedMine->SetEnableSyncHMDTick(true);
				}
			}
		}
	}
	break;
	default:
	{

	}
	break;
	}

	WRFSMAttack::End(InReservedFSM);

}

bool WRFSMMaintainingSkill::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	case EWRFSM::MaintainingSkill:
	{
		UWRActorComponentFSM* FSMComponent = InFSMInfo.OwnerCharacter->GetComponent<UWRActorComponentFSM>();
		if (nullptr == FSMComponent)
			return false;

		return FSMComponent->IsSkipAnimation();
	}
	break;
	case EWRFSM::Arm:
	case EWRFSM::Disarm:
	{
		return false;
	}
	break;
	default:
	{

	}
	break;
	}

	return true;
}

EWRFSM WRFSMMaintainingSkill::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	EWRFSM NextFSM = WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);

	// 	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InChar->GetActorID());
	// 	if (CombatInfo && CombatInfo->GetSkillTable())
	// 	{
	// 		FWRSkillTable* SkillTable = CombatInfo->GetSkillTable();
	// 		FWRPlayerSkillTable* PrecedingSkillTable = WRTableManager::Get()->FindRow<FWRPlayerSkillTable>(SkillTable->PrecedingSkillTID);
	// 		if (PrecedingSkillTable)
	// 		{
	// 
	// 		}
	//	}

	return NextFSM;
}

void WRFSMMaintainingSkill::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{
	WRFSMAttack::ProcessWhenAnimationEnd(OwnerCharacter);

}

void WRFSMMaintainingSkill::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{
	WRFSMAttack::ProcessWhenAnimationInterrupted(OwnerCharacter);

}
