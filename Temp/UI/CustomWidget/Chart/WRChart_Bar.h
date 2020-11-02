// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/CustomWidget/Chart/Base/WRChartBase.h"
#include "WRChart_Bar.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRChart_Bar : public UWRChartBase
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
