// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMFSMHittedBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMHittedNormal : public DMFSMHittedBase
{
public:
	DMFSMHittedNormal();
	virtual ~DMFSMHittedNormal();

public:
	virtual void OnEnter(const FDMFSMData IN InFSMData);
	virtual void OnExit();
	virtual void OnTick();
};
