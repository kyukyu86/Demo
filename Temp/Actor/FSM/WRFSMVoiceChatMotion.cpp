#include "WRFSMVoiceChatMotion.h"



WRFSMVoiceChatMotion::WRFSMVoiceChatMotion()
	:WRFSMBase(EWRFSM::VoiceChatMotion, EWRFSM::Idle)
{
}


WRFSMVoiceChatMotion::~WRFSMVoiceChatMotion()
{
}

void WRFSMVoiceChatMotion::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);	
}

void WRFSMVoiceChatMotion::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMVoiceChatMotion::End(const WRFSMInfo& InReservedFSM)
{
}

void WRFSMVoiceChatMotion::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMVoiceChatMotion::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMVoiceChatMotion::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	if (InFSMInfo.FSMType == EWRFSM::Disarm)
		return false;

	return true;
}
