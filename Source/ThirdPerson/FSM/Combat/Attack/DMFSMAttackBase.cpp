// Fill out your copyright notice in the Description page of Project Settings.


#include "DMFSMAttackBase.h"

DMFSMAttackBase::DMFSMAttackBase()
{
}

DMFSMAttackBase::~DMFSMAttackBase()
{
}

void DMFSMAttackBase::OnEnter(const FDMFSMData IN InFSMData)
{
	DMFSMCombatBase::OnEnter(InFSMData);

}

void DMFSMAttackBase::OnExit()
{

	DMFSMCombatBase::OnExit();
}

void DMFSMAttackBase::OnTick()
{
	DMFSMCombatBase::OnTick();

}
