// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMFSMBase.h"
#include "../../Enum/DMActorEnum.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMCombatBase : public DMFSMBase
{
protected:
	EDMSkillNotice SkillNoticeType = EDMSkillNotice::MouseL;

protected:
	void CheckSkillNotice(const FDMFSMData& IN InFSMData);

public:
	DMFSMCombatBase();
	virtual ~DMFSMCombatBase();
	
	virtual void OnEnter(const FDMFSMData IN InFSMData);
	virtual void OnReEnter(const FDMFSMData IN InFSMData);
	virtual void OnExit();
	virtual void OnTick();
};
