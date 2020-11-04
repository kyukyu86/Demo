// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Struct/DMActorStruct.h"

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
	virtual void OnEnter(const FDMFSMData IN InFSMData);
	virtual void OnReEnter(const FDMFSMData IN InFSMData);
	virtual void OnExit();
	virtual void OnTick();

	FORCEINLINE void SetOwner(ADMCharacterBase* IN InOwnerCharacter) { OwnerChatacter = InOwnerCharacter; }
	FORCEINLINE ADMCharacterBase* GetOwner() { return OwnerChatacter; }
	void PlayAnimation(UAnimationAsset* IN InAnimAsset, bool IN InLooping = false);
};
