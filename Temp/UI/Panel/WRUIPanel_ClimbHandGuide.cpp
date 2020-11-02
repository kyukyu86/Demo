// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_ClimbHandGuide.h"
#include "Image.h"

#include "Manager/WRUIManager.h"
#include "Utility/WRUIUtility.h"

void UWRUIPanel_ClimbHandGuide::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Left);
	BIND_WIDGET(UImage, Image_Right);
}

void UWRUIPanel_ClimbHandGuide::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_ClimbHandGuide::SetLeft(const bool IN InLeft)
{
	WRUIUtility::SetVisibilitySafe(Image_Left, InLeft ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(Image_Right, InLeft ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}