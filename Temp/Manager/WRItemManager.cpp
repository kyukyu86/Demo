// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRItemManager.h"
#include <list>
#include "Table/WRItemTable.h"
#include "Table/Base/WRTableManager.h"
#include "Utility/WREnumUtility.h"
#include "Network/Share/Protocol/Protocol_Game_User_Inventory.h"
#include "WRTutorialManager.h"
#include "WRCharacterManager.h"
#include "Enum/EWRCharacter.h"
#include "Network/IMMessageSender.h"
#include "Actor/Character/WRCharacterMine.h"
#include "WRCheersManager.h"

WRItemManager::WRItemManager()
{
	TypeDevidedItemLists.Add(EWRItemMainType::Weapon,		&EachItemList[uint64(EWRItemListArray::WeaponArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Armor,		&EachItemList[uint64(EWRItemListArray::ArmorArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Temporary,	&EachItemList[uint64(EWRItemListArray::TemporaryArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Costume,		&EachItemList[uint64(EWRItemListArray::CostumeArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Material,		&EachItemList[uint64(EWRItemListArray::MaterialArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Interior,		&EachItemList[uint64(EWRItemListArray::InteriorArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Housing,		&EachItemList[uint64(EWRItemListArray::HousingArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Emote,		&EachItemList[uint64(EWRItemListArray::EmoteArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Placement,	&EachItemList[uint64(EWRItemListArray::PlacementArray)]);
	TypeDevidedItemLists.Add(EWRItemMainType::Vehicle,		&EachItemList[uint64(EWRItemListArray::VehicleArray)]);




}

void WRItemManager::OnInitialize()
{
	//TestCode
	//AddItem(1000001);
	//AddItem(1000002);
	//AddItem(1000003);
	//AddItem(1000004);
	//AddItem(1000005);


	//EquipItem(1);
	//UnEquipItem(1);
	//EquipItem(2);
	//EquipItem(3);
	//UnEquipItem(1);
}

void WRItemManager::OnShutdown()
{
	ClearAllItems();
}

void WRItemManager::OnLoadLevelComplete()
{
	
}

void WRItemManager::OnLoadLevelStart()
{
	
}

EWRItemListArray WRItemManager::ConvertMainTypeToArrayType(const EWRItemMainType IN InItemMainType)
{
	FString ItemMainTypeString = WREnumUtility::EnumToString("EWRItemMainType", InItemMainType) + TEXT("Array");

	EWRItemListArray ReturnValue = EWRItemListArray(WREnumUtility::StringToEnum("EWRItemListArray", ItemMainTypeString));

	return ReturnValue;
}

WRItemBase* WRItemManager::AddItemToList(TDoubleLinkedList<WRItemBase*>& IN InEachList, WRItemBase* IN InItemData)
{
	if (InItemData != nullptr)
	{
		InEachList.AddTail(InItemData); // Add Each List
		return InItemData;
	}
	return nullptr;
}

WRItemBase* WRItemManager::FindItemInListByISN(TDoubleLinkedList<WRItemBase*>& IN InList, const ISN_t IN InItemToFind)
{
	if (InItemToFind == INVALID_SERIAL_NUMBER)
	{
		ensureMsgf(false, TEXT("Invalid ISN Entered"));
		return nullptr;
	}

	for (auto Node = InList.GetHead(); Node != nullptr; Node = Node->GetNextNode())
	{
		WRItemBase* TempItem = Node->GetValue();
		if (TempItem && TempItem->GetISN() == InItemToFind)
		{
			return TempItem;
		}
	}

	return nullptr;
}

void WRItemManager::EquipWeaponCostume(const EWRItemMainType IN InMainType, const EWRItemSubType IN InSubType)
{
	EWRChangeEquipment ChangeEquipment = EWRChangeEquipment::MainWeapon;

	switch (InMainType)
	{
	case EWRItemMainType::Weapon:
		if (InSubType == EWRItemSubType::Weapon)
		{
			ChangeEquipment = EWRChangeEquipment::MainWeapon;
		}
		else
		{
			ChangeEquipment = EWRChangeEquipment::SubWeapon;
		}
		
		break;

	case EWRItemMainType::Temporary:
		ChangeEquipment = EWRChangeEquipment::Temporary;
		break;

	default:
		return;
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if(MyCharacter)
		MyCharacter->ChangeWeaponWithAnimation(ChangeEquipment);
}

void WRItemManager::AddItem(const WRTableID IN InTID, const ISN_t IN InISN /* = INVALID_ISN */, const ITEM_COUNT_t IN InCount /* = 1 */)
{
	FWRItemTableBase* FoundItemTable = WRTableManager::Get()->FindRowInItemTable(InTID);

	if (FoundItemTable == nullptr)
	{
		ensureMsgf(false, TEXT("There is no matched Table."));
		return;
	}

	WRItemBase* AddedItem = nullptr;

	// EWRItemListArray ListArray = ConvertMainTypeToArrayType(FoundItemTable->MainType);
	switch (FoundItemTable->MainType)
	{
	case EWRItemMainType::Weapon:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::WeaponArray)], new WRWeaponItem(InISN, InTID));

		// for Tutorial
		WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::DropItem, InTID);
		if (AddedItem->IsBodyArmor())
		{
			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::DropMain, InTID);

		}
		else if (AddedItem->IsShield())
		{
			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::DropShield, InTID);
		}
		else if (AddedItem->IsHook())
		{
			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::DropHook, InTID);
		}
	}
	break;

	case EWRItemMainType::Armor:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::ArmorArray)], new WRWeaponItem(InISN, InTID));
		break;
	}

	case EWRItemMainType::Temporary:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::TemporaryArray)], new WRTemporaryItem(InISN, InTID));

		IMMessageSender::RequestEquipItem(AddedItem->GetISN());

		// for Tutorial
		WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::DropItem, InTID);
	}
	break;

	case EWRItemMainType::Costume:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::CostumeArray)], new WRCostumeItem(InISN, InTID));		
	}
	break;

	case EWRItemMainType::Material:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::MaterialArray)], new WRMaterialItem(InISN, InTID, InCount));
	}
	break;

	case EWRItemMainType::Interior:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::InteriorArray)], new WRInteriorItem(InISN, InTID, InCount));
	}
	break;

	case EWRItemMainType::Housing:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::HousingArray)], new WRHousingItem(InISN, InTID, InCount));
	}
	break;

	case EWRItemMainType::Emote:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::EmoteArray)], new WREmoteItem(InISN, InTID, InCount));
	}
	break;

	case EWRItemMainType::Placement:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::PlacementArray)], new WRPlacementItem(InISN, InTID, InCount));
	}
	break;

	case EWRItemMainType::Vehicle:
	{
		AddedItem = AddItemToList(EachItemList[uint64(EWRItemListArray::VehicleArray)], new WRVehicleItem(InISN, InTID));
	}
	break;

	default:
	{
		ensureMsgf(false, TEXT("Not defined Item Type entered"));
	}
	break;
	}
}

