// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Singleton/DMSingleton.h"
#include "../Enum/DMActorEnum.h"
#include <Delegates/DelegateCombinations.h>
#include <Delegates/IDelegateInstance.h>


DECLARE_MULTICAST_DELEGATE_TwoParams(FDMInputDelegate, EDMInput, EInputEvent);

class THIRDPERSON_API DMInputManager : public DMSingleton<DMInputManager>
{
private:
	TMap<EDMInput, FDMInputDelegate> InputDelegateList;

	bool bIgnoreInput = false;
	bool bIgnoreAction = false;

public:
	DMInputManager();
	virtual ~DMInputManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;
	virtual bool OnTick(float InDeltaTime) override;

	FDMInputDelegate& GetInputDelegate(const EDMInput IN InInputType);
	void UnBindInputEvent(const EDMInput IN InInputType, FDelegateHandle& IN InDelegateHandle);

	FORCEINLINE void SetIgnoreInput(const bool IN InSet) { bIgnoreInput = InSet; }
	FORCEINLINE bool IsIgnoreInput() { return bIgnoreInput; }
	FORCEINLINE void SetIgnoreAction(const bool IN InSet) { bIgnoreAction = InSet; }
	FORCEINLINE bool IsIgnoreAction() { return bIgnoreAction; }
};
