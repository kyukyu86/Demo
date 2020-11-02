#include "WRFSMBeParried.h"

#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Manager/WRMaterialParameterCollectionManager.h"

WRFSMBeParried::WRFSMBeParried()
	:WRFSMBase(EWRFSM::BeParried, EWRFSM::Idle)
{
}


WRFSMBeParried::~WRFSMBeParried()
{
}

void WRFSMBeParried::Begin(const WRFSMInfo& FSMInfo)
{
	ProcessAnimation(FSMInfo);

	if (FSMInfo.OwnerCharacter == nullptr)
	{
		return;
	}

	FSMInfo.OwnerCharacter->SetMaterialParameterValue("HitRim", 1.0F);
}

void WRFSMBeParried::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMBeParried::End(const WRFSMInfo& InReservedFSM)
{
	if (InReservedFSM.OwnerCharacter == nullptr)
	{
		return;
	}

	UWRActorComponentManageMaterial* ActorComponentManageMaterial = InReservedFSM.OwnerCharacter->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	float Duration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "HitRimTime");

	ActorComponentManageMaterial->Spawn(EWRManageMaterial::Weakness, Duration);
}

void WRFSMBeParried::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMBeParried::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

EWRFSM WRFSMBeParried::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	EWRFSM NextFSM = WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);

	return NextFSM;
}

bool WRFSMBeParried::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	case EWRFSM::Move:
	case EWRFSM::BeParried:
	{
		return false;
	}
	default:
		break;
	}

	return true;
}
