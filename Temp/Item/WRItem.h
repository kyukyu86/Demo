// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#pragma once

#include "Define/WRTypeDefine.h"
#include "Define/WRItemDefine.h"
#include "../Network/Share/Define/Define_Type.h"
#include "../Network/Share/Define/Define_BluePrint.h"

//typedef uint64 ISN_t;

class WR_API WRItemBase
{
public:
	WRItemBase() {}
	WRItemBase(WRTableID IN InTID);
	WRItemBase(ISN_t IN InISN, WRTableID IN InTID);
	virtual ~WRItemBase() {}

protected:
	ISN_t SerialNumber;
	WRTableID TID;
	struct FWRItemTableBase* Table;
	EWRItemMainType MainType;
	EWRItemSubType SubType;

public:
	// 상위 Category 기준 분류
	FORCEINLINE bool IsEquipment() const { return (EWRItemMainType)ITEM_MAINTYPE_EQUIPMENT_BEGIN <= MainType && MainType < EWRItemMainType::ITEM_MAINTYPE_EQUIPMENT_END; }
	FORCEINLINE bool IsConsume() const { return (EWRItemMainType)ITEM_MAINTYPE_CONSUME_BEGIN <= MainType && MainType < EWRItemMainType::ITEM_MAINTYPE_CONSUME_END; }

	// MainType 기준 분류
	FORCEINLINE bool IsWeapon()		const { return MainType == EWRItemMainType::Weapon ? true : false; }
	FORCEINLINE bool IsArmor()		const { return MainType == EWRItemMainType::Armor ? true : false; }
	FORCEINLINE bool IsCostume()	const { return MainType == EWRItemMainType::Costume ? true : false; }
	FORCEINLINE bool IsMaterial()	const { return MainType == EWRItemMainType::Material ? true : false; }
	FORCEINLINE bool IsInterior()	const { return MainType == EWRItemMainType::Interior ? true : false; }
	FORCEINLINE bool IsHousing()	const { return MainType == EWRItemMainType::Housing ? true : false; }
	FORCEINLINE bool IsEmote()		const { return MainType == EWRItemMainType::Emote ? true : false; }
	FORCEINLINE bool IsPlacement()	const { return MainType == EWRItemMainType::Placement ? true : false; }
	FORCEINLINE bool IsVehicle()	const { return MainType == EWRItemMainType::Vehicle ? true : false; }
	FORCEINLINE bool IsTemporary()	const { return MainType == EWRItemMainType::Temporary ? true : false; }

