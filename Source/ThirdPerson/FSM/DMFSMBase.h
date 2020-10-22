// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class ADMCharacterBase;
class UAnimationAsset;

class THIRDPERSON_API DMFSMBase
{
protected:
	ADMCharacterBase* OwnerChatacter = nullptr;

public:
	DMFSMBase();
	virtual ~DMFSMBase();

public:
	virtual void OnEnter();
	virtual void OnExit();
	virtual void OnTick();

	FORCEINLINE void SetOwner(ADMCharacterBase* IN InOwnerCharacter) { OwnerChatacter = InOwnerCharacter; }
	void PlayAnimation(UAnimationAsset* IN InAnimAsset, bool IN InLooping = false);
};
