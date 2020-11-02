// Fill out your copyright notice in the Description page of Project Settings.


#include "WRSlideElement.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UWRSlideElement::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWRSlideElement::NativeDestruct()
{

	Super::NativeDestruct();
}

void UWRSlideElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRSlideElement::SetTranslationIndex(const int32 IN InIndex)
{
	TranslationIndex = InIndex;
	TextBlock_TranslationIndex->SetText(FText::FromString(FString::FormatAsNumber(InIndex)));
}

void UWRSlideElement::SetIndex(const int32 IN InIndex)
{
	Index = InIndex;
	TextBlock_Index->SetText(FText::FromString(FString::FormatAsNumber(InIndex)));
}