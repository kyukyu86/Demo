#include "WRFSMTalk.h"



WRFSMTalk::WRFSMTalk()
	:WRFSMBase(EWRFSM::Talk, EWRFSM::Idle)
{
}


WRFSMTalk::~WRFSMTalk()
{
}

void WRFSMTalk::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMTalk::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMTalk::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMTalk::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMTalk::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMTalk::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
