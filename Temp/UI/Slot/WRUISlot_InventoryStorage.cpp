// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryStorage.h"
#include "WRUISlot_InventoryItemSlot.h"

#include "Define/WRUIDefine.h"

#include "UI/CustomWidget/WRFocusButton.h"
#include "UI/CustomWidget/WRTextBlock.h"

#include "WidgetPath.h"
#include "SWindow.h"
#include "Button.h"

#include "Item/WRItem.h"

#include "Manager/WRInventoryManager.h"

#include "Table/WRItemTable.h"
#include "Table/Base/WRTableManager.h"
#include "Utility/WREnumUtility.h"
#include "UI/Component/WRUIComponentFocus.h"


void UWRUISlot_InventoryStorage::NativeConstruct()
{
	Super::NativeConstruct();

	int32 nIndex = 0;
	bool bCreating = true;
	while (bCreating)
	{
		FString WidgetName = FString::Printf(TEXT("Slot_InventorySlot_%d"), nIndex);
		UWRUISlot_InventoryItemSlot* pItemSlot = Cast<UWRUISlot_InventoryItemSlot>(GetWidgetFromName(FName(*WidgetName)));
		if (pItemSlot != nullptr)
		{
			pItemSlot->SetSlotIndex(nIndex);
			pItemSlot->SetRenderOpacity(0.f);	// 연출용

			// Set Focus Change Delegate
			FOnFocusChanged FocusChangedDelegate;
			FocusChangedDelegate.BindUObject(this, &UWRUISlot_InventoryStorage::OnFocusChanged);
			pItemSlot->GetFocusButton()->SetFocusChangedDelegate(FocusChangedDelegate);

			ItemSlotList.Add(pItemSlot);
			++nIndex;
		}
		else
		{
			bCreating = false;
		}
	}

	// UI Component
	WRUIComponentFocus* UIComponentFocus = AddUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		UIComponentFocus->SetFocusButton(ItemSlotList[0]->GetFocusButton(), true);
	}

	bool bUp = false;
	bool bDown = false;
	bool bLeft = false;
	bool bRight = false;
	for (int32 i = 0; i < ItemSlotList.Num(); ++i)
	{
		int32 UpIndex = i - SlotWidth;
		if (ItemSlotList.IsValidIndex(UpIndex))
		{
			// 0 ~ (n-1) 번은 위가 없음
			if (i > (SlotWidth - 1))
			{
				ItemSlotList[i]->SetNavigationRuleExplicit(EUINavigation::Up, ItemSlotList[UpIndex]->GetFocusButton());
				bUp = true;
			}
		}
		int32 DownIndex = i + SlotWidth;
		if (ItemSlotList.IsValidIndex(DownIndex))
		{
			// 마지막 n 번부터는 밑이 없음
			if (i < (ItemSlotList.Num() - SlotWidth))
			{
				ItemSlotList[i]->SetNavigationRuleExplicit(EUINavigation::Down, ItemSlotList[DownIndex]->GetFocusButton());
				bDown = true;
			}
		}		
		int32 LeftIndex = i - 1;
		if (ItemSlotList.IsValidIndex(LeftIndex))
		{
			// 0과 n배수는 왼쪽이 없음
			if (i != 0 && (i % SlotWidth) != 0)
			{
				ItemSlotList[i]->SetNavigationRuleExplicit(EUINavigation::Left, ItemSlotList[LeftIndex]->GetFocusButton());
				bLeft = true;
			}
		}
		int32 RightIndex = i + 1;
		if (ItemSlotList.IsValidIndex(RightIndex))
		{
			// n-1의 배수는 오른쪽이 없음
			if (((i + 1) % SlotWidth) != 0)
			{
				ItemSlotList[i]->SetNavigationRuleExplicit(EUINavigation::Right, ItemSlotList[RightIndex]->GetFocusButton());
				bRight = true;
			}
		}
	}

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Title);
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Prev, FocusButton_Prev->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_InventoryStorage::OnClicked_Prev););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Next, FocusButton_Next->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_InventoryStorage::OnClicked_Next););
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Page);
}

void UWRUISlot_InventoryStorage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

