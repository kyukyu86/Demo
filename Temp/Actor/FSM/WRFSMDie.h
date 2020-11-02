#pragma once
#include "Actor/FSM/WRFSMBase.h"
class WRFSMDie :
	public WRFSMBase
{
public:
	WRFSMDie();
	virtual ~WRFSMDie();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;

	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;

	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;

private:
	void MakeVFX(AWRCharacter* InCharacter, const FRotator& InDirection);
	void MakeSFX(AWRCharacter* InCharacter);
	void MakePhysics(AWRCharacter* InCharacter);
};