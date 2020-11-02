#include "WRFSMDead.h"

#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Manager/WRVFXManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRCharacterManager.h"

#include "Define/WRPathDefine.h"

#include "Utility/WRActorUtility.h"

#include "Core/WRGameInstance.h"

WRFSMDead::WRFSMDead()
	:WRFSMBase(EWRFSM::Dead, EWRFSM::None)
{
}


WRFSMDead::~WRFSMDead()
{
}

void WRFSMDead::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);

	//// [ 2019-10-21 : animal14 ] 캡슐 충돌 비활성화
	//WRActorUtility::SetComponentCollisionEnabled(FSMInfo.OwnerCharacter->GetRootComponent(), ECollisionEnabled::NoCollision);
	//
	//// [ 2019-10-21 : animal14 ] 피직스 활성화
	//FSMInfo.OwnerCharacter->SetSimulatePhysics(true);

	//// [ 2019-10-21 : animal14 ] 
	//FSMInfo.OwnerCharacter->AddImpulseToAllBodiesBelow(0.0F);
}

void WRFSMDead::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	//OwnerCharacter->TickDead(DeltaTime);
}

void WRFSMDead::End(const WRFSMInfo& InReservedFSM)
{
	
}

void WRFSMDead::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDead::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMDead::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Spawn:
	case EWRFSM::Revival:
	case EWRFSM::Stand:
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
