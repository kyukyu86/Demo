// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMFSMCombatBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMHittedBase : public DMFSMCombatBase
{
public:
	DMFSMHittedBase();
	virtual ~DMFSMHittedBase();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();
};
