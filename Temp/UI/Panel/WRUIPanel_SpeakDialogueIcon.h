// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_SpeakDialogueIcon.generated.h"


UCLASS()
class WR_API UWRUIPanel_SpeakDialogueIcon: public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
};