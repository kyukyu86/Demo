// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_CircleSelectorElement.h"
#include "WRUISlot_Social_CircleSelectorElement.generated.h"


UCLASS()
class WR_API UWRUISlot_Social_CircleSelectorElement : public UWRUISlot_CircleSelectorElement
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:
	

};