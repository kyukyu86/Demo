// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "WRUISlot_TargetLocationGuide.h"
#include "WRUISlot_FocusTalk.generated.h"

class UCanvasPanel;
class UWRTextBlock;
UCLASS()
class WR_API UWRUISlot_FocusTalk : public UWRUISlot_TargetLocationGuide
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	virtual void SetTargetLocationInfo(const FWRTargetLocationInfoBase* IN InTargetLocationInfo) override;
	void UpdateUIForHMDRotation(const AID_t IN InTargetAID);
private:
	UCanvasPanel* CanvasPanel_HMDRotation;
	UWRTextBlock* WRTextBlock_Name;
};