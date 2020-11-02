// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIPanel_SubColorSelect.h"
#include "Manager/WRCharacterCustomizingManager.h"


void UWRUIPanel_SubColorSelect::NativeConstruct()
{
	Super::NativeConstruct();
	//WRCharacterCustomizingManager::Get()->GetPhaseChangedDelegate().AddUObject(this, &UWRUIPanel_SubColorSelect::OnPhaseChanged);
}

void UWRUIPanel_SubColorSelect::OnPhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase)
{

}
