#pragma once

#include "Base/WRTableRow.h"
//#include "../Enum/EWRCharacter.h"
#include "WRCustomizingCategoryTable.generated.h"

#define MAX_DATA_SET_COUNT 12

USTRUCT(BlueprintType)
struct FWRCustomizingCategoryTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CharData_CustomizingCategory"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CustomizingCategoryTable)
		TArray<int32> PartEquipmentLookTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CustomizingCategoryTable)
		TArray<int32> LowerCategoryTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CustomizingCategoryTable)
		TArray<FString> PartName;
	UPROPERTY(EditAnywhere, Category = CustomizingCategoryTable)
		TArray<float> Scale;
	UPROPERTY(EditAnywhere, Category = CustomizingCategoryTable)
		TArray<float> PositionZ;
	UPROPERTY(EditAnywhere, Category = CustomizingCategoryTable)
		TArray<int32> ItemTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CustomizingCategoryTable)
		float RelativeLocationZ;

	struct FCustomizningDataSet
	{
		int32 PartEquipmentLookTID;
		int32 LowerCategoryTID;
		FString PartName;
		float Scale;
		float PositionZ;
		int32 ItemTID;
	};

	TArray<FCustomizningDataSet> DataSet;

	virtual void GenerateCustomData()
	{
		if (!(PartEquipmentLookTID.Num() == MAX_DATA_SET_COUNT &&
			LowerCategoryTID.Num() == MAX_DATA_SET_COUNT &&
			PartName.Num() == MAX_DATA_SET_COUNT &&
			Scale.Num() == MAX_DATA_SET_COUNT &&
			PositionZ.Num() == MAX_DATA_SET_COUNT &&
			ItemTID.Num() == MAX_DATA_SET_COUNT))
		{
			ensureMsgf(false, TEXT("GenerateCustomeData Failed"));
			return;
		}

		for (int32 i = 0; i < MAX_DATA_SET_COUNT; ++i)
		{
			FCustomizningDataSet TempDataSet;
			TempDataSet.PartEquipmentLookTID = PartEquipmentLookTID[i];
			TempDataSet.LowerCategoryTID = LowerCategoryTID[i];
			TempDataSet.PartName = PartName[i];
			TempDataSet.Scale = Scale[i];
			TempDataSet.PositionZ = PositionZ[i];
			TempDataSet.ItemTID = ItemTID[i];

			DataSet.Add(TempDataSet);
		}
	}
};