// Fill out your copyright notice in the Description page of Project Settings.


#include "DMFSMManager.h"
#include "../Util/DMEnumUtil.h"
#include "../Actor/Character/Base/DMCharacterBase.h"
#include "../Component/DMComponentFSM.h"

#include "../FSM/Idle/DMFSMIdleStand.h"
#include "../FSM/Idle/DMFSMIdleFly.h"
#include "../FSM/Idle/DMFSMIdleSit.h"

#include "../FSM/Combat/Attack/DMFSMAttackNormal.h"
#include "../FSM/Combat/Skill/DMFSMSkillNormal.h"
#include "../FSM/Combat/Damaged/DMFSMHittedNormal.h"


DMFSMManager::DMFSMManager()
{
	
}

DMFSMManager::~DMFSMManager()
{

}

void DMFSMManager::OnInit()
{

}

void DMFSMManager::OnShutdown()
{

}

//====================================================================================
//	Determine
//====================================================================================

void DMFSMManager::DetermineFSM(ADMCharacterBase* IN InCharacter, EDMFSMType IN InNewFSM)
{
	DetermineFSM(InCharacter, FDMFSMData(InNewFSM));
}

void DMFSMManager::DetermineFSM(ADMCharacterBase* IN InCharacter, FDMFSMData IN InNewFSMData)
{
	if (InCharacter == nullptr)
	{
		ensureMsgf(false, TEXT("InCharacter is Invalid"));
		return;
	}

	UDMComponentFSM* FSMComponent = InCharacter->GetComponent<UDMComponentFSM>();
	if (FSMComponent == nullptr)
	{
		ensureMsgf(false, TEXT("FSMComponent is Invalid"));
		return;
	}

	FSMComponent->DetermineFSM(InNewFSMData);
}

void DMFSMManager::DetermineIdle(ADMCharacterBase* IN InCharacter, EDMIdleType IN InNewIdle)
{

}

//====================================================================================
//	Factory
//====================================================================================

DMFSMBase* DMFSMManager::FactoryFSM(ADMCharacterBase* IN InCharacter, EDMFSMType IN InFSMType)
{
	switch (InFSMType)
	{
	case EDMFSMType::Idle:				return FactoryFSMIdle(InCharacter, InFSMType);		break;
	case EDMFSMType::AttackNormal:		return new DMFSMAttackNormal;						break;
	case EDMFSMType::SkillNormal:		return new DMFSMSkillNormal;						break;
	case EDMFSMType::Hitted:			return new DMFSMHittedNormal;						break;
	}

	return nullptr;
}

DMFSMIdleBase* DMFSMManager::FactoryFSMIdle(ADMCharacterBase* IN InCharacter, EDMFSMType IN InFSMType)
{
	UDMComponentFSM* FSMComponent = InCharacter->GetComponent<UDMComponentFSM>();
	if (FSMComponent == nullptr)
	{
		ensureMsgf(false, TEXT("FSMComponent is Invalid"));
		return nullptr;
	}

	EDMIdleType CurrentIdleType = FSMComponent->GetIdleType();
	switch (CurrentIdleType)
	{
	case EDMIdleType::Stand:		return new DMFSMIdleStand;		break;
	case EDMIdleType::Fly:			return new DMFSMIdleFly;		break;
	case EDMIdleType::Sit:			return new DMFSMIdleSit;		break;
	}

	return nullptr;
}
