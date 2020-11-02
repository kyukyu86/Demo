// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_CircleSelectorElement.generated.h"


UCLASS()
class WR_API UWRUISlot_CircleSelectorElement : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:
	

};