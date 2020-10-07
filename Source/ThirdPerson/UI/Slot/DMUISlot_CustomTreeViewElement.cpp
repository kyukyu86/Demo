// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_CustomTreeViewElement.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "../Data/DMCustomTreeViewData.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void UDMUISlot_CustomTreeViewElement::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UDMUISlot_CustomTreeViewElement::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDMUISlot_CustomTreeViewElement::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUISlot_CustomTreeViewElement::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UDMCustomTreeViewData* CastedData = Cast<UDMCustomTreeViewData>(ListItemObject);
	if (CastedData == nullptr)
		return;

	switch (CastedData->Level)
	{
	case 0:
	{
		SetCategory(FString::Printf(TEXT("Category : %d"), CastedData->Index));
	}
	break;

	case 1:
	{
		SetElement(FString::Printf(TEXT("Child 1 : %d"), CastedData->Index), CastedData->Level);
	}
	break;

	case 2:
	{
		SetElement(FString::Printf(TEXT("Child 2 : %d"), CastedData->Index), CastedData->Level);
	}
	break;
	}
}

void UDMUISlot_CustomTreeViewElement::SetCategory(const FString IN InString)
{
	WidgetSwitcher_Type->SetActiveWidgetIndex((int32)EDMCustomTreeViewType::Category);
	TextBlock_Category->SetText(FText::FromString(InString));
}

void UDMUISlot_CustomTreeViewElement::SetElement(const FString IN InString, const int32 IN InLevel)
{
	WidgetSwitcher_Type->SetActiveWidgetIndex((int32)EDMCustomTreeViewType::Children);
	TextBlock_Element->SetText(FText::FromString(InString));

	UHorizontalBoxSlot* HorizontalBoxSlot = UWidgetLayoutLibrary::SlotAsHorizontalBoxSlot(TextBlock_Element);
	if (HorizontalBoxSlot)
	{
		float Space = LevelSpace * InLevel;
		HorizontalBoxSlot->SetPadding(FMargin(Space, 0, 0, 0));
	}
}