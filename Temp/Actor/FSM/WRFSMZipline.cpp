#include "WRFSMZipline.h"
#include "Enum/EWRCharacter.h"



WRFSMZipline::WRFSMZipline()
	:WRFSMBase(EWRFSM::Zipline, EWRFSM::Idle)
{
}

WRFSMZipline::~WRFSMZipline()
{
}

void WRFSMZipline::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMZipline::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMZipline::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMZipline::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMZipline::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMZipline::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
