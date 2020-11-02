// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_Test.h"
#include "TextBlock.h"
#include "Define/WRUIDefine.h"

void UWRUISlot_Test::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Num);
}

void UWRUISlot_Test::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


}

void UWRUISlot_Test::SetNum(const int32 IN InNum)
{
	TextBlock_Num->SetText(FText::AsNumber(InNum));
}
