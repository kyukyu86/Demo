// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_QuestBoard.generated.h"


struct FWRQuestTaskTable;
struct S_QuestNotificatorInfo;
class UWRUISlot_QuestBoardList;
//class UScrollBox;
class UWRTextBlock;
UCLASS()
class WR_API UWRUISlot_QuestBoard : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	/*void OnQuestTaskAccepted(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);
	void OnQuestTaskCompleted(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);
	void OnQuestCompleted(const WRTableID IN InQuestTID);*/

	void DisplayQuestBoard();

	UFUNCTION(BlueprintCallable)
		void UpdateQuestInformation();
private:
	void DisplayQuestBoard_Internal();
	void HideQuestBoard();
	UWRUISlot_QuestBoardList* FindQuestBoardList(const WRTableID IN InQuestTID);
public:
	//UScrollBox* ScrollBox_QuestList;
	//UTextBlock* TextBlock_QuestName;
	UWRTextBlock* WRTextBlock_TaskDescription;
	bool bReserveDisplay;
	bool bDisplaying;
	float DisplayTime;
};