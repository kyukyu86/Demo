// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Foliage/Public/FoliageInstancedStaticMeshComponent.h"
#include "WRFoliageInstancedStaticMeshComponent.generated.h"

/**
 *
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRFoliageInstancedStaticMeshComponent : public UFoliageInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foliage)
		int32 FoliageTID = 0;

	FORCEINLINE const int32& GetTID() { return FoliageTID; }
};
