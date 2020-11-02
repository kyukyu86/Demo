// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_ScoreBoardList.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"

void UWRUISlot_ScoreBoardList::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Rank);
	BIND_WIDGET(UTextBlock, TextBlock_Name);
	BIND_WIDGET(UTextBlock, TextBlock_Score);
}

void UWRUISlot_ScoreBoardList::SetData(const int32 IN InRank, const FName& IN InName, const int32 IN InScore)
{
	TextBlock_Rank->SetText(FText::AsNumber(InRank));
	TextBlock_Name->SetText(WRUIUtility::ConvertFNameToFText(InName));
	TextBlock_Score->SetText(FText::AsNumber(InScore));
}
