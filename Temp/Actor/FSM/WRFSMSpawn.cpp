#include "WRFSMSpawn.h"



WRFSMSpawn::WRFSMSpawn()
	:WRFSMBase(EWRFSM::Spawn, EWRFSM::Idle)
{
}


WRFSMSpawn::~WRFSMSpawn()
{
}

void WRFSMSpawn::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMSpawn::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMSpawn::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMSpawn::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMSpawn::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMSpawn::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
