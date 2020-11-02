// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_ClimbHandGuide.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_ClimbHandGuide : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetLeft(const bool IN InLeft);

private:
	class UImage* Image_Left;
	class UImage* Image_Right;
};
