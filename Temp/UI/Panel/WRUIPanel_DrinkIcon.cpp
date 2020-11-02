// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_DrinkIcon.h"
#include "WidgetSwitcher.h"
#include "Define/WRUIDefine.h"

void UWRUIPanel_DrinkIcon::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWidgetSwitcher, WidgetSwitcher_Icons);
}

void UWRUIPanel_DrinkIcon::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_DrinkIcon::SetIconType(const EWRDrinkIconType IN InType)
{
	uint8 Index = (uint8)InType - 1;
	if (WidgetSwitcher_Icons)
		WidgetSwitcher_Icons->SetActiveWidgetIndex(Index);
}