// 	if (bAnalogDelayTime)
// 	{
// 		fNavigationElapsedDelayTIme += InDeltaTime;
// 		if (fNavigationElapsedDelayTIme >= AnalogNavigationDelay)
// 		{
// 			bAnalogDelayTime = false;
// 			fNavigationElapsedDelayTIme = 0.f;
// 		}
// 	}
}

FReply UWRUISlot_InventoryStorage::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bAnalogDelayTime)
	{
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

// FReply UWRUISlot_InventoryStorage::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
// {
// 	if (bAnalogDelayTime)
// 	{
// 		return FReply::Handled();
// 	}
// 	FReply ResultReply = Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);
// 	bAnalogDelayTime = true;
// 
// 	return ResultReply;
// }

void UWRUISlot_InventoryStorage::OnClicked_Prev()
{
	WRInventoryManager::Get()->OnInputEvent(EWRInput::TouchPadDragRightX);

}

void UWRUISlot_InventoryStorage::OnClicked_Next()
{
	WRInventoryManager::Get()->OnInputEvent(EWRInput::TouchPadDragLeftX);

}

class UWRUISlot_InventoryItemSlot* UWRUISlot_InventoryStorage::FindSlot(class UWRFocusButton* IN InFocusButton)
{
	for (auto& EachSlot : ItemSlotList)
	{
		if (EachSlot->GetFocusButton() == InFocusButton)
			return EachSlot;
	}
	return nullptr;
}

void UWRUISlot_InventoryStorage::UpdateStorageTitle()
{
	FString strTitle = "Item List";

	EWRInventoryCategoryType CurrentMainCategory = WRInventoryManager::Get()->GetCurrentInventoryMainCategory();
	switch (CurrentMainCategory)
	{
	case EWRInventoryCategoryType::Equipment:
	case EWRInventoryCategoryType::Costume:
	{
		EWRRegistrationSlotType CurrentSelectedRegistrationSlotType = WRInventoryManager::Get()->GetCurrentInventorySelectedRegistrationSlot();
		switch (CurrentSelectedRegistrationSlotType)
		{
		case EWRRegistrationSlotType::None:
		{

		}
		break;
		case EWRRegistrationSlotType::LeftWeapon_1:
		{
			strTitle = "Left Weapon - 1";
		}
		break;
		case EWRRegistrationSlotType::LeftWeapon_2:
		{
			strTitle = "Left Weapon - 2";
		}
		break;
		case EWRRegistrationSlotType::LeftWeapon_3:
		{
			strTitle = "Left Weapon - 3";
		}
		break;

		case EWRRegistrationSlotType::RightWeapon_1:
		{
			strTitle = "Right Weapon - 1";
		}
		break;
		case EWRRegistrationSlotType::RightWeapon_2:
		{
			strTitle = "Right Weapon - 2";
		}
		break;
		case EWRRegistrationSlotType::RightWeapon_3:
		{
			strTitle = "Right Weapon - 3";
		}
		break;

		default:
		{
			strTitle = WREnumUtility::EnumToString("EWRRegistrationSlotType", CurrentSelectedRegistrationSlotType);
		}
		break;
	}
	break;
	}
	
	case EWRInventoryCategoryType::Vehicle:
	{
		strTitle = "Vehicle";
	}
	break;

	case EWRInventoryCategoryType::Consume:
	{
		EWRInventoryConsumeCategory InnerCategoryType = (EWRInventoryConsumeCategory)WRInventoryManager::Get()->GetCurrentInventoryInnerCategoryType();
		switch (InnerCategoryType)
		{
		case EWRInventoryConsumeCategory::Material:
		{
			strTitle = "Material";
		}
		break;
		case EWRInventoryConsumeCategory::Interior:
		{
			strTitle = "Interior";
		}
		break;
		case EWRInventoryConsumeCategory::Emote:
		{
			strTitle = "Emote";
		}
		break;
		case EWRInventoryConsumeCategory::Placement:
		{
			strTitle = "Placement";
		}
		break;
		}		
	}
	break;
	}

	WRTextBlock_Title->SetText(strTitle);
}

void UWRUISlot_InventoryStorage::UpdateStorage(TDoubleLinkedList<class WRItemBase*>& IN InItemList)
{
	UWRUISlot_InventoryItemSlot* LastFocusedItemSlot = nullptr;
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		LastFocusedItemSlot = FindSlot(UIComponentFocus->GetFocusedButton());
	}
	

	int32 nSlotIndex = 0;
	for (auto Node = InItemList.GetHead(); Node != nullptr; Node = Node->GetNextNode())
	{
		WRItemBase* ItemBase = Node->GetValue();
		if (ItemBase == nullptr)
			continue;

		if (ItemSlotList.IsValidIndex(nSlotIndex) == false)
			continue;

		ItemSlotList[nSlotIndex]->SetItemIcon(ItemBase);

		// [ 2020-7-15 : kyu ] 업데이트 시 동일슬롯에 대해서 갱신해준다
		if (ItemSlotList[nSlotIndex] == LastFocusedItemSlot)
		{
			WRInventoryManager::Get()->UpdateFocusedInventorySlot(ItemBase->GetISN(), EWRInventorySlotGroup::Storage, LastFocusedItemSlot->GetSlotInfo().GetRegistrationSlotType());
		}

		++nSlotIndex;
	}

	for (int32 i = nSlotIndex; ; ++nSlotIndex)
	{
		if (ItemSlotList.IsValidIndex(nSlotIndex) == false)
			break;

		// 이건 숨기던가 빈 슬롯이던가...
		ItemSlotList[nSlotIndex]->SetItemIcon(nullptr);

		// [ 2020-7-15 : kyu ] 업데이트 시 동일슬롯에 대해서 갱신해준다
		if (ItemSlotList[nSlotIndex] == LastFocusedItemSlot)
		{
			WRInventoryManager::Get()->UpdateFocusedInventorySlot(INVALID_ISN, EWRInventorySlotGroup::Storage, LastFocusedItemSlot->GetSlotInfo().GetRegistrationSlotType());
		}
	}

	UpdatePage();
	UpdateStorageTitle();
}