void WRItemManager::UpdateItemQuantity(const ISN_t IN InISN, const ITEM_COUNT_t IN InQuantity)
{
	WRItemBase* FoundConsumeItem = FindItem(InISN);

	if (FoundConsumeItem == nullptr)
		return;

	if (FoundConsumeItem->IsConsume() == false)
		return;

	WRConsumeItem* CastedConsumeItem = static_cast<WRConsumeItem*>(FoundConsumeItem);
	if (CastedConsumeItem)
	{
		CastedConsumeItem->SetQuantity(InQuantity);
	}
}

void WRItemManager::DeleteItem(const ISN_t IN InISN)
{
	for (uint8 i = (uint8)EWRItemListArray::WeaponArray; i < (uint8)EWRItemListArray::MAX_ARRAY_COUNT; ++i)
	{
		for (auto Node = EachItemList[i].GetHead(); Node != nullptr; Node = Node->GetNextNode())
		{
			WRItemBase* ItemToDelete = Node->GetValue();
			if (ItemToDelete && (ItemToDelete->GetISN() == InISN))
			{
				EachItemList[i].RemoveNode(Node);
				if (ItemToDelete)
				{
					delete ItemToDelete;
					ItemToDelete = nullptr;
					return;
				}
			}
		}
	}
}

WRItemBase* WRItemManager::FindItem(const ISN_t IN InISNtoFindItem)
{
	TMap<EWRItemMainType, TDoubleLinkedList<WRItemBase*>*>::TIterator TypeIter = TypeDevidedItemLists.CreateIterator();
	for (; TypeIter; ++TypeIter)
	{
		TDoubleLinkedList<WRItemBase*>* TypeList = TypeIter.Value();
		if (TypeList)
		{
			WRItemBase* FoundItem = FindItemInListByISN((*TypeList), InISNtoFindItem);
			if (FoundItem != nullptr)
				return FoundItem;
		}
	}

	return nullptr;
}

