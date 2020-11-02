// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_TargetLocationSearched.generated.h"


UCLASS()
class WR_API UWRUIPanel_TargetLocationSearched : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
public:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation);
};
