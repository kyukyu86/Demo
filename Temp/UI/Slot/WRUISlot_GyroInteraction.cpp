// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_GyroInteraction.h"
#include "Image.h"
#include "TextBlock.h"
#include "Utility/WRUIUtility.h"
#include "Define/WRUIDefine.h"

void UWRUISlot_GyroInteraction::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Icon);
	BIND_WIDGET(UTextBlock, TextBlock_Description);
}

void UWRUISlot_GyroInteraction::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWRUISlot_GyroInteraction::SetInformation(const FString& IN InIconPath, const FString& IN InDescription)
{
	// Icon
	WRUIUtility::SetResourceObject(Image_Icon, InIconPath);
	// Name
	WRUIUtility::SetTextSafe(TextBlock_Description, InDescription);
}