WREquipItemBase* WRItemManager::FindItemInEquippedList(const ISN_t IN InISN)
{
	for (auto& EachItem : EquippedItemList)
	{
		// 장착 할 수 있는 SubType 의 갯수는 무조건 하나
		WREquipItemBase* TempItem = EachItem.Value;
		if (TempItem->GetISN() == InISN)
		{
			return TempItem;
		}
	}
	return nullptr;
}

//WREquipItemBase* WRItemManager::FindItemInEquippedList(const EWRItemMainType IN InItemMainTypeToFind, const EWRItemSubType IN InItemSubTypeToFind)
//{
//	TMap<EWRItemSubType, WREquipItemBase*> FoundList = EquippedItemList.FindRef(InItemMainTypeToFind);
//
//	for (auto& Node : FoundList)
//	{
//		// 장착 할 수 있는 SubType 의 갯수는 무조건 하나
//		WREquipItemBase* TempItem = Node.Value;
//
//		if (TempItem->GetSubType() == InItemSubTypeToFind)
//		{
//			return TempItem;
//		}
//	}
//
//	return nullptr;
//}

WREquipItemBase* WRItemManager::FindItemInEquippedList(const EWRItemEquippableSlot IN InEquippableSlot)
{
	WREquipItemBase* FoundItem = EquippedItemList.FindRef(InEquippableSlot);
	return FoundItem;
}

// WREquipItemBase* WRItemManager::FindItemInEquippedList(const EWRItemMainType IN InItemMainTypeToFind, const EWRItemEquippableSlot IN InItemSubTypeToFind)
// {
// 	TMap<EWRItemSubType, WREquipItemBase*> FoundList = EquippedItemList.FindRef(InItemMainTypeToFind);
// 
// 	for (auto& Node : FoundList)
// 	{
// 		// 장착 할 수 있는 SubType 의 갯수는 무조건 하나
// 		if(TempItem->IsSubWeapon())
// 
// 		if (TempItem->GetSubType() == InItemSubTypeToFind)
// 		{
// 			return TempItem;
// 		}
// 	}
// 
// 	return nullptr;
// }

void WRItemManager::ClearItems(const EWRItemMainType IN InItemTypeToClear)
{
	switch (InItemTypeToClear)
	{
	case EWRItemMainType::Weapon:
	{
	}
	break;

#if 0
	case EWRItemMainType::SubWeapon:
	{
	}
	break;
#endif
	case EWRItemMainType::Temporary:
	{
		
	}
	break;
	}

	TDoubleLinkedList<WRItemBase*>* FoundList = GetItemList(InItemTypeToClear);
	if (FoundList)
	{
		TDoubleLinkedList<WRItemBase*>::TDoubleLinkedListNode* ItemListNode = (*FoundList).GetHead();
		for (; ItemListNode != nullptr; ItemListNode = ItemListNode->GetNextNode())
		{
			WRItemBase* DataToDelete = ItemListNode->GetValue();
			if (DataToDelete != nullptr)
			{
				delete DataToDelete;
				DataToDelete = nullptr;
			}
		}
		FoundList->Empty();
	}
}

