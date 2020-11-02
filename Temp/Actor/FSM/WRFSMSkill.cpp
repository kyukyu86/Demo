#include "WRFSMSkill.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRTutorialManager.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "Manager/WRCombatManager.h"
#include "CombatSystem/Combat/WRCombatInfo.h"
#include "../Character/WRCharacterPlayer.h"



WRFSMSkill::WRFSMSkill()
	:WRFSMAttack(EWRFSM::Skill, EWRFSM::Idle)
{
}


WRFSMSkill::WRFSMSkill(EWRFSM InType, EWRFSM InLinkedFsmWhenAnimationEnd)
	:WRFSMAttack(EWRFSM::Skill, EWRFSM::Idle)
{

}

WRFSMSkill::~WRFSMSkill()
{
}

void WRFSMSkill::Begin(const WRFSMInfo& FSMInfo)
{
	WRFSMAttack::Begin(FSMInfo);

	// [ 2019-10-7 : kyu ] Hook이 아닌 공격(스킬)일 경우 갈고리 끊어짐 추가
	FWRSkillTable* pSkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(FSMInfo.SkillTableID, FSMInfo.OwnerCharacter->GetSkillTableFileName());
	if (pSkillTable != nullptr && pSkillTable->SkillType != EWRSkillType::Hook)
	{
		WRCharacterManager::Get()->CutOffMyHookChain(false);
	}

	// [ 2019-11-18 : kyu ] 튜토리얼
	WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::SkillUse, FSMInfo.SkillTableID);
}

void WRFSMSkill::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	WRFSMAttack::Tick(OwnerCharacter, DeltaTime);
}

void WRFSMSkill::End(const WRFSMInfo& InReservedFSM)
{
	WRFSMAttack::End(InReservedFSM);
}

void WRFSMSkill::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{
	WRFSMAttack::ProcessWhenAnimationEnd(OwnerCharacter);
}

void WRFSMSkill::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{
	WRFSMAttack::ProcessWhenAnimationInterrupted(OwnerCharacter);

	if (nullptr == OwnerCharacter)
		return;

// 	UWRCapsuleCollisionComponent* BlockingCapsule = OwnerCharacter->FindCapsuleCollisionComponent(EWRCollisionType::Shield);
// 	if (BlockingCapsule)
// 	{
// 		BlockingCapsule->DisableCollisionResponse();
// 	}
}

bool WRFSMSkill::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
// 	if (WRFSMAttack::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
// 		return false;

	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	{
		UWRActorComponentFSM* FSMComponent = InFSMInfo.OwnerCharacter->GetComponent<UWRActorComponentFSM>();
		if (nullptr == FSMComponent)
			return false;

		return FSMComponent->IsSkipAnimation();
	}
	break;
	case EWRFSM::Arm:
	case EWRFSM::Disarm:
	case EWRFSM::Damage:
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






