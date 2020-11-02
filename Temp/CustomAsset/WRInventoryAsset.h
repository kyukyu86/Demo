// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Enum/EWRUIEnum.h"
#include "Define/WRTypeDefine.h"
#include "WRInventoryAsset.generated.h"


USTRUCT(BlueprintType)
struct FWRInventorySound
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName SFX;
};

/**
 * 
 */
UCLASS()
class WR_API UWRInventoryAsset : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TMap<EWRInventorySoundType, FWRInventorySound> InventorySoundMap;

public:
	FName GetSound(const EWRInventorySoundType IN InType);

};
