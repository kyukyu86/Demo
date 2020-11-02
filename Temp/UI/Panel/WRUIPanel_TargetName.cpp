// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_TargetName.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"

#include "Actor/Character/WRCharacterIO.h"

#include "TextBlock.h"
#include "CanvasPanel.h"

void UWRUIPanel_TargetName::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Name);
	BIND_WIDGET(UTextBlock, TextBlock_Name);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Line);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Contents);
	BIND_WIDGET(UTextBlock, TextBlock_Contents);
}

void UWRUIPanel_TargetName::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_TargetName::SetData(FString IN InName /* = "" */, FString IN InContents /* = "" */)
{
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Name, InName.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Contents, InContents.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	bool bShowLine = (InName.IsEmpty() == false && InContents.IsEmpty() == false);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Line, bShowLine ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

	WRUIUtility::SetTextSafe(TextBlock_Name, InName);
	WRUIUtility::SetTextSafe(TextBlock_Contents, InContents);
}

void UWRUIPanel_TargetName::SetTestCase(uint16 IN InTestCase)
{
// 	switch (InTestCase)
// 	{
// 	case (uint16)EWRObjectInteractionType::NPC_1:
// 	{
// 		FString strName = TEXT("NPC_1");
// 		FString strContents = TEXT("Fixed Transform");
// 		SetData(strName, strContents);
// 	}
// 	break;
// 	case (uint16)EWRObjectInteractionType::NPC_2:
// 	{
// 		FString strName = TEXT("NPC_2");
// 		FString strContents = TEXT("Billboard\nRotate To MainChar\nFixed Parent Relative Offset");
// 		SetData(strName, strContents);
// 	}
// 	break;
// 	}
}
