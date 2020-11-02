// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MediaSource.h"
#include "Materials/MaterialInstance.h"
#include "Curves/CurveFloat.h"
#include "Sound/SoundCue.h"
#include "WRPlay360VideoAsset.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRPlay360VideoAsset : public UObject
{
	GENERATED_BODY()

public:
	
	//====================================================================================
	
	UPROPERTY(EditAnywhere, Category = "Play")
	TAssetPtr<UMediaSource>	MediaSource;

	UPROPERTY(EditAnywhere, Category = "Play")
	TAssetPtr<UMaterialInstance>	PlayMovieMaterial;

	UPROPERTY(EditAnywhere, Category = "Play", meta = (ClampMin = "0.01", UIMin = "0.01", UIMax = "10.0"))
	float SphereMeshScale = 1.0f;
	
	//====================================================================================
	
	UPROPERTY(EditAnywhere, Category = "Direction")
	TAssetPtr<UMaterialInstance>	CreateSphereMaterial;

	UPROPERTY(EditAnywhere, Category = "Direction")
	TAssetPtr<USoundCue>	CreateDirectionSound;

	UPROPERTY(EditAnywhere, Category = "Direction")
	TAssetPtr<UCurveFloat>	CreateSphereCurve;

	UPROPERTY(EditAnywhere, Category = "Direction")
	TAssetPtr<UCurveFloat>	LoadingCurve;

	//====================================================================================
};