void UWRUISlot_InventoryStorage::UpdateStorageSlot(const ISN_t IN InISN)
{
	for (auto& EachSlot : ItemSlotList)
	{
		if (EachSlot->GetISN() == InISN)
		{
			EachSlot->UpdateSlot();
		}
	}
}

void UWRUISlot_InventoryStorage::UpdatePage()
{
	int32 CurPage = WRInventoryManager::Get()->GetCurrentInventoryStoragePage();
	int32 MaxPage = WRInventoryManager::Get()->GetInventoryStorageMaxPage();

	FString strPage = FString::Printf(TEXT("%d / %d"), CurPage, MaxPage);
	WRTextBlock_Page->SetText(strPage);
}

void UWRUISlot_InventoryStorage::SetStorageSlotSelected(const ISN_t IN InISN)
{
	if (InISN == INVALID_ISN)
	{
		// InISN에 해당하는 슬롯 선택해제
		for (auto& EachSlot : ItemSlotList)
		{
			EachSlot->SetSelect(false);
		}		
		RefocusOnLastFocusedButton();
	}
	else
	{
		// InISN에 해당하는 슬롯 선택설정
		for (auto& EachSlot : ItemSlotList)
		{
			if (EachSlot->GetISN() == InISN)
			{
				EachSlot->SetSelect(true);
			}
		}
	}
}

void UWRUISlot_InventoryStorage::OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet)
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		// 기본 호출
		UIComponentFocus->OnFocusChanged(InFocusButton, InSet);

		// 추가 작업
		UWRUISlot_InventoryItemSlot* ItemSlot = FindSlot(InFocusButton);
		if (ItemSlot && InSet)
		{
			WRInventoryManager::Get()->OnFocusedInventorySlot(ItemSlot->GetISN(), EWRInventorySlotGroup::Storage, ItemSlot->GetSlotInfo().GetRegistrationSlotType(), true);
		}
	}
}

void UWRUISlot_InventoryStorage::HideAllSlot()
{
	for (auto& EachSlot : ItemSlotList)
	{
		EachSlot->SetRenderOpacity(0.f);
	}
}
