// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMFSMIdleBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMIdleFly : public DMFSMIdleBase
{
public:
	DMFSMIdleFly();
	virtual ~DMFSMIdleFly();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();
};
