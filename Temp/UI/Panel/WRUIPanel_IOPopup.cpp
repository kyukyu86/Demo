// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_IOPopup.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Enum/EWRCharacter.h"

#include "Image.h"
#include "TextBlock.h"
#include "CanvasPanel.h"
#include "ProgressBar.h"
#include "WidgetAnimation.h"

#include "Actor/Character/WRCharacterIO.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterManager.h"

#include "Component/Character/WRActorComponentInteractionObject.h"
#include "Network/Share/Define/Define_Actor.h"
#include "Manager/WRInteriorManager.h"

#include "Localization/WRLocalizationProcessor.h"


void FWRIOInteractionWidget::SetFontColor(FLinearColor IN InColor)
{
	WRUIUtility::SetTextColorSafe(TextBlock_Contents, InColor);
}


void UWRUIPanel_IOPopup::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Popup);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_PopupTitle);
	BIND_WIDGET(UTextBlock, TextBlock_PopupTitle);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_PopupContents);
	BIND_WIDGET(UTextBlock, TextBlock_PopupContents);
	BIND_WIDGET(UImage, Image_PopupBG);

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Mark);
	for (int8 i = 0; i < 2; ++i)
	{
		FString WidgetName = FString::Printf(TEXT("CanvasPanel_Slot_%d"), i);
		UCanvasPanel* CanvasPanel_Slot = Cast<UCanvasPanel>(GetWidgetFromName(FName(*WidgetName)));
		WidgetName = FString::Printf(TEXT("Image_Icon_%d"), i);
		UImage* Image_Icon = Cast<UImage>(GetWidgetFromName(FName(*WidgetName)));
		WidgetName = FString::Printf(TEXT("TextBlock_Contents_%d"), i);
		UTextBlock* TextBlock_Contents = Cast<UTextBlock>(GetWidgetFromName(FName(*WidgetName)));

		if (CanvasPanel_Slot != nullptr &&
			Image_Icon != nullptr &&
			TextBlock_Contents != nullptr)
		{
			FWRIOInteractionWidget WidgetSlot;
			WidgetSlot.CanvasPanel_Slot = CanvasPanel_Slot;
			WidgetSlot.Image_Icon = Image_Icon;
			WidgetSlot.TextBlock_Contents = TextBlock_Contents;
			InteractionWidgets.Add(WidgetSlot);
		}
	}
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Progress);
	BIND_WIDGET(UProgressBar, ProgressBar);

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Notify);
	BIND_WIDGET(UTextBlock, TextBlock_Notify);
}

void UWRUIPanel_IOPopup::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_IOPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateProgressBar(InDeltaTime);
}

void UWRUIPanel_IOPopup::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == Ani_Cancel)
	{
		OnProgressCompleted.ExecuteIfBound();
	}
}

//====================================================================================

