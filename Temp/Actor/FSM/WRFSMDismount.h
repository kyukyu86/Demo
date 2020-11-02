#pragma once
#include "Actor/FSM/WRFSMBase.h"
class WRFSMDismount :
	public WRFSMBase
{
public:
	WRFSMDismount();
	virtual ~WRFSMDismount();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;

};

