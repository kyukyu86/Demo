// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DMMontageTable.generated.h"

#define DEF_MONTAGE_TABLE_PATH TEXT("/Game/Data/Battle/MontageTable.MontageTable")

USTRUCT(BlueprintType)
struct FDMMontageTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Montage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SectionName = "";
};