void UWRUIPanel_IOPopup::SetDataFromTable(FWRInteractionObjectTable* IN InTable, class UWRActorComponentInteractionObject* const IN InInteractionInfo, const ISN_t IN InRelativeISN /* = INVALID_ISN */)
{
	SetNotify(false);

	uint8 SlotIndex = 0;
	if (InTable->DirectType == EWRIODirectType::KeyInput)
	{
		if (InTable->DirectRetainCount == 0
			|| InTable->DirectRetainCount > InInteractionInfo->GetDirectRetainCount())
		{
			FLinearColor FontColor = DisableFontColor;
			if (InInteractionInfo->IsInteractionableState())
			{
				FontColor = EnableFontColor;
			}
// 			if (InTable->DirectKeyFontColor.Equals(FLinearColor::Transparent) == false)
// 			{
// 				FontColor = InTable->DirectKeyFontColor;
// 			}

			SetType(EWRIOPopupType::Mark);
			SetMarkInfo(SlotIndex, InTable->DirectKeyIcon, WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, InTable->DirectKeyDescription).ToString(), FontColor);
			++SlotIndex;
		}

// 		if (InRelativeISN != INVALID_ISN && WRInteriorManager::Get()->IsSpawnedByMe(InRelativeISN))
// 		{
// 			FLinearColor FontColor = DisableFontColor;
// 			if (InInteractionInfo->IsInteractionableState())
// 			{
// 				FontColor = EnableFontColor;
// 			}
// 			SetMarkDefault(SlotIndex, FontColor);
// 			++SlotIndex;
// 		}
	}
	if (InTable->HookType != EWRHookType::None)
	{
		// + 갈고리 타입의 상호작용이 있는데, 갈고리 보조무기가 없는 경우 출력하지 않음
		// [ 2019-11-5 : kyu ] 갈고리 없거나 장착중이 아니면 갈고리 장착하라고 출력할거임

		if (InTable->HookRetainCount == 0
			|| InTable->HookRetainCount > InInteractionInfo->GetHookRetainCount())
		{
			FLinearColor FontColor = DisableFontColor;
			if (WRCharacterManager::Get()->IsEquippedHook() == false)
			{
				SetNotify(true, ChangeToHookSubWeaponNotify.ToString());
				FontColor = ExceptionFontColor;
			}
			else
			{
				if (InInteractionInfo->IsHookableState())
				{
					FontColor = EnableFontColor;
				}
			}			

			SetType(EWRIOPopupType::Mark);
			// [ 2019-11-25 : kyu ] 케이블이 활성화 되어있을 때, 케이블 타겟이라면 Action에 대해 출력해야한다
			if (WRCharacterManager::Get()->IsHookConnected()
				&& WRCharacterManager::Get()->IsCableTarget(InInteractionInfo->GetOwnerCharacter()))
			{
				bHookConnected = true;
				SetMarkInfo(SlotIndex, InTable->HookKeyActionIcon, WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, InTable->HookKeyActionDescription).ToString(), FontColor);
			}
			else
			{
				bHookConnected = false;
				SetMarkInfo(SlotIndex, InTable->HookKeyIcon, WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, InTable->HookKeyDescription).ToString(), FontColor);
			}
			++SlotIndex;
		}
	}

	for (int32 i = SlotIndex; i < InteractionWidgets.Num(); ++i)
	{
		if (InteractionWidgets[i].CanvasPanel_Slot != nullptr)
		{
			InteractionWidgets[i].CanvasPanel_Slot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (SlotIndex == 0)
		WRUIManager::Get()->CloseUI(UIHandle);
}

void UWRUIPanel_IOPopup::SetHookInfo(const FWRHookInfo& IN InHookInfo, const bool IN InConnected)
{
	SetNotify(false);

	FLinearColor FontColor = DisableFontColor;
	if (WRCharacterManager::Get()->IsEquippedHook() == false)
	{
		SetNotify(true, ChangeToHookSubWeaponNotify.ToString());
		FontColor = ExceptionFontColor;
	}
	else
	{
		FontColor = EnableFontColor;
	}

	SetType(EWRIOPopupType::Mark);
	// [ 2019-11-25 : kyu ] 케이블이 활성화 되어있을 때, 케이블 타겟이라면 Action에 대해 출력해야한다
	if (InConnected)
	{
		bHookConnected = true;
		SetMarkInfo(0, InHookInfo.strActionIconPath, InHookInfo.strActionDescription, FontColor);
	}
	else
	{
		bHookConnected = false;
		SetMarkInfo(0, InHookInfo.strTargetingIconPath, InHookInfo.strTargetingDescription, FontColor);
	}
}

void UWRUIPanel_IOPopup::SetType(const EWRIOPopupType IN InPopupType)
{
	switch (InPopupType)
	{
	case EWRIOPopupType::Popup:
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Popup, ESlateVisibility::SelfHitTestInvisible);
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Mark, ESlateVisibility::Collapsed);
	}
	break;
	case EWRIOPopupType::Mark:
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Popup, ESlateVisibility::Collapsed);
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Mark, ESlateVisibility::SelfHitTestInvisible);
	}
	break;
	}
}

//====================================================================================