	// SubType 기준 분류
	FORCEINLINE bool IsSword()		const { return (MainType == EWRItemMainType::Weapon && SubType == EWRItemSubType::Weapon) ? true : false; }
	FORCEINLINE	bool IsBodyArmor()	const { return (MainType == EWRItemMainType::Armor && SubType == EWRItemSubType::Armor) ? true : false; }
	FORCEINLINE bool IsHook()		const { return (MainType == EWRItemMainType::Weapon && SubType == EWRItemSubType::Hook) ? true : false; }
	FORCEINLINE bool IsShield()		const { return (MainType == EWRItemMainType::Weapon && SubType == EWRItemSubType::Shield) ? true : false; }
	FORCEINLINE bool IsTorch()		const { return (MainType == EWRItemMainType::Weapon && SubType == EWRItemSubType::Torch) ? true : false; }
	FORCEINLINE bool IsTop()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Top) ? true : false; }
	FORCEINLINE bool IsBottom()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Bottom) ? true : false; }
	FORCEINLINE bool IsShoes()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Shoes) ? true : false; }
	FORCEINLINE bool IsHat()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Hat) ? true : false; }
	FORCEINLINE bool IsGloves()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Gloves) ? true : false; }
	FORCEINLINE bool IsGlasses()	const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Glasses) ? true : false; }
	FORCEINLINE bool IsNeck()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Neck) ? true : false; }
	FORCEINLINE bool IsMask()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Mask) ? true : false; }
	FORCEINLINE bool IsBack()		const { return (MainType == EWRItemMainType::Costume && SubType == EWRItemSubType::Back) ? true : false; }
	FORCEINLINE bool IsRecycling()	const { return (MainType == EWRItemMainType::Material && SubType == EWRItemSubType::Recycling) ? true : false; }
	//FORCEINLINE bool IsInterior()	const { return (MainType == EWRItemMainType::Interior && SubType == EWRItemSubType::Interior) ? true : false; }
	FORCEINLINE bool IsCar()		const { return (MainType == EWRItemMainType::Vehicle && SubType == EWRItemSubType::Car) ? true : false; }
	FORCEINLINE bool IsEmoticon()	const { return (MainType == EWRItemMainType::Emote && SubType == EWRItemSubType::Emoticon) ? true : false; }
	FORCEINLINE bool IsMotion()		const { return (MainType == EWRItemMainType::Emote && SubType == EWRItemSubType::Motion) ? true : false; }
	FORCEINLINE bool IsSocialTools()	const { return (MainType == EWRItemMainType::Placement && SubType == EWRItemSubType::SocialTools) ? true : false; }

	FORCEINLINE virtual bool IsEquipped() const { return false; }
	FORCEINLINE virtual bool IsEquipped(const EWRItemEquippableSlot IN InSlotToCheckEquipped) const { return false; }

	FORCEINLINE ISN_t		GetISN() const { return SerialNumber; }
	FORCEINLINE WRTableID	GetTID() const { return TID; }
	FORCEINLINE EWRItemMainType GetMainType() { return MainType; }
	FORCEINLINE EWRItemSubType	GetSubType() { return SubType; }
	FORCEINLINE FWRItemTableBase* GetTable() const { if(Table == nullptr) ensureMsgf(false, TEXT("The table of this item has not been set.")); return Table; }
	FORCEINLINE virtual ITEM_COUNT_t GetQuantity() const { return 1; }

	// vuvle TODO : 8월빌드까지 임시, Weapon 은 양손에 가능하지만 8월 빌드 까지는 장착 부위에 따라 나눌수 있다.
	bool IsSubWeapon();
};

class WR_API WREquipItemBase : public WRItemBase
{
public:
	WREquipItemBase() {}
	WREquipItemBase(WRTableID IN InTID);
	WREquipItemBase(ISN_t IN InISN, WRTableID IN InTID);
	virtual ~WREquipItemBase() {}

protected:
	EWRItemEquipState EquipState;
	EWRItemEquippableSlot EquippableSlot;
	EWRRegistrationSlotType RegisteredSlot;

public:
	FORCEINLINE virtual bool IsEquipped() const final { return EquipState == EWRItemEquipState::Equipped ? true : false; }
	FORCEINLINE virtual bool IsEquipped(const EWRItemEquippableSlot IN InSlotToCheckEquipped) const final { return (IsEquipped() && EquippableSlot == EWRItemEquippableSlot::LeftWeapon) ? true : false; }
	FORCEINLINE void Equip() { EquipState = EWRItemEquipState::Equipped; }
	FORCEINLINE void UnEquip() { EquipState = EWRItemEquipState::UnEquipped; }
	FORCEINLINE bool IsEquippedInLeftWeapon() const { return (IsEquipped() && EquippableSlot == EWRItemEquippableSlot::LeftWeapon) ? true : false; }		// vuvle TODO : 8 월까지 유효한 함수.(모든 무기는 양손장착 가능하지만 8월까지는 왼손가능 오른손 가능 무기가 존재)
	FORCEINLINE bool IsEquippedInRightWeapon() const { return (IsEquipped() && EquippableSlot == EWRItemEquippableSlot::RightWeapon) ? true : false; }	// vuvle TODO : 8 월까지 유효한 함수.(모든 무기는 양손장착 가능하지만 8월까지는 왼손가능 오른손 가능 무기가 존재)
	FORCEINLINE bool IsEquippedInTemporary() const { return (IsEquipped() && EquippableSlot == EWRItemEquippableSlot::Temporary) ? true : false; }	// vuvle TODO : 8 월까지 유효한 함수.(모든 무기는 양손장착 가능하지만 8월까지는 왼손가능 오른손 가능 무기가 존재)
	FORCEINLINE EWRItemEquippableSlot GetEquippableSlot() { return EquippableSlot; }
};

