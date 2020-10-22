// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Singleton/DMSingleton.h"
#include "../Enum/DMPathEnum.h"
#include "../Enum/DMActorEnum.h"
#include "../FSM/Idle/DMFSMIdleBase.h"
#include "../Component/DMActorComponentFSM.h"

class ADMCharacterBase;

class THIRDPERSON_API DMFSMManager : public DMSingleton<DMFSMManager>
{
	friend UDMActorComponentFSM;

private:
	DMFSMBase* FactoryFSM(ADMCharacterBase* IN InCharacter, EDMFSMType IN InFSMType);
	DMFSMIdleBase* FactoryFSMIdle(ADMCharacterBase* IN InCharacter, EDMFSMType IN InFSMType);

public:
	DMFSMManager();
	virtual ~DMFSMManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;

	void DetermineFSM(ADMCharacterBase* IN InCharacter, EDMFSMType IN InNewFSM);
	void DetermineIdle(ADMCharacterBase* IN InCharacter, EDMIdleType IN InNewIdle);
	
};
