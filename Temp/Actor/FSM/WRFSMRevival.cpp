#include "WRFSMRevival.h"



WRFSMRevival::WRFSMRevival()
	:WRFSMBase(EWRFSM::Revival, EWRFSM::Idle)
{
}


WRFSMRevival::~WRFSMRevival()
{
}

void WRFSMRevival::Begin(const WRFSMInfo& FSMInfo)
{
	//ProcessAnimation(FSMInfo.OwnerCharacter, (int32)EWRFSM::Revival);

	if (FSMInfo.OwnerCharacter != nullptr && FSMInfo.OwnerCharacter->IsMe() == false)
	{
		USkeletalMeshComponent* InSkelmeshComp = FSMInfo.OwnerCharacter->GetMesh();
		if (InSkelmeshComp != nullptr)
			InSkelmeshComp->SetCollisionProfileName(FName("NoCollision"));
	}

	ProcessAnimation(FSMInfo);

}

void WRFSMRevival::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMRevival::End(const WRFSMInfo& InReservedFSM)
{

}

void WRFSMRevival::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMRevival::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMRevival::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Idle:
	{
		return true;
	}
	break;
	case EWRFSM::AnimationEnd:
	{
		return true;
	}
	break;
	case EWRFSM::Move:
	case EWRFSM::Attack:
	{
		if (InFSMInfo.OwnerCharacter->IsPlayer() && InFSMInfo.OwnerCharacter->IsMe() == false)
		{
			return true;
		}
	}
	break;
	default:
	{
	}
	break;
	}

	return true;
}
