// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_DungeonTimer.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "RichTextBlock.h"

void UWRUIPanel_DungeonTimer::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Hour);
	BIND_WIDGET(UTextBlock, TextBlock_Minute);
	BIND_WIDGET(UTextBlock, TextBlock_Second);

	FlowTime = 0.f;
	DisplayTime = 0;
}

void UWRUIPanel_DungeonTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FlowTime += InDeltaTime;

	if (DisplayTime != int32(FlowTime))
	{
		SetTime(int32(FlowTime));
	}
}

void UWRUIPanel_DungeonTimer::SetTime(int32 IN InDisplayTime)
{
	DisplayTime = InDisplayTime;

	int8 Hour = InDisplayTime / 3600;
	InDisplayTime -= Hour * 3600;
	int8 Minute = InDisplayTime / 60;
	InDisplayTime -= Minute * 60;
	int8 Second = InDisplayTime;

	FString HourStr = FString::Printf(TEXT("%02d"), Hour);
	FString MinuteStr = FString::Printf(TEXT("%02d"), Minute);
	FString SecondStr = FString::Printf(TEXT("%02d"), Second);

	TextBlock_Hour->SetText(FText::FromString(HourStr));
	TextBlock_Minute->SetText(FText::FromString(MinuteStr));
	TextBlock_Second->SetText(FText::FromString(SecondStr));
}
