// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "UI/Slot/WRUISlot_QuestBoard.h"
#include "WRUIPanel_QuestBoard.generated.h"


UCLASS()
class WR_API UWRUIPanel_QuestBoard : public UWRUIPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void DisplayQuestBoard() { Slot_QuestBoard->DisplayQuestBoard(); }
private:
	UWRUISlot_QuestBoard* Slot_QuestBoard;
};