// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentEventAction.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Table/WREnpcTable.h"
#include "Manager/WRCharacterManager.h"
#include "Utility/WRActorUtility.h"
#include "Manager/WRItemManager.h"



void UWRActorComponentEventAction::OnCreateComponent()
{
	Super::OnCreateComponent();

	if (CheckEventActionCondition())
	{
		SetEnableTick(true);
	}
	else
	{
		SetEnableTick(false);
	}
}

void UWRActorComponentEventAction::OnDestroyComponent()
{
	
	Super::OnDestroyComponent();
}

void UWRActorComponentEventAction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CheckDistance())
	{
		if (bAvailableAction)
			ProgressEventAction();
	}
	else
	{
		if (bAvailableAction == false)
			bAvailableAction = true;
	}
}

void UWRActorComponentEventAction::UpdateEventActoinCodition()
{
	if (CheckEventActionCondition())
	{
		SetEnableTick(true);
	}
	else
	{
		SetEnableTick(false);
	}
}

bool UWRActorComponentEventAction::CheckEventActionCondition()
{
	AWRCharacterNPC* NPCCharacter = Cast<AWRCharacterNPC>(GetOwnerCharacter());
	if (NPCCharacter == nullptr)
	{
		bPreviousEventActionCondition = false;
		return false;
	}
		

	if (NPCCharacter->GetCharacterType() != EWRCharacterType::NPC)
	{ 
		bPreviousEventActionCondition = false;
		return false;
	}

	const FWRNpcTable* NpcTalbe = static_cast<const FWRNpcTable*>(NPCCharacter->GetNpcBaseTable());
	if (NpcTalbe == nullptr)
	{
		bPreviousEventActionCondition = false;
		return false;
	}

	for (int32 i = 0; i < NpcTalbe->EventActionConditions.Num(); ++i)
	{
		const FWREventActionCondition& Condition = NpcTalbe->EventActionConditions[i];
		if (Condition.Type == EWREventActionConditionType::None)
			break;

		if (CheckEventActionCondition(Condition) == false)
		{
			bPreviousEventActionCondition = false;
			return false;
		}
	}

	if (bPreviousEventActionCondition == false)
		bAvailableAction = true;

	bPreviousEventActionCondition = true;
	return true;
}

bool UWRActorComponentEventAction::CheckEventActionCondition(const FWREventActionCondition& IN InEventActionCondition)
{
	if (InEventActionCondition.Type == EWREventActionConditionType::HaveItem)
	{
		ITEM_COUNT_t ItemQuantity = WRItemManager::Get()->FindItemTotalQuantity(InEventActionCondition.Value01);
		if (ItemQuantity < InEventActionCondition.Value02)
			return false;
	}

	return true;
}

bool UWRActorComponentEventAction::CheckDistance()
{
	AWRCharacterNPC* NPCCharacter = Cast<AWRCharacterNPC>(GetOwnerCharacter());
	if (NPCCharacter == nullptr)
		return false;

	if (NPCCharacter->GetCharacterType() != EWRCharacterType::NPC)
		return false;

	const FWRNpcTable* NpcTalbe = static_cast<const FWRNpcTable*>(NPCCharacter->GetNpcBaseTable());
	if (NpcTalbe == nullptr)
		return false;
	
	AWRCharacter* MyCharacter = Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (MyCharacter == nullptr)
		return false;

	float Dis = WRActorUtility::GetDistance(GetOwnerCharacter(), MyCharacter);
	if (Dis > NpcTalbe->EventActionRange)
		return false;

	return true;
}

void UWRActorComponentEventAction::ProgressEventAction()
{
	AWRCharacterNPC* NPCCharacter = Cast<AWRCharacterNPC>(GetOwnerCharacter());
	if (NPCCharacter == nullptr)
		return;

	if (NPCCharacter->GetCharacterType() != EWRCharacterType::NPC)
		return;

	const FWRNpcTable* NpcTalbe = static_cast<const FWRNpcTable*>(NPCCharacter->GetNpcBaseTable());
	if (NpcTalbe == nullptr)
		return;

	int32 MaxNum = NpcTalbe->EventActions.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		if(NpcTalbe->EventActions[i].Type == EWREventAction::None)
			continue;

		ProgressEventAction(NpcTalbe->EventActions[i]);
	}

	bAvailableAction = false;
}

void UWRActorComponentEventAction::ProgressEventAction(const FWREventAction& IN InEventAction)
{
	if (InEventAction.Type == EWREventAction::None)
	{
		return;
	}
	else if (InEventAction.Type == EWREventAction::PlayAni)
	{
		AWRCharacter* Character = GetOwnerCharacter();

		if (Character == nullptr)
			return;

		WRFSMInfo FSMInfo;
		FSMInfo.FSMType = EWRFSM::EventAction;
		FSMInfo.JumpToSectionName = FString::FromInt(InEventAction.Value01);
		Character->DetermineFSMSignal(FSMInfo);
	}
}
