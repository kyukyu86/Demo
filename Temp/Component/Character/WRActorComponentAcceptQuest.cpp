// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentAcceptQuest.h"
#include "Manager/WRQuestManager.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Table/WREnpcTable.h"
#include "WRActorComponentStateMark.h"
#include "Manager/WRCompassManager.h"
#include "Manager/WRMapManager.h"




void UWRActorComponentAcceptQuest::OnCreateComponent()
{
	Super::OnCreateComponent();

}

void UWRActorComponentAcceptQuest::OnDestroyComponent()
{
	if (CompassMarkHandle != INVALID_COMPASS_MARK_HANDLE)
	{
		WRCompassManager::Get()->RemoveMark(CompassMarkHandle);
		CompassMarkHandle = INVALID_COMPASS_MARK_HANDLE;
	}

	Super::OnDestroyComponent();
}

void UWRActorComponentAcceptQuest::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWRActorComponentAcceptQuest::Init()
{
	UpdateReadyQuest();
}

void UWRActorComponentAcceptQuest::OnQuestStateChanged()
{
	UpdateReadyQuest();
}

void UWRActorComponentAcceptQuest::UpdateReadyQuest()
{
	return;

	AWRCharacterNPC* OwnerCharacter = Cast<AWRCharacterNPC>(GetOwnerCharacter());
	if (OwnerCharacter == nullptr)
		return;

	FWRNpcTable* NPCTable = static_cast<FWRNpcTable*>(OwnerCharacter->GetNpcBaseTable());
	if (NPCTable == nullptr)
		return;

	if (WRQuestManager::Get()->HasReadyQuest(OwnerCharacter->GetCharacterTableID()))
	{
		if(CompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
			CompassMarkHandle = WRCompassManager::Get()->AddMark(WRMapManager::Get()->GetCurrentMapTID(), OwnerCharacter, "Dialogue", FWRCompassMarkInfoBase::EWRCompassMarkUIType::Exclamation);
	}
	else
	{
		if (CompassMarkHandle != INVALID_COMPASS_MARK_HANDLE)
		{
			WRCompassManager::Get()->RemoveMark(CompassMarkHandle);
			CompassMarkHandle = INVALID_COMPASS_MARK_HANDLE;
		}			
	}
}
