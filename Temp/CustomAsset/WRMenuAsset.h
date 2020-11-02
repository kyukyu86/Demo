// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Enum/EWRUIEnum.h"
#include "Define/WRTypeDefine.h"
#include "WRMenuAsset.generated.h"


USTRUCT(BlueprintType)
struct FWRMenuForm
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString MainTitle;
	UPROPERTY(EditAnywhere)
	FString ListTitle;
	UPROPERTY(EditAnywhere)
	int32 VoiceSubCommandTID;
};

/**
 * 
 */
UCLASS()
class WR_API UWRMenuAsset : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "MainTitle")
		TMap<EWRMenuCategory, FWRMenuForm> Title;
};
