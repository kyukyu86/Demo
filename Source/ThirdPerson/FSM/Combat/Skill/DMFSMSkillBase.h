// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMFSMCombatBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMSkillBase : public DMFSMCombatBase
{
public:
	DMFSMSkillBase();
	virtual ~DMFSMSkillBase();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();
};
