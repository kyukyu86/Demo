// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "WRUISlot_TargetLocationGuide.h"
#include "WRUISlot_QuestMarkForTargetLocationGuide.generated.h"

class UWRTextBlock;
UCLASS()
class WR_API UWRUISlot_QuestMarkForTargetLocationGuide : public UWRUISlot_TargetLocationGuide
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	void Tick_UpdateDistText();
private:
	UWRTextBlock* WRTextBlock_Dist;
};