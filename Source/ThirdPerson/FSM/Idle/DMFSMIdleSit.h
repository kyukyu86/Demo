// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMFSMIdleBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMIdleSit : public DMFSMIdleBase
{
public:
	DMFSMIdleSit();
	virtual ~DMFSMIdleSit();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();
};
