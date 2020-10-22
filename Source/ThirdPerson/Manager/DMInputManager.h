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

public:
	DMInputManager();
	virtual ~DMInputManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;
	virtual bool OnTick(float InDeltaTime) override;

	FDMInputDelegate& GetInputDelegate(const EDMInput IN InInputType);
	void UnBindInputEvent(const EDMInput IN InInputType, FDelegateHandle& IN InDelegateHandle);
};
