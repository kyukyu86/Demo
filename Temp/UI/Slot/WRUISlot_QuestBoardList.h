// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_QuestBoardList.generated.h"



struct FWRQuestTaskTable;
class UTextBlock;
class UScrollBox;
class UWRUISlot_QuestBoardList_TaskList;
UCLASS()
class WR_API UWRUISlot_QuestBoardList : public UWRUISlot
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetQuestData(const WRTableID IN InQuestTID);
	void AddQuestTask(const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);
	void RemoveQuestTask(const WRTableID IN InQuestTaskTID);

	WRTableID GetQuestTID() { return QuestTID; }

private:
	UWRUISlot_QuestBoardList_TaskList* FindTaskList(const WRTableID IN InQuestTaskTID);
public:
	UTextBlock* TextBlock_QuestName;
	UScrollBox* ScrollBox_TaskList;

	WRTableID QuestTID;
};