#include "WRItem.h"
#include "Manager/WRItemManager.h"
#include "Manager/WRInventoryManager.h"
#include "Table/WRItemTable.h"
#include "Table/Base/WRTableManager.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Define/WRItemDefine.h"

WRItemBase::WRItemBase(WRTableID IN InTID) :
	SerialNumber(INVALID_ISN),
	TID(INVALID_TABLE_TID),
	Table(nullptr),
	MainType(EWRItemMainType::None),
	SubType(EWRItemSubType::None)
{
	SerialNumber = WRItemManager::Get()->GenerateSerialNumber();
	TID = InTID;

	FWRItemTableBase* ItemTable = WRTableManager::Get()->FindRowInItemTable(TID);
	if (ItemTable != nullptr)
	{
		MainType = ItemTable->MainType;
		SubType = ItemTable->SubType;
		Table = ItemTable;
	}
	else
	{
		ensureMsgf(false, TEXT("Item table must be exist."));
	}

}

WRItemBase::WRItemBase(ISN_t IN InISN, WRTableID IN InTID) :
	SerialNumber(INVALID_ISN),
	TID(INVALID_TABLE_TID),
	Table(nullptr),
	MainType(EWRItemMainType::None),
	SubType(EWRItemSubType::None)
{
	if (InISN == INVALID_ISN)
		SerialNumber = WRItemManager::Get()->GenerateSerialNumber();
	else
		SerialNumber = InISN;

	TID = InTID;
	FWRItemTableBase* ItemTable = WRTableManager::Get()->FindRowInItemTable(TID);
	if (ItemTable != nullptr)
	{
		MainType = ItemTable->MainType;
		SubType = ItemTable->SubType;
		Table = ItemTable;
	}
	else
	{
		ensureMsgf(false, TEXT("Item table must be exist."));
	}
}

// vuvle TODO : 8월빌드까지 임시, Weapon 은 양손에 가능하지만 8월 빌드 까지는 장착 부위에 따라 나눌수 있다.
bool WRItemBase::IsSubWeapon()
{
	if (IsEquipment() == false)
		return false;

	if (Table == nullptr)
		return false;

	FWRWeaponItemTable* WeaponTable = static_cast<FWRWeaponItemTable*>(Table);
	if (WeaponTable == nullptr)
		return false;

	if (WeaponTable->EquippableSlot == EWRItemEquippableSlot::LeftWeapon)
		return true;

	return false;
}

WREquipItemBase::WREquipItemBase(WRTableID IN InTID) : 
	WRItemBase(InTID),
	EquipState(EWRItemEquipState::UnEquipped), 
	EquippableSlot(EWRItemEquippableSlot::None),
	RegisteredSlot(EWRRegistrationSlotType::None)
{
	if (Table != nullptr)
	{
		FWREquipmentItemTableBase* EquipmentItemTable = static_cast<FWREquipmentItemTableBase*>(Table);

		if (EquipmentItemTable)
		{
			EquippableSlot = EquipmentItemTable->EquippableSlot;
		} 
	}
	else
	{
		ensureMsgf(false, TEXT("Item table must be exist."));
	}
		
}

WREquipItemBase::WREquipItemBase(ISN_t IN InISN, WRTableID IN InTID) :
	WRItemBase(InISN, InTID),
	EquipState(EWRItemEquipState::UnEquipped),
	EquippableSlot(EWRItemEquippableSlot::None),
	RegisteredSlot(EWRRegistrationSlotType::None)
{
	if (Table != nullptr)
	{
		FWREquipmentItemTableBase* EquipmentItemTable = static_cast<FWREquipmentItemTableBase*>(Table);

		if (EquipmentItemTable)
		{
			EquippableSlot = EquipmentItemTable->EquippableSlot;
		}
	}
	else
	{
		ensureMsgf(false, TEXT("Item table must be exist."));
	}
}

