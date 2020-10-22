// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMFSMSkillBase.h"

/**
 * 
 */
class THIRDPERSON_API DMFSMSkillNormal : public DMFSMSkillBase
{
public:
	DMFSMSkillNormal();
	virtual ~DMFSMSkillNormal();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();
};
