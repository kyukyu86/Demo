// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryEquipList.h"
#include "WRUISlot_InventoryItemSlot.h"

#include "WidgetPath.h"
#include "SWindow.h"
#include "Button.h"
#include "UI/CustomWidget/WRFocusButton.h"
#include "Manager/WRInventoryManager.h"
#include "Item/WRItem.h"
#include "UI/Component/WRUIComponentFocus.h"
#include "Manager/WRItemManager.h"



void UWRUISlot_InventoryEquipList::NativeConstruct()
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

			// Set Focus Change Delegate
			FOnFocusChanged Delegate;
			Delegate.BindUObject(this, &UWRUISlot_InventoryEquipList::OnFocusChanged);
			pItemSlot->GetFocusButton()->SetFocusChangedDelegate(Delegate);

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
		if (ItemSlotList.IsValidIndex(0))
		{
			// Set Focus Button & Default Focus Button
			UIComponentFocus->SetFocusButton(ItemSlotList[0]->GetFocusButton(), true);

			// Set Focus Change Delegate
// 			FWRUIComponentFocusChangedDelegate Delegate;
// 			Delegate.BindUObject(this, &UWRUISlot_InventoryEquipList::OnFocusChanged);
// 			UIComponentFocus->SetFocusChangedDelegate(Delegate);
		}
	}
}

void UWRUISlot_InventoryEquipList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

FReply UWRUISlot_InventoryEquipList::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return FReply::Unhandled();
}

class UWRUISlot_InventoryItemSlot* UWRUISlot_InventoryEquipList::FindSlot(class UWRFocusButton* IN InFocusButton)
{
	for (auto& EachSlot : ItemSlotList)
	{
		if (EachSlot->GetFocusButton() == InFocusButton)
			return EachSlot;
	}
	return nullptr;
}

void UWRUISlot_InventoryEquipList::UpdateEquipList(TDoubleLinkedList<class WREquipItemBase *>& IN InEquippedItemList)
{
	UWRUISlot_InventoryItemSlot* LastFocusedItemSlot = nullptr;
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		LastFocusedItemSlot = FindSlot(UIComponentFocus->GetFocusedButton());
	}


	for (auto EachSlot : ItemSlotList)
	{
		// ���� ���� ��Ͻ����� Ÿ���� �����Ȱ� ������ ����
		if (EachSlot->GetRegistrationSlotType() == EWRRegistrationSlotType::None)
		{
			EachSlot->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		// ������ ������ ����߿� Ÿ���� �´� �������� ã�Ƽ� ���� ����
		bool bSet = false;
		for (auto Node = InEquippedItemList.GetHead(); Node != nullptr; Node = Node->GetNextNode())
		{
			WREquipItemBase* EquipItemBase = Node->GetValue();
			if (EquipItemBase == nullptr)
				continue;

			if(EachSlot->GetRegistrationSlotType() == WRInventoryManager::Get()->GetRegisteredSlotType(EquipItemBase->GetISN())
				|| EachSlot->GetRegistrationSlotType() == WRInventoryManager::Get()->ConvertEquippableTypeToRegistrationType(EquipItemBase->GetEquippableSlot()))
			{
				EachSlot->SetItemIcon(EquipItemBase);
				EachSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

				// [ 2020-7-15 : kyu ] ������Ʈ �� ���Ͻ��Կ� ���ؼ� �������ش�
				if (EachSlot == LastFocusedItemSlot)
				{
					WRInventoryManager::Get()->UpdateFocusedInventorySlot(EquipItemBase->GetISN(), EWRInventorySlotGroup::EquipList, LastFocusedItemSlot->GetSlotInfo().GetRegistrationSlotType());
				}

				bSet = true;
				break;
			}
		}

		// �������� ���� ����
		if (bSet == false)
		{
			EachSlot->SetItemIcon(nullptr);
			EachSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			// [ 2020-7-15 : kyu ] ������Ʈ �� ���Ͻ��Կ� ���ؼ� �������ش�
			if (EachSlot == LastFocusedItemSlot)
			{
				WRInventoryManager::Get()->UpdateFocusedInventorySlot(INVALID_ISN, EWRInventorySlotGroup::EquipList, LastFocusedItemSlot->GetSlotInfo().GetRegistrationSlotType());
			}
		}
	}
}

