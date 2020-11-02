#include "WRFSMDance.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Network/IMMessageSender.h"



WRFSMDance::WRFSMDance()
	:WRFSMBase(EWRFSM::Dance, EWRFSM::Idle)
{
}


WRFSMDance::~WRFSMDance()
{
}

void WRFSMDance::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);

	// [ 2020-3-9 : animal14 ] 임시 코드
	FSMInfo.OwnerCharacter->MakeUnderWaterDisplay();
	if (FSMInfo.OwnerCharacter)
	{
		FSMInfo.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::LeftHand, false);
		FSMInfo.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::RightHand, false);
	}
}

void WRFSMDance::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMDance::End(const WRFSMInfo& InReservedFSM)
{
	if (InReservedFSM.OwnerCharacter)
	{
		if (InReservedFSM.OwnerCharacter->IsInBattle())
		{
			InReservedFSM.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::LeftHand, true);
			InReservedFSM.OwnerCharacter->SetEquipPartVisibility(EWREquipPart::RightHand, true);
		}
	}
}

void WRFSMDance::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{
	if (OwnerCharacter)
	{
		UWRActorComponentDanceModeForMine* DanceModeComponent = OwnerCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent)
		{
			DanceModeComponent->OnFinishDanceEnded();
		}
	}
}

void WRFSMDance::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{
	if (OwnerCharacter && OwnerCharacter->IsMe())
	{
		IMMessageSender::RequestEndDance();
	}
}

bool WRFSMDance::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	if (InFSMInfo.FSMType == EWRFSM::Disarm)
		return false;

	return true;
}
