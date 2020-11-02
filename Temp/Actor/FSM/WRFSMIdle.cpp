#include "WRFSMIdle.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRWeaponTable.h"
#include "../Animation/WRAnimInstance.h"



WRFSMIdle::WRFSMIdle()
	:WRFSMBase(EWRFSM::Idle, EWRFSM::Idle)
{
}


WRFSMIdle::~WRFSMIdle()
{
}

void WRFSMIdle::Begin(const WRFSMInfo& FSMInfo)
{
	UWRAnimInstance* AnimInst = FSMInfo.OwnerCharacter->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->Montage_Stop(0.25f);
	}

	if (FSMInfo.OwnerCharacter->IsPlayer())
	{
		bool bPlayMontage = true;
		if (FSMInfo.VoidPtr != nullptr)
		{
			bPlayMontage = *((bool*)(FSMInfo.VoidPtr));
		}

		if (bPlayMontage)
		{
			if (FSMInfo.OwnerCharacter->IsInBattle() || FSMInfo.OwnerCharacter->GetTemporaryTID() != INVALID_TABLE_TID)
			{
				ProcessAnimation(FSMInfo);
			}
		}
	}

	if (nullptr == FSMInfo.OwnerCharacter)
		return;

	//ProcessAnimation(FSMInfo);
	
	// [ 2020-3-9 : animal14 ] 임시 코드
	FSMInfo.OwnerCharacter->MakeUnderWaterDisplay();
}

void WRFSMIdle::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMIdle::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMIdle::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMIdle::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMIdle::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}
