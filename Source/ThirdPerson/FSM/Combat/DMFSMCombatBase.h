// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMFSMBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMCombatBase : public DMFSMBase
{
public:
	DMFSMCombatBase();
	virtual ~DMFSMCombatBase();

public:
	virtual void OnEnter(const FDMFSMData IN InFSMData);
	virtual void OnExit();
	virtual void OnTick();
};