void WRItemManager::DeleteAllItems()
{
	for (uint8 i = (uint8)EWRItemListArray::WeaponArray; i < (uint8)EWRItemListArray::MAX_ARRAY_COUNT; ++i)
	{
		TDoubleLinkedList<WRItemBase*>::TDoubleLinkedListNode* ItemListNode = EachItemList[i].GetHead();

		for (; ItemListNode != nullptr; ItemListNode = ItemListNode->GetNextNode())
		{
			WRItemBase* DataToDelete = ItemListNode->GetValue();
			if (DataToDelete != nullptr)
			{
				delete DataToDelete;
				DataToDelete = nullptr;
			}
		}

		EachItemList[i].Empty();
	}
}

void WRItemManager::ClearAllItems()
{
	DeleteAllItems();
	EquippedItemList.Empty();
	TypeDevidedItemLists.Empty();
}

bool WRItemManager::ThrowTemporaryItemAway(const EWRReasonForThrowTemporaryItemAway IN InReason)
{
	WREquipItemBase* EquippedTemporaryItem = FindEquippedTemporaryWeapon();

	if (EquippedTemporaryItem == nullptr)
		return false;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return false;

	if (MyCharacter->GetCurrentFSMType() == EWRFSM::Skill)
		return false;

	UnEquipItem(EquippedTemporaryItem);

	// 버리면서 장착해제 패킷 보냄
	IMMessageSender::RequestUnEquipItem(EquippedTemporaryItem->GetISN());	// 임시장착류는 서버에 UnEquip을 보내면 서버에서 해제 및 inf_DeleteItem_G2U 패킷을 보내준다

	switch (InReason)
	{
	case EWRReasonForThrowTemporaryItemAway::MainWeaponEquip:
	{
		// 메인장비 장착 연출은 밖에서 처리함 + SubWeapon 보이기 추가
		MyCharacter->ChangeWeaponWithoutAnimation(EWRChangeEquipment::SubWeapon);
	}
	break;
	case EWRReasonForThrowTemporaryItemAway::SubWeaponEquip:
	{
		// 서브장비 장착 연출은 밖에서 처리함 + MainWeapon 보이기 추가
		MyCharacter->ChangeWeaponWithoutAnimation(EWRChangeEquipment::MainWeapon);
	}
	break;
	case EWRReasonForThrowTemporaryItemAway::ThrowOutManual:
	{
		// 장비 상태 원래대로 돌림 + 애니메이션
		MyCharacter->RevertToSrcEquippedWeapons(true);
	}
	break;
	case EWRReasonForThrowTemporaryItemAway::Damage:
	{
		// 장비 상태 원래대로 돌림 + 애니메이션 없음
		MyCharacter->RevertToSrcEquippedWeapons(false);
	}
	break;
	case EWRReasonForThrowTemporaryItemAway::Ladder:
	case EWRReasonForThrowTemporaryItemAway::Dance:
	{
		// 장비 상태 원래대로 돌림 + 애니메이션 없음
		MyCharacter->RevertToSrcEquippedWeapons(false);
	}
	break;
	case EWRReasonForThrowTemporaryItemAway::ChangeEquip:
	{
		// 장비 교체 (Non Animation)
	}
	break;
	case EWRReasonForThrowTemporaryItemAway::IOOccupy:
	{
		// 장비 상태 원래대로 돌림 + 애니메이션 없음
		MyCharacter->RevertToSrcEquippedWeapons(false);
	}
	break;
	}

	return true;
}

void WRItemManager::OnEntryItemList(const std::list<stItem>* IN InReceivedItemList, const bool IN InFinished)
{
	OnAddItemReceived(InReceivedItemList);
}