void UWRUIPanel_IOPopup::SetPopupTitle(bool IN InVisible, FString IN InTitle /* = TEXT("") */)
{
	if (InVisible)
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_PopupTitle, ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_PopupTitle, ESlateVisibility::Collapsed);
		return;
	}

	WRUIUtility::SetTextSafe(TextBlock_PopupTitle, InTitle);
}

void UWRUIPanel_IOPopup::SetPopupContents(FString IN InContents)
{
	WRUIUtility::SetTextSafe(TextBlock_PopupContents, InContents);
}

//====================================================================================

void UWRUIPanel_IOPopup::SetMarkInfo(uint8 IN InSlotIndex, FString IN InIconPath, FString IN InContents, FLinearColor IN InFontColor)
{
	if (InteractionWidgets.IsValidIndex(InSlotIndex))
	{
		FWRIOInteractionWidget& WidgetData = InteractionWidgets[InSlotIndex];
		WRUIUtility::SetVisibilitySafe(WidgetData.CanvasPanel_Slot, ESlateVisibility::SelfHitTestInvisible);
		// Icon
		WRUIUtility::SetResourceObject(WidgetData.Image_Icon, InIconPath);
		// Text
		WRUIUtility::SetTextSafe(WidgetData.TextBlock_Contents, InContents);
		WRUIUtility::SetTextColorSafe(WidgetData.TextBlock_Contents, InFontColor);
	}
}

void UWRUIPanel_IOPopup::SetMarkDefault(uint8 IN InSlotIndex, FLinearColor IN InFontColor)
{
	if (InteractionWidgets.IsValidIndex(InSlotIndex))
	{
		FWRIOInteractionWidget& WidgetData = InteractionWidgets[InSlotIndex];
		WRUIUtility::SetVisibilitySafe(WidgetData.CanvasPanel_Slot, ESlateVisibility::SelfHitTestInvisible);
		// Text
		WRUIUtility::SetTextColorSafe(WidgetData.TextBlock_Contents, InFontColor);
	}
}

void UWRUIPanel_IOPopup::UpdateProgressBar(float IN InDeltaTime)
{
	if (IsCanceling())
		return;

	if (fProgressGoalTime == 0.f)
		return;

	fProgressTime += InDeltaTime;
	if (fProgressTime >= fProgressGoalTime)
	{
//		OnProgressCompleted.ExecuteIfBound();

//		WRUIManager::Get()->CloseUI(GetUIHandle());
	}
	else
	{
		float fPercent = fProgressTime / fProgressGoalTime;
		ProgressBar->SetPercent(fPercent);
	}
}

void UWRUIPanel_IOPopup::SetNotify(bool IN InVisible, FString IN InNotify /*= TEXT("")*/)
{
	if (InVisible)
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Notify, ESlateVisibility::SelfHitTestInvisible);
		WRUIUtility::SetTextSafe(TextBlock_Notify, InNotify);
	}
	else
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Notify, ESlateVisibility::Collapsed);
	}
}

//====================================================================================

void UWRUIPanel_IOPopup::ShowOnProgress(const float IN InGoalTime)
{
	if (IsCanceling())
		StopAnimation(Ani_Cancel);

	fProgressTime = 0.f;
	fProgressGoalTime = InGoalTime;

	WRUIUtility::SetVisibilitySafe(CanvasPanel_Progress, ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_Progress->SetRenderOpacity(1.f);
}

void UWRUIPanel_IOPopup::ShowOffProgress(const bool IN InComplete)
{
	fProgressTime = 0.f;
	fProgressGoalTime = 0.f;

	if (InComplete)
	{
		WRUIUtility::SetVisibilitySafe(CanvasPanel_Progress, ESlateVisibility::Collapsed);
	}
	else
	{
		OnCancel();
	}
}

void UWRUIPanel_IOPopup::OnCancel()
{
	PlayAnimation(Ani_Cancel);
}

bool UWRUIPanel_IOPopup::IsCanceling()
{
	if (Ani_Cancel != nullptr)
	{
		return IsAnimationPlaying(Ani_Cancel);
	}
	return false;
}