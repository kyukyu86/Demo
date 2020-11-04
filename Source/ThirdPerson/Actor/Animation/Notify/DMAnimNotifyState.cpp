// Fill out your copyright notice in the Description page of Project Settings.


#include "DMAnimNotifyState.h"
#include "../../../Manager/DMInputManager.h"


void UDMAnimNotifyState_IgnoreInput::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (DMInputManager::Get() == nullptr)
		return;

	DMInputManager::Get()->SetIgnoreInput(true);
}

void UDMAnimNotifyState_IgnoreInput::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (DMInputManager::Get() == nullptr)
		return;

	DMInputManager::Get()->SetIgnoreInput(false);
}

void UDMAnimNotifyState_IgnoreAction::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (DMInputManager::Get() == nullptr)
		return;

	DMInputManager::Get()->SetIgnoreAction(true);
}

void UDMAnimNotifyState_IgnoreAction::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (DMInputManager::Get() == nullptr)
		return;

	DMInputManager::Get()->SetIgnoreAction(false);
}
