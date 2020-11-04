// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMFSMCombatBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMAttackBase : public DMFSMCombatBase
{
public:
	DMFSMAttackBase();
	virtual ~DMFSMAttackBase();

public:
	virtual void OnEnter(const FDMFSMData IN InFSMData);
	virtual void OnExit();
	virtual void OnTick();
};
