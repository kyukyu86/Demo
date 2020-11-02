#include "WRFSMRide.h"



WRFSMRide::WRFSMRide()
	:WRFSMBase(EWRFSM::Ride, EWRFSM::Idle)
{
}


WRFSMRide::~WRFSMRide()
{
}

void WRFSMRide::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo.OwnerCharacter, (int32)EWRFSM::Ride);
}

void WRFSMRide::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMRide::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMRide::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMRide::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMRide::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
