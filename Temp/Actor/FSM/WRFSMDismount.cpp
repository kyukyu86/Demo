#include "WRFSMDismount.h"



WRFSMDismount::WRFSMDismount()
	:WRFSMBase(EWRFSM::Dismount, EWRFSM::Idle)
{
}


WRFSMDismount::~WRFSMDismount()
{
}

void WRFSMDismount::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo.OwnerCharacter, (int32)EWRFSM::Dismount);
}

void WRFSMDismount::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMDismount::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMDismount::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDismount::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMDismount::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
