// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIPanel_Tutorial.h"

#include "Image.h"
#include "Overlay.h"
#include "SizeBox.h"
#include "TextBlock.h"
#include "ProgressBar.h"
#include "CanvasPanel.h"

#include "Utility/WRUIUtility.h"

#include "Define/WRUIDefine.h"

#include "Table/WRTutorialTable.h"
#include "Core/WRGameInstance.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRAsyncLoadUObjectManager.h"




void UWRUIPanel_Tutorial::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Guide);
	BIND_WIDGET(UImage, Image_Guide);
	BIND_WIDGET(USizeBox, SizeBox_Description);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Description);
	BIND_WIDGET(UTextBlock, TextBlock_RepeatCount);
	BIND_WIDGET(UProgressBar, ProgressBar);
	BIND_WIDGET(UOverlay, Overlay_Dynamic);
}

void UWRUIPanel_Tutorial::NativeDestruct()
{
	if (Slot_Dynamic != nullptr)
	{
		Overlay_Dynamic->RemoveChild(Slot_Dynamic);
		Slot_Dynamic = nullptr;
	}

#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
	if (strInnerWidgetAsyncKey != "")
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(strInnerWidgetAsyncKey);
	}
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU

	Super::NativeDestruct();
}

void UWRUIPanel_Tutorial::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWRUIPanel_Tutorial::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == Ani_ShowOff)
	{
		if (bReOpen)
		{
			bReOpen = false;
			return;
		}

		if (PanelType == EWRTutorialPanel::Main)
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::Tutorial);
		else if (PanelType == EWRTutorialPanel::Sub)
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::TutorialMini);
	}
}

void UWRUIPanel_Tutorial::SetInformation(FWRTutorialTable* IN InTable)
{
	if (IsAnimationPlaying(Ani_ShowOff))
	{
		bReOpen = true; // [ 2020-3-5 : kyu ] 다시 오픈되는 경우임. OnAnimationFinished_Implementation에 예외를 두기 위해 추가
		StopAnimation(Ani_ShowOff);
	}

	PanelType = InTable->TutorialPanel;

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(ProgressBar, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(TextBlock_RepeatCount, ESlateVisibility::Collapsed);

	// Icon
	if (Slot_Dynamic != nullptr)
	{
		Overlay_Dynamic->RemoveChild(Slot_Dynamic);
 		Slot_Dynamic = nullptr;
	}
	if (InTable->PopupCanvasImagePath.IsEmpty() == false)
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide, ESlateVisibility::SelfHitTestInvisible);
		WRUIUtility::SetResourceObject(Image_Guide, InTable->PopupCanvasImagePath, true);		
	}
	else
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide, ESlateVisibility::Collapsed);
	}
	if(InTable->PopupCanvasBPPath.IsEmpty() == false)
	{
		WRUIUtility::SetVisibilitySafe(Overlay_Dynamic, ESlateVisibility::SelfHitTestInvisible);
		//UClass* pClass = InTable->PopupCanvasBPPath.LoadSynchronous();
		
#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
		auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
		{
			Slot_Dynamic = Cast<UWRUserWidget>(InObject);
			if (Slot_Dynamic != nullptr)
			{
				Overlay_Dynamic->AddChildToOverlay(Slot_Dynamic);
			}
			strInnerWidgetAsyncKey = "";
		});

		strInnerWidgetAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncCreateWidget(InTable->PopupCanvasBPPath, AsyncCreateComplete);
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
		UClass* pClass = LoadObject<UClass>(nullptr, *(InTable->PopupCanvasBPPath));
		Slot_Dynamic = CreateWidget<UWRUserWidget>(UWRGameInstance::GetGameInstance()->GetWorld(), pClass);
		if (Slot_Dynamic != nullptr)
		{
			Overlay_Dynamic->AddChildToOverlay(Slot_Dynamic);
		}
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU
	}
	else
	{
		WRUIUtility::SetVisibilitySafe(Overlay_Dynamic, ESlateVisibility::Collapsed);
	}

	// Name
	if (InTable->PopupDescrition.IsEmpty())
	{
		WRUIUtility::SetVisibilitySafe(SizeBox_Description, ESlateVisibility::Collapsed);
	}
	else
	{
		WRUIUtility::SetVisibilitySafe(SizeBox_Description, ESlateVisibility::SelfHitTestInvisible);
		WRUIUtility::SetTextSafe(WRTextBlock_Description, InTable->PopupDescrition);
	}

	PlayAnimation(Ani_ShowOn);
}

void UWRUIPanel_Tutorial::SetProgressBar(float IN InRate)
{
	WRUIUtility::SetVisibilitySafe(ProgressBar, ESlateVisibility::SelfHitTestInvisible);
	ProgressBar->SetPercent(InRate);
}

void UWRUIPanel_Tutorial::SetRepeatCount(FString& IN InText)
{
	WRUIUtility::SetVisibilitySafe(TextBlock_RepeatCount, ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetTextSafe(TextBlock_RepeatCount, InText);
}

void UWRUIPanel_Tutorial::CloseTutorialPanel()
{
	PlayAnimation(Ani_ShowOff);
}