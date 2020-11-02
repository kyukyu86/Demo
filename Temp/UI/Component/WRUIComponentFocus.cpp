// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIComponentFocus.h"
#include "UI/CustomWidget/WRFocusButton.h"
#include "UI/Base/WRUserWidget.h"

WRUIComponentFocus::WRUIComponentFocus()
{
}

WRUIComponentFocus::~WRUIComponentFocus()
{
}

void WRUIComponentFocus::SetFocusButton(class UWRFocusButton* IN InFocusButton, const bool IN InDefault /* = false */)
{
	LastFocusedButton = InFocusButton;

	// 디폴트로 포커싱 될 버튼
	if (InDefault)
	{
		DefaultFocusedButton = LastFocusedButton;
	}
}

/*
// [ 2020-7-30 : kyu ] 현재 안씀
void WRUIComponentFocus::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	if (OwnerWidget == nullptr)
		return;

	UWRFocusButton* PrevFocusedButton = nullptr;
	for (auto EachWidget : PreviousFocusPath.Widgets)
	{
		if (EachWidget.IsValid() == false)
			continue;

		UWidget* FoundWidget = OwnerWidget->GetWidgetHandle(EachWidget.Pin().ToSharedRef());
		if (FoundWidget != nullptr)
		{
			if (FoundWidget->IsA<UWRFocusButton>())
			{
				PrevFocusedButton = Cast<UWRFocusButton>(FoundWidget);
			}
		}
	}

//	UWidget* TestWidget = OwnerWidget->GetWidgetHandle(NewWidgetPath.Widgets.Last().Widget);

	int32 nIndex = 0;
	UWRFocusButton* NewFocusedButton = nullptr;	
	for (auto EachWidget : NewWidgetPath.Widgets.GetInternalArray())
	{
		UWidget* FoundWidget = OwnerWidget->GetWidgetHandle(EachWidget.Widget);
		if (FoundWidget != nullptr)
		{
			if (FoundWidget->IsA<UWRFocusButton>())
			{
				NewFocusedButton = Cast<UWRFocusButton>(FoundWidget);
			}
			else
			{
				UUserWidget* CatedFoundWidget = Cast<UUserWidget>(FoundWidget);
				if (CatedFoundWidget)
				{
					FoundWidget = CatedFoundWidget->GetWidgetHandle(NewWidgetPath.Widgets.Last().Widget);
					if (FoundWidget != nullptr)
					{
						if (FoundWidget->IsA<UWRFocusButton>())
						{
							NewFocusedButton = Cast<UWRFocusButton>(FoundWidget);
						}
					}
				}
			}
		}
		++nIndex;
	}

	// New
	if (NewFocusedButton != nullptr)// && LastFocusedButton != NewFocusedButton)
	{
		bool IsDiff = LastFocusedButton != NewFocusedButton;
		LastFocusedButton = NewFocusedButton;
		FocusChangedDelegate.ExecuteIfBound(LastFocusedButton, IsDiff);
	}
}
*/

void WRUIComponentFocus::OnClickedFocusedButton()
{
	if (LastFocusedButton->IsValidLowLevel() == false)
		return;

	LastFocusedButton->OnClickedButton();
}

void WRUIComponentFocus::RefocusOnLastFocusedButton()
{
	if (OwnerWidget == nullptr)
		return;

	if (LastFocusedButton->IsValidLowLevel())
	{
		OwnerWidget->SetFocusableInputMode(true, LastFocusedButton);
	}
	else
	{
		LastFocusedButton = DefaultFocusedButton;
		OwnerWidget->SetFocusableInputMode(true, LastFocusedButton);
	}
}

void WRUIComponentFocus::FocusToDefaultFocusButton()
{
	LastFocusedButton = DefaultFocusedButton;
	RefocusOnLastFocusedButton();
}

void WRUIComponentFocus::OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet)
{
	if (InSet)
	{
		LastFocusedButton = InFocusButton;
	}
//	FocusChangedDelegate.ExecuteIfBound(InFocusButton, InSet);
}
