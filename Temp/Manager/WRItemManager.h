// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Item/WRItem.h"
#include "Enum/EWRItem.h"
#include "Network/WRProtocolHeaders.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_Item.h"
#include "Network/Share/Define/Define_BluePrint.h"


enum class EWRItemListArray : uint8
{
	WeaponArray = 0,
	ArmorArray,
	TemporaryArray,
	CostumeArray,
	MaterialArray,
	InteriorArray,
	HousingArray,
	EmoteArray,
	PlacementArray,
	VehicleArray,

	// �ӽ�
	SubWeaponArray,
	MAX_ARRAY_COUNT,
};

#define INVALID_SERIAL_NUMBER 0

class WR_API WRItemManager : public WRSingleton<WRItemManager>
{
	CDECLARE_MANAGER(WRItemManager);

	friend class WRInventoryManager;

private:
	WRItemManager();
	virtual ~WRItemManager() {}

	TDoubleLinkedList<WRItemBase*> EachItemList[int32(EWRItemListArray::MAX_ARRAY_COUNT)];

	TMap<EWRItemEquippableSlot, WREquipItemBase*> EquippedItemList;
	TMap<EWRItemMainType, TDoubleLinkedList<WRItemBase*>*>	TypeDevidedItemLists;

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override;
	virtual void OnLoadLevelStart() override;
	virtual void OnRenderStart() override {};

private:
	EWRItemListArray ConvertMainTypeToArrayType(const EWRItemMainType IN InItemMainType);
	WRItemBase* AddItemToList(TDoubleLinkedList<WRItemBase*>& IN InEachList, WRItemBase* IN InItemData);
	WRItemBase* FindItemInListByISN(TDoubleLinkedList<WRItemBase*>& IN InList, const ISN_t IN InItemToFind);
	void EquipWeaponCostume(const EWRItemMainType IN InMainType, const EWRItemSubType IN InSubType);
	void ClearItems(const EWRItemMainType IN InItemTypeToClear);
	void DeleteAllItems();

	void AddItem(const WRTableID IN InTID, const ISN_t IN InISN = INVALID_ISN, const ITEM_COUNT_t IN InCount = 1);
	void UpdateItemQuantity(const ISN_t IN InISN, const ITEM_COUNT_t IN InQuantity);
	void DeleteItem(const ISN_t IN InISN);

	void ClearAllItems();

	void EquipItem(const ISN_t IN InISN);
	void EquipItem(WREquipItemBase* IN InNewEquipItem);
	void UnEquipItem(const ISN_t IN InISN);
	void UnEquipItem(WREquipItemBase* IN InTargetItem);
	TDoubleLinkedList<WRItemBase*>* GetItemList(const EWRItemMainType IN InItemTypeToGetList);

	void OpenEquipmentChangeUI(WREquipItemBase* IN InSourceItem, WREquipItemBase* IN InTargetItem);

public:
	WRTableID ConvertItemTIDToCharacterWeaponTID(const WRTableID IN InItemTID);
	FORCEINLINE ISN_t GenerateSerialNumber() { static ISN_t StoredSerialNumber = INVALID_SERIAL_NUMBER; ++StoredSerialNumber; return StoredSerialNumber; }	// Temporary Code

	template <class TItem>
	TItem*				FindItem(const ISN_t IN InISNtoFindItem);
	WRItemBase*			FindItem(const ISN_t IN InISNtoFindItem);
	//WREquipItemBase*	FindItemInEquippedList(const EWRItemMainType IN InItemMainTypeToFind, const EWRItemSubType IN InItemSubTypeToFind);		// [ 2020-7-13 : vuvle ] Main Type �� SubType ���δ� �������� �������� ����
	WREquipItemBase*	FindItemInEquippedList(const ISN_t IN InISN);
	WREquipItemBase*	FindItemInEquippedList(const EWRItemEquippableSlot IN InEquippableSlot);
	WREquipItemBase*	FindEquippedMainWeapon();								// vuvle TODO : 8 ������ ��ȿ�� �Լ�.(��� ����� ������� ���������� 8�������� �޼հ��� ������ ���� ���Ⱑ ����)
	WREquipItemBase*	FindEquippedSubWeapon();								// vuvle TODO : 8 ������ ��ȿ�� �Լ�.(��� ����� ������� ���������� 8�������� �޼հ��� ������ ���� ���Ⱑ ����)
	WREquipItemBase*	FindEquippedTemporaryWeapon();							// vuvle TODO : 8 ������ ��ȿ�� �Լ�.(��� ����� ������� ���������� 8�������� �޼հ��� ������ ���� ���Ⱑ ����)
	ITEM_COUNT_t		FindItemQuantity(const ISN_t IN InISN);
	ITEM_COUNT_t		FindItemTotalQuantity(const WRTableID IN InTID);		// [ 2020-7-14 : vuvle ] ��ü����Ʈ�� ���� TID ���� ������ ������ ������. ������ �κ��丮 ���� ������ TID �� ���� �������� ������ ���� �� ��� �ش� �������� ���� ���� �Һ��Ͽ����� �����ʿ��� Delete �� ���������� ���� ������ Ȯ���� �ʿ��ϴ�.

	// [ 2020-3-12 : kyu ] for Temporary
	bool ThrowTemporaryItemAway(const EWRReasonForThrowTemporaryItemAway IN InReason);
	
	// Packet
	void OnEntryItemList(const std::list<stItem>* IN InReceivedItemList, const bool IN InFinished);
	void OnAddItemReceived(const std::list<stItem>* IN InReceivedItemList);
	void OnUpdateItemQuantityReceived(const std::list<stItem>* IN InReceivedItemList);
	void OnDeleteItemReceived(const std::list<stItem>* IN InReceivedItemList);
	void OnEquipItemReceived(struct pt_Game_User_Inventory::Ack_EquipItem_G2U* IN InPacket);
	void OnUnEquipItemReceived(const ISN_t IN InISN);
};


template <class TItem>
TItem* WRItemManager::FindItem(const ISN_t IN InISNtoFindItem)
{
	TMap<EWRItemMainType, TDoubleLinkedList<WRItemBase*>*>::TIterator TypeIter = TypeDevidedItemLists.CreateIterator();
	for (; TypeIter; ++TypeIter)
	{
		TDoubleLinkedList<WRItemBase*>* TypeList = TypeIter.Value();
		if (TypeList)
		{
			WRItemBase* FoundItem = FindItemInListByISN((*TypeList), InISNtoFindItem);
			if (FoundItem)
			{
				TItem* CastedItem = static_cast<TItem*>(FoundItem);
				if (CastedItem != nullptr)
					return CastedItem;
			}
		}
	}

	return nullptr;
}