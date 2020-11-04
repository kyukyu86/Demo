// Fill out your copyright notice in the Description page of Project Settings.


#include "DMFSMHittedBase.h"

DMFSMHittedBase::DMFSMHittedBase()
{
}

DMFSMHittedBase::~DMFSMHittedBase()
{
}

void DMFSMHittedBase::OnEnter(const FDMFSMData IN InFSMData)
{
	DMFSMCombatBase::OnEnter(InFSMData);

}

void DMFSMHittedBase::OnExit()
{

	DMFSMCombatBase::OnExit();
}

void DMFSMHittedBase::OnTick()
{
	DMFSMCombatBase::OnTick();

}