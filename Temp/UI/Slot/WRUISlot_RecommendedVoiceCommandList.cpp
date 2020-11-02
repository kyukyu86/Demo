// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_RecommendedVoiceCommandList.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"


void UWRUISlot_RecommendedVoiceCommandList::NativeConstruct()
{
	Super::NativeConstruct();
	
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Contents);
}

void UWRUISlot_RecommendedVoiceCommandList::SetContentsText(const FName& IN InContents)
{
	FString Contents = FString::Printf(TEXT("\"%s\""), *WRUIUtility::ConvertFNameToFString(InContents));
	WRTextBlock_Contents->SetText(Contents);
}
