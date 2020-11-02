// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_QuestBoard.h"
#include "WRUISlot_QuestBoardList.h"
#include "ScrollBox.h"
#include "TextBlock.h"

#include "Define/WRUIDefine.h"
#include "Object.h"
#include "OutputDeviceNull.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRSFXManager.h"

#include "Localization/WRLocalizationProcessor.h"



#define DEF_DISPLAY_QUEST_BOARD_TIME 6

void UWRUISlot_QuestBoard::NativeConstruct()
{
	Super::NativeConstruct();

	//BIND_WIDGET(UTextBlock, TextBlock_QuestName);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_TaskDescription);
	//BIND_WIDGET(UScrollBox, ScrollBox_QuestList);

	bReserveDisplay = false;
	bDisplaying = false;
	DisplayTime = 0.f;
}

void UWRUISlot_QuestBoard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DisplayTime > 0)
	{
		DisplayTime -= InDeltaTime;
		if (DisplayTime <= 0)
			HideQuestBoard();
		else if (bReserveDisplay && DisplayTime < DEF_DISPLAY_QUEST_BOARD_TIME - (1 / WRConsoleManager::Get()->GetFloat(EWRConsole::UI_CyberAniSpeed)))
		{
			HideQuestBoard();
		}
	}
}

//void UWRUISlot_QuestBoard::OnQuestTaskAccepted(const WRTableID IN InQuestTID,const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
//{
//	/*if (InQuestTID == INVALID_TABLE_TID)
//		return;
//
//	UWRUISlot_QuestBoardList* Slot_QuestBoardList = FindQuestBoardList(InQuestTID);
//	if (Slot_QuestBoardList == nullptr)
//	{
//		Slot_QuestBoardList = Cast<UWRUISlot_QuestBoardList>(WRUIManager::Get()->CreateUISlot(EWRUISlotKind::QuestBoardList));
//		if (Slot_QuestBoardList)
//		{
//			ScrollBox_QuestList->AddChild(Slot_QuestBoardList);
//			Slot_QuestBoardList->SetQuestData(InQuestTID);
//		}
//	}
//
//	if (Slot_QuestBoardList == nullptr)
//		return;
//
//	Slot_QuestBoardList->AddQuestTask(InQuestTaskTID, InQuestTaskTable);*/
//}
//
//void UWRUISlot_QuestBoard::OnQuestTaskCompleted(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
//{
//	/*if (InQuestTID == INVALID_TABLE_TID || InQuestTaskTID == INVALID_TABLE_TID)
//		return;
//
//	UWRUISlot_QuestBoardList* Slot_QuestBoardList = FindQuestBoardList(InQuestTID);
//	if (Slot_QuestBoardList)
//	{
//		Slot_QuestBoardList->RemoveQuestTask(InQuestTaskTID);
//	}*/
//}
//
//void UWRUISlot_QuestBoard::OnQuestCompleted(const WRTableID IN InQuestTID)
//{
//	/*if (InQuestTID == INVALID_TABLE_TID)
//		return;
//
//	UWRUISlot_QuestBoardList* Slot_QuestBoardList = FindQuestBoardList(InQuestTID);
//	if (Slot_QuestBoardList == nullptr)
//		return;
//
//	ScrollBox_QuestList->RemoveChild(Slot_QuestBoardList);*/
//}

void UWRUISlot_QuestBoard::DisplayQuestBoard()
{
	if (bDisplaying)
	{
		bReserveDisplay = true;
		return;
	}

	DisplayQuestBoard_Internal();
}

void UWRUISlot_QuestBoard::UpdateQuestInformation()
{
	WRTableID TrackedQuestTID = WRQuestManager::Get()->GetTrackedQuestTID();
	if (TrackedQuestTID == INVALID_TABLE_TID)
		return;

	TDoubleLinkedList<WRTableID> TaskTIDs;
	WRQuestManager::Get()->GetProgressTaskByQuest(TrackedQuestTID, TaskTIDs);

	const FWRQuestTable* QuestTable = WRQuestManager::Get()->FindQuestTable(TrackedQuestTID);
	//if (QuestTable)
	//{
	//	TextBlock_QuestName->SetText(FText::FromName(QuestTable->QuestNameLocalkey));
	//}

	if (TaskTIDs.Num() > 0)
	{
		const FWRQuestTaskTable* TaskTable = WRQuestManager::Get()->FindQuestTaskTable(TaskTIDs.GetHead()->GetValue());
		if (TaskTable)
		{
			FString LocalKey = TaskTable->DiscriptionLocalkey.ToString();
			WRTextBlock_TaskDescription->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::QuestStringTable, LocalKey));
		}
	}
}

void UWRUISlot_QuestBoard::DisplayQuestBoard_Internal()
{
	if (bDisplaying)
		return;

	bDisplaying = true;
	DisplayTime = DEF_DISPLAY_QUEST_BOARD_TIME;
	bReserveDisplay = false;

	WRTableID TrackedQuestTID = WRQuestManager::Get()->GetTrackedQuestTID();
	if (TrackedQuestTID == INVALID_TABLE_TID)
	{
		HideQuestBoard();
		return;
	}
	
	UpdateQuestInformation();
	WRSFXManager::Get()->AsyncSpawn2D("Notification_sound_1_Cue.Notification_sound_1_Cue");

	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::QuestBoard);
	if (CombinedPanelData)
	{
		if (CombinedPanelData->Is3DWidget())
		{
			CombinedPanelData->WidgetComponentBase->StartCyberAni();
		}
	}
}

void UWRUISlot_QuestBoard::HideQuestBoard()
{
	bDisplaying = false;
	DisplayTime = 0.f;
	if (bReserveDisplay)
	{
		DisplayQuestBoard_Internal();
	}
	else
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::QuestBoard);
	}
}

UWRUISlot_QuestBoardList* UWRUISlot_QuestBoard::FindQuestBoardList(const WRTableID IN InQuestTID)
{
	//int32 MaxNum = ScrollBox_QuestList->GetChildrenCount();
	//for (int32 i = 0; i < MaxNum; ++i)
	//{
	//	UWRUISlot_QuestBoardList* Slot_QuestBoardList = Cast<UWRUISlot_QuestBoardList>(ScrollBox_QuestList->GetChildAt(i));
	//	if(Slot_QuestBoardList == nullptr)
	//		continue;

	//	if(Slot_QuestBoardList->GetQuestTID() != InQuestTID)
	//		continue;

	//	return Slot_QuestBoardList;
	//}

	return nullptr;
}