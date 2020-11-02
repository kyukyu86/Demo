#pragma once
#include "WRFSMBase.h"
#include "WRFSMAttack.h"

class WRFSMSkill :
	public WRFSMAttack
{
public:
	WRFSMSkill();
	virtual ~WRFSMSkill();
	WRFSMSkill(EWRFSM InType, EWRFSM InLinkedFsmWhenAnimationEnd);

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;

public:
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;
};


//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
