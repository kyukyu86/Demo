// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "WRVolume.generated.h"

/**
 * 
 */
UCLASS(DisplayName="WR Volume (Do Not Use)")
class WR_API AWRVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVolume")
	bool IsDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVolume")
	float Thickness = 10.0F;
	
};
