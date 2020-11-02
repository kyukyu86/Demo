#include "WRFSMActing.h"



WRFSMActing::WRFSMActing()
	:WRFSMBase(EWRFSM::Acting, EWRFSM::Idle)
{
}


WRFSMActing::~WRFSMActing()
{
}

void WRFSMActing::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo.OwnerCharacter, (int32)EWRFSM::Acting);
}

void WRFSMActing::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMActing::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMActing::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMActing::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMActing::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
