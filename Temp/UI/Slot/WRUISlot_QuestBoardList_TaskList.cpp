// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_QuestBoardList_TaskList.h"
#include "TextBlock.h"
#include "Table/WRQuestTable.h"
#include "Define/WRUIDefine.h"

#include "Localization/WRLocalizationProcessor.h"

void UWRUISlot_QuestBoardList_TaskList::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_TaskDescription);

	QuestTaskTID = INVALID_TABLE_TID;
}

void UWRUISlot_QuestBoardList_TaskList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUISlot_QuestBoardList_TaskList::SetQuestTaskData(const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
{
	if (InQuestTaskTable == nullptr)
		return;

	QuestTaskTID = InQuestTaskTID;

	FString LocalKey = InQuestTaskTable->DiscriptionLocalkey.ToString();
	TextBlock_TaskDescription->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::QuestStringTable, LocalKey));
}