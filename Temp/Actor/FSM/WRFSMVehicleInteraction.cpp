#include "WRFSMVehicleInteraction.h"

#include "../Character/WRCharacter.h"

#include "Define/WRPathDefine.h"

#include "Enum/EWRCharacter.h"

#include "Manager/WRSFXManager.h"
#include "../Vehicle/WRVehicle.h"
#include "Manager/WRTutorialManager.h"

WRFSMVehicleInteraction::WRFSMVehicleInteraction()
	:WRFSMBase(EWRFSM::VehicleInteraction, EWRFSM::DrivingIdle)
{

}

WRFSMVehicleInteraction::~WRFSMVehicleInteraction()
{

}

void WRFSMVehicleInteraction::Begin(const WRFSMInfo& FSMInfo)
{
	WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_VEHICLE_START, FSMInfo.OwnerCharacter->GetActorLocation());

	WRFSMInfo CopiedFSM = FSMInfo;
	if (CopiedFSM.PreviousFSMType == EWRFSM::EnterVehicle)
	{
		CopiedFSM.JumpToSectionName = "VehicleEngineStart";
	}

	if (FSMInfo.OwnerCharacter->IsMe())
	{
		// [ 2020-9-2 : kyu ] Â÷·®Å¾½Â Æ©Åä¸®¾ó
		WRTutorialManager::Get()->OnInvokeReadyEvent(EWRTutorialConditionType::BoardingVehicle, 0);
	}

	ProcessAnimation(CopiedFSM);
}

void WRFSMVehicleInteraction::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMVehicleInteraction::End(const WRFSMInfo& InReservedFSM)
{
	if (InReservedFSM.OwnerCharacter->GetBoardingVehicle())
	{
		InReservedFSM.OwnerCharacter->GetBoardingVehicle()->ToggleAutoPilotIndicator(false);
	}
}

bool WRFSMVehicleInteraction::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::DrivingIdle:
	case EWRFSM::ExitVehicle:
	{
		return true;
	}
	break;
	default:
	{
		return false;
	}
	break;
	}
}

EWRFSM WRFSMVehicleInteraction::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	return WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);
}

void WRFSMVehicleInteraction::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMVehicleInteraction::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

