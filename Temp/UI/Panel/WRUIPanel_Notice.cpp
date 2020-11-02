// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Notice.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRUIManager.h"

#include "TextBlock.h"
#include "UI/CustomWidget/WRTextBlock.h"

void UWRUIPanel_Notice::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Notice);
}

void UWRUIPanel_Notice::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bShowOff)
		return;

	fShowDuringTime += InDeltaTime;
	if (fShowDuringTime >= ShowOnTime)
	{
		FString strNextNotice;
		if (GetNextNotice(strNextNotice))
		{
			ShowOnNotice(strNextNotice);
		}
		else
		{
			ShowOffNotice();
		}
	}
}

void UWRUIPanel_Notice::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation->MovieScene->GetFName() == FName("ShowOnAnimation"))
	{

	}
	else if (Animation->MovieScene->GetFName() == FName("ShowOffAnimation"))
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWRUIPanel_Notice::AddNotice(const FString& IN InNotice)
{
	Notices.Enqueue(InNotice);

	// + 추가시 닫혀있거나 종료중이면 다시 실행

	if (GetVisibility() == ESlateVisibility::Hidden
		|| IsAnimationPlayingByName("ShowOffAnimation"))
	{
		FString strNextNotice;
		if (GetNextNotice(strNextNotice))
		{
			ShowOnNotice(strNextNotice);
		}
	}
}

bool UWRUIPanel_Notice::GetNextNotice(FString& OUT OutNotice)
{
	if (Notices.IsEmpty())
		return false;

	Notices.Dequeue(OutNotice);
	return true;
}

void UWRUIPanel_Notice::ShowOnNotice(FString IN InNotice)
{
	bShowOff = false;
	fShowDuringTime = 0.0f;

	WRUIUtility::SetTextSafe(WRTextBlock_Notice, InNotice);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	PlayAnimationByName("ShowOnAnimation");
}

void UWRUIPanel_Notice::ShowOffNotice()
{
	bShowOff = true;
	PlayAnimationByName("ShowOffAnimation");
}
