#pragma once
#include "WRFSMBase.h"

class WRFSMEnterVehicle :	public WRFSMBase
{
public:
	WRFSMEnterVehicle();
	virtual ~WRFSMEnterVehicle();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;
	virtual EWRFSM GetNextFSMWhenAnimationEnd(AWRCharacter* InChar) override;

	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;
};

