#include "WRFSMEventAction.h"



WRFSMEventAction::WRFSMEventAction()
	:WRFSMBase(EWRFSM::SocialAnimation, EWRFSM::Idle)
{
}


WRFSMEventAction::~WRFSMEventAction()
{
}

void WRFSMEventAction::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMEventAction::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMEventAction::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMEventAction::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMEventAction::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMEventAction::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
