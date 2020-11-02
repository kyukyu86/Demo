#include "WRFSMSocialAnimation.h"



WRFSMSocialAnimation::WRFSMSocialAnimation()
	:WRFSMBase(EWRFSM::SocialAnimation, EWRFSM::Idle)
{
}


WRFSMSocialAnimation::~WRFSMSocialAnimation()
{
}

void WRFSMSocialAnimation::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMSocialAnimation::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMSocialAnimation::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMSocialAnimation::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMSocialAnimation::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMSocialAnimation::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
