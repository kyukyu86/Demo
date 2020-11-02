// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_DungeonTimer.generated.h"


class UCanvasPanel;
class UTextBlock;

UCLASS()
class WR_API UWRUIPanel_DungeonTimer : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetTime(int32 IN InDisplayTime);
private:
	UTextBlock* TextBlock_Hour;
	UTextBlock* TextBlock_Minute;
	UTextBlock* TextBlock_Second;

	float FlowTime;
	int32 DisplayTime;
};