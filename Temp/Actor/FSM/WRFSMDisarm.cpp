#include "WRFSMDisarm.h"



WRFSMDisarm::WRFSMDisarm() : WRFSMBase(EWRFSM::Disarm, EWRFSM::Idle, true)
{
}


WRFSMDisarm::~WRFSMDisarm()
{
}

void WRFSMDisarm::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);
}

void WRFSMDisarm::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMDisarm::End(const WRFSMInfo& InReservedFSM)
{
	//fsm이 종료되는 시점에는 무조건 무기를 안보이게 해야 함
	if (InReservedFSM.OwnerCharacter)
	{
		InReservedFSM.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::RightHand, false);
		InReservedFSM.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::LeftHand, false);
	}
}

bool WRFSMDisarm::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}

EWRFSM WRFSMDisarm::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	EWRFSM NextFSM = WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);

	return NextFSM;
}

void WRFSMDisarm::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDisarm::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}
