
#include "WRFSMInteract.h"
#include "Table/WRPlayerTable.h"
#include "Manager/WRInputSystemManager.h"
#include "../Character/WRCharacterIO.h"
#include "../Character/WRCharacterMine.h"
#include "Component/Character/WRActorComponentInteraction.h"


WRFSMInteract::WRFSMInteract()
	:WRFSMBase(EWRFSM::Interact, EWRFSM::Idle)
{
}


WRFSMInteract::~WRFSMInteract()
{
}

void WRFSMInteract::Begin(const WRFSMInfo& FSMInfo)
{
	if (FSMInfo.OwnerCharacter == nullptr)
		return;

	if (FSMInfo.VoidPtr == nullptr)
		return;

	ProcessAnimation(FSMInfo);
}

void WRFSMInteract::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMInteract::End(const WRFSMInfo& InReservedFSM)
{
	if (InReservedFSM.OwnerCharacter == nullptr)
		return;

}

void WRFSMInteract::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMInteract::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMInteract::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Arm:
	case EWRFSM::Disarm:
	{
		return false;
	}
	break;
	default:
	{

	}
	break;
	}

	return true;
}