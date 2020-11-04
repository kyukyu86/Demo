// Fill out your copyright notice in the Description page of Project Settings.


#include "DMFSMSkillBase.h"

DMFSMSkillBase::DMFSMSkillBase()
{
}

DMFSMSkillBase::~DMFSMSkillBase()
{
}

void DMFSMSkillBase::OnEnter(const FDMFSMData IN InFSMData)
{
	DMFSMCombatBase::OnEnter(InFSMData);

}

void DMFSMSkillBase::OnExit()
{

	DMFSMCombatBase::OnExit();
}

void DMFSMSkillBase::OnTick()
{
	DMFSMCombatBase::OnTick();

}