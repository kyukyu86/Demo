// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_TargetName.generated.h"

class UCanvasPanel;
class UTextBlock;

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_TargetName : public UWRUIPanel
{
	GENERATED_BODY()

public:
	UCanvasPanel* CanvasPanel_Name;
	UTextBlock* TextBlock_Name;
	UCanvasPanel* CanvasPanel_Line;
	UCanvasPanel* CanvasPanel_Contents;
	UTextBlock* TextBlock_Contents;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetData(FString IN InName = "", FString IN InContents = "");

	void SetTestCase(uint16 IN InTestCase);
	
};
