// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_CustomSlideElement.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "../../Util/DMUIUtil.h"
#include "../Custom/SlideList/DMUISlideList.h"

void UDMUISlot_CustomSlideElement::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDMUISlot_CustomSlideElement::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUISlot_CustomSlideElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UDMUISlot_CustomSlideElement::SetMain(const bool IN InSet)
{
	UDMUISlideElement::SetMain(InSet);

	Image_Main->SetVisibility(InSet ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UDMUISlot_CustomSlideElement::SetData(const int32 IN InDataIndex)
{
	Image_BG->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	TextBlock_Data->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	TArray<FDMCustomSlideData>& TestDataList = ParentSlideListWidget->GetTestData();
	if (TestDataList.IsValidIndex(InDataIndex))
	{
		TextBlock_Data->SetText(FText::FromString(FString::FormatAsNumber(TestDataList[InDataIndex].ID)));
		DMUIUtil::SetResourceObject(Image_Icon, TestDataList[InDataIndex].ImagePath, true);
	}
}

void UDMUISlot_CustomSlideElement::SetEmpty()
{
	Image_BG->SetVisibility(ESlateVisibility::Collapsed);
	Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
	TextBlock_Data->SetVisibility(ESlateVisibility::Collapsed);
}
