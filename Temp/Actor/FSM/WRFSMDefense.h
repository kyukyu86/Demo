#pragma once
#include "WRFSMBase.h"
#include "WRFSMAttack.h"
class WRFSMDefense :
	public WRFSMAttack
{
public:
	WRFSMDefense();
	virtual ~WRFSMDefense();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;
	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;		//fsm시간이 끝나서 상태가 변환될때
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;	//fsm이 강제로 변환될때

public:
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;

public:

};

