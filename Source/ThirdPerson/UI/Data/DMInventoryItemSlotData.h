// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DMInventoryItemSlotData.generated.h"


#define DEF_INVENTORY_SLOT_INVALID_ID -1

USTRUCT(BlueprintType)
struct FDMInventoryItemSlotData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ID = DEF_INVENTORY_SLOT_INVALID_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString IconPath;
};

UCLASS(BlueprintType)
class THIRDPERSON_API UDMInventoryItemSlotDataObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDMInventoryItemSlotData Data;

	bool bIsEmpty = true;
};
