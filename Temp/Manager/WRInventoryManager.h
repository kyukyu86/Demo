// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Item/WRItem.h"
#include "Struct/WRUIStruct.h"

#include "Enum/EWRItem.h"
#include "Enum/EWRUIEnum.h"
#include "Enum/EWRCharacter.h"

#include "InputCoreTypes.h"

#include "Network/WRProtocolHeaders.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_BluePrint.h"


// For Header
DECLARE_LOG_CATEGORY_EXTERN(InventoryManager, Display, All);

DECLARE_DELEGATE(FWRCompletedInventoryClose);

#define INVALID_SERIAL_NUMBER 0
#define DEF_INVENTORY_STORAGE_BASE_EXTEND 15

class WR_API WRInventoryManager : public WRSingleton<WRInventoryManager>
{
	CDECLARE_MANAGER(WRInventoryManager);

private:
	//====================================================================================
	// for Data
	TMap<EWRRegistrationSlotType, WREquipItemBase*> RegistrationItemMap;
	TDoubleLinkedList<ISN_t> NewItemList;
	TDoubleLinkedList<WRItemBase*> StorageItemList;
	EWRRegistrationSlotType EquippedLeftWeaponRegistrationSlotType = EWRRegistrationSlotType::None;
	EWRRegistrationSlotType EquippedRightWeaponRegistrationSlotType = EWRRegistrationSlotType::None;
	TMap<TID_t, std::list<stSekaiItemInfo>> SekaiItemInfoMap;
	TMap<TID_t, std::list<stSekaiCurrencyInfo>> SekaiCurrencyInfoMap;
	//====================================================================================
	// for Widget
	EWRInventoryMainSwitcher CurrentInventoryMainSwitcher = EWRInventoryMainSwitcher::Category;
	EWRInventoryCategoryType CurrentInventoryMainCategory = EWRInventoryCategoryType::MAX;
	EWRRegistrationSlotType CurrentInventorySelectedRegistrationSlot = EWRRegistrationSlotType::None;
	EWRInventorySlotGroup LastFocusedSlotGroup = EWRInventorySlotGroup::None;
	EWRRegistrationSlotType LastFocusedRegistrationSlot = EWRRegistrationSlotType::None;
	int32 CurrentInventoryStoragePage = 1;
	int32 CurrentInventorySetIndex = 0;
	int32 CurrentInventoryInnerCategoryIndex = -1;
	ISN_t CurrentFocusedISN = INVALID_ISN;
	ISN_t SelectedStorageISN = INVALID_ISN;

	FWRInventoryDiscardData DiscardData;

	FString strInventoryAssetAsyncKey = "";
	class UWRInventoryAsset* InventoryAsset = nullptr;

	bool bInputLock = true;
	bool bInputEventActivate = false;

public:
	FWRCompletedInventoryClose CompletedInventoryCloseDelegate;

private:
	WRInventoryManager();
	virtual ~WRInventoryManager() {}

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override {};
	virtual void OnLoadLevelStart() override;
	virtual void OnRenderStart() override {};
	void OnMapMovementComplete();

private:
	//====================================================================================
	// for Data
	void AddNewItemISN(const ISN_t IN InISN);
	void ClearNewItemList();	

	WREquipItemBase* GetRegistrationSlotItemForWeapon(const int32 IN InStartIndex, const int32 IN InEndIndex, const int32 IN InStandardIndex);
	WREquipItemBase* GetNextRegisteredMainWeapon(bool IN bInReturnNullIfReturnedToHeadInList = false);
	WREquipItemBase* GetNextRegisteredSubWeapon(bool IN bInReturnNullIfReturnedToHeadInList = false);
	
	//====================================================================================
	// for Widget
	void ReleaseData();
	void SetEquipListSlotSelected(const EWRRegistrationSlotType IN InSlotType);
	void SetStorageSlotSelected(const ISN_t IN InISN);
		
	void UpdateTitle();
	void UpdateEquipList();
	void UpdateStorage();
	void UpdateStorageSlot(const ISN_t IN InISN);
	void UpdateSimpleInformation();
	void UpdatePriceInfo();
	void UpdateControlGuide();
	void UpdateStorageToCustomPage(const int32 IN InCustomPage);

	void OnPrevStoragePage();
	void OnNextStoragePage();

	void CollectStorageItemList();
	void GetStorageItemListByCurPage(TDoubleLinkedList<WRItemBase*>& OUT OutList);

	void RefocusToInventoryContents();

public:	
	//====================================================================================
	// for Data
	void EquipNextMainWeapon();
	void EquipNextSubWeapon();

	bool IsNewItem(const ISN_t IN InISN);
	bool IsRegisteredItem(const ISN_t IN InISN);
	bool IsLeftWeaponRegistarationType(const EWRRegistrationSlotType IN InRegistrationSlotType);
	bool IsRightWeaponRegistarationType(const EWRRegistrationSlotType IN InRegistrationSlotType);
	bool IsSlotRegistableItemType(const EWRRegistrationSlotType IN InTargetRegistrationSlotType, const EWRItemMainType IN InItemMainType, const EWRItemSubType IN InItemSubType);

