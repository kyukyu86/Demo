#include "WRFSMAvoid.h"

#include "Manager/WRCharacterManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRConsoleManager.h"

#include "../Animation/WRAnimInstance.h"

#include "Component/Character/WRActorComponentManageMaterial.h"

#include "../Character/WRCharacterMine.h"


WRFSMAvoid::WRFSMAvoid()
	:WRFSMBase(EWRFSM::Avoiding, EWRFSM::Idle)
{
}


WRFSMAvoid::~WRFSMAvoid()
{
}

void WRFSMAvoid::Begin(const WRFSMInfo& FSMInfo)
{
	//WRFSMAttack::Begin(FSMInfo);
	ProcessAnimation(FSMInfo);

	if (FSMInfo.OwnerCharacter == nullptr)
	{
		return;
	}

	if (FSMInfo.OwnerCharacter->IsMe() == false)
	{
		return;
	}

	if (WRConsoleManager::Get()->GetBool(EWRConsole::BlackOutIn_Enable))
	{
		AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(FSMInfo.OwnerCharacter);
		if (MyCharacter == nullptr)
		{
			return;
		}

		MyCharacter->DashOut();
	}
	else
	{
		float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::Dodge_Duration);
		WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::Dodge, Duration);
	}
}

void WRFSMAvoid::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	//WRFSMAttack::Tick(OwnerCharacter, DeltaTime);
}

void WRFSMAvoid::End(const WRFSMInfo& InReservedFSM)
{
	//WRFSMAttack::End(InReservedFSM);
}

bool WRFSMAvoid::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
// 	if (WRFSMAttack::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
// 		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	{
		return false;
	}
	break;
	default:
		break;
	}

	return true;
}

void WRFSMAvoid::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMAvoid::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}
