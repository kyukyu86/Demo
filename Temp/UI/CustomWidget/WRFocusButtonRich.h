// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRFocusButton.h"
#include "WRFocusButtonRich.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRFocusButtonRich : public UWRFocusButton
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
