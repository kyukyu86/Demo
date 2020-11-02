#include "WRFSMCC.h"



WRFSMCC::WRFSMCC()
	:WRFSMBase(EWRFSM::CC, EWRFSM::Idle)
{
}


WRFSMCC::~WRFSMCC()
{
}

void WRFSMCC::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);

#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
	FSMInfo.OwnerCharacter->ClearAIMoveTo();
#endif // DEF_FIX_CLEARAIMOVETO
}

void WRFSMCC::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMCC::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMCC::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMCC::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMCC::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	case EWRFSM::Move:
	case EWRFSM::Damage:
	case EWRFSM::Interact:
	case EWRFSM::Emotion:
	case EWRFSM::Ride:
	case EWRFSM::Acting:
	case EWRFSM::Defense:
	case EWRFSM::BeBlocked:
	case EWRFSM::Parrying:
	case EWRFSM::BeParried:
	{
		return false;
	}
	default:
		break;
	}

	return true;
}
