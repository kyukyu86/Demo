#pragma once

#include "Base/WRTableRow.h"
#include "../Network/Share/Define/Define_BluePrint.h"
#include "Struct/WRPreviewStruct.h"
#include "WRItemTable.generated.h"

#define DEF_ITEM_TID_BAND						1000000

#define DEF_ITEM_TYPE_WEAPON_START				1000000
#define DEF_ITEM_TYPE_ARMOR_START				2000000
#define DEF_ITEM_TYPE_COSTUME_START				3000000
#define DEF_ITEM_TYPE_TEMPORARY_START			4000000
#define DEF_ITEM_TYPE_MATERIAL_START			5000000
#define DEF_ITEM_TYPE_INTERIOR_START			6000000
#define DEF_ITEM_TYPE_VEHICLE_START				7000000
#define DEF_ITEM_TYPE_HOUSING_START				8000000
#define DEF_ITEM_TYPE_EMOTE_START				9000000
#define DEF_ITEM_TYPE_PLACEMENT_START			1000000


USTRUCT(BlueprintType)
struct FWRItemTableBase : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		FName NameLocalKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ItemTable)
		FString IconPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		EWRItemMainType MainType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		EWRItemSubType SubType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ItemTable)
		FString MainDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ItemTable)
		FString SubDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		bool UseSekai;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 PurchasePrice;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 SellingPrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemTable)
		FWRPreviewDataBase Preview;

 	static EWRItemMainType GetItemMainType(WRTableID IN InItemTID) 
	{
		// Equipment
 		if (DEF_ITEM_TYPE_WEAPON_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_WEAPON_START + DEF_ITEM_TID_BAND)				return EWRItemMainType::Weapon;
 		else if (DEF_ITEM_TYPE_ARMOR_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_ARMOR_START + DEF_ITEM_TID_BAND)			return EWRItemMainType::Armor; 		
 		else if (DEF_ITEM_TYPE_TEMPORARY_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_TEMPORARY_START + DEF_ITEM_TID_BAND)	return EWRItemMainType::Temporary;
		// Costume
		else if (DEF_ITEM_TYPE_COSTUME_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_COSTUME_START + DEF_ITEM_TID_BAND)		return EWRItemMainType::Costume;
		// Consume
 		else if (DEF_ITEM_TYPE_MATERIAL_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_MATERIAL_START + DEF_ITEM_TID_BAND)		return EWRItemMainType::Material;
 		else if (DEF_ITEM_TYPE_INTERIOR_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_INTERIOR_START + DEF_ITEM_TID_BAND)		return EWRItemMainType::Interior;
		else if (DEF_ITEM_TYPE_EMOTE_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_EMOTE_START + DEF_ITEM_TID_BAND)			return EWRItemMainType::Emote;
		else if (DEF_ITEM_TYPE_PLACEMENT_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_PLACEMENT_START + DEF_ITEM_TID_BAND)	return EWRItemMainType::Placement;
		// Housing
		else if (DEF_ITEM_TYPE_HOUSING_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_HOUSING_START + DEF_ITEM_TID_BAND)		return EWRItemMainType::Housing;
		// Vehicle
		else if (DEF_ITEM_TYPE_VEHICLE_START <= InItemTID && InItemTID < DEF_ITEM_TYPE_VEHICLE_START + DEF_ITEM_TID_BAND)		return EWRItemMainType::Vehicle;
 		else return EWRItemMainType::None;
	}
};

//====================================================================================
//	Equipment
//====================================================================================

USTRUCT(BlueprintType)
struct FWREquipmentItemTableBase : public FWRItemTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		EWRItemEquippableSlot EquippableSlot;
};

USTRUCT(BlueprintType)
struct FWRWeaponItemTable : public FWREquipmentItemTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Equipment_Weapon"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 CharWeaponTID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 EquipmentLookTID;
};

USTRUCT(BlueprintType)
struct FWRArmorItemTable : public FWREquipmentItemTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Equipment_Armor"); }
};

USTRUCT(BlueprintType)
struct FWRCostumeItemTable : public FWREquipmentItemTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Equipment_Costume"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 EquipmentLookTID;
};

USTRUCT(BlueprintType)
struct FWRTemporaryItemTable : public FWREquipmentItemTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Equipment_Temporary"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 EquipmentLookTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 CharWeaponTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		FString DropSound;
};

//====================================================================================
//	Vehicle Table
//====================================================================================

USTRUCT(BlueprintType)
struct FWRVehicleItemTable : public FWRItemTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Vehicle_Vehicle"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 VehicleTID;
};

//====================================================================================
//	Consume
//====================================================================================

USTRUCT(BlueprintType)
struct FWRConsumeItemBase : public FWRItemTableBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 StackCount;
};

USTRUCT(BlueprintType)
struct FWRMaterialItemTable : public FWRConsumeItemBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Consume_Material"); }
};

USTRUCT(BlueprintType)
struct FWRInteriorItemTable : public FWRConsumeItemBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Consume_Interior"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 ObjectTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		int32 ObjectWeight;
};

USTRUCT(BlueprintType)
struct FWRHousingItemTable : public FWRConsumeItemBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Consume_Housing"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemTable)
		TArray<int32> HouseListTIDs;
};

USTRUCT(BlueprintType)
struct FWREmoteItemTable : public FWRConsumeItemBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Consume_Emote"); }

};

USTRUCT(BlueprintType)
struct FWRPlacementItemTable : public FWRConsumeItemBase
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("ItemData_Consume_Placement"); }

};