// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMFSMAttackBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMAttackNormal : public DMFSMAttackBase
{
public:
	DMFSMAttackNormal();
	virtual ~DMFSMAttackNormal();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();
};
