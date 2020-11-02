// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Debug.h"
#include "Define/WRUIDefine.h"
#include "RichTextBlock.h"
#include "Utility/WRUIUtility.h"

void UWRUIPanel_Debug::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Contents);
}

void UWRUIPanel_Debug::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_Debug::SetDebugContents(FString IN InContents)
{
	WRUIUtility::SetTextSafe(WRTextBlock_Contents, InContents);
}
