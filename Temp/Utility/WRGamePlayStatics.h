// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Table/WRMapTable.h"
#include "Define/WRTypeDefine.h"
#include "WRGamePlayStatics.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRGamePlayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
public:

// 	UFUNCTION(BlueprintCallable, Category = "WRLoadLevel", meta = (WorldContext = "WorldContextObject"))
// 	static bool GoToMap(EWRMapType InMapType, int32 InTID);
};
