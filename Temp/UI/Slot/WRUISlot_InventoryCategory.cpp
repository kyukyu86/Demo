// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryCategory.h"
#include "Define/WRUIDefine.h"
#include "UI/CustomWidget/WRFocusButton.h"
#include "Manager/WRInventoryManager.h"
#include "Enum/EWRUIEnum.h"

#include "WidgetPath.h"
#include "SWindow.h"
#include "Button.h"

#include "Utility/WRUIUtility.h"
#include "Manager/WRUIManager.h"
#include "UI/Component/WRUIComponentFocus.h"
#include "Controller/WRVideoChatController.h"
#include "Manager/WRVoiceContentsManager.h"

void UWRUISlot_InventoryCategory::NativeConstruct()
{
	Super::NativeConstruct();

	FOnFocusChanged FocusChangedDelegate;
	FocusChangedDelegate.BindUObject(this, &UWRUISlot_InventoryCategory::OnFocusChanged);

	if (FocusButton_Equipment)
	{
		FocusButton_Equipment->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_InventoryCategory::OnClickedEquipment);
		FocusButton_Equipment->SetFocusChangedDelegate(FocusChangedDelegate);
	}
	if (FocusButton_Costume)
	{
		FocusButton_Costume->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_InventoryCategory::OnClickedCostume);
		FocusButton_Costume->SetFocusChangedDelegate(FocusChangedDelegate);
	}
	if (FocusButton_Vehicle)
	{
		FocusButton_Vehicle->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_InventoryCategory::OnClickedVehicle);
		FocusButton_Vehicle->SetFocusChangedDelegate(FocusChangedDelegate);
	}
	if (FocusButton_Consume)
	{
		FocusButton_Consume->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_InventoryCategory::OnClickedConsume);
		FocusButton_Consume->SetFocusChangedDelegate(FocusChangedDelegate);
	}

	// [ 2020-9-7 : kyu ] 임시 추가 (feat. 승엽씨)
	if (FocusButton_VideoChat)
	{
		FocusButton_VideoChat->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_InventoryCategory::OnClickedVideoChat);
		FocusButton_VideoChat->SetFocusChangedDelegate(FocusChangedDelegate);
	}

	// UI Component
	WRUIComponentFocus* UIComponentFocus = AddUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		UIComponentFocus->SetFocusButton(FocusButton_Equipment, true);		

		// Set Focus Change Delegate
// 		FWRUIComponentFocusChangedDelegate Delegate;
// 		Delegate.BindUObject(this, &UWRUISlot_InventoryCategory::OnFocusChanged);
// 		UIComponentFocus->SetFocusChangedDelegate(Delegate);
	}
}

void UWRUISlot_InventoryCategory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

FReply UWRUISlot_InventoryCategory::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
	{
		return FReply::Handled();
	}

	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (ConvertedInputType != EWRInput::None)
	{
		switch (ConvertedInputType)
		{
		case EWRInput::Triangle:
		{

		}
		break;
		case EWRInput::Square:
		{

		}
		break;
		case EWRInput::Circle:
		{
			// [선택] : 해당 카테고리 선택
			OnClickedFocusedButton();
			WRInventoryManager::Get()->PlaySound(EWRInventorySoundType::SelectCategory);
			return FReply::Handled();
		}
		break;
		case EWRInput::Cross:
		{
			// [돌아가기] : 인벤토리 닫기
			WRInventoryManager::Get()->CloseInventory();
			return FReply::Handled();
		}
		break;
		}
	}
	
	return FReply::Unhandled();
}

void UWRUISlot_InventoryCategory::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	//if (Animation == Anim_Opening)
	if (Animation->MovieScene->GetFName() == FName("OpenAnimation"))
	{
		WRInventoryManager::Get()->OnCategoryOpenDirectionFinished();
	}
	else
	{
		WRInventoryManager::Get()->SwitchInventory(EWRInventoryMainSwitcher::Contents, SelectedCategory);
	}
}

void UWRUISlot_InventoryCategory::OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet)
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		// 기본 호출
		UIComponentFocus->OnFocusChanged(InFocusButton, InSet);

		// 추가 작업
		if (InSet)
		{
			WRInventoryManager::Get()->PlaySound(EWRInventorySoundType::FocusMoveMainCategory);
		}
	}
}

void UWRUISlot_InventoryCategory::OnClickedEquipment()
{
	OnCategorySelected(EWRInventoryCategoryType::Equipment);
}

void UWRUISlot_InventoryCategory::OnClickedCostume()
{
	OnCategorySelected(EWRInventoryCategoryType::Costume);
}

void UWRUISlot_InventoryCategory::OnClickedVehicle()
{
	OnCategorySelected(EWRInventoryCategoryType::Vehicle);
}

void UWRUISlot_InventoryCategory::OnClickedConsume()
{
	OnCategorySelected(EWRInventoryCategoryType::Consume);
}

void UWRUISlot_InventoryCategory::OnClickedVideoChat()
{	
	WRInventoryManager::Get()->CompletedInventoryCloseDelegate.BindLambda([]()
	{
		// 화상통화 켜는 작업 (승엽씨)
		WRVoiceContentsManager::Get()->GetVideoChatController().OpenUI_VideoChatUserList();
	});

	// 인벤 닫고
	WRInventoryManager::Get()->CloseInventory();
}

void UWRUISlot_InventoryCategory::OnOpening()
{
	PlayAnimationByName(FName("OpenAnimation"));
}

void UWRUISlot_InventoryCategory::OnCategorySelected(const EWRInventoryCategoryType IN InCategory)
{
	if (IsPlayingAnimation())
		return;

	SelectedCategory = InCategory;
	FName SelectAnimName = "";

	switch (InCategory)
	{
	case EWRInventoryCategoryType::Equipment:
	{
		SelectAnimName = "SelectEquipmentAnimation";
	}
	break;

	case EWRInventoryCategoryType::Costume:
	{
		SelectAnimName = "SelectCostumeAnimation";
	}
	break;

	case EWRInventoryCategoryType::Vehicle:
	{
		SelectAnimName = "SelectVehicleAnimation";
	}
	break;

	case EWRInventoryCategoryType::Consume:
	{
		SelectAnimName = "SelectConsumeAnimation";
	}
	break;
	}

	// 애니가 있으면 애니 재생하고, 아니면 바로 카테고리 선택으로
	if (SelectAnimName != "")
	{
		PlayAnimationByName(FName(SelectAnimName));
	}
	else
	{
		WRInventoryManager::Get()->SwitchInventory(EWRInventoryMainSwitcher::Contents, InCategory);
	}
}