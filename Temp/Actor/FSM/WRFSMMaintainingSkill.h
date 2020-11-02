#pragma once
#include "WRFSMAttack.h"
#include "Actor/Character/WRCharacter.h"
#include "Enum/EWRCharacter.h"

class WRFSMMaintainingSkill :
	public WRFSMAttack
{
public:
	WRFSMMaintainingSkill();
	virtual ~WRFSMMaintainingSkill();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;
	virtual EWRFSM GetNextFSMWhenAnimationEnd(AWRCharacter* InChar) override;


	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;

};