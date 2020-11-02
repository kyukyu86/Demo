#pragma once

#include "Base/WRTableRow.h"
#include "../Enum/EWRCharacter.h"
#include "WRCharacterPresetTable.generated.h"

UENUM(BlueprintType)
enum class EWRCharacterGender : uint8
{
	Male_A,
	Male_B,
	Male_C,
	Female_A,
	Female_B,
	Female_C,
};

USTRUCT(BlueprintType)
struct FWRCharacterPresetTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CharData_Preset"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		EWRCharacterGender Gender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString MyPresetBlueprint; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString PresetBlueprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 NormalEquipmentLookTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloBodyEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloFaceEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloHairEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloBeardEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloTopEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloBottomEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloOutfitEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloGlassesEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloMaskEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloNeckEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloGlovesEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloShoesEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloBackpackEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 HoloPreviewEquipmentLookTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FVector SpawnPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 SpawnAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)			
		int32 CharTableID;

public:
	TMap<EWREquipPart, int32> PresetDefaultParts;

public:
	virtual void GenerateCustomData() override
	{
		PresetDefaultParts.Add(EWREquipPart::Body, HoloBodyEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Face, HoloFaceEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Hair, HoloHairEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::FacialHair, HoloBeardEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Top, HoloTopEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Bottom, HoloBottomEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Outfit, HoloOutfitEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Glasses, HoloGlassesEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Mask, HoloMaskEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Neckwear, HoloNeckEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Gloves, HoloGlovesEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Shoes, HoloShoesEquipmentLookTID);
		PresetDefaultParts.Add(EWREquipPart::Backpack, HoloBackpackEquipmentLookTID);
	}
};