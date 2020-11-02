// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Manager/WRCharacterCustomizingManager.h"
#include "WRUIPanel_SubColorSelect.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_SubColorSelect : public UUserWidget
{
	GENERATED_BODY()

		void NativeConstruct();
	
	void OnPhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase);
};
