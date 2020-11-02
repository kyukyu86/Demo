// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#include "WRInventoryManager.h"
#include "WRCharacterManager.h"
#include "WRTutorialManager.h"
#include "WRCheersManager.h"
#include "WRPopupManager.h"
#include "WRItemManager.h"

#include "Utility/WRUIUtility.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"
#include "Table/WRWeaponTable.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Network/IMMessageSender.h"

#include "UI/Panel/WRUIPanel_EquipmentChange.h"
#include "UI/Panel/WRUIPanel_Inventory.h"
#include "UI/Slot/WRUISlot_InventoryCategory.h"
#include "UI/Slot/WRUISlot_InventoryEquipList.h"
#include "UI/Slot/WRUISlot_InventoryStorage.h"
#include "UI/Slot/WRUISlot_InventoryItemSimpleInfo.h"
#include "UI/Slot/WRUISlot_InventoryItemSlot.h"
#include "UI/Slot/WRUISlot_InventoryInnerCategory.h"
#include "UI/Slot/WRUISlot_InventoryPriceInfo.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/WRGameModeBase.h"
#include "Core/WRGameInstance.h"
#include "WRNoticeManager.h"
#include "Table/WRCustomTextTable.h"
#include "WRInteriorManager.h"
#include "WRVehicleManager.h"
#include "UI/Panel/WRUIPanel_CrossHair.h"
#include "CustomAsset/WRInventoryAsset.h"
#include "WRAsyncLoadUObjectManager.h"
#include "WRSFXManager.h"
#include "WRUIManager.h"
#include "WRAccountManager.h"

#include "Localization/WRLocalizationProcessor.h"


// For Cpp
DEFINE_LOG_CATEGORY(InventoryManager)

#define INVENTORY_ASSET_PATH TEXT("/Game/Asset/Data/Asset/Widget/Inventory.Inventory")

WRInventoryManager::WRInventoryManager()
{
	
}

void WRInventoryManager::OnInitialize()
{
	if (WRMapManager::IsValid())
	{
		WRMapManager::Get()->MapMovementCompleteDelegate.AddRaw(this, &WRInventoryManager::OnMapMovementComplete);
	}
}

void WRInventoryManager::OnShutdown()
{
	RegistrationItemMap.Empty();
	StorageItemList.Empty();
	EquippedLeftWeaponRegistrationSlotType = EWRRegistrationSlotType::None;
	EquippedRightWeaponRegistrationSlotType = EWRRegistrationSlotType::None;

	if (strInventoryAssetAsyncKey.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(strInventoryAssetAsyncKey);
	}
	if (InventoryAsset->IsValidLowLevel() == true)
	{
		InventoryAsset->RemoveFromRoot();
	}
}

void WRInventoryManager::OnLoadLevelStart()
{
	auto AsyncLoadComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		strInventoryAssetAsyncKey = "";

		InventoryAsset = Cast<UWRInventoryAsset>(InObject);
		checkf(InventoryAsset != nullptr, TEXT("Null InventoryAsset : %s"), INVENTORY_ASSET_PATH);
		InventoryAsset->AddToRoot();
	});

	strInventoryAssetAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(INVENTORY_ASSET_PATH, AsyncLoadComplete);

	bInputLock = true;
}

void WRInventoryManager::OnMapMovementComplete()
{
	bInputLock = false;
}

bool WRInventoryManager::IsRegisteredItem(const ISN_t IN InISN)
{
	if (GetRegisteredSlotType(InISN) == EWRRegistrationSlotType::None)
		return false;

	return true;
}

bool WRInventoryManager::IsLeftWeaponRegistarationType(const EWRRegistrationSlotType IN InRegistrationSlotType)
{
	if ((EWRRegistrationSlotType)REGISTRATION_SLOT_LEFT_WEAPON_BEGIN <= InRegistrationSlotType && InRegistrationSlotType < EWRRegistrationSlotType::REGISTRATION_SLOT_LEFT_WEAPON_END)
		return true;
	
	return false;
}

bool WRInventoryManager::IsRightWeaponRegistarationType(const EWRRegistrationSlotType IN InRegistrationSlotType)
{
	if ((EWRRegistrationSlotType)REGISTRATION_SLOT_RIGHT_WEAPON_BEGIN <= InRegistrationSlotType && InRegistrationSlotType < EWRRegistrationSlotType::REGISTRATION_SLOT_RIGHT_WEAPON_END)
		return true;

	return false;
}

bool WRInventoryManager::IsSlotRegistableItemType(const EWRRegistrationSlotType IN InTargetRegistrationSlotType, const EWRItemMainType IN InItemMainType, const EWRItemSubType IN InItemSubType)
{
	if (IsLeftWeaponRegistarationType(InTargetRegistrationSlotType))
	{
		if (InItemMainType == EWRItemMainType::Weapon && InItemSubType != EWRItemSubType::Weapon)
		{
			return true;
		}
	}
	else if (IsRightWeaponRegistarationType(InTargetRegistrationSlotType))
	{
		if (InItemMainType == EWRItemMainType::Weapon && InItemSubType == EWRItemSubType::Weapon)
			return true;
	}
	else
	{
		switch (InTargetRegistrationSlotType)
		{
		case EWRRegistrationSlotType::Armor:
		{
			if (InItemMainType == EWRItemMainType::Armor)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Top:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Top)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Bottom:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Bottom)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Shoes:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Shoes)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Hat:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Hat)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Gloves:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Gloves)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Glasses:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Glasses)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Neck:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Neck)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Mask:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Mask)
				return true;
		}
		break;
		case EWRRegistrationSlotType::Back:
		{
			if (InItemMainType == EWRItemMainType::Costume && InItemSubType == EWRItemSubType::Back)
				return true;
		}
		break;
		}
	}	

	return false;
}

//====================================================================================
//
//====================================================================================

void WRInventoryManager::EquipNextMainWeapon()
{
	WREquipItemBase* NextItem = GetNextRegisteredMainWeapon();
	if (NextItem)
	{
		// [ 2020-7-8 : kyu ] ��ü�� ���� �������� �������� ������ UnEquip�� ������ �ʴ´�.
		IMMessageSender::RequestEquipItem(NextItem->GetISN());
	}
}

void WRInventoryManager::EquipNextSubWeapon()
{
	WREquipItemBase* NextItem = GetNextRegisteredSubWeapon();
	if (NextItem)
	{
		// [ 2020-7-8 : kyu ] ��ü�� ���� �������� �������� ������ UnEquip�� ������ �ʴ´�.
		IMMessageSender::RequestEquipItem(NextItem->GetISN());
	}
}

WREquipItemBase* WRInventoryManager::GetNextRegisteredMainWeapon(bool IN bInReturnNullIfReturnedToHeadInList)
{
	return GetRegistrationSlotItemForWeapon(REGISTRATION_SLOT_RIGHT_WEAPON_BEGIN, (int32)EWRRegistrationSlotType::REGISTRATION_SLOT_RIGHT_WEAPON_END, (int32)EquippedRightWeaponRegistrationSlotType);
}

WREquipItemBase* WRInventoryManager::GetNextRegisteredSubWeapon(bool IN bInReturnNullIfReturnedToHeadInList /*= false*/)
{
	return GetRegistrationSlotItemForWeapon(REGISTRATION_SLOT_LEFT_WEAPON_BEGIN, (int32)EWRRegistrationSlotType::REGISTRATION_SLOT_LEFT_WEAPON_END, (int32)EquippedLeftWeaponRegistrationSlotType);
}

EWRItemEquippableSlot WRInventoryManager::ConvertRegistrationTypeToEquippableType(const EWRRegistrationSlotType IN InRegistrationSlotType)
{
	switch (InRegistrationSlotType)
	{
	case EWRRegistrationSlotType::Armor:
	{
		return EWRItemEquippableSlot::Armor;
	}
	break;
		
	case EWRRegistrationSlotType::Hat:
	{
		return EWRItemEquippableSlot::Hat;
	}
	break;
	case EWRRegistrationSlotType::Glasses:
	{
		return EWRItemEquippableSlot::Glasses;
	}
	break;
	case EWRRegistrationSlotType::Mask:
	{
		return EWRItemEquippableSlot::Mask;
	}
	break;
	case EWRRegistrationSlotType::Neck:
	{
		return EWRItemEquippableSlot::Neck;
	}
	break;
	case EWRRegistrationSlotType::Top:
	{
		return EWRItemEquippableSlot::Top;
	}
	break;
	case EWRRegistrationSlotType::Bottom:
	{
		return EWRItemEquippableSlot::Bottom;
	}
	break;
	case EWRRegistrationSlotType::Gloves:
	{
		return EWRItemEquippableSlot::Gloves;
	}
	break;
	case EWRRegistrationSlotType::Shoes:
	{
		return EWRItemEquippableSlot::Shoes;
	}
	break;
	case EWRRegistrationSlotType::Back:
	{
		return EWRItemEquippableSlot::Back;
	}
	break;

	default:
	{
		if (IsLeftWeaponRegistarationType(InRegistrationSlotType))
		{
			return EWRItemEquippableSlot::LeftWeapon;
		}
		else if (IsRightWeaponRegistarationType(InRegistrationSlotType))
		{
			return EWRItemEquippableSlot::RightWeapon;
		}
	}
	break;
	}

	return EWRItemEquippableSlot::None;
}

