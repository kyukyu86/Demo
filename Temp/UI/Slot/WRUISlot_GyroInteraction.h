// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRUISlot_GyroInteraction.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_GyroInteraction : public UWRUISlot
{
	GENERATED_BODY()
	
public:
	class UImage* Image_Icon;
	class UTextBlock* TextBlock_Description;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetInformation(const FString& IN InIconPath, const FString& IN InDescription);
};
