// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_TardisTimer.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"

void UWRUIPanel_TardisTimer::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsBelongToBP)
		return;

	// [ 2020-3-27 : kyu ] 귀속된 UI도 있으므로 BINGING 및 초기화는 ConstructDetail()에 작성해주세요.
	ConstructDetail();
}

void UWRUIPanel_TardisTimer::ConstructDetail()
{
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Timer);
	BIND_WIDGET(UTextBlock, TextBlock_Hour);
	BIND_WIDGET(UTextBlock, TextBlock_Minute);
	BIND_WIDGET(UTextBlock, TextBlock_Second);
	BIND_WIDGET_WITH_IMPLEMENTATION(UCanvasPanel, CanvasPanel_Contents, CanvasPanel_Contents->SetVisibility(ESlateVisibility::Collapsed););
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Contents);

	TickActivate = true;
	FlowTime = 0.f;
	DisplayTime = 0;
}

void UWRUIPanel_TardisTimer::SetBelongToBP()
{
	IsBelongToBP = true;
	ConstructDetail();
}

void UWRUIPanel_TardisTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TickActivate == false)
		return;

	if (TimeDirection == EWRTimerDirection::Increase)
	{
		FlowTime += InDeltaTime;
	}
	else if (TimeDirection == EWRTimerDirection::Decrease)
	{
		FlowTime -= InDeltaTime;
		if (FlowTime < 0.f)
			FlowTime = 0.f;
	}

	if (DisplayTime != int32(FlowTime))
	{
		SetTime(int32(FlowTime));
	}
}

void UWRUIPanel_TardisTimer::SetStandby()
{
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Timer, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Contents, ESlateVisibility::SelfHitTestInvisible);
	TickActivate = false;
	FlowTime = 0.f;
}

void UWRUIPanel_TardisTimer::SetTimerInfo(const EWRTimerDirection IN InDirection, const float IN InStartTime /*= 0.f*/)
{
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Timer, ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Contents, ESlateVisibility::Collapsed);
	TickActivate = true;
	FlowTime = InStartTime;
	TimeDirection = InDirection;
}

void UWRUIPanel_TardisTimer::SetTimerStop()
{
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Timer, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Contents, ESlateVisibility::Collapsed);
	TickActivate = false;
	FlowTime = 0.f;
}

void UWRUIPanel_TardisTimer::SetContents(const FString IN InContents)
{
	WRUIUtility::SetTextSafe(WRTextBlock_Contents, InContents);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Contents, ESlateVisibility::SelfHitTestInvisible);
}

void UWRUIPanel_TardisTimer::SetTime(int32 IN InDisplayTime)
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
