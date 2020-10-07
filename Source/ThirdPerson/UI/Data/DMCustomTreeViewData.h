// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DMCustomTreeViewData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDPERSON_API UDMCustomTreeViewData : public UObject
{
	GENERATED_BODY()
	
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
		int32 Level = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
		int32 Index = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UDMCustomTreeViewData*> ChildrenDataList;

};
