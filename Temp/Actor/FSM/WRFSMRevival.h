#pragma once
#include "Actor/FSM/WRFSMBase.h"
class WRFSMRevival :
	public WRFSMBase
{
public:
	WRFSMRevival();
	virtual ~WRFSMRevival();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;

};

