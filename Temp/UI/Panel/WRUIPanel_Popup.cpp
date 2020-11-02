// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Popup.h"

#include "Utility/WRUIUtility.h"
#include "Enum/EWRCharacter.h"

#include "Define/WRUIDefine.h"

#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Manager/WRUIPathManager.h"
#include "Manager/WRPopupManager.h"

#include "CanvasPanel.h"
#include "Overlay.h"

#include "UI/CustomWidget/WRTextBlock.h"
#include "UI/Slot/WRUISlot_QuantitySelector.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUIPanel_Popup::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Title);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Contents);

	BIND_WIDGET(UCanvasPanel, CanvasPanel_FirstButton);
	BIND_WIDGET(UImage, Image_FirstButton);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_FirstButton);

	BIND_WIDGET(UCanvasPanel, CanvasPanel_SecondButton);
	BIND_WIDGET(UImage, Image_SecondButton);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_SecondButton);

	BIND_WIDGET(UOverlay, Overlay_Quantity);

	SetFocusableInputMode(true, this);
}

void UWRUIPanel_Popup::NativeDestruct()
{
	if (PopupInfo.bFocusReleaseWhenClosed)
	{
		SetFocusableInputMode(false);
	}

	DestroyQuantity();

	Super::NativeDestruct();
}

FReply UWRUIPanel_Popup::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Unhandled();

	EWRInput WRInput = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (PopupInfo.FirstBtnKey == WRInput)
	{
		OnButtonClicked_First();
		return FReply::Handled();
	}
	else if (PopupInfo.SecondBtnKey == WRInput)
	{
		OnButtonClicked_Second();
		return FReply::Handled();
	}
	else
	{
		if (Slot_Quantity != nullptr)
		{
			return Slot_Quantity->NativeOnKeyDown(InGeometry, InKeyEvent);
		}
	}

	//return FReply::Unhandled();
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWRUIPanel_Popup::OnButtonClicked_First()
{
	if (PopupInfo.Delegate_First.IsBound())
	{
		PopupInfo.Delegate_First.ExecuteIfBound();
	}

	ClosePopup();
}

void UWRUIPanel_Popup::OnButtonClicked_Second()
{
	if (PopupInfo.Delegate_Second.IsBound())
	{
		PopupInfo.Delegate_Second.ExecuteIfBound();
	}

	ClosePopup();
}

void UWRUIPanel_Popup::SetText(UWRTextBlock* IN InTextBlock, const FString IN InText)
{
	if (InTextBlock == nullptr)
		return;

	if (InText.IsEmpty())
	{
		InTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		InTextBlock->SetText(InText);
		InTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWRUIPanel_Popup::ClosePopup()
{
	WRPopupManager::Get()->ClosePopup();
}

void UWRUIPanel_Popup::MakeQuantity()
{
	auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		strQuantityAsyncKey = "";

		if (Overlay_Quantity == nullptr)
			return;

		Slot_Quantity = Cast<UWRUISlot_QuantitySelector>(InObject);
		if (Slot_Quantity == nullptr)
			return;

		Overlay_Quantity->AddChildToOverlay(Slot_Quantity);

		FWRQuantityChangeDelegate Delegate;
		Delegate.BindUObject(this, &UWRUIPanel_Popup::OnQuantityChanged);
		Slot_Quantity->SetQuantityChangeDelegate(Delegate);
		Slot_Quantity->OnFocus();
		Slot_Quantity->SetMaxQuantity(PopupInfo.QuantityInfo.MaxQuantity);
	});
	strQuantityAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncCreateWidget(WRUIPathManager::Get()->GetUIPath(EWRUISlotKind::QuantitySelector), AsyncCreateComplete);
}

void UWRUIPanel_Popup::DestroyQuantity()
{
	if (Slot_Quantity->IsValidLowLevel())
	{
		Overlay_Quantity->RemoveChild(Slot_Quantity);
		Overlay_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		Slot_Quantity = nullptr;
	}
}

void UWRUIPanel_Popup::OnQuantityChanged(const int32 IN InQuantity)
{
	PopupInfo.QuantityInfo.Delegate_QuantityChanged.ExecuteIfBound(InQuantity);
}

void UWRUIPanel_Popup::SetPopupInfo(const FWRPopupInfo& IN InInfo)
{
	PopupInfo = InInfo;

	SetText(WRTextBlock_Title, PopupInfo.Title);
	SetText(WRTextBlock_Contents, PopupInfo.Contents);

	UpdateOption();

	// Button Visibility
	if (InInfo.eButtonFlag == EPopupButtonFlag::OK)
	{
		CanvasPanel_FirstButton->SetVisibility(ESlateVisibility::Visible);
		CanvasPanel_SecondButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (InInfo.eButtonFlag == EPopupButtonFlag::OK_CANCEL)
	{
		CanvasPanel_FirstButton->SetVisibility(ESlateVisibility::Visible);
		CanvasPanel_SecondButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CanvasPanel_FirstButton->SetVisibility(ESlateVisibility::Collapsed);
		CanvasPanel_SecondButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	// Button Text
	if (InInfo.FirstBtnText.IsEmpty() == false)
	{
		WRTextBlock_FirstButton->SetText(InInfo.FirstBtnText);
	}
	else
	{
		WRTextBlock_FirstButton->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_ConfirmKey"));
	}
	if (InInfo.SecondBtnText.IsEmpty() == false)
	{
		WRTextBlock_SecondButton->SetText(InInfo.SecondBtnText);
	}
	else
	{
		WRTextBlock_SecondButton->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_CancelKey"));
	}
}

void UWRUIPanel_Popup::UpdateOption()
{
	EPopupOptionFlag OptionFlag = (EPopupOptionFlag)PopupInfo.eOptionFlag;

	// + Options

	if ((OptionFlag & EPopupOptionFlag::Quantity) != EPopupOptionFlag::None)
	{
		MakeQuantity();
	}
	else
	{
		DestroyQuantity();
	}
}

void UWRUIPanel_Popup::SetButtonVisibility(const bool IN InFirst, const bool IN InSecond)
{
	if (CanvasPanel_FirstButton != nullptr)
	{
		CanvasPanel_FirstButton->SetVisibility(InFirst == true ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (CanvasPanel_SecondButton != nullptr)
	{
		CanvasPanel_SecondButton->SetVisibility(InSecond == true ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}