#include "WRFSMDamage.h"

#include "Manager/WRInputSystemManager.h"
#include "Manager/WRConsoleManager.h"

#include "Component/Character/WRActorComponentManageMaterial.h"

WRFSMDamage::WRFSMDamage()
	:WRFSMBase(EWRFSM::Damage, EWRFSM::Idle)
{
}


WRFSMDamage::~WRFSMDamage()
{
}

void WRFSMDamage::Begin(const WRFSMInfo& FSMInfo)
{
	if (FSMInfo.PreviousFSMType == EWRFSM::BeParried)
	{
		ProcessAnimation(FSMInfo.OwnerCharacter, FSMInfo.OwnerCharacter->GetMontageTID(FSMInfo.FSMType, "cdamage"), "cdamage");
	}
	else
	{
		ProcessAnimation(FSMInfo);
	}

	if (FSMInfo.OwnerCharacter->IsMe())
	{
		WRInputSystemManager::Get()->OnMyCharacterDameged();
	}

	if (FSMInfo.OwnerCharacter == nullptr)
	{
		return;
	}

#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
	if (FSMInfo.OwnerCharacter->IsAIMoveTo())
	{
		FSMInfo.OwnerCharacter->ClearAIMoveTo();
	}
#endif // DEF_FIX_CLEARAIMOVETO
}

void WRFSMDamage::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
}

void WRFSMDamage::End(const WRFSMInfo& InReservedFSM)
{
}

void WRFSMDamage::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDamage::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMDamage::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	{
		return false;
	}
	break;
	default:
		break;
	}

	return true;
}

EWRFSM WRFSMDamage::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	if (InChar->GetHP() <= 0)
	{
		return EWRFSM::Die;
	}
	else
	{
		return LinkedFSMWhenAnimationEnd;
	}
}
