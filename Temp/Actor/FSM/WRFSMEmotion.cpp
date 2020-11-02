#include "WRFSMEmotion.h"



WRFSMEmotion::WRFSMEmotion()
	:WRFSMBase(EWRFSM::Emotion, EWRFSM::Idle)
{
}


WRFSMEmotion::~WRFSMEmotion()
{
}

void WRFSMEmotion::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMEmotion::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMEmotion::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMEmotion::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMEmotion::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMEmotion::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
