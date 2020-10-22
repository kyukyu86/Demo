// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMFSMBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMIdleBase : public DMFSMBase
{
public:
	DMFSMIdleBase();
	virtual ~DMFSMIdleBase();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();
};