EWRRegistrationSlotType WRInventoryManager::ConvertEquippableTypeToRegistrationType(const EWRItemEquippableSlot IN InEquippableSlotType)
{
	switch (InEquippableSlotType)
	{
	case EWRItemEquippableSlot::Armor:
	{
		return EWRRegistrationSlotType::Armor;
	}
	break;

	case EWRItemEquippableSlot::Top:
	{
		return EWRRegistrationSlotType::Top;
	}
	break;
	case EWRItemEquippableSlot::Bottom:
	{
		return EWRRegistrationSlotType::Bottom;
	}
	break;
	case EWRItemEquippableSlot::Shoes:
	{
		return EWRRegistrationSlotType::Shoes;
	}
	break;
	case EWRItemEquippableSlot::Hat:
	{
		return EWRRegistrationSlotType::Hat;
	}
	break;
	case EWRItemEquippableSlot::Gloves:
	{
		return EWRRegistrationSlotType::Gloves;
	}
	break;
	case EWRItemEquippableSlot::Glasses:
	{
		return EWRRegistrationSlotType::Glasses;
	}
	break;
	case EWRItemEquippableSlot::Neck:
	{
		return EWRRegistrationSlotType::Neck;
	}
	break;
	case EWRItemEquippableSlot::Mask:
	{
		return EWRRegistrationSlotType::Mask;
	}
	break;
	case EWRItemEquippableSlot::Back:
	{
		return EWRRegistrationSlotType::Back;
	}
	break;
	}
	return EWRRegistrationSlotType::None;
}

void WRInventoryManager::AddNewItemISN(const ISN_t IN InISN)
{
	NewItemList.AddTail(InISN);
}

void WRInventoryManager::ClearNewItemList()
{
	NewItemList.Empty();
}

bool WRInventoryManager::IsNewItem(const ISN_t IN InISN)
{
	for (auto Node = NewItemList.GetHead(); Node != nullptr; Node = Node->GetNextNode())
	{
		ISN_t ItemSN = Node->GetValue();
		if (InISN == ItemSN)
		{
			return true;
		}
	}
	return false;
}

EWRRegistrationSlotType WRInventoryManager::GetRegisteredSlotType(const ISN_t IN InISN)
{
	for (auto& EachSlot : RegistrationItemMap)
	{
		WREquipItemBase* RefEquipItemBase = EachSlot.Value;
		if(RefEquipItemBase == nullptr)
			continue;

		if (RefEquipItemBase->GetISN() == InISN)
			return EachSlot.Key;
	}
	return EWRRegistrationSlotType::None;
}

EWRItemEquippableSlot WRInventoryManager::GetEquippableSlotType(const ISN_t IN InISN)
{
	WREquipItemBase* FoundEquipItemBase = WRItemManager::Get()->FindItem<WREquipItemBase>(InISN);
	if (FoundEquipItemBase == nullptr)
		return EWRItemEquippableSlot::None;

	return FoundEquipItemBase->GetEquippableSlot();
}

WREquipItemBase* WRInventoryManager::GetRegistrationSlotItemForWeapon(const int32 IN InStartIndex, const int32 IN InEndIndex, const int32 IN InStandardIndex)
{
	int32 nSearchSlotIndex = InStartIndex;
	int32 nStandardSlotIndex = InStandardIndex;

	if (nStandardSlotIndex == (int32)EWRRegistrationSlotType::None)
	{
		nStandardSlotIndex = InStartIndex;
	}
	else
	{
		nSearchSlotIndex = nStandardSlotIndex;

		++nSearchSlotIndex;

		if (nSearchSlotIndex == InEndIndex)
		{
			nSearchSlotIndex = InStartIndex;
		}
	}

	bool bExit = true;
	while (bExit)
	{
		WREquipItemBase* RegisteredItem = RegistrationItemMap.FindRef((EWRRegistrationSlotType)nSearchSlotIndex);
		if (RegisteredItem)
		{
			return RegisteredItem;
		}
		else
		{
			// ���� �ε���
			++nSearchSlotIndex;

			// �ִ�� ó������
			if (nSearchSlotIndex == InEndIndex)
			{
				nSearchSlotIndex = InStartIndex;
			}
		}

		// �����̸� ����
		if (nSearchSlotIndex == nStandardSlotIndex)
		{
			bExit = false;
		}
	}
	return nullptr;
}

//====================================================================================
// [ 2020-6-12 : kyu ] Inventory
//====================================================================================

void WRInventoryManager::OnClickedSet(const int32 IN InSetIndex)
{
	// EWRInventoryCategoryType�� SetIndex�� �ش��ϴ� EquipListUI�� ����
	// EquipListUI ������ �ٸ� Slot���� �̵��� �Ұ�. 
	// Ŭ���ϸ� �ش� EquipType�� ���������� ����Ʈ�� ������ Storage�� �����ǰ� ��Ŀ�� �̵�
	// ����ϸ� EquipListUI�� ������, �ٽ� SetList�� ��Ŀ�� �̵�

	CurrentInventorySetIndex = InSetIndex;
}

bool WRInventoryManager::OnInputEvent(const EWRInput IN InInput, EInputEvent InEventType)
{
	if (InEventType != IE_Pressed)
	{
		if (bInputEventActivate)
		{
			bInputEventActivate = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	bInputEventActivate = OnInputEvent(InInput);
	return bInputEventActivate;
}

bool WRInventoryManager::OnInputEvent(const EWRInput IN InInput)
{
	switch (InInput)
	{
	case EWRInput::TouchPadDragLeftX:
	{
		UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
		if (InventoryPanel == nullptr)
			return false;

		// �������� ���� ���� ������ �н�
		if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
			return false;

		// ����������â�� �������� �н�
		if (InventoryPanel->GetInventoryItemInfo() != nullptr)
			return false;

		OnNextStoragePage();
		return true;
	}
	break;
	case EWRInput::TouchPadDragRightX:
	{
		UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
		if (InventoryPanel == nullptr)
			return false;

		// �������� ���� ���� ������ �н�
		if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
			return false;

		// ����������â�� �������� �н�
		if (InventoryPanel->GetInventoryItemInfo() != nullptr)
			return false;

		OnPrevStoragePage();
		return true;
	}
	break;
	case EWRInput::Triangle:
	{
		// [ 2020-9-3 : kyu ] �κ� ���� ��ư Triangle -> Option ����
		return false;
	}
	break;
	case EWRInput::Option:
	{
		AWRGameModeBase* GameModeBase = Cast<AWRGameModeBase>(UGameplayStatics::GetGameMode(UWRGameInstance::GetGameInstance()));
		if (GameModeBase == nullptr || GameModeBase->GetGameModeType() == EWRGameMode::Lab)
			return false;

		if (bInputLock)
			return false;

		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
			return false;

		OpenInventory();
		return true;
	}
	break;
	case EWRInput::R3:
	{
		UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
		if (InventoryPanel == nullptr)
			return false;

		CloseInventory();
		return true;
	}
	break;
	}

	return false;
}

void WRInventoryManager::ReleaseData()
{
	CurrentInventoryMainSwitcher = EWRInventoryMainSwitcher::Category;
	CurrentInventoryMainCategory = EWRInventoryCategoryType::MAX;
	CurrentInventorySelectedRegistrationSlot = EWRRegistrationSlotType::None;
	LastFocusedSlotGroup = EWRInventorySlotGroup::None;
	CurrentInventoryStoragePage = 1;
	CurrentInventorySetIndex = 0;
	CurrentInventoryInnerCategoryIndex = -1;
	CurrentFocusedISN = INVALID_ISN;
	SelectedStorageISN = INVALID_ISN;
}

void WRInventoryManager::UpdateTitle()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	FString strTitle = "";

	switch (CurrentInventoryMainSwitcher)
	{
	case EWRInventoryMainSwitcher::Category:
	{
		strTitle = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_InventoryKey").ToString();
	}
	break;
	case EWRInventoryMainSwitcher::Contents:
	{
		switch (CurrentInventoryMainCategory)
		{
		case EWRInventoryCategoryType::Equipment:
		{
			strTitle = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_EquipmentKey").ToString();
		}
		break;
		case EWRInventoryCategoryType::Costume:
		{
			strTitle = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_CostumeKey").ToString();
		}
		break;
		case EWRInventoryCategoryType::Vehicle:
		{
			strTitle = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_VehicleKey").ToString();
		}
		break;
		case EWRInventoryCategoryType::Consume:
		{
			strTitle = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_ConsumeKey").ToString();
		}
		break;
		}
	}
	break;
	}

	InventoryPanel->SetTitle(strTitle);
}

void WRInventoryManager::UpdateEquipList()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
		return;

	UWRUISlot_InventoryEquipList* InventoryEquipListUI = InventoryPanel->GetInventoryEquipListUI();
	if (InventoryEquipListUI == nullptr)
		return;

	TDoubleLinkedList<WREquipItemBase*> EquippedItemList;

	auto AddEquippedItemListByRegistrationType = [&](EWRRegistrationSlotType _Type)
	{
		WREquipItemBase* FoundItem = RegistrationItemMap.FindRef(_Type);
		if (FoundItem)
		{
			EquippedItemList.AddTail(FoundItem);
		}
	};
	auto AddEquippedItemListByEquippableType = [&](EWRItemEquippableSlot _Type)
	{
		WREquipItemBase* FoundItem = WRItemManager::Get()->FindItemInEquippedList(_Type);
		if (FoundItem)
		{
			EquippedItemList.AddTail(FoundItem);
		}
	};

	// Update By 
	// + CurCategory
	switch (CurrentInventoryMainCategory)
	{
	case EWRInventoryCategoryType::Equipment:
	{
		AddEquippedItemListByRegistrationType(EWRRegistrationSlotType::LeftWeapon_1);
		AddEquippedItemListByRegistrationType(EWRRegistrationSlotType::LeftWeapon_2);
		AddEquippedItemListByRegistrationType(EWRRegistrationSlotType::LeftWeapon_3);

		AddEquippedItemListByRegistrationType(EWRRegistrationSlotType::RightWeapon_1);
		AddEquippedItemListByRegistrationType(EWRRegistrationSlotType::RightWeapon_2);
		AddEquippedItemListByRegistrationType(EWRRegistrationSlotType::RightWeapon_3);

		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Armor);
	}
	break;
	case EWRInventoryCategoryType::Costume:
	{
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Hat);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Glasses);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Mask);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Neck);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Top);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Bottom);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Gloves);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Shoes);
		AddEquippedItemListByEquippableType(EWRItemEquippableSlot::Back);
	}
	break;
	}

	InventoryEquipListUI->UpdateEquipList(EquippedItemList);
}

