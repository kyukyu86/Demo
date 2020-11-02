#include "WRFSMParrying.h"
#include "Actor/Character/WRCharacter.h"



WRFSMParrying::WRFSMParrying()
	:WRFSMAttack(EWRFSM::Parrying, EWRFSM::Idle)
{
}


WRFSMParrying::~WRFSMParrying()
{
}

void WRFSMParrying::Begin(const WRFSMInfo& FSMInfo)
{
	WRFSMAttack::Begin(FSMInfo);
	//ProcessAnimation(FSMInfo);
}

void WRFSMParrying::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	WRFSMAttack::Tick(OwnerCharacter, DeltaTime);
}

void WRFSMParrying::End(const WRFSMInfo& InReservedFSM)
{
	WRFSMAttack::End(InReservedFSM);
}

void WRFSMParrying::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMParrying::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMParrying::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMAttack::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;
	
	return true;
}
