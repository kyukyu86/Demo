// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlideElement.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDMUISlideElement::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDMUISlideElement::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUISlideElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UDMUISlideElement::SetMain(const bool IN InSet)
{
	bIsMain = InSet;
}

void UDMUISlideElement::SetTranslationIndex(const int32 IN InIndex)
{
	TranslationIndex = InIndex;
	TextBlock_TranslationIndex->SetText(FText::FromString(FString::FormatAsNumber(InIndex)));
}

void UDMUISlideElement::SetIndex(const int32 IN InIndex)
{
	Index = InIndex;
	TextBlock_Index->SetText(FText::FromString(FString::FormatAsNumber(InIndex)));
}