// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_SlideElement.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDMUISlot_SlideElement::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDMUISlot_SlideElement::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUISlot_SlideElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UDMUISlot_SlideElement::SetMain(const bool IN InSet)
{
	bIsMain = InSet;
}

void UDMUISlot_SlideElement::SetTranslationIndex(const int32 IN InIndex)
{
	TranslationIndex = InIndex;
	TextBlock_TranslationIndex->SetText(FText::FromString(FString::FormatAsNumber(InIndex)));
}

void UDMUISlot_SlideElement::SetIndex(const int32 IN InIndex)
{
	Index = InIndex;
	TextBlock_Index->SetText(FText::FromString(FString::FormatAsNumber(InIndex)));
}