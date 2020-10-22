// Fill out your copyright notice in the Description page of Project Settings.


#include "DMFSMBase.h"
#include <Animation/AnimationAsset.h>
#include "../Actor/Character/Base/DMCharacterBase.h"

DMFSMBase::DMFSMBase()
{
}

DMFSMBase::~DMFSMBase()
{
}

void DMFSMBase::OnEnter()
{

}

void DMFSMBase::OnExit()
{

}

void DMFSMBase::OnTick()
{

}

void DMFSMBase::PlayAnimation(UAnimationAsset* IN InAnimAsset, bool IN InLooping /*= false*/)
{
	if (OwnerChatacter == nullptr)
	{
		return;
	}

	OwnerChatacter->GetMesh()->PlayAnimation(InAnimAsset, InLooping);
}
