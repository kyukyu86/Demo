#include "WRFSMArm.h"



WRFSMArm::WRFSMArm() : WRFSMBase(EWRFSM::Arm, EWRFSM::Idle, true)
{
}


WRFSMArm::~WRFSMArm()
{
}

void WRFSMArm::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);

	//fsm�� ����Ǵ� �������� ������ ���⸦ �Ⱥ��̰� �ؾ� ��
	if (FSMInfo.OwnerCharacter)
	{
		FSMInfo.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::RightHand, true);
		FSMInfo.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::LeftHand, true);
	}
}

void WRFSMArm::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMArm::End(const WRFSMInfo& InReservedFSM)
{

}

bool WRFSMArm::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}

EWRFSM WRFSMArm::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	EWRFSM NextFSM = WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);

	return NextFSM;
}

void WRFSMArm::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMArm::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}
