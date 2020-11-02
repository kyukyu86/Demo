#include "WRFSMDown.h"



WRFSMDown::WRFSMDown()
	:WRFSMBase(EWRFSM::Down, EWRFSM::Stand)
{
}


WRFSMDown::~WRFSMDown()
{
}

void WRFSMDown::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo.OwnerCharacter, (int32)EWRFSM::Down);
}

void WRFSMDown::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMDown::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMDown::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDown::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMDown::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