class WR_API WRConsumeItem : public WRItemBase
{
public:
	WRConsumeItem() {}
	WRConsumeItem(WRTableID IN InTID);
	WRConsumeItem(ISN_t IN InISN, WRTableID IN InTID);
	WRConsumeItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity);
	virtual ~WRConsumeItem() {}

protected:
	ITEM_COUNT_t Quantity;

public:
	FORCEINLINE virtual ITEM_COUNT_t GetQuantity() const { return Quantity; }
	FORCEINLINE void SetQuantity(const ITEM_COUNT_t IN InQuantity) { Quantity = InQuantity; }
};

class WR_API WRMaterialItem : public WRConsumeItem
{
public:
	WRMaterialItem() {}
	WRMaterialItem(WRTableID IN InTID);
	WRMaterialItem(ISN_t IN InISN, WRTableID IN InTID);
	WRMaterialItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity);
	virtual ~WRMaterialItem() {}

};

class WR_API WRInteriorItem : public WRConsumeItem
{
public:
	WRInteriorItem() {}
	WRInteriorItem(WRTableID IN InTID);
	WRInteriorItem(ISN_t IN InISN, WRTableID IN InTID);
	WRInteriorItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity);
	virtual ~WRInteriorItem() {}

};

class WR_API WRHousingItem : public WRConsumeItem
{
public:
	WRHousingItem() {}
	WRHousingItem(WRTableID IN InTID);
	WRHousingItem(ISN_t IN InISN, WRTableID IN InTID);
	WRHousingItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity);
	virtual ~WRHousingItem() {}

};

class WR_API WREmoteItem : public WRConsumeItem
{
public:
	WREmoteItem() {}
	WREmoteItem(WRTableID IN InTID);
	WREmoteItem(ISN_t IN InISN, WRTableID IN InTID);
	WREmoteItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity);
	virtual ~WREmoteItem() {}

};

class WR_API WRPlacementItem : public WRConsumeItem
{
public:
	WRPlacementItem() {}
	WRPlacementItem(WRTableID IN InTID);
	WRPlacementItem(ISN_t IN InISN, WRTableID IN InTID);
	WRPlacementItem(ISN_t IN InISN, WRTableID IN InTID, ITEM_COUNT_t IN InQuantity);
	virtual ~WRPlacementItem() {}

};

class WR_API WRWeaponItem : public WREquipItemBase
{
public:
	WRWeaponItem() {}
	WRWeaponItem(WRTableID IN InTID);
	WRWeaponItem(ISN_t IN InISN, WRTableID IN InTID);
	virtual ~WRWeaponItem() {}

};

class WR_API WRArmorItem : public WREquipItemBase
{
public:
	WRArmorItem() {}
	WRArmorItem(WRTableID IN InTID);
	WRArmorItem(ISN_t IN InISN, WRTableID IN InTID);
	virtual ~WRArmorItem() {}

};

class WR_API WRCostumeItem : public WREquipItemBase
{
public:
	WRCostumeItem() {}
	WRCostumeItem(WRTableID IN InTID);
	WRCostumeItem(ISN_t IN InISN, WRTableID IN InTID);	
	virtual ~WRCostumeItem() {}

};

class WR_API WRVehicleItem : public WRItemBase
{
public:
	WRVehicleItem() {}
	WRVehicleItem(WRTableID IN InTID);
	WRVehicleItem(ISN_t IN InISN, WRTableID IN InTID);
	virtual ~WRVehicleItem() {}
};

class WR_API WRTemporaryItem : public WREquipItemBase
{
public:
	WRTemporaryItem() {}
	WRTemporaryItem(WRTableID IN InTID);
	WRTemporaryItem(ISN_t IN InISN, WRTableID IN InTID);
	virtual ~WRTemporaryItem() {}
};

class WR_API WRSubWeaponItem : public WREquipItemBase
{
public:
	WRSubWeaponItem() {}
	WRSubWeaponItem(WRTableID IN InTID);
	WRSubWeaponItem(ISN_t IN InISN, WRTableID IN InTID);
	virtual ~WRSubWeaponItem() {}

};