void WRInventoryManager::UpdateStorage()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
		return;

	UWRUISlot_InventoryStorage* InventoryStorageUI = InventoryPanel->GetInventoryStorageUI();
	if (InventoryStorageUI == nullptr)
		return;
			
	TDoubleLinkedList<WRItemBase*> CurrentPageItemList;
	GetStorageItemListByCurPage(CurrentPageItemList);

	InventoryStorageUI->UpdateStorage(CurrentPageItemList);
}

void WRInventoryManager::UpdateStorageSlot(const ISN_t IN InISN)
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
		return;

	UWRUISlot_InventoryStorage* InventoryStorageUI = InventoryPanel->GetInventoryStorageUI();
	if (InventoryStorageUI == nullptr)
		return;

	InventoryStorageUI->UpdateStorageSlot(InISN);
}

void WRInventoryManager::UpdateSimpleInformation()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
		return;

	UWRUISlot_InventoryItemSimpleInfo* InventorySimpleInfoUI = InventoryPanel->GetInventorySimpleInfoUI();
	if (InventorySimpleInfoUI == nullptr)
		return;

	if (CurrentFocusedISN != INVALID_TABLE_TID)
	{
		if (InventorySimpleInfoUI->GetVisibility() == ESlateVisibility::Collapsed 
			|| InventorySimpleInfoUI->IsAnimationPlayingByName("CloseAnimation")
			|| InventorySimpleInfoUI->GetCurrentSlotGroup() != LastFocusedSlotGroup)
		{
			InventorySimpleInfoUI->StopAllAnimations();
			InventorySimpleInfoUI->PlayAnimationByName("OpenAnimation");
			
			switch (LastFocusedSlotGroup)
			{
			case EWRInventorySlotGroup::EquipList:
			{
				FWidgetTransform TF;
				TF.Translation = FVector2D(0, 0);
				InventorySimpleInfoUI->SetRenderTransform(TF);
			}
			break;
			case EWRInventorySlotGroup::Storage:
			{
				FWidgetTransform TF;
				TF.Translation = InventoryPanel->GetSimpleInfoOffset();
				InventorySimpleInfoUI->SetRenderTransform(TF);
			}
			break;
			}
		}		

		InventorySimpleInfoUI->SetSimpleInfo(CurrentFocusedISN, LastFocusedSlotGroup);
	}	
	else
	{
		if (InventorySimpleInfoUI->GetVisibility() != ESlateVisibility::Collapsed)
		{			
			InventorySimpleInfoUI->StopAllAnimations();
			InventorySimpleInfoUI->PlayAnimationByName("CloseAnimation");

			switch (InventorySimpleInfoUI->GetCurrentSlotGroup())
			{
			case EWRInventorySlotGroup::EquipList:
			{
				FWidgetTransform TF;
				TF.Translation = FVector2D(0, 0);
				InventorySimpleInfoUI->SetRenderTransform(TF);
			}
			break;
			case EWRInventorySlotGroup::Storage:
			{
				FWidgetTransform TF;
				TF.Translation = InventoryPanel->GetSimpleInfoOffset();
				InventorySimpleInfoUI->SetRenderTransform(TF);
			}
			break;
			}
		}	

		InventorySimpleInfoUI->SetCloseInfo();
	}
}

void WRInventoryManager::UpdatePriceInfo()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
		return;

	UWRUISlot_InventoryPriceInfo* InventoryPriceInfoUI = InventoryPanel->GetInventoryPriceInfoUI();
	if (InventoryPriceInfoUI == nullptr)
		return;

	InventoryPriceInfoUI->SetPriceInfo(CurrentFocusedISN);
}

void WRInventoryManager::UpdateControlGuide()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	InventoryPanel->UpdateControlGuide();
}

void WRInventoryManager::UpdateStorageToCustomPage(const int32 IN InCustomPage)
{
	if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
		return;

	if (InCustomPage == CurrentInventoryStoragePage)
		return;

	if (InCustomPage < 1)
		return;

	if (InCustomPage > GetInventoryStorageMaxPage())
		return;

	CurrentInventoryStoragePage = InCustomPage;

	UpdateStorage();
}

void WRInventoryManager::OnPrevStoragePage()
{
	UpdateStorageToCustomPage(CurrentInventoryStoragePage - 1);
}

void WRInventoryManager::OnNextStoragePage()
{
	UpdateStorageToCustomPage(CurrentInventoryStoragePage + 1);
}

