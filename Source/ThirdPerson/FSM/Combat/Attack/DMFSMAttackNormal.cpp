// Fill out your copyright notice in the Description page of Project Settings.


#include "DMFSMAttackNormal.h"
#include "../../../Actor/Character/Base/DMCharacterBase.h"
#include "../../../Enum/DMActorEnum.h"

DMFSMAttackNormal::DMFSMAttackNormal()
{
}

DMFSMAttackNormal::~DMFSMAttackNormal()
{
}

void DMFSMAttackNormal::OnEnter(const FDMFSMData IN InFSMData)
{
	DMFSMAttackBase::OnEnter(InFSMData);

	ADMCharacterBase* OwnerCharacter = GetOwner();
	if (OwnerCharacter == nullptr)
		return;

	OwnerCharacter->MontagePlay(InFSMData.AttackTable->Data.Montage, InFSMData.AttackTable->Data.SectionName);
}

void DMFSMAttackNormal::OnReEnter(const FDMFSMData IN InFSMData)
{
	DMFSMAttackBase::OnReEnter(InFSMData);

	ADMCharacterBase* OwnerCharacter = GetOwner();
	if (OwnerCharacter == nullptr)
		return;

	OwnerCharacter->MontagePlay(InFSMData.AttackTable->Data.Montage, InFSMData.AttackTable->Data.SectionName);
}

void DMFSMAttackNormal::OnExit()
{

	DMFSMAttackBase::OnExit();
}

void DMFSMAttackNormal::OnTick()
{
	DMFSMAttackBase::OnTick();

}