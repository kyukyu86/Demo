// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_InputIcon.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "TextBlock.h"
#include "Image.h"

void UWRUIPanel_InputIcon::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Content);
	BIND_WIDGET(UImage, Image_Icon);
}

void UWRUIPanel_InputIcon::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_InputIcon::SetInputKey(const EWRInput IN InInput)
{
	if (InInput == EWRInput::Triangle)
	{
		WRUIUtility::SetResourceObject(Image_Icon, Cast<UObject>(Texture_Triangle));
	}
	else if (InInput == EWRInput::Square)
	{
		WRUIUtility::SetResourceObject(Image_Icon, Cast<UObject>(Texture_Square));
	}
	else if (InInput == EWRInput::Circle)
	{
		WRUIUtility::SetResourceObject(Image_Icon, Cast<UObject>(Texture_Circle));
	}
	else if (InInput == EWRInput::Cross)
	{
		WRUIUtility::SetResourceObject(Image_Icon, Cast<UObject>(Texture_Cross));
	}
}

void UWRUIPanel_InputIcon::SetContent(const FText& IN InContent)
{
	TextBlock_Content->SetText(InContent);
}