void WRInventoryManager::CollectStorageItemList()
{
	StorageItemList.Empty();

	// Update By 
	// + CurCategory 
	switch (CurrentInventoryMainCategory)
	{
	case EWRInventoryCategoryType::Equipment:
	{
		auto AddMainTypeLambda = [&](EWRItemMainType MainType, EWRItemEquippableSlot EquippableSlotType)
		{
			TDoubleLinkedList<WRItemBase*>* GettedList = WRItemManager::Get()->GetItemList(MainType);
			if (GettedList != nullptr)
			{
				for (auto Node = GettedList->GetHead(); Node != nullptr; Node = Node->GetNextNode())
				{
					WRItemBase* TempItem = Node->GetValue();
					if (TempItem)
					{
						WREquipItemBase* CastedTempItem = static_cast<WREquipItemBase*>(TempItem);
						if (CastedTempItem && CastedTempItem->GetEquippableSlot() == EquippableSlotType)
						{
							StorageItemList.AddTail(TempItem);
						}
					}
				}
			}
		};

		// + CurrentInventorySelectedRegistrationSlot
		if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
			|| IsLeftWeaponRegistarationType(CurrentInventorySelectedRegistrationSlot))
		{
			AddMainTypeLambda(EWRItemMainType::Weapon, EWRItemEquippableSlot::LeftWeapon);
		}

		// + CurrentInventorySelectedRegistrationSlot
		if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
			|| IsRightWeaponRegistarationType(CurrentInventorySelectedRegistrationSlot))
		{
			AddMainTypeLambda(EWRItemMainType::Weapon, EWRItemEquippableSlot::RightWeapon);
		}

		// + CurrentInventorySelectedRegistrationSlot
		if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
			|| CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::Armor)
		{
			AddMainTypeLambda(EWRItemMainType::Armor, EWRItemEquippableSlot::Armor);
		}

	}
	break;
	case EWRInventoryCategoryType::Costume:
	{
		// [ 2020-9-15 : kyu ] �ӽ�. Ŀ������ ���� �������� ����
		TArray<WRTableID>& CustomizingSelectedCostumeItemArray = WRAccountManager::Get()->GetCustomizingSelectedCostumeItemArray();
		auto IsCustomizingItemLambda = [CustomizingSelectedCostumeItemArray](WRTableID InTID)
		{
			for (auto CustomizingItemTID : CustomizingSelectedCostumeItemArray)
			{
				if (CustomizingItemTID == InTID)
					return true;
			}
			return false;
		};


		TDoubleLinkedList<WRItemBase*>* GettedList = WRItemManager::Get()->GetItemList(EWRItemMainType::Costume);
		if (GettedList != nullptr)
		{
			for (auto Node = GettedList->GetHead(); Node != nullptr; Node = Node->GetNextNode())
			{
				WRItemBase* TempItem = Node->GetValue();
				if (TempItem)
				{
					// [ 2020-9-15 : kyu ] �ӽ�. Ŀ������ ���� �������� ����
					if (IsCustomizingItemLambda(TempItem->GetTID()))
						continue;

					// + CurrentInventorySelectedRegistrationSlot
					if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None 
						|| IsSlotRegistableItemType(CurrentInventorySelectedRegistrationSlot, TempItem->GetMainType(), TempItem->GetSubType()))
					{
						StorageItemList.AddTail(TempItem);
					}
				}
			}
		}
	}
	break;
	case EWRInventoryCategoryType::Vehicle:
	{
		TDoubleLinkedList<WRItemBase*>* GettedList = WRItemManager::Get()->GetItemList(EWRItemMainType::Vehicle);
		if (GettedList != nullptr)
		{
			for (auto Node = GettedList->GetHead(); Node != nullptr; Node = Node->GetNextNode())
			{
				WRItemBase* TempItem = Node->GetValue();
				if (TempItem)
				{
					// + CurrentInventoryInnerCategoryIndex
					if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
						|| (TempItem->IsCar() && CurrentInventoryInnerCategoryIndex == (int32)EWRInventoryVehicleCategory::Car))
					{
						StorageItemList.AddTail(TempItem);
					}
				}
			}
		}
	}
	break;
	case EWRInventoryCategoryType::Consume:
	{
		if (CurrentInventoryInnerCategoryIndex == -1 || CurrentInventoryInnerCategoryIndex == (int32)EWRInventoryConsumeCategory::Material)
		{
			TDoubleLinkedList<WRItemBase*>* GettedList = WRItemManager::Get()->GetItemList(EWRItemMainType::Material);
			if (GettedList != nullptr)
			{
				for (auto Node = GettedList->GetHead(); Node != nullptr; Node = Node->GetNextNode())
				{
					WRItemBase* TempItem = Node->GetValue();
					if (TempItem)
					{
						// + CurrentInventoryInnerCategoryIndex
						if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
							|| TempItem->IsMaterial())
						{
							StorageItemList.AddTail(TempItem);
						}
					}
				}
			}
		}
		
		if (CurrentInventoryInnerCategoryIndex == -1 || CurrentInventoryInnerCategoryIndex == (int32)EWRInventoryConsumeCategory::Interior)
		{
			TDoubleLinkedList<WRItemBase*>* GettedList = WRItemManager::Get()->GetItemList(EWRItemMainType::Interior);
			if (GettedList != nullptr)
			{
				for (auto Node = GettedList->GetHead(); Node != nullptr; Node = Node->GetNextNode())
				{
					WRItemBase* TempItem = Node->GetValue();
					if (TempItem)
					{
						// [ 2020-7-24 : kyu ] �������� ���׸���� �κ���Ͽ� �������� ����
						if(WRInteriorManager::Get()->IsSpawnedByMe(TempItem->GetISN()))
							continue;

						// + CurrentInventoryInnerCategoryIndex
						if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
							|| TempItem->IsInterior())
						{
							StorageItemList.AddTail(TempItem);
						}
					}
				}
			}
		}		

		if (CurrentInventoryInnerCategoryIndex == -1 || CurrentInventoryInnerCategoryIndex == (int32)EWRInventoryConsumeCategory::Emote)
		{
			TDoubleLinkedList<WRItemBase*>* GettedList = WRItemManager::Get()->GetItemList(EWRItemMainType::Emote);
			if (GettedList != nullptr)
			{
				for (auto Node = GettedList->GetHead(); Node != nullptr; Node = Node->GetNextNode())
				{
					WRItemBase* TempItem = Node->GetValue();
					if (TempItem)
					{
						// + CurrentInventoryInnerCategoryIndex
						if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
							|| TempItem->IsEmote())
						{
							StorageItemList.AddTail(TempItem);
						}
					}
				}
			}
		}

		if (CurrentInventoryInnerCategoryIndex == -1 || CurrentInventoryInnerCategoryIndex == (int32)EWRInventoryConsumeCategory::Placement)
		{
			TDoubleLinkedList<WRItemBase*>* GettedList = WRItemManager::Get()->GetItemList(EWRItemMainType::Placement);
			if (GettedList != nullptr)
			{
				for (auto Node = GettedList->GetHead(); Node != nullptr; Node = Node->GetNextNode())
				{
					WRItemBase* TempItem = Node->GetValue();
					if (TempItem)
					{
						// + CurrentInventoryInnerCategoryIndex
						if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None
							|| TempItem->IsPlacement())
						{
							StorageItemList.AddTail(TempItem);
						}
					}
				}
			}
		}
	}
	break;
	}
}

void WRInventoryManager::GetStorageItemListByCurPage(TDoubleLinkedList<WRItemBase*>& OUT OutList)
{
	int32 nIndex = 0;
	int32 nStartIndex = (CurrentInventoryStoragePage - 1) * DEF_INVENTORY_STORAGE_BASE_EXTEND;
	int32 nEndIndex = nStartIndex + DEF_INVENTORY_STORAGE_BASE_EXTEND;

	for (auto Node = StorageItemList.GetHead(); Node != nullptr; Node = Node->GetNextNode(), ++nIndex)
	{
		WRItemBase* TempItem = Node->GetValue();
		if (TempItem == nullptr)
			continue;

		if (nIndex < nStartIndex)
			continue;

		if (nIndex > nEndIndex)
			continue;

		OutList.AddTail(TempItem);
	}
}

void WRInventoryManager::RefocusToInventoryContents()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	switch (LastFocusedSlotGroup)
	{
	case EWRInventorySlotGroup::EquipList:
	{
		UWRUISlot_InventoryEquipList* InventoryEquipListUI = InventoryPanel->GetInventoryEquipListUI();
		if (InventoryEquipListUI == nullptr)
			return;

		InventoryEquipListUI->RefocusOnLastFocusedButton();	// ������ ��Ŀ���� �������� �ٽ� ��Ŀ��
	}
	break;

	case EWRInventorySlotGroup::Storage:
	{
		UWRUISlot_InventoryStorage* InventoryStorageUI = InventoryPanel->GetInventoryStorageUI();
		if (InventoryStorageUI == nullptr)
			return;

		InventoryStorageUI->RefocusOnLastFocusedButton();	// ������ ��Ŀ���� �������� �ٽ� ��Ŀ��
	}
	break;
	}
}

void WRInventoryManager::OnDiscardItem()
{
	// + ������ ��û
	IMMessageSender::RequestDiscardItem(CurrentFocusedISN, DiscardData.DiscardQuantity);
	
	DiscardData.ClearData();

	RefocusToInventoryContents();
}

void WRInventoryManager::OnUnspawnVehicleReceived(pt_Game_Vehicle::Ack_UnspawnVehicle_G2U* IN InPacket)
{
	UpdateStorageSlot(InPacket->itemSN);
}

void WRInventoryManager::OnSekaiItemInfoReceived(pt_Game_User::Inf_SekaiItemInfo_G2U* IN Packet)
{
	if (Packet->bFinish == false)
		return;
	
	SekaiItemInfoMap.Add(Packet->listSekaiItemInfo.begin()->itemTID, Packet->listSekaiItemInfo);
}

void WRInventoryManager::OnSekaiCurrencyInfoReceived(pt_Game_User::Inf_SekaiCurrencyInfo_G2U* IN Packet)
{

}

void WRInventoryManager::OnUpdateCurrency(const EWRCurrencyType IN InCurrencyType)
{
	if (InCurrencyType != EWRCurrencyType::Gold)
		return;

	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	InventoryPanel->UpdateGoldInfo();
}

void WRInventoryManager::OnDiscardItemCancel()
{
	// + ������ ���
	DiscardData.ClearData();

	RefocusToInventoryContents();
}

void WRInventoryManager::OnDiscardItemQuantityChanged(const int32 IN InQuantity)
{
	DiscardData.DiscardQuantity = InQuantity;
}

