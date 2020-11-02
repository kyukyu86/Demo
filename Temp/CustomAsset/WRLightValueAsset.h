// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Materials/MaterialInterface.h"
#include "WRLightValueAsset.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRLightValueAsset : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Light Kind")
	bool bSpotLight = false;

	UPROPERTY(EditAnywhere, Category = "Spot Light Setting", meta = (EditCondition = "bSpotLight", UIMin = "1.0", UIMax = "80.0"))
	float InnerConeAngle;

	UPROPERTY(EditAnywhere, Category = "Spot Light Setting", meta = (EditCondition = "bSpotLight", UIMin = "1.0", UIMax = "80.0"))
	float OuterConeAngle = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Spot Light Setting", meta = (EditCondition = "bSpotLight"))
	float LightShaftConeAngle;

	
	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
	float Intensity = 8.0f;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		float AttenuationRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		float SourceRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		bool bUseInverseSquaredFalloff = false;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		float LightFalloffExponent = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		bool bUseTemperature = true;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		float Temperature = 6000.0f;

	UPROPERTY(EditAnywhere, Category = "Light Setting"/*, meta = (EditCondition = "bEdit")*/)
		TAssetPtr<UMaterialInterface> LightFunctionMaterial;
};
