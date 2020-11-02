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
	EWRAutoSystemState CurrentStateType = EWRAutoSystemState::None;	//스테이트타입

	EWRAutoProcessType::en AutoProcessType = EWRAutoProcessType::None;	//생성시킬때 사용할 오토 목적
	EWRAutoSystemState NextAutoStateType = EWRAutoSystemState::None; //해당 스테이트가 끝날때 실행될 다음 스테이트
};
