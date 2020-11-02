#pragma once
#include "Actor/FSM/WRFSMBase.h"
class WRFSMMove :
	public WRFSMBase
{
public:
	WRFSMMove();
	virtual ~WRFSMMove();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;

public:
	void CalculateForwardDeltaAndRightDeltaForBlendSpace(AWRCharacter* OwnerCharacter);
	
};

