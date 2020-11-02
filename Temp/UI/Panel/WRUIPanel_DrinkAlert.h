// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_DrinkAlert.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_DrinkAlert : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float OpenTime = 2.f;

private:
	float fElapsedTime = 0.f;
	
};
