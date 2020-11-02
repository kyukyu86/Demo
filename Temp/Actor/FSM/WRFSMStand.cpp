#include "WRFSMStand.h"



WRFSMStand::WRFSMStand()
	:WRFSMBase(EWRFSM::Stand, EWRFSM::Idle)
{
}


WRFSMStand::~WRFSMStand()
{
}

void WRFSMStand::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMStand::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMStand::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMStand::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMStand::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMStand::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}

EWRFSM WRFSMStand::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	EWRFSM NextFSM = WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);

	//전투 상태인데 무기가 보이지 않을때
	if (InChar->IsInBattle() && InChar->IsEquipedVisible(EWREquipPart::RightHand) == false)
	{
		return EWRFSM::Arm;
	}

	return NextFSM;
}
