#pragma once
#include "WRFSMBase.h"
class WRFSMBeBlocked :
	public WRFSMBase
{
public:
	WRFSMBeBlocked();
	virtual ~WRFSMBeBlocked();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) override;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) override;
	virtual void End(const WRFSMInfo& InReservedFSM) override;
	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) override;		//fsm�ð��� ������ ���°� ��ȯ�ɶ�
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) override;	//fsm�� ������ ��ȯ�ɶ�

public:
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) override;
};

