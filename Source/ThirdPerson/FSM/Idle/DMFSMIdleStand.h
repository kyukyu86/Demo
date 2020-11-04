// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMIdleStand : public DMFSMIdleBase
{
public:
	DMFSMIdleStand();
	virtual ~DMFSMIdleStand();

public:
	virtual void OnEnter(const FDMFSMData IN InFSMData);
	virtual void OnExit();
	virtual void OnTick();
};