int32 WRInventoryManager::GetInventoryStorageMaxPage()
{
	int32 StorageItemCount = StorageItemList.Num();

	if (StorageItemCount == 0 || StorageItemCount <= DEF_INVENTORY_STORAGE_BASE_EXTEND)
	{
		return 1;	// 1 �������� ����
	}

	int32 MaxPage = (StorageItemCount / DEF_INVENTORY_STORAGE_BASE_EXTEND) + 1;
	return MaxPage;
}

EWRInventoryMiddleType WRInventoryManager::GetInventoryMiddleType()
{
	switch (CurrentInventoryMainCategory)
	{
	case EWRInventoryCategoryType::Equipment:
	case EWRInventoryCategoryType::Costume:
	{
		return EWRInventoryMiddleType::EquipList;
	}
	break;

	case EWRInventoryCategoryType::Vehicle:
	case EWRInventoryCategoryType::Consume:
	{
		return EWRInventoryMiddleType::InnerCategory;
	}
	break;
	}
	return EWRInventoryMiddleType::EquipList;
}

void WRInventoryManager::OpenInventory(const EWRInventoryCategoryType IN InManualCategory /* = EWRInventoryCategoryType::MAX */)
{
	// �⺻������ ������ ī�װ��� ���µ�
	
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);

			if (InManualCategory == EWRInventoryCategoryType::MAX)
			{
				SwitchInventory(EWRInventoryMainSwitcher::Category, InManualCategory, true);				
			}
			else
			{
				SwitchInventory(EWRInventoryMainSwitcher::Contents, InManualCategory, true);				
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Inventory, EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::CreateWidgetComponent, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	
	// Hide Cross Hair
	UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
	if (Panel_CrossHair)
		Panel_CrossHair->HideCrossHairPanel();
}

void WRInventoryManager::SwitchInventory(const EWRInventoryMainSwitcher IN InSwitcher, const EWRInventoryCategoryType IN InCategory /* = EWRInventoryCategoryType::MAX */, const bool IN InNewOpen /* = false */)
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	ReleaseData();

	CurrentInventoryMainSwitcher = InSwitcher;
	CurrentInventoryMainCategory = InCategory;

	InventoryPanel->SetInventorySwitcher(InSwitcher);

	UpdateTitle();

	if (InNewOpen == false)
	{
		ClearNewItemList();
	}

	switch (InSwitcher)
	{
	case EWRInventoryMainSwitcher::Category:
	{
		UWRUISlot_InventoryCategory* InventoryCategoryUI = InventoryPanel->GetInventoryCategoryUI();
		if (InventoryCategoryUI == nullptr)
			return;

		PlaySound(EWRInventorySoundType::OpenInventory);

		InventoryCategoryUI->OnOpening();
		InventoryCategoryUI->RefocusOnLastFocusedButton();	// ������ ��Ŀ���� �������� �ٽ� ��Ŀ��
	}
	break;

	case EWRInventoryMainSwitcher::Contents:
	{
		PlaySound(EWRInventorySoundType::OpenContents);

		// ** �������� ī�װ��� ���� ���ݾ� �ٸ���.
				
		switch (CurrentInventoryMainCategory)
		{
		case EWRInventoryCategoryType::Equipment:
		case EWRInventoryCategoryType::Costume:
		{
			InventoryPanel->SetInventoryMiddleType(EWRInventoryMiddleType::EquipList);
			LastFocusedSlotGroup = EWRInventorySlotGroup::EquipList;

			UWRUISlot_InventoryEquipList* InventoryEquipListUI = InventoryPanel->GetInventoryEquipListUI();
			if (InventoryEquipListUI == nullptr)
				return;

			// ī�װ� ���� �� ù ��Ŀ���� EquipList�� ù ����
			InventoryEquipListUI->FocusToDefaultFocusButton();

			// Storage ������Ʈ
			// Update By 
			// + CurCategory 
			// + CurrentInventorySelectedRegistrationSlot
			// + CurrentInventoryInnerCategoryIndex
			// + CurrentPage
			CollectStorageItemList();

			// + �߾� EquipListUI ����
			UpdateEquipList();
			// + ���� StorageUI ����
			UpdateStorage();
			// + ���� Simple Information ����
			UpdateSimpleInformation();
			// + ���� �ü����� ����
			UpdatePriceInfo();
			// + �ϴ� ���۹� ����
			UpdateControlGuide();
		}
		break;

		case EWRInventoryCategoryType::Vehicle:
		case EWRInventoryCategoryType::Consume:
		{
			InventoryPanel->SetInventoryMiddleType(EWRInventoryMiddleType::InnerCategory);
			LastFocusedSlotGroup = EWRInventorySlotGroup::None;

			//SetSelectInnerCategory(-1, true);
 			UWRUISlot_InventoryInnerCategory* InventoryInnerCategoryUI = InventoryPanel->GetInventoryInnerCategoryUI();
 			if (InventoryInnerCategoryUI == nullptr)
 				return;
 
 			// ī�װ� ���� �� ù ��Ŀ���� Inner Category�� ù ���� + ����
 			InventoryInnerCategoryUI->FocusToDefaultFocusButton();
 			InventoryInnerCategoryUI->UpdateInnerCategory();
 			//InventoryInnerCategoryUI->SetSelectInnerCategory(0, true);
 			
			CurrentInventoryInnerCategoryIndex = -1;

 			// Storage ������Ʈ
 			// Update By 
 			// + CurCategory 
 			// + CurrentInventorySelectedRegistrationSlot
 			// + CurrentInventoryInnerCategoryIndex
 			// + CurrentPage
 			CollectStorageItemList();
 
 			// + ���� StorageUI ����
 			UpdateStorage();
 			// + ���� Simple Information ����
 			UpdateSimpleInformation();		
 			// + ���� �ü����� ����
 			UpdatePriceInfo();
 			// + �ϴ� ���۹� ����
 			UpdateControlGuide();
		}
		break;

		}
	}
	break;
	}
}

void WRInventoryManager::CloseInventory()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel)
	{
		InventoryPanel->CloseInventory();
	}	

	ClearNewItemList();

	// Show Cross Hair
	UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
	if (Panel_CrossHair)
		Panel_CrossHair->ShowCrossHairPanel();

	PlaySound(EWRInventorySoundType::CloseInventory);
}

void WRInventoryManager::OnCompletedInventoryClose()
{
	CompletedInventoryCloseDelegate.ExecuteIfBound();
	CompletedInventoryCloseDelegate.Unbind();
}

void WRInventoryManager::OnCategoryOpenDirectionFinished()
{
	// ī�װ� ������ ���� ����

}

void WRInventoryManager::OpenItemInformation(const ISN_t IN InISN)
{
	if (InISN == INVALID_ISN)
		return;

	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel)
	{
		InventoryPanel->OpenItemInformation(InISN);
	}
}

void WRInventoryManager::CloseItemInformation()
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	// Close
	InventoryPanel->CloseItemInformation();

	// Refocus
	RefocusToInventoryContents();
}

void WRInventoryManager::OnFocusedInventorySlot(const ISN_t IN InISN, const EWRInventorySlotGroup IN InSlotGroup, const EWRRegistrationSlotType IN InRegistrationSlotType, const bool IN InDiff /* = true */)
{
	CurrentFocusedISN = InISN;
	LastFocusedSlotGroup = InSlotGroup;
	LastFocusedRegistrationSlot = InRegistrationSlotType;

	UpdateSimpleInformation();
	UpdateControlGuide();
	UpdatePriceInfo();

	if (InDiff)
	{
		PlaySound(EWRInventorySoundType::FocusMoveSlot);
	}
}

void WRInventoryManager::UpdateFocusedInventorySlot(const ISN_t IN InISN, const EWRInventorySlotGroup IN InSlotGroup, const EWRRegistrationSlotType IN InRegistrationSlotType, const bool IN InDiff /* = true */)
{
	if (LastFocusedSlotGroup != InSlotGroup)
		return;

	OnFocusedInventorySlot(InISN, InSlotGroup, InRegistrationSlotType, InDiff);
}

