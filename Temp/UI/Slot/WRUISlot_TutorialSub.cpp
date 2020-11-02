// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_TutorialSub.h"

#include "ProgressBar.h"
#include "CanvasPanel.h"

#include "Utility/WRUIUtility.h"
#include "Define/WRUIDefine.h"

void UWRUISlot_TutorialSub::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Skip);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_ProgressBar);
	BIND_WIDGET(UProgressBar, ProgressBar);
}

void UWRUISlot_TutorialSub::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWRUISlot_TutorialSub::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == Ani_ShowOff)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		ProgressBar->SetPercent(0.f);
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Skip, ESlateVisibility::Collapsed);
		WRUIUtility::SetVisibilitySafe(CanvasPanel_ProgressBar, ESlateVisibility::Collapsed);
	}
}

void UWRUISlot_TutorialSub::SetProgressBar(float IN InRate)
{
	if (bClosing)
	{
		StopAnimation(Ani_ShowOff);
		PlayAnimation(Ani_ShowOn);
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_ProgressBar, ESlateVisibility::SelfHitTestInvisible);
	ProgressBar->SetPercent(InRate);
}

void UWRUISlot_TutorialSub::OpenTutorialSub(const bool IN InShowSkip)
{
	if (bClosing)
	{
		bClosing = false;
		StopAnimation(Ani_ShowOff);
		PlayAnimation(Ani_ShowOn);
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Skip, InShowSkip ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_ProgressBar, ESlateVisibility::Collapsed);
}

void UWRUISlot_TutorialSub::CloseTutorialSub()
{
	bClosing = true;
	PlayAnimation(Ani_ShowOff);
}