	EWRRegistrationSlotType GetRegisteredSlotType(const ISN_t IN InISN);
	EWRItemEquippableSlot GetEquippableSlotType(const ISN_t IN InISN);
	EWRItemEquippableSlot ConvertRegistrationTypeToEquippableType(const EWRRegistrationSlotType IN InRegistrationSlotType);
	EWRRegistrationSlotType ConvertEquippableTypeToRegistrationType(const EWRItemEquippableSlot IN InEquippableSlotType);

	//====================================================================================
	// for Widget
	void OnClickedSet(const int32 IN InSetIndex);

	bool OnInputEvent(const EWRInput IN InInput);
	bool OnInputEvent(const EWRInput IN InInput, EInputEvent InEventType);
	bool OnInputEventInventorySlot(const FKey IN InKey, const FWRInventorySlotInfo& IN InSlotInfo);		// input !!!!!!!!!!!

	EWRInventorySlotAction GetInventorySlotAction();

	void OnDiscardItem();
	void OnDiscardItemCancel();
	void OnDiscardItemQuantityChanged(const int32 IN InQuantity);

	FORCEINLINE EWRInventoryMainSwitcher GetReserveInventoryMainSwitcher() { return CurrentInventoryMainSwitcher; }
	FORCEINLINE EWRInventoryCategoryType GetCurrentInventoryMainCategory() { return CurrentInventoryMainCategory; }
	FORCEINLINE EWRRegistrationSlotType GetCurrentInventorySelectedRegistrationSlot() { return CurrentInventorySelectedRegistrationSlot; }
	FORCEINLINE EWRInventorySlotGroup GetLastFocusedSlotGroup() { return LastFocusedSlotGroup; }
	FORCEINLINE ISN_t GetCurrentFocusedISN() { return CurrentFocusedISN; }
	FORCEINLINE int32 GetCurrentInventoryStoragePage() { return CurrentInventoryStoragePage; }
	FORCEINLINE int32 GetCurrentInventorySetIndex() { return CurrentInventorySetIndex; }
	EWRInventoryMiddleType GetInventoryMiddleType();
	int32 GetCurrentInventoryInnerCategoryType() { return CurrentInventoryInnerCategoryIndex; }
	int32 GetInventoryStorageMaxPage();

	// Main Inventory
	void OpenInventory(const EWRInventoryCategoryType IN InManualCategory = EWRInventoryCategoryType::MAX);
	void SwitchInventory(const EWRInventoryMainSwitcher IN InSwitcher, const EWRInventoryCategoryType IN InCategory = EWRInventoryCategoryType::MAX, const bool IN InNewOpen = false);
	void CloseInventory();
	void OnCompletedInventoryClose();

	// Category
	void OnCategoryOpenDirectionFinished();

	// InvenSlot
	void OpenItemInformation(const ISN_t IN InISN);
	void CloseItemInformation();
	void OnFocusedInventorySlot(const ISN_t IN InISN, const EWRInventorySlotGroup IN InSlotGroup, const EWRRegistrationSlotType IN InRegistrationSlotType, const bool IN InDiff = true);
	void UpdateFocusedInventorySlot(const ISN_t IN InISN, const EWRInventorySlotGroup IN InSlotGroup, const EWRRegistrationSlotType IN InRegistrationSlotType, const bool IN InDiff = true);

	// Inner Category
	void SetSelectInnerCategory(const int32 IN InIndex, const bool IN InInit = false);

	void PlaySound(const EWRInventorySoundType IN InSoundType);

	// Sekai
	std::list<stSekaiItemInfo>* GetItemInfoList(const TID_t IN InTID);
	std::list<stSekaiCurrencyInfo>* GetCurrentInventoryMainCategory(const TID_t IN InTID);
	
	//====================================================================================
	// Packet
	void OnAddItemReceived(const std::list<stItem>* IN InReceivedItemList);
	void OnUpdateItemCountReceived();
	void OnDeleteItemReceived();

	void OnEquipItemReceived(const ISN_t IN InEquippedISN);
	void OnUnEquipItemReceived(const ISN_t IN InUnEquippedISN);
	void OnChangeEquipmentReceived();

	void OnRegisterEquipmentSlot(pt_Game_User_Inventory::Ack_RegisterEquipmentSlot_G2U* IN InPacket);
	void OnUnRegisterEquipmentSlot(pt_Game_User_Inventory::Ack_UnRegisterEquipmentSlot_G2U* IN InPacket);

	void OnUseItem(pt_Game_User_Inventory::Ack_UseItem_G2U* IN InPacket);
	void OnDiscardItem(pt_Game_User_Inventory::Ack_DiscardItem_G2U* IN InPacket);

	void OnUnspawnVehicleReceived(pt_Game_Vehicle::Ack_UnspawnVehicle_G2U* IN InPacket);

	void OnSekaiItemInfoReceived(pt_Game_User::Inf_SekaiItemInfo_G2U* IN Packet);
	void OnSekaiCurrencyInfoReceived(pt_Game_User::Inf_SekaiCurrencyInfo_G2U* IN Packet);

	void OnUpdateCurrency(const EWRCurrencyType IN InCurrencyType);
};