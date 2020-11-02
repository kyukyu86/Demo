// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_AvailableDialogueIcon.generated.h"


class AWRCharacterNPC;
class UImage;
UCLASS()
class WR_API UWRUIPanel_AvailableDialogueIcon: public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	void SetOwnerCharacter(const AWRCharacterNPC* IN InNpcCharacter);
private:
	void UpdateUI();
private:
	const AWRCharacterNPC* OwnerNpcCharacter;
	UImage* Image_Icon;
};