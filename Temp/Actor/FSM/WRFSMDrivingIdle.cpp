
#include "WRFSMDrivingIdle.h"
#include "../Vehicle/WRVehicle.h"
#include "Manager/WRVehicleManager.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "../Character/WRCharacterMine.h"
#include "Manager/WRAutoPilotManager.h"

WRFSMDrivingIdle::WRFSMDrivingIdle()
	:WRFSMBase(EWRFSM::DrivingIdle, EWRFSM::DrivingIdle)
{

}

WRFSMDrivingIdle::~WRFSMDrivingIdle()
{

}

void WRFSMDrivingIdle::Begin(const WRFSMInfo& FSMInfo)
{
	WRFSMInfo tempFSM = FSMInfo;
	//��� ž���� ���¸� ��ġ ����
	
	AWRVehicle* pVehicle = FSMInfo.OwnerCharacter->GetBoardingVehicle();
	if (pVehicle)
	{
		if (FSMInfo.PreviousFSMType == EWRFSM::EnterVehicle)
		{
			FSMInfo.OwnerCharacter->SetActorRelativeTransform(FTransform::Identity);
		}

		if (tempFSM.JumpToSectionName.IsEmpty())
		{
			EWRVehicleSeatSide SeatSide = WRVehicleManager::Get()->GetSeatingSide(FSMInfo.OwnerCharacter->GetActorID());
			tempFSM.JumpToSectionName = WREnumUtility::EnumToString("EWRFSM", EWRFSM::DrivingIdle);
			if (FSMInfo.OwnerCharacter->IsMe() && pVehicle->DriverIsMe() && WRAutoPilotManager::Get()->IsActiveAutoPilot())
			{
				tempFSM.JumpToSectionName += "R";	//���� �������̹Ƿ� �����뿡�� ���� ����
				FSMInfo.OwnerCharacter->SetSkeletalMeshComponentToFABRIK();
			}
			else
			{				
				if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
				{
					tempFSM.JumpToSectionName += "L";

					//FABRIK�� ���̷�Ż�޽� ����
					FSMInfo.OwnerCharacter->SetSkeletalMeshComponentToFABRIK(pVehicle->GetMesh());
				}
				else
				{
					tempFSM.JumpToSectionName += "R";
					FSMInfo.OwnerCharacter->SetSkeletalMeshComponentToFABRIK();
				}
			}
		}

		ProcessAnimation(tempFSM);
	}
}

void WRFSMDrivingIdle::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMDrivingIdle::End(const WRFSMInfo& InReservedFSM)
{
	if (InReservedFSM.FSMType != EWRFSM::DrivingIdle)
	{
		//FABRIK�� ���̷�Ż�޽� ����
		InReservedFSM.OwnerCharacter->SetSkeletalMeshComponentToFABRIK();
	}
}

bool WRFSMDrivingIdle::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::ExitVehicle:
	case EWRFSM::DrivingIdle:
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

EWRFSM WRFSMDrivingIdle::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	return WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);
}

void WRFSMDrivingIdle::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDrivingIdle::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}
