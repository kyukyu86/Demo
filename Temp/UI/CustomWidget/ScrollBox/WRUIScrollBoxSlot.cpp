// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIScrollBoxSlot.h"
#include "WRUISScrollBox.h"

#include "Components/ScrollBoxSlot.h"
#include "Components/Widget.h"

/////////////////////////////////////////////////////
// UScrollBoxSlot

UWRUIScrollBoxSlot::UWRUIScrollBoxSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Slot(nullptr)
{
	HorizontalAlignment = HAlign_Fill;
}

void UWRUIScrollBoxSlot::BuildSlot(TSharedRef<WRUISScrollBox> ScrollBox)
{
	Slot = &ScrollBox->AddSlot()
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
}

void UWRUIScrollBoxSlot::SetPadding(FMargin InPadding)
{
	Padding = InPadding;
	if ( Slot )
	{
		Slot->Padding(InPadding);
	}
}

void UWRUIScrollBoxSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if ( Slot )
	{
		Slot->HAlign(InHorizontalAlignment);
	}
}

void UWRUIScrollBoxSlot::SynchronizeProperties()
{
	SetPadding(Padding);
	SetHorizontalAlignment(HorizontalAlignment);
}

void UWRUIScrollBoxSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Slot = nullptr;
}
