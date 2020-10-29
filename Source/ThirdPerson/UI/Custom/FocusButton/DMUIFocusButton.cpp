// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIFocusButton.h"
#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>


void UDMUIFocusButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime())
	{
		if (Image_Normal)
		{
			Image_Normal->SetBrush(NormalBrush);
			if (FocusType == EDMFocusButtonFocusType::Change)
			{
				Image_Normal->SetVisibility(TestType == EDMFocusButtonTestType::Normal ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			}
			else
			{
				Image_Normal->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		if (Image_Focus)
		{
			Image_Focus->SetBrush(FocusBrush);
			Image_Focus->SetVisibility(TestType == EDMFocusButtonTestType::Focus ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
		if (Image_Select)
		{
			Image_Select->SetBrush(SelectBrush);
			Image_Select->SetVisibility(TestType == EDMFocusButtonTestType::Select ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}

	if (TextBlock_Contents)
	{
		TextBlock_Contents->SetText(FText::FromString(DefaultString));
	}
}

void UDMUIFocusButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Image_Normal)
	{
		Image_Normal->SetBrush(NormalBrush);
		Image_Normal->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (Image_Focus)
	{
		Image_Focus->SetBrush(FocusBrush);
		Image_Focus->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Image_Select)
	{
		Image_Select->SetBrush(SelectBrush);
		Image_Select->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDMUIFocusButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDMUIFocusButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HasAnyUserFocus() || Button_Contents->HasAnyUserFocus())
	{
		if (bIsFocus == false)
		{
			bIsFocus = true;
			UpdateState();
		}
	}
	else
	{
		if (bIsFocus)
		{
			bIsFocus = false;
			UpdateState();
		}
	}
}

FReply UDMUIFocusButton::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

	return FReply::Unhandled();
}

void UDMUIFocusButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (TextBlock_Contents)
	{
		TextBlock_Contents->SetText(FText::FromString(DefaultString));
	}
}

void UDMUIFocusButton::UpdateState()
{
	if (bIsSelect)
	{
		Image_Select->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Image_Select->SetVisibility(ESlateVisibility::Collapsed);
	}	

	if (bIsFocus)
	{
		Image_Focus->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Image_Focus->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDMUIFocusButton::OnClickedButton()
{

}

void UDMUIFocusButton::OnHoveredButton()
{

}

void UDMUIFocusButton::OnUnHoveredButton()
{

}

void UDMUIFocusButton::SetSelect(const bool IN InSet)
{
	bIsSelect = InSet;
	UpdateState();
}