bool WRInventoryManager::OnInputEventInventorySlot(const FKey IN InKey, const FWRInventorySlotInfo& IN InSlotInfo)
{
	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKey);
	if (ConvertedInputType == EWRInput::None)
		return false;

	switch (ConvertedInputType)
	{
	case EWRInput::Triangle:
	{
		if (CurrentInventoryMainSwitcher == EWRInventoryMainSwitcher::Contents)
		{
			if (InSlotInfo.SlotGroupType == EWRInventorySlotGroup::Storage)
			{
				// [������] - �˾�

				if (CurrentFocusedISN == INVALID_ISN)
					return false;

				WRItemBase* FoundItem = WRItemManager::Get()->FindItem(CurrentFocusedISN);
				if (FoundItem == nullptr)
					return false;

				// ����) �������̸� ������ �Ұ� �޽���
				if (FoundItem->IsEquipped())
				{
					FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(37);
					if (pTextTable)
					{
						FString LocalKey = pTextTable->Localkey.ToString();
						WRNoticeManager::Get()->AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString());
					}
					return true;
				}
				// ����) ������̸� ������ �Ұ� �޽���
				else if (IsRegisteredItem(CurrentFocusedISN))
				{
					FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(38);
					if (pTextTable)
					{
						FString LocalKey = pTextTable->Localkey.ToString();
						WRNoticeManager::Get()->AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString());
					}
					return true;
				}

				if (FoundItem->GetQuantity() > 1)
				{
					// ��ø�Ȱ� : �������� + ������ �˾�

					FWRPopupInfo PopupInfo;
					PopupInfo.eButtonFlag = EPopupButtonFlag::OK_CANCEL;
					PopupInfo.eOptionFlag = (BitFlag8)EPopupOptionFlag::Quantity;
					PopupInfo.Contents = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_QuantitySelectKey").ToString();
					PopupInfo.Delegate_First.BindRaw(this, &WRInventoryManager::OnDiscardItem);
					PopupInfo.Delegate_Second.BindRaw(this, &WRInventoryManager::OnDiscardItemCancel);
					PopupInfo.QuantityInfo.Delegate_QuantityChanged.BindRaw(this, &WRInventoryManager::OnDiscardItemQuantityChanged);
					PopupInfo.QuantityInfo.MaxQuantity = FoundItem->GetQuantity();
					PopupInfo.bFocusReleaseWhenClosed = false;
					WRPopupManager::Get()->OpenPopupScreen(PopupInfo);
				}
				else
				{
					// ��ø���� ������ : ������ �˾�

					FWRPopupInfo PopupInfo;
					PopupInfo.eButtonFlag = EPopupButtonFlag::OK_CANCEL;
					PopupInfo.Contents = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_DiscardAskKey").ToString();
					PopupInfo.Delegate_First.BindRaw(this, &WRInventoryManager::OnDiscardItem);
					PopupInfo.Delegate_Second.BindRaw(this, &WRInventoryManager::OnDiscardItemCancel);
					PopupInfo.bFocusReleaseWhenClosed = false;
					WRPopupManager::Get()->OpenPopupScreen(PopupInfo);
				}				
			}
		}
	}
	break;

	case EWRInput::Square:
	{
		if (CurrentInventoryMainSwitcher == EWRInventoryMainSwitcher::Contents)
		{
			// [��������] : ���� Slot�� �������� ���

			UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
			if (InventoryPanel == nullptr)
				return false;

			InventoryPanel->OpenItemInformation(CurrentFocusedISN);

			return true;
		}
	}
	break;

	case EWRInput::Circle:
	{
		EWRInventorySlotAction SlotAction = GetInventorySlotAction();
		switch (SlotAction)
		{
		case EWRInventorySlotAction::RegisterToSelectedRegistrationSlot:
		{
			IMMessageSender::RequestRegisterEquipmentSlot(CurrentFocusedISN, CurrentInventorySelectedRegistrationSlot);
		}
		break;
		case EWRInventorySlotAction::RegisterWhenStorageSelecting:
		{
			IMMessageSender::RequestRegisterEquipmentSlot(SelectedStorageISN, InSlotInfo.GetRegistrationSlotType());
			SetStorageSlotSelected(INVALID_ISN);
		}
		break;
		case EWRInventorySlotAction::UnRegister:
		{
			IMMessageSender::RequestUnRegisterEquipmentSlot(CurrentFocusedISN);
		}
		break;
		case EWRInventorySlotAction::Equip:
		{
			IMMessageSender::RequestEquipItem(CurrentFocusedISN);
		}
		break;
		case EWRInventorySlotAction::UnEquip:
		{
			IMMessageSender::RequestUnEquipItem(CurrentFocusedISN);
		}
		break;
		case EWRInventorySlotAction::SpawnVehicle:
		{
			WRItemBase* FoundItem = WRItemManager::Get()->FindItem(CurrentFocusedISN);
			if (FoundItem == nullptr)
				return false;

			FWRVehicleItemTable* CastedItemTable = static_cast<FWRVehicleItemTable*>(FoundItem->GetTable());
			if (CastedItemTable)
			{
				WRVehicleManager::Get()->SpawnVehiceToSimulate(CurrentFocusedISN, CastedItemTable->VehicleTID);
				CloseInventory();
			}
		}
		break;
		case EWRInventorySlotAction::UnSpawnVehicle:
		{
			WRVehicleManager::Get()->RequestUnspawnVehicle(CurrentFocusedISN);
		}
		break;
		case EWRInventorySlotAction::SpawnInterior:
		{
			WRItemBase* FoundItem = WRItemManager::Get()->FindItem(CurrentFocusedISN);
			if (FoundItem == nullptr)
				return false;

			WRInteriorManager::Get()->SpawnInteriorToSimulate(CurrentFocusedISN, FoundItem->GetTID());
			CloseInventory();
		}
		break;
		case EWRInventorySlotAction::SpawnPlacement:
		{
			
		}
		break;
		case EWRInventorySlotAction::Use:
		{
			WRItemBase* FoundItem = WRItemManager::Get()->FindItem(CurrentFocusedISN);
			if (FoundItem == nullptr)
				return false;

		}
		break;
		case EWRInventorySlotAction::GotoStorage:
		{
			SetEquipListSlotSelected(InSlotInfo.GetRegistrationSlotType());
			PlaySound(EWRInventorySoundType::Circle);
		}
		break;
		case EWRInventorySlotAction::GotoEquipList:
		{
			SetStorageSlotSelected(CurrentFocusedISN);
			PlaySound(EWRInventorySoundType::Circle);
		}
		break;



		case EWRInventorySlotAction::Exception_AlreadySameSlotRegistered:
		{
			PlaySound(EWRInventorySoundType::Alert);
		}
		break;
		case EWRInventorySlotAction::Exception_AlreadyOtherSlotRegistered:
		{
			FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(36);
			if (pTextTable)
			{
				FString localKey = pTextTable->Localkey.ToString();
				WRNoticeManager::Get()->AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, localKey).ToString());
			}
			PlaySound(EWRInventorySoundType::Alert);
		}
		break;
		case EWRInventorySlotAction::Exception_AlreadySameSlotEquipped:
		{
			PlaySound(EWRInventorySoundType::Alert);
		}
		break;
		case EWRInventorySlotAction::Exception_AlreadyOtherSlotEquipped:
		{
			FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(35);
			if (pTextTable)
			{
				FString localKey = pTextTable->Localkey.ToString();
				WRNoticeManager::Get()->AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, localKey).ToString());
			}
			PlaySound(EWRInventorySoundType::Alert);
		}
		break;
		}
	}
	break;

	case EWRInput::Cross:
	{
		// EquipList �����߿� Storage�� Slot�̸� EquipList�� [���ư���]
		//		+ SelectedRegistrationSlot ���� 
		//		+ Storage All�� ����
		// �� �ܴ� Panel_Inventory�� �Է� �ѱ� [â ���ư���]

		if (CurrentInventorySelectedRegistrationSlot != EWRRegistrationSlotType::None)
		{
			if (InSlotInfo.SlotGroupType == EWRInventorySlotGroup::Storage)
			{
				// [���ư���] : EquipList�� �̵� �� Storage �ʱ�ȭ
				SetEquipListSlotSelected(EWRRegistrationSlotType::None);
				PlaySound(EWRInventorySoundType::Cross);
				return true;
			}
		}
		else
		{
			if (InSlotInfo.SlotGroupType == EWRInventorySlotGroup::Storage)
			{
				switch (CurrentInventoryMainCategory)
				{
				case EWRInventoryCategoryType::Equipment:
				case EWRInventoryCategoryType::Costume:
				{

				}
				break;

				case EWRInventoryCategoryType::Vehicle:
				case EWRInventoryCategoryType::Consume:
				{
					// [���ư���] : InnerCategory�� �̵� �� Storage �ʱ�ȭ
					SetSelectInnerCategory(0, true);
					PlaySound(EWRInventorySoundType::Cross);
					return true;
				}
				break;
				}				
			}
			else if (SelectedStorageISN != INVALID_ISN)
			{
				SetStorageSlotSelected(INVALID_ISN);
				PlaySound(EWRInventorySoundType::Cross);
				return true;
			}
		}
		
		// [���ư���] : Contents -> Category �̵�
		SwitchInventory(EWRInventoryMainSwitcher::Category);
		PlaySound(EWRInventorySoundType::Cross);

		return true;
	}
	break;
	}

	return false;
}