WRConsumeItem::WRConsumeItem(WRTableID IN InTID) : WRItemBase(InTID), Quantity(0) { }
WRConsumeItem::WRConsumeItem(ISN_t IN InISN, WRTableID IN InTID) : WRItemBase(InISN, InTID), Quantity(0) { }
WRConsumeItem::WRConsumeItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity) : WRItemBase(InISN, InTID), Quantity(InQuantity) { }

// Weapon
WRWeaponItem::WRWeaponItem(WRTableID IN InTID) : WREquipItemBase(InTID) { }
WRWeaponItem::WRWeaponItem(ISN_t IN InISN, WRTableID IN InTID) : WREquipItemBase(InISN, InTID) { }


// Sub Weapon 임시
WRSubWeaponItem::WRSubWeaponItem(WRTableID IN InTID) : WREquipItemBase(InTID) { }
WRSubWeaponItem::WRSubWeaponItem(ISN_t IN InISN, WRTableID IN InTID) : WREquipItemBase(InISN, InTID) { }

// Armor
WRArmorItem::WRArmorItem(WRTableID IN InTID) : WREquipItemBase(InTID) { }
WRArmorItem::WRArmorItem(ISN_t IN InISN, WRTableID IN InTID) : WREquipItemBase(InISN, InTID) { }

// Costume
WRCostumeItem::WRCostumeItem(WRTableID IN InTID) : WREquipItemBase(InTID) { }
WRCostumeItem::WRCostumeItem(ISN_t IN InISN, WRTableID IN InTID) : WREquipItemBase(InISN, InTID) { }

// Temporary
WRTemporaryItem::WRTemporaryItem(WRTableID IN InTID) : WREquipItemBase(InTID) { }
WRTemporaryItem::WRTemporaryItem(ISN_t IN InISN, WRTableID IN InTID) : WREquipItemBase(InISN, InTID) { }

// Vehicle
WRVehicleItem::WRVehicleItem(WRTableID IN InTID) : WRItemBase(InTID) { }
WRVehicleItem::WRVehicleItem(ISN_t IN InISN, WRTableID IN InTID) : WRItemBase(InISN, InTID) { }

// Material
WRMaterialItem::WRMaterialItem(WRTableID IN InTID) : WRConsumeItem(InTID) { }
WRMaterialItem::WRMaterialItem(ISN_t IN InISN, WRTableID IN InTID) : WRConsumeItem(InISN, InTID) { }
WRMaterialItem::WRMaterialItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity) : WRConsumeItem(InISN, InTID, InQuantity) { }

// Interior
WRInteriorItem::WRInteriorItem(WRTableID IN InTID) : WRConsumeItem(InTID) { }
WRInteriorItem::WRInteriorItem(ISN_t IN InISN, WRTableID IN InTID) : WRConsumeItem(InISN, InTID) { }
WRInteriorItem::WRInteriorItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity) : WRConsumeItem(InISN, InTID, InQuantity) { }

// Housing
WRHousingItem::WRHousingItem(WRTableID IN InTID) : WRConsumeItem(InTID) { }
WRHousingItem::WRHousingItem(ISN_t IN InISN, WRTableID IN InTID) : WRConsumeItem(InISN, InTID) { }
WRHousingItem::WRHousingItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity) : WRConsumeItem(InISN, InTID, InQuantity) { }

// Emote
WREmoteItem::WREmoteItem(WRTableID IN InTID) : WRConsumeItem(InTID) { }
WREmoteItem::WREmoteItem(ISN_t IN InISN, WRTableID IN InTID) : WRConsumeItem(InISN, InTID) { }
WREmoteItem::WREmoteItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity) : WRConsumeItem(InISN, InTID, InQuantity) { }

// Placement
WRPlacementItem::WRPlacementItem(WRTableID IN InTID) : WRConsumeItem(InTID) { }
WRPlacementItem::WRPlacementItem(ISN_t IN InISN, WRTableID IN InTID) : WRConsumeItem(InISN, InTID) { }
WRPlacementItem::WRPlacementItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity) : WRConsumeItem(InISN, InTID, InQuantity) { }