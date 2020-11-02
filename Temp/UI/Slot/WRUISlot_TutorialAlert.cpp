// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_TutorialAlert.h"
#include "TextBlock.h"
#include "Utility/WRUIUtility.h"
#include "Define/WRUIDefine.h"



void UWRUISlot_TutorialAlert::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Alert);
	fElapsedTime = 0.f;
}

void UWRUISlot_TutorialAlert::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bTimeMode)
	{
		fElapsedTime += InDeltaTime;
		if (fElapsedTime >= fMaxTime)
		{
			OnClose();
		}
	}
}

void UWRUISlot_TutorialAlert::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == Ani_ShowOn)
	{
		if (bTimeMode)
		{
			PlayAnimation(Ani_Flicker, 0.f, 0);
		}
		else
		{
			PlayAnimation(Ani_Flicker, 0.f, FlickerCount);
		}
	}
	else if (Animation == Ani_Flicker)
	{
		PlayAnimation(Ani_ShowOff);
	}
	else if (Animation == Ani_ShowOff)
	{
		OnClose();
	}
}

void UWRUISlot_TutorialAlert::OpenTutorialAlert(const float IN InTime /* = 0.f */)
{	
	if (IsAnimationPlaying(Ani_Flicker))
		StopAnimation(Ani_Flicker);
	if (IsAnimationPlaying(Ani_ShowOff))
		StopAnimation(Ani_ShowOff);

	fElapsedTime = 0.f;

	if (InTime != 0.f)
	{
		bTimeMode = true;
		fMaxTime = InTime;
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(Ani_ShowOn);
}

void UWRUISlot_TutorialAlert::CloseTutorialAlert()
{
	if (IsAnimationPlaying(Ani_Flicker))
		StopAnimation(Ani_Flicker);
	if (IsAnimationPlaying(Ani_ShowOff))
		StopAnimation(Ani_ShowOff);
	if (IsAnimationPlaying(Ani_ShowOn))
		StopAnimation(Ani_ShowOn);

	OnClose();
}

void UWRUISlot_TutorialAlert::SetAlert(FString& IN InAlert)
{
	WRUIUtility::SetTextSafe(TextBlock_Alert, InAlert);
}

void UWRUISlot_TutorialAlert::OnClose()
{
	bTimeMode = false;
	fElapsedTime = 0.f;
	fMaxTime = 0.f;
	SetVisibility(ESlateVisibility::Collapsed);
}
