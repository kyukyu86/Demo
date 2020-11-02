#pragma once
#include "Actor/FSM/WRFSMBase.h"

class WRFSMAttack :
	public WRFSMBase
{
public:
	WRFSMAttack();
	virtual ~WRFSMAttack();

	WRFSMAttack(EWRFSM InType, EWRFSM InLinkedFsmWhenAnimationEnd);

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;
	virtual EWRFSM GetNextFSMWhenAnimationEnd(AWRCharacter* InChar) override;


	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;

public:
	void ProcessSoFX(const WRFSMInfo& InFSM);
};

