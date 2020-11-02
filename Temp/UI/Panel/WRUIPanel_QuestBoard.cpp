// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_QuestBoard.h"
#include "UI/Slot/WRUISlot_QuestBoard.h"
#include "Define/WRUIDefine.h"

void UWRUIPanel_QuestBoard::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRUISlot_QuestBoard, Slot_QuestBoard);
}

void UWRUIPanel_QuestBoard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
