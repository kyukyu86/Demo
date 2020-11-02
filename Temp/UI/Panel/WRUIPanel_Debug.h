// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_Debug.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_Debug : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetDebugContents(FString IN InContents);

private:
	class UWRTextBlock* WRTextBlock_Contents;
};