void WRItemManager::EquipItem(const ISN_t IN InISN)
{
	WRItemBase* FoundItem = FindItem(InISN);

	if (FoundItem == nullptr)
	{
		ensureMsgf(false, TEXT("Item could not be find."));
		return;
	}

	if (FoundItem->IsEquipment() == false)
	{
		ensureMsgf(false, TEXT("Equip failed. Tried item is not a equipment."));
		return;
	}

	WREquipItemBase* CastedItem = static_cast<WREquipItemBase*>(FoundItem);
	if (CastedItem)
	{
		EquipItem(CastedItem);
	}
}

void WRItemManager::EquipItem(WREquipItemBase* IN InNewEquipItem)
{
	if (InNewEquipItem == nullptr)
		return;
			
	switch (InNewEquipItem->GetMainType())
	{
	case EWRItemMainType::Weapon:
	{
		// UnEquip
		WREquipItemBase* PrevEquipItem = WRItemManager::Get()->FindItemInEquippedList(InNewEquipItem->GetEquippableSlot());
		if (PrevEquipItem)
		{
			PrevEquipItem->UnEquip();
		}

		// Equip
		InNewEquipItem->Equip();

		EquippedItemList.Emplace(InNewEquipItem->GetEquippableSlot(), InNewEquipItem);
		
		OpenEquipmentChangeUI(PrevEquipItem, InNewEquipItem);

		// Weapon Costume
		EquipWeaponCostume(InNewEquipItem->GetMainType(), InNewEquipItem->GetSubType());

		// [ 2020-3-12 : kyu ] 장착 전 임시장비 버리기 추가
		if (InNewEquipItem->IsSubWeapon())
		{
			ThrowTemporaryItemAway(EWRReasonForThrowTemporaryItemAway::SubWeaponEquip);
		}
		else
		{
			ThrowTemporaryItemAway(EWRReasonForThrowTemporaryItemAway::MainWeaponEquip);
		}		
	}
	break;

	case EWRItemMainType::Armor:
	{
		// UnEquip
		WREquipItemBase* PrevEquipItem = WRItemManager::Get()->FindItemInEquippedList(InNewEquipItem->GetEquippableSlot());
		if (PrevEquipItem)
		{
			PrevEquipItem->UnEquip();
		}

		// Equip
		InNewEquipItem->Equip();

		EquippedItemList.Emplace(InNewEquipItem->GetEquippableSlot(), InNewEquipItem);
	}
	break;

	case EWRItemMainType::Costume:
	{
		// UnEquip
		WREquipItemBase* PrevEquipItem = WRItemManager::Get()->FindItemInEquippedList(InNewEquipItem->GetEquippableSlot());
		if (PrevEquipItem)
		{
			PrevEquipItem->UnEquip();
		}

		// Equip
		InNewEquipItem->Equip();

		EquippedItemList.Emplace(InNewEquipItem->GetEquippableSlot(), InNewEquipItem);

		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter)
			MyCharacter->SetEquippedCostume(InNewEquipItem->GetEquippableSlot(), InNewEquipItem->GetTID());
	}
	break;

	case EWRItemMainType::Temporary:
	{
		// Temp Equip
		InNewEquipItem->Equip();

		EquippedItemList.Emplace(InNewEquipItem->GetEquippableSlot(), InNewEquipItem);

		// 외형변경 애니전에 무기를 숨겨야한다
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter != nullptr)
		{
			MyCharacter->ClearAllEquippedHands();
		}

		// 추가
		switch (InNewEquipItem->GetSubType())
		{
		case EWRItemSubType::Drink:
		{
			WRCheersManager::Get()->OpenCheersUI_Guide();

			if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
				WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);
		}
		break;
		}

		// Weapon Costume
		EquipWeaponCostume(InNewEquipItem->GetMainType(), InNewEquipItem->GetSubType());
	} 
	break; 

	default:
		break;
	}

}

void WRItemManager::UnEquipItem(const ISN_t IN InISN)
{
	WREquipItemBase* FoundItem = FindItemInEquippedList(InISN);

	if (FoundItem)
	{
		UnEquipItem(FoundItem);
	}
}

