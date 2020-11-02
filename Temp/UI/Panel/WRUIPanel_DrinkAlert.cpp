// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_DrinkAlert.h"
#include "Manager/WRUIManager.h"

void UWRUIPanel_DrinkAlert::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWRUIPanel_DrinkAlert::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_DrinkAlert::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	fElapsedTime += InDeltaTime;
	if (fElapsedTime >= OpenTime)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::DrinkAlert);
	}
}
