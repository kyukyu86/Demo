// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_QuestBoardList.h"
#include "WRUISlot_QuestBoardList_TaskList.h"
#include "TextBlock.h"
#include "ScrollBox.h"

#include "Manager/WRQuestManager.h"
#include "Manager/WRUIManager.h"

#include "Table/WRQuestTable.h"
#include "Define/WRUIDefine.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUISlot_QuestBoardList::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_QuestName);
	BIND_WIDGET(UScrollBox, ScrollBox_TaskList);

	QuestTID = INVALID_TABLE_TID;
}

void UWRUISlot_QuestBoardList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


}

void UWRUISlot_QuestBoardList::SetQuestData(const WRTableID IN InQuestTID)
{
	const FWRQuestTable* QuestTable = WRQuestManager::Get()->FindQuestTable(InQuestTID);
	if (QuestTable == nullptr)
		return;

	QuestTID = InQuestTID;
	FString QuestNameLocalKey = QuestTable->QuestNameLocalkey.ToString();
	TextBlock_QuestName->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::QuestStringTable, QuestNameLocalKey));
}

void UWRUISlot_QuestBoardList::AddQuestTask(const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
{
	UWRUISlot_QuestBoardList_TaskList* Slot_QuestBoardList_TaskList = Cast<UWRUISlot_QuestBoardList_TaskList>(WRUIManager::Get()->CreateUISlot(EWRUISlotKind::QuestBoardList_TaskList));
	if (Slot_QuestBoardList_TaskList == nullptr)
		return;

	ScrollBox_TaskList->AddChild(Slot_QuestBoardList_TaskList);
	Slot_QuestBoardList_TaskList->SetQuestTaskData(InQuestTaskTID, InQuestTaskTable);
}

void UWRUISlot_QuestBoardList::RemoveQuestTask(const WRTableID IN InQuestTaskTID)
{
	if (InQuestTaskTID == INVALID_TABLE_TID)
		return;

	UWRUISlot_QuestBoardList_TaskList* Slot_QuestBoardList_TaskList = FindTaskList(InQuestTaskTID);
	if (Slot_QuestBoardList_TaskList == nullptr)
		return;

	ScrollBox_TaskList->RemoveChild(Slot_QuestBoardList_TaskList);
}

UWRUISlot_QuestBoardList_TaskList* UWRUISlot_QuestBoardList::FindTaskList(const WRTableID IN InQuestTaskTID)
{
	int32 MaxNum = ScrollBox_TaskList->GetChildrenCount();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		UWRUISlot_QuestBoardList_TaskList* Slot_QuestBoardList_TaskList = Cast<UWRUISlot_QuestBoardList_TaskList>(ScrollBox_TaskList->GetChildAt(i));
		if (Slot_QuestBoardList_TaskList == nullptr)
			continue;

		if (Slot_QuestBoardList_TaskList->GetQuestTaskTID() != InQuestTaskTID)
			continue;

		return Slot_QuestBoardList_TaskList;
	}

	return nullptr;
}