void WRItemManager::UnEquipItem(WREquipItemBase* IN InTargetItem)
{
	// [ 2020-3-18 : kyu ] 일단 임시 아이템에 대해서만 작성함
	if (InTargetItem == nullptr)
		return;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	WRTableID EquipLookTID = INVALID_TABLE_TID;
	switch (InTargetItem->GetMainType())
	{
	case EWRItemMainType::Weapon:
	{
		FWRWeaponItemTable* WeaponItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, InTargetItem->GetTID());
		if (WeaponItemTable == nullptr)
			return;

		EquipLookTID = WeaponItemTable->EquipmentLookTID;
	}
	break;
	case EWRItemMainType::Temporary:
	{
		FWRTemporaryItemTable* TemporaryItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, InTargetItem->GetTID());
		if (TemporaryItemTable == nullptr)
			return;

		EquipLookTID = TemporaryItemTable->EquipmentLookTID;				
	}
	break;
	}

	if (EquipLookTID != INVALID_TABLE_TID)
	{
		FWREquipmentLookTable* pEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(EquipLookTID);
		if (pEquipLookTable != nullptr)
		{
			UWRActorComponentAttack* AttackComponent = MyCharacter->GetComponent<UWRActorComponentAttack>();
			if (AttackComponent)
			{
				AttackComponent->RemoveSkillInfo(pEquipLookTable->EquipPart);
			}

			// [ 2020-3-18 : kyu ] 임시장비는 버리는 액션이 있어야한다
			switch (InTargetItem->GetMainType())
			{
			case EWRItemMainType::Temporary:
			{
				WRCharacterManager::Get()->SpawnAbandonedThing(pEquipLookTable->StaticMesh, MyCharacter, InTargetItem->GetTID());
				ClearItems(EWRItemMainType::Temporary);
			}
			break;
			}
		}
	}
	
	InTargetItem->UnEquip();

	EquippedItemList.Remove(InTargetItem->GetEquippableSlot());


	// [ 2020-7-24 : kyu ] 추가
	switch (InTargetItem->GetSubType())
	{
	case EWRItemSubType::Drink:
	{
		WRCheersManager::Get()->OnUnEquipBottle();
	}
	break;
	}
}

TDoubleLinkedList<WRItemBase*>* WRItemManager::GetItemList(const EWRItemMainType IN InItemTypeToGetList)
{
	TDoubleLinkedList<WRItemBase*>* TempList = TypeDevidedItemLists.FindRef(InItemTypeToGetList);

	if (TempList == nullptr)
	{
		ensureMsgf(false, TEXT("IMInventoryManager::GetItemList : Undefined itemtype entered."));
		return nullptr;
	}

	return TempList;
}

WREquipItemBase* WRItemManager::FindEquippedMainWeapon()
{
	WREquipItemBase* FoundItem = EquippedItemList.FindRef(EWRItemEquippableSlot::RightWeapon);
	return FoundItem;
}

WREquipItemBase* WRItemManager::FindEquippedSubWeapon()
{
	WREquipItemBase* FoundItem = EquippedItemList.FindRef(EWRItemEquippableSlot::LeftWeapon);
	return FoundItem;
}

WREquipItemBase* WRItemManager::FindEquippedTemporaryWeapon()
{
	WREquipItemBase* FoundItem = EquippedItemList.FindRef(EWRItemEquippableSlot::Temporary);
	return FoundItem;
}

ITEM_COUNT_t WRItemManager::FindItemQuantity(const ISN_t IN InISN)
{
	WRItemBase* FoundItem = FindItem(InISN);
	if (FoundItem == nullptr)
		return 0;

	WRConsumeItem* CastedItem = static_cast<WRConsumeItem*>(FoundItem);
	if (CastedItem == nullptr)
		return 0;

	return CastedItem->GetQuantity();
}

