// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryInnerCategory.h"
#include "UI/CustomWidget/WRFocusButton.h"

#include "WidgetPath.h"
#include "SWindow.h"
#include "Button.h"

#include "Manager/WRInventoryManager.h"
#include "Utility/WRUIUtility.h"
#include "UI/Component/WRUIComponentFocus.h"
#include "UI/Panel/WRUIPanel_Inventory.h"
#include "Manager/WRUIManager.h"
#include "WRUISlot_InventoryStorage.h"


void UWRUISlot_InventoryInnerCategory::NativeConstruct()
{
	Super::NativeConstruct();

	int32 nSlotIndex = 0;
	bool bCreating = true;
	while (bCreating)
	{
		FString WidgetName = FString::Printf(TEXT("FocusButton_%d"), nSlotIndex);
		UWRFocusButton* pFocusButton = Cast<UWRFocusButton>(GetWidgetFromName(FName(*WidgetName)));
		if (pFocusButton != nullptr)
		{
			pFocusButton->SetIndex(nSlotIndex);
			pFocusButton->OnClicked.BindUObject(this, &UWRUISlot_InventoryInnerCategory::OnClicked_List);

			// Set Focus Change Delegate
			FOnFocusChanged FocusChangedDelegate;
			FocusChangedDelegate.BindUObject(this, &UWRUISlot_InventoryInnerCategory::OnFocusChanged);
			pFocusButton->SetFocusChangedDelegate(FocusChangedDelegate);

			FocusButtonList.Add(pFocusButton);
			++nSlotIndex;
		}
		else
		{
			bCreating = false;
		}
	}

	// UI Component
	WRUIComponentFocus* UIComponentFocus = AddUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr && FocusButtonList.IsValidIndex(0))
	{
		UIComponentFocus->SetFocusButton(FocusButtonList[0], true);

		// Set Focus Change Delegate
// 		FWRUIComponentFocusChangedDelegate Delegate;
// 		Delegate.BindUObject(this, &UWRUISlot_InventoryInnerCategory::OnFocusChanged);
// 		UIComponentFocus->SetFocusChangedDelegate(Delegate);
	}
}

void UWRUISlot_InventoryInnerCategory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

FReply UWRUISlot_InventoryInnerCategory::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Handled();

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
			return FReply::Handled();
		}
		break;
		case EWRInput::Cross:
		{
			// [돌아가기] : 메인 카테고리로 돌아가기
			WRInventoryManager::Get()->SwitchInventory(EWRInventoryMainSwitcher::Category);
			WRInventoryManager::Get()->PlaySound(EWRInventorySoundType::Cross);
			return FReply::Handled();
		}
		break;
		}
	}

	return FReply::Unhandled();
}

void UWRUISlot_InventoryInnerCategory::OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet)
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		// 기본 호출
		UIComponentFocus->OnFocusChanged(InFocusButton, InSet);

		// 추가 작업
		if (InSet)
		{
			WRInventoryManager::Get()->PlaySound(EWRInventorySoundType::FocusMoveInnerCategory);
		}
	}
}

void UWRUISlot_InventoryInnerCategory::OnClicked_List(int32 InIndex)
{	
	WRInventoryManager::Get()->SetSelectInnerCategory(InIndex);
}

void UWRUISlot_InventoryInnerCategory::UpdateInnerCategory()
{
	EWRInventoryCategoryType CurrentInventoryMainCategory = WRInventoryManager::Get()->GetCurrentInventoryMainCategory();
	const FWRInventoryInnerCategoryInfo& InnerCategoryInfo = InnerCategoryInfos.FindRef(CurrentInventoryMainCategory);

	UWRFocusButton* LastButton = nullptr;

	int32 nIndex = 0;
	for (auto& EachButton : FocusButtonList)
	{
		if (InnerCategoryInfo.Names.IsValidIndex(nIndex))
		{
			EachButton->SetContents(InnerCategoryInfo.Names[nIndex].ToString());
			EachButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			EachButton->SetSelect(false);
			EachButton->SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Escape);
			++nIndex;

			LastButton = EachButton;
		}
		else
		{
			EachButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (LastButton)
	{
		UWRUIPanel_Inventory* Inventory = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
		if (Inventory)
		{
			UWRUISlot_InventoryStorage* Storage = Inventory->GetInventoryStorageUI();
			if (Storage)
			{
				UWRFocusButton* StorageDefaultButton = Storage->GetDefaultFocusButton();
				if (StorageDefaultButton)
				{
					LastButton->SetNavigationRuleExplicit(EUINavigation::Down, StorageDefaultButton);
				}
			}
		}
	}
}

void UWRUISlot_InventoryInnerCategory::SetSelectInnerCategory(const int32 IN InIndex, const bool IN InSet)
{
	if (FocusButtonList.IsValidIndex(InIndex) && FocusButtonList[InIndex]->IsValidLowLevel())
	{
		FocusButtonList[InIndex]->SetSelect(InSet);
	}
}