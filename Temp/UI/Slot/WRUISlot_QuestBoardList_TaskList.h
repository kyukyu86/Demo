// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_QuestBoardList_TaskList.generated.h"



class UTextBlock;
struct FWRQuestTaskTable;

UCLASS()
class WR_API UWRUISlot_QuestBoardList_TaskList : public UWRUISlot
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetQuestTaskData(const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);
	WRTableID GetQuestTaskTID() { return QuestTaskTID; }

public:
	UTextBlock* TextBlock_TaskDescription;

	WRTableID QuestTaskTID;
};