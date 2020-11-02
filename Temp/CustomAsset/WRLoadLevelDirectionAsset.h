// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Color.h"
#include "Curves/CurveFloat.h"
#include "WRLoadLevelDirectionAsset.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRLoadLevelDirectionAsset : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Global")
	float MaxDistance = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Global")
	float Brightness = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Grid")
	float GridWidth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Grid")
	float GridThickness = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Grid")
	TAssetPtr<UCurveFloat>	GridFlow;

	UPROPERTY(EditAnywhere, Category = "Grid")
	float GridBoundThickness = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Grid")
	FLinearColor GridColor = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, Category = "Grid")
	FLinearColor GridBoundColor = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, Category = "Scene")
	TAssetPtr<UCurveFloat>	SceneFlow;

	UPROPERTY(EditAnywhere, Category = "Scene")
	float SceneBoundThickness = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Scene")
	FLinearColor SceneBoundColor = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TAssetPtr<class USoundCue>	Sound;
};