EWRInventorySlotAction WRInventoryManager::GetInventorySlotAction()
{
	if (CurrentInventoryMainSwitcher != EWRInventoryMainSwitcher::Contents)
		return EWRInventorySlotAction::None;


	switch (LastFocusedSlotGroup)
	{
	case EWRInventorySlotGroup::EquipList:
	{
		if (SelectedStorageISN != INVALID_ISN)
		{
			WRItemBase* FoundItem = WRItemManager::Get()->FindItem(SelectedStorageISN);
			if (FoundItem == nullptr)
			{
				// �ǵ����� ������ �н�
				return EWRInventorySlotAction::None;
			}
			if (IsSlotRegistableItemType(LastFocusedRegistrationSlot, FoundItem->GetMainType(), FoundItem->GetSubType()) == false)
			{
				// ��� �Ұ����� Ÿ���� �����̸� �н�
				return EWRInventorySlotAction::None;
			}
			if (FoundItem->IsWeapon())
			{
				// ����� [���]
				return EWRInventorySlotAction::RegisterWhenStorageSelecting;
			}
			else
			{
				// ���õ� Storage �������� �ִ� ��� ����� �ƴϸ� �����ε� ������ ���û����� �ƴ�
			}
		}
		else
		{
//			if (CurrentFocusedISN == INVALID_ISN)
			{
				return EWRInventorySlotAction::GotoStorage;
			}
// 			else
// 			{
// 				WRItemBase* FoundItem = WRItemManager::Get()->FindItem(CurrentFocusedISN);
// 				if (FoundItem)
// 				{
// 					if (FoundItem->IsEquipped())
// 					{
// 						return EWRInventorySlotAction::UnEquip;
// 					}
// 					else if (IsRegisteredItem(CurrentFocusedISN))
// 					{
// 						return EWRInventorySlotAction::UnRegister;
// 					}
// 				}
// 			}
		}
	}
	break;

	case EWRInventorySlotGroup::Storage:
	{
		if (CurrentFocusedISN == INVALID_ISN)
		{
			// ���� ���Կ� ������ ������ �н�
			return EWRInventorySlotAction::None;
		}

		WRItemBase* FoundItem = WRItemManager::Get()->FindItem(CurrentFocusedISN);
		if (FoundItem == nullptr)
		{
			// ���� ������ ������ �н�
			return EWRInventorySlotAction::None;
		}
		

		if (FoundItem->IsEquipped())
		{
			EWRRegistrationSlotType RegisterdSlotType = GetRegisteredSlotType(CurrentFocusedISN);
			if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None)
			{
				// �������� ��Ͻ����� ���� �� [��������]
				return EWRInventorySlotAction::UnEquip;
			}
			else if (CurrentInventorySelectedRegistrationSlot == RegisterdSlotType)
			{
				// �̹� ���� ���Կ� ������ [��������]
				return EWRInventorySlotAction::UnEquip; //EWRInventorySlotAction::Exception_AlreadySameSlotEquipped;
			}
			else
			{
				// �̹� �ٸ� ���Կ� ������ - �޽��� (35)
				return EWRInventorySlotAction::Exception_AlreadyOtherSlotEquipped;
			}
		}
		else if (IsRegisteredItem(CurrentFocusedISN))
		{
			EWRRegistrationSlotType RegisterdSlotType = GetRegisteredSlotType(CurrentFocusedISN);
			if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None)
			{
				// �������� ��Ͻ����� ���� �� [�������]
				return EWRInventorySlotAction::UnRegister;
			}
			if (CurrentInventorySelectedRegistrationSlot == RegisterdSlotType)
			{
				// �̹� ���� ���Կ� ����� [�������]
				return EWRInventorySlotAction::UnRegister; //EWRInventorySlotAction::Exception_AlreadySameSlotRegistered;
			}
			else
			{
				// �̹� �ٸ� ���Կ� ����� - �޽��� (36)
				return EWRInventorySlotAction::Exception_AlreadyOtherSlotRegistered;
			}
		}
		else if (FoundItem->IsVehicle())
		{
			if (WRVehicleManager::Get()->IsSpawnedVehicle() && WRVehicleManager::Get()->IsValidSapwnedVehicleISN(CurrentFocusedISN))
			{
				// ���� �������̸� [����]
				return EWRInventorySlotAction::UnSpawnVehicle;
			}
			else
			{
				FWRVehicleItemTable* CastedItemTable = static_cast<FWRVehicleItemTable*>(FoundItem->GetTable());
				if (CastedItemTable)
				{
					// ���� [����]
					return EWRInventorySlotAction::SpawnVehicle;
				}
			}
		}
		else if (FoundItem->IsConsume())
		{
			if (FoundItem->IsInterior())
			{
				// ���׸��� [����]
				return EWRInventorySlotAction::SpawnInterior;
			}
			else if (FoundItem->IsMaterial())
			{

			}
			else if (FoundItem->IsEmote())
			{
				// ���������� [���]
				return EWRInventorySlotAction::Use;
			}
			else if (FoundItem->IsPlacement())
			{
				// ��ġ [����]
				return EWRInventorySlotAction::SpawnPlacement;
			}
		}
		else
		{
			if (CurrentInventorySelectedRegistrationSlot == EWRRegistrationSlotType::None)
			{
				if (FoundItem->IsWeapon())
				{
					// ����� [����Ϸ�����]
					return EWRInventorySlotAction::GotoEquipList;
				}
				else
				{
					// ��Ÿ [����]
					return EWRInventorySlotAction::Equip;
				}
			}
			else
			{
				if (IsLeftWeaponRegistarationType(CurrentInventorySelectedRegistrationSlot) || IsRightWeaponRegistarationType(CurrentInventorySelectedRegistrationSlot))
				{
					// ����� [���]
					return EWRInventorySlotAction::RegisterToSelectedRegistrationSlot;
				}
				else
				{
					// ��Ÿ [����]
					return EWRInventorySlotAction::Equip;
				}
			}
		}
	}
	break;
	}

	return EWRInventorySlotAction::None;
}

void WRInventoryManager::SetEquipListSlotSelected(const EWRRegistrationSlotType IN InSlotType)
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	UWRUISlot_InventoryEquipList* InventoryEquipListUI = InventoryPanel->GetInventoryEquipListUI();
	if (InventoryEquipListUI == nullptr)
		return;

	UWRUISlot_InventoryStorage* InventoryStorageUI = InventoryPanel->GetInventoryStorageUI();
	if (InventoryStorageUI == nullptr)
		return;
	
	switch (InSlotType)
	{
	case EWRRegistrationSlotType::None:
	{
		// -- EquipList�� �ǵ��ư���

		// 1. EquipList Update
		InventoryEquipListUI->SetEquipListSlotSelected(CurrentInventorySelectedRegistrationSlot, false);

		CurrentInventorySelectedRegistrationSlot = InSlotType;

		// 2. Focus Update
		InventoryEquipListUI->RefocusOnLastFocusedButton();											// ��Ͻ����� �����ϸ� Storage -> EquipList�� �̵�. ������ ��Ŀ���� �������� �ٽ� ��Ŀ��
		InventoryStorageUI->SetNavigationRuleBase(EUINavigation::Up, EUINavigationRule::Escape);	// Storage���� EquipList���� �̵��� Ǯ��
	}
	break;

	default:
	{
		CurrentInventorySelectedRegistrationSlot = InSlotType;

		// 1. EquipList Update
		InventoryEquipListUI->SetEquipListSlotSelected(CurrentInventorySelectedRegistrationSlot, true);

		// 2. Focus Update
		InventoryStorageUI->FocusToDefaultFocusButton();											// ��Ͻ����� �����ϸ� EquipList -> Storage�� �̵�. ù �������� ��Ŀ��
		InventoryStorageUI->SetNavigationRuleBase(EUINavigation::Up, EUINavigationRule::Stop);		// Storage���� EquipList���� �̵��� ����
	}
	break;
	}

	// Storage ������Ʈ
	// Update By 
	// + CurCategory 
	// + CurrentInventorySelectedRegistrationSlot
	// + CurrentInventoryInnerCategoryIndex
	// + CurrentPage
	CollectStorageItemList();

	UpdateStorage();
	UpdateSimpleInformation();
	UpdateControlGuide();
}

void WRInventoryManager::SetStorageSlotSelected(const ISN_t IN InISN)
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	UWRUISlot_InventoryEquipList* InventoryEquipListUI = InventoryPanel->GetInventoryEquipListUI();
	if (InventoryEquipListUI == nullptr)
		return;

	UWRUISlot_InventoryStorage* InventoryStorageUI = InventoryPanel->GetInventoryStorageUI();
	if (InventoryStorageUI == nullptr)
		return;
	
	// Storage Slot ����
	// EquipList�� �̵�
	// EquipList���� Storage�� �̵� �Ұ�
		
	SelectedStorageISN = InISN;

	InventoryEquipListUI->SetStorageSlotSelected(InISN);
	InventoryStorageUI->SetStorageSlotSelected(InISN);

	//UpdateSimpleInformation();
	UpdateControlGuide();
}