void UWRUISlot_InventoryEquipList::SetEquipListSlotSelected(const EWRRegistrationSlotType IN InSlotType, const bool IN InSet)
{
	for (auto& EachSlot : ItemSlotList)
	{
		if (EachSlot == nullptr)
			continue;

		if (EachSlot->GetRegistrationSlotType() != InSlotType)
			continue;

		EachSlot->SetSelect(InSet);
	}
}

void UWRUISlot_InventoryEquipList::SetStorageSlotSelected(const ISN_t IN InISN)
{
	if (InISN == INVALID_ISN)
	{
		// ��� ���� ���ð��ɻ��� ����
		for (auto& EachSlot : ItemSlotList)
		{
			if (EachSlot == nullptr)
				continue;

			EachSlot->SetSelectable(false);

			// [ 2020-8-5 : kyu ] �ӽ� �׺���̼� ���� ����
			if (WRInventoryManager::Get()->ConvertRegistrationTypeToEquippableType(EachSlot->GetRegistrationSlotType()) == EWRItemEquippableSlot::LeftWeapon)
			{
				EachSlot->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
			}
			else if (WRInventoryManager::Get()->ConvertRegistrationTypeToEquippableType(EachSlot->GetRegistrationSlotType()) == EWRItemEquippableSlot::RightWeapon)
			{
				EachSlot->SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Escape);
			}
		}

		// �׺���̼� �ʱ�ȭ
		SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Escape);
	}
	else
	{
		WREquipItemBase* FoundEquipItemBase = WRItemManager::Get()->FindItem<WREquipItemBase>(InISN);
		if (FoundEquipItemBase == nullptr)
			return;

		UWRUISlot_InventoryItemSlot* ItemSlot = nullptr;

		// InISN�� �ش��ϴ� ���� ���ð��ɻ��� ����
		for (auto& EachSlot : ItemSlotList)
		{
			if (EachSlot == nullptr)
				continue;

			if (FoundEquipItemBase->GetEquippableSlot() != WRInventoryManager::Get()->ConvertRegistrationTypeToEquippableType(EachSlot->GetRegistrationSlotType()))
				continue;

			EachSlot->SetSelectable(true);

			// [ 2020-8-5 : kyu ] �ӽ� �׺���̼� ����
			if (WRInventoryManager::Get()->ConvertRegistrationTypeToEquippableType(EachSlot->GetRegistrationSlotType()) == EWRItemEquippableSlot::LeftWeapon)
			{
				EachSlot->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Stop);
			}
			else if (WRInventoryManager::Get()->ConvertRegistrationTypeToEquippableType(EachSlot->GetRegistrationSlotType()) == EWRItemEquippableSlot::RightWeapon)
			{
				EachSlot->SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Stop);
			}

			if (ItemSlot == nullptr)
				ItemSlot = EachSlot;
		}

		// ���ð����� ������ ù��°�� ��Ŀ��
		if (ItemSlot != nullptr)
		{
			SetFocusableInputMode(true, ItemSlot->GetFocusButton());
		}

		// �׺���̼� ����
		SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Stop);
	}
}

void UWRUISlot_InventoryEquipList::OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet)
{
	WRUIComponentFocus* UIComponentFocus = FindUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		// �⺻ ȣ��
		UIComponentFocus->OnFocusChanged(InFocusButton, InSet);

		// �߰� �۾�
		UWRUISlot_InventoryItemSlot* FoundSlot = FindSlot(InFocusButton);
		if (FoundSlot && InSet)
		{
			WRInventoryManager::Get()->OnFocusedInventorySlot(FoundSlot->GetISN(), EWRInventorySlotGroup::EquipList, FoundSlot->GetSlotInfo().GetRegistrationSlotType());
		}
	}
}
