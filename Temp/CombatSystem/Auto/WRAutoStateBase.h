// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

struct EWRAutoProcessType
{
	enum en
	{
		None,
	};
};

enum class EWRAutoSystemState : uint8
{
	None,

	Idle,
	MoveTo,
	Searching,
	Attacking,



	CommonProcess,
	ChaseTo,
	Escort,
	InteractTo,
	TalkTo,
	Taming,
	Counter,
};

class WR_API WRAutoStateBase
{
public:
	WRAutoStateBase() {};
	WRAutoStateBase(EWRAutoSystemState InNextLinkedAutostate);
	virtual ~WRAutoStateBase();

	friend class CAutoSystemBase;

public:
	virtual void ResetState();
	virtual void Begin();
	virtual void Tick(float DeltaTime);
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();
	//virtual void LockUpdate(bool bFlag);
	//virtual void SetQuestTID(const FIMQuestDataBase* InCurrentQuest);

public:
	EWRAutoSystemState GetAutoStateType() { return CurrentStateType; }
	void SetNextAutoStateType(EWRAutoSystemState InNextState) { NextAutoStateType = InNextState; }
	

protected:
	EWRAutoSystemState CurrentStateType = EWRAutoSystemState::None;	//������ƮŸ��

	EWRAutoProcessType::en AutoProcessType = EWRAutoProcessType::None;	//������ų�� ����� ���� ����
	EWRAutoSystemState NextAutoStateType = EWRAutoSystemState::None; //�ش� ������Ʈ�� ������ ����� ���� ������Ʈ
};
