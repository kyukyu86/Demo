#include "WRFSMDefense.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"



WRFSMDefense::WRFSMDefense()
	:WRFSMAttack(EWRFSM::Defense, EWRFSM::Idle)
{
}


WRFSMDefense::~WRFSMDefense()
{
}

void WRFSMDefense::Begin(const WRFSMInfo& FSMInfo)
{
	WRFSMAttack::Begin(FSMInfo);
}

void WRFSMDefense::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	WRFSMAttack::Tick(OwnerCharacter, DeltaTime);
}

void WRFSMDefense::End(const WRFSMInfo& InReservedFSM)
{
	WRFSMAttack::End(InReservedFSM);
}

void WRFSMDefense::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDefense::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMDefense::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
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