ITEM_COUNT_t WRItemManager::FindItemTotalQuantity(const WRTableID IN InTID)
{
	ITEM_COUNT_t AccumulatedItemQuantity = 0;

	for (auto& Node : TypeDevidedItemLists)
	{
		TDoubleLinkedList<WRItemBase*>* FoundList = Node.Value;

		for (auto SecondNode : *FoundList)
		{
			if (SecondNode && SecondNode->GetTID() == InTID)
			{
				AccumulatedItemQuantity += SecondNode->GetQuantity();
			}
		}
	}

	return AccumulatedItemQuantity;
}

void WRItemManager::OpenEquipmentChangeUI(WREquipItemBase* IN InSourceItem, WREquipItemBase* IN InTargetItem)
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.UIPanel)
		{
			UWRUIPanel_EquipmentChange* EquipmentChangePanel = Cast<UWRUIPanel_EquipmentChange>(InCombinedPanelData.UIPanel);
			if (EquipmentChangePanel)
			{
				// [ 2020-7-15 : kyu ] IsSubWeapon 임시
				if (InTargetItem->IsSubWeapon())
				{
					EquipmentChangePanel->OnLeftHandChanged(InSourceItem, InTargetItem);
				}
				else if (InTargetItem->IsWeapon())
				{
					EquipmentChangePanel->OnRightHandChanged(InSourceItem, InTargetItem);
				}
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::EquipmentChange, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::EquipmentChange, FTransform(FRotator(0.f, 180.f, 0.f), FVector(500, -200, -150), FVector(2.0f, 2.0f, 2.0f)), EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
}

WRTableID WRItemManager::ConvertItemTIDToCharacterWeaponTID(const WRTableID IN InItemTID)
{
	if (InItemTID == INVALID_TABLE_TID)
		return INVALID_TABLE_TID;
	
	FWRItemTableBase* FoundTable = WRTableManager::Get()->FindRowInItemTable(InItemTID);
	if (FoundTable == nullptr)
		return INVALID_TABLE_TID;

	WRTableID FoundCharacterWeaponTID = INVALID_TABLE_TID;

	switch (FoundTable->MainType)
	{
	case EWRItemMainType::Weapon:
	{
		FWRWeaponItemTable* CastedTable = static_cast<FWRWeaponItemTable*>(FoundTable);
		FoundCharacterWeaponTID = CastedTable->CharWeaponTID;
	}
	break;
	case EWRItemMainType::Temporary:
	{
		FWRTemporaryItemTable* CastedTable = static_cast<FWRTemporaryItemTable*>(FoundTable);
		FoundCharacterWeaponTID = CastedTable->CharWeaponTID;
	}
	break;
	}

	return FoundCharacterWeaponTID;
}

void WRItemManager::OnAddItemReceived(const std::list<stItem>* IN InReceivedItemList)
{
	if (InReceivedItemList == nullptr)
		return;

	std::list<stItem>::const_iterator Iter = InReceivedItemList->begin();

	for (; Iter != InReceivedItemList->end(); ++Iter)
	{
		AddItem(Iter->TID, Iter->ISN, Iter->itemCount);
	}
}

void WRItemManager::OnUpdateItemQuantityReceived(const std::list<stItem>* IN InReceivedItemList)
{
	if (InReceivedItemList == nullptr)
		return;

	std::list<stItem>::const_iterator Iter = InReceivedItemList->begin();

	for (; Iter != InReceivedItemList->end(); ++Iter)
	{
		UpdateItemQuantity(Iter->ISN, Iter->itemCount);
	}
}

void WRItemManager::OnDeleteItemReceived(const std::list<stItem>* IN InReceivedItemList)
{
	if (InReceivedItemList == nullptr)
		return;

	std::list<stItem>::const_iterator Iter = InReceivedItemList->begin();

	for (; Iter != InReceivedItemList->end(); ++Iter)
	{
		DeleteItem(Iter->ISN);
	}
}

void WRItemManager::OnEquipItemReceived(struct pt_Game_User_Inventory::Ack_EquipItem_G2U* IN InPacket)
{
	if (InPacket == nullptr)
		return;

	EquipItem(InPacket->ISN);
}

void WRItemManager::OnUnEquipItemReceived(const ISN_t IN InISN)
{
	UnEquipItem(InISN);
}

