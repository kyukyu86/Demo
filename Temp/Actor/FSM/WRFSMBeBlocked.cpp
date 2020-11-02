#include "WRFSMBeBlocked.h"
#include "Table/WRWeaponTable.h"
#include "Table/Base/WRTableManager.h"



WRFSMBeBlocked::WRFSMBeBlocked()
	:WRFSMBase(EWRFSM::BeBlocked, EWRFSM::Idle)
{
}


WRFSMBeBlocked::~WRFSMBeBlocked()
{
}

void WRFSMBeBlocked::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMBeBlocked::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMBeBlocked::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMBeBlocked::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMBeBlocked::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMBeBlocked::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	case EWRFSM::Defense:
	case EWRFSM::Move:
	{
		return false;
	}
	default:
		break;
	}
	
	return true;
}
