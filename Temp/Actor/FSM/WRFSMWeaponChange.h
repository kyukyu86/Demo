#pragma once
#include "Actor/FSM/WRFSMBase.h"

class WRFSMWeaponChange :
	public WRFSMBase
{
public:
	WRFSMWeaponChange();
	virtual ~WRFSMWeaponChange();

private:
	EWRChangeEquipment ChangeEquipmentType = EWRChangeEquipment::MainWeapon;

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;
	virtual EWRFSM GetNextFSMWhenAnimationEnd(AWRCharacter* InChar) override;

};