void WRInventoryManager::SetSelectInnerCategory(const int32 IN InIndex, const bool IN InInit /* = false */)
{
	UWRUIPanel_Inventory* InventoryPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Inventory>(EWRUIPanelKind::Inventory);
	if (InventoryPanel == nullptr)
		return;

	UWRUISlot_InventoryInnerCategory* InventoryInnerCategoryUI = InventoryPanel->GetInventoryInnerCategoryUI();
	if (InventoryInnerCategoryUI == nullptr)
		return;

	UWRUISlot_InventoryStorage* InventoryStorageUI = InventoryPanel->GetInventoryStorageUI();
	if (InventoryStorageUI == nullptr)
		return;

	if (InInit)
	{
		// Prev Release
		InventoryInnerCategoryUI->SetSelectInnerCategory(CurrentInventoryInnerCategoryIndex, false);

		// New Select
		CurrentInventoryInnerCategoryIndex = -1;

		// Focus Update
		InventoryInnerCategoryUI->RefocusOnLastFocusedButton();										// ����ī�װ��� �����ϸ� Storage -> InnerCategory�� �̵�. ������ ��Ŀ���� �������� �ٽ� ��Ŀ��
		InventoryStorageUI->SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Escape);	// Storage���� InnerCategory���� �̵��� Ǯ��
	}
	else
	{
		// Prev Release
		InventoryInnerCategoryUI->SetSelectInnerCategory(CurrentInventoryInnerCategoryIndex, false);

		// New Select
		CurrentInventoryInnerCategoryIndex = InIndex;
		InventoryInnerCategoryUI->SetSelectInnerCategory(CurrentInventoryInnerCategoryIndex, true);

		// Focus Update
		InventoryStorageUI->FocusToDefaultFocusButton();											// ��Ͻ����� �����ϸ� EquipList -> InnerCategory�� �̵�. ù �������� ��Ŀ��
		InventoryStorageUI->SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Stop);	// Storage���� InnerCategory���� �̵��� ����
	}

	CollectStorageItemList();

	UpdateStorage();
	UpdateSimpleInformation();
	UpdatePriceInfo();
	UpdateControlGuide();
}

void WRInventoryManager::PlaySound(const EWRInventorySoundType IN InSoundType)
{
	if (InventoryAsset == nullptr)
		return;

	FName SoundName = InventoryAsset->GetSound(InSoundType);
	if (SoundName == NAME_None)
		return;

	WRSFXManager::Get()->Spawn2D(WRUIUtility::ConvertFNameToFString(SoundName));
}

std::list<stSekaiItemInfo>* WRInventoryManager::GetItemInfoList(const TID_t IN InTID)
{
	std::list<stSekaiItemInfo>* InList = nullptr;
	if (SekaiItemInfoMap.Contains(InTID) == true)
		InList = SekaiItemInfoMap.Find(InTID);
	return InList;
}

std::list<stSekaiCurrencyInfo>* WRInventoryManager::GetCurrentInventoryMainCategory(const TID_t IN InTID)
{
	std::list<stSekaiCurrencyInfo>* InList = nullptr;
	if (SekaiItemInfoMap.Contains(InTID) == true)
		InList = SekaiCurrencyInfoMap.Find(InTID);
	return InList;
}

//====================================================================================
// Packet
//====================================================================================

void WRInventoryManager::OnAddItemReceived(const std::list<stItem>* IN InReceivedItemList)
{
	// Storage ������Ʈ
	// Update By 
	// + CurCategory 
	// + CurrentInventorySelectedRegistrationSlot
	// + CurrentInventoryInnerCategoryIndex
	// + CurrentPage
	CollectStorageItemList();

	std::list<stItem>::const_iterator Iter = InReceivedItemList->begin();
	for (; Iter != InReceivedItemList->end(); ++Iter)
	{
		AddNewItemISN(Iter->ISN);

		WRNoticeManager::Get()->AddItem(Iter->TID, Iter->itemCount);
	}

	UpdateStorage();
	UpdateControlGuide();
}

void WRInventoryManager::OnUpdateItemCountReceived()
{
	UpdateStorage();
	UpdateControlGuide();
}

void WRInventoryManager::OnDeleteItemReceived()
{
	// Storage ������Ʈ
	// Update By 
	// + CurCategory 
	// + CurrentInventorySelectedRegistrationSlot
	// + CurrentInventoryInnerCategoryIndex
	// + CurrentPage
	CollectStorageItemList();

	UpdateStorage();
	UpdateControlGuide();
}

void WRInventoryManager::OnEquipItemReceived(const ISN_t IN InEquippedISN)
{	
	WREquipItemBase* NewEquipItem = WRItemManager::Get()->FindItem<WREquipItemBase>(InEquippedISN);
	if (NewEquipItem == nullptr)
		return;

	// Save
	EWRRegistrationSlotType EquippedRegistrationSlotType = GetRegisteredSlotType(NewEquipItem->GetISN());
	if (IsLeftWeaponRegistarationType(EquippedRegistrationSlotType))
	{
		EquippedLeftWeaponRegistrationSlotType = EquippedRegistrationSlotType;
	}
	else if (IsRightWeaponRegistarationType(EquippedRegistrationSlotType))
	{
		EquippedRightWeaponRegistrationSlotType = EquippedRegistrationSlotType;
	}

	// Update Inventory UI
	UpdateEquipList();
	UpdateStorage();
	UpdateControlGuide();
	UpdateSimpleInformation();

	PlaySound(EWRInventorySoundType::Equip);
}

void WRInventoryManager::OnUnEquipItemReceived(const ISN_t IN InUnEquippedISN)
{
	WRItemBase* EquippedItem = WRItemManager::Get()->FindItem(InUnEquippedISN);
	if (EquippedItem == nullptr)
		return;

	// Release
	EWRRegistrationSlotType UnEquippedRegistrationSlotType = GetRegisteredSlotType(InUnEquippedISN);
	if (IsLeftWeaponRegistarationType(UnEquippedRegistrationSlotType))
	{
		EquippedLeftWeaponRegistrationSlotType = EWRRegistrationSlotType::None;
	}
	else if (IsRightWeaponRegistarationType(UnEquippedRegistrationSlotType))
	{
		EquippedRightWeaponRegistrationSlotType = EWRRegistrationSlotType::None;
	}

	UpdateEquipList();
	UpdateStorage();
	UpdateControlGuide();
	UpdateSimpleInformation();

	PlaySound(EWRInventorySoundType::Unequip);
}

void WRInventoryManager::OnChangeEquipmentReceived()
{
	UpdateEquipList();
	UpdateStorage();
	UpdateControlGuide();
}

void WRInventoryManager::OnRegisterEquipmentSlot(pt_Game_User_Inventory::Ack_RegisterEquipmentSlot_G2U* IN InPacket)
{
	WREquipItemBase* NewRegisteredItem = WRItemManager::Get()->FindItem<WREquipItemBase>(InPacket->ISN);
	if (NewRegisteredItem == nullptr)
		return;

	EWRRegistrationSlotType RegistrationSlotType = (EWRRegistrationSlotType)InPacket->slotType;

	WREquipItemBase* PrevRegisteredItem = nullptr;
	if (InPacket->bIsAlreadyEquippedSlot)
	{
		PrevRegisteredItem = RegistrationItemMap.FindRef(RegistrationSlotType);

		// + �������� ���Կ� ����ϴ°��̶� ���� ��ü ó��
	}	

	RegistrationItemMap.Remove(RegistrationSlotType);
	RegistrationItemMap.Add(RegistrationSlotType, NewRegisteredItem);

	UpdateEquipList();
	UpdateStorage();
	UpdateControlGuide();
	UpdateSimpleInformation();

	PlaySound(EWRInventorySoundType::Register);
}

void WRInventoryManager::OnUnRegisterEquipmentSlot(pt_Game_User_Inventory::Ack_UnRegisterEquipmentSlot_G2U* IN InPacket)
{
	WREquipItemBase* UnRegisteredItem = WRItemManager::Get()->FindItem<WREquipItemBase>(InPacket->ISN);
	if (UnRegisteredItem == nullptr)
		return;

	EWRRegistrationSlotType RegistrationSlotType = GetRegisteredSlotType(InPacket->ISN);
	RegistrationItemMap.Remove(RegistrationSlotType);

	UpdateEquipList();
	UpdateStorage();
	UpdateControlGuide();
	UpdateSimpleInformation();

	PlaySound(EWRInventorySoundType::Unregister);
}

void WRInventoryManager::OnUseItem(pt_Game_User_Inventory::Ack_UseItem_G2U* IN InPacket)
{
	CollectStorageItemList();

	UpdateStorage();
	UpdateControlGuide();
	UpdateSimpleInformation();
}

void WRInventoryManager::OnDiscardItem(pt_Game_User_Inventory::Ack_DiscardItem_G2U* IN InPacket)
{
// 	CollectStorageItemList();
// 
// 	UpdateStorage();
// 	UpdateControlGuide();

	PlaySound(EWRInventorySoundType::Discard);
}
