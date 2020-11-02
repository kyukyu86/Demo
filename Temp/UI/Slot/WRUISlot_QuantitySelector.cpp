// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_QuantitySelector.h"
#include "UI/CustomWidget/WRTextBlock.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "UI/Panel/WRUIPanel_Popup.h"
#include "Enum/EWRCharacter.h"
#include "UI/CustomWidget/WRFocusButton.h"
#include "UnrealMathUtility.h"
#include "WidgetPath.h"
#include "SWindow.h"

void UWRUISlot_QuantitySelector::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRTextBlock, WRTextBlock_Contents, WRTextBlock_Contents->SetVisibility(ESlateVisibility::Collapsed););
	BIND_WIDGET(UWRTextBlock, WRTextBlock_QuantityTest);

	int32 nWidgetIndex = 0;
	bool bCreating = true;
	while (bCreating)
	{
		FString WidgetName = FString::Printf(TEXT("CanvasPanel_%d"), nWidgetIndex);
		UCanvasPanel* pCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(FName(*WidgetName)));
		if (pCanvasPanel == nullptr)
		{
			bCreating = false;
		}
		else
		{
			WidgetName = FString::Printf(TEXT("FocusButton_%d"), nWidgetIndex);
			UWRFocusButton* pFocusButton = Cast<UWRFocusButton>(GetWidgetFromName(FName(*WidgetName)));
			if (pFocusButton)
			{
				pFocusButton->SetIndex(nWidgetIndex);
				if (nWidgetIndex == EWRQuantityIndex::One)
					LastFocusedButton = pFocusButton;

				ElementList[nWidgetIndex].CanvasPanel = pCanvasPanel;
				ElementList[nWidgetIndex].FocusButton = pFocusButton;
				++nWidgetIndex;
			}
		}
	}
}

void UWRUISlot_QuantitySelector::NativeDestruct()
{

	Super::NativeDestruct();
}

FReply UWRUISlot_QuantitySelector::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Unhandled();

	EWRInput WRInput = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	switch (WRInput)
	{
	case EWRInput::LStickUp:
	case EWRInput::DpadUP:
	{
		OnQuantityUp();
		return FReply::Handled();
	}
	break;
	case EWRInput::LStickDown:
	case EWRInput::DpadDown:
	{
		OnQuantityDown();
		return FReply::Handled();
	}
	break;
	case EWRInput::LStickLeft:
	case EWRInput::DpadLeft:
	{
		OnQuantityLeft();
		return FReply::Handled();
	}
	break;
	case EWRInput::LStickRight:
	case EWRInput::DpadRight:
	{
		OnQuantityRight();
		return FReply::Handled();
	}
	break;
	}
	return FReply::Unhandled();
}

void UWRUISlot_QuantitySelector::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	UWRFocusButton* PrevFocusedButton = nullptr;
	for (auto EachWidget : PreviousFocusPath.Widgets)
	{
		if (EachWidget.IsValid() == false)
			continue;

		UWidget* FoundWidget = GetWidgetHandle(EachWidget.Pin().ToSharedRef());
		if (FoundWidget != nullptr)
		{
			if (FoundWidget->IsA<UWRFocusButton>())
			{
				PrevFocusedButton = Cast<UWRFocusButton>(FoundWidget);
			}
		}
	}

	for (auto EachWidget : NewWidgetPath.Widgets.GetInternalArray())
	{
		//UWidget* FoundWidget = TopWidget->GetWidgetHandle(EachWidget.Widget);
		UWidget* FoundWidget = GetWidgetHandle(EachWidget.Widget);
		if (FoundWidget != nullptr)
		{
			if (FoundWidget->IsA<UWRFocusButton>())
			{
				LastFocusedButton = Cast<UWRFocusButton>(FoundWidget);
			}
		}
	}
}

int32 UWRUISlot_QuantitySelector::GetValueOfIndex(const EWRQuantityIndex IN InIndex)
{
	int32 nResult = 0;
	if (InIndex == EWRQuantityIndex::One)
	{
		nResult = Quantity % 10;
	}
	else
	{
		int32 nValue = FMath::Pow(10, InIndex);
		int32 nPrevValue = EWRQuantityIndex::One;
		if (InIndex != EWRQuantityIndex::One)
		{
			nPrevValue = FMath::Pow(10, InIndex - 1);
		}
		int32 nNextValue = FMath::Pow(10, InIndex + 1);

		// 더 큰 자릿수를 원하는 경우
		if (Quantity < nValue)
		{
			return 0;
		}
		else
		{
			// 12345를 나누는 경우 3번째 구하고자
			int32 nTest = 12345;
			nTest %= nNextValue;	// 다음 자릿수(1000)로 나눈 나머지 : 345
			nTest /= (int32)nValue;	// 현재 자릿수(100)로 나눈 값 : 3

			nResult = Quantity;
			nResult %= nNextValue;
			nResult /= (int32)nValue;
		}
	}
	return nResult;
}

void UWRUISlot_QuantitySelector::OnQuantityUp()
{
	if (LastFocusedButton->IsValidLowLevel() == false)
		return;

	// 예) 12345 : 자릿수는 0부터 시작
	// 0번째 : 5
	// 1번째 : 4

	// 대상 자릿수 (1번째)
	int32 nIndex = LastFocusedButton->GetIndex();
	// 자릿수의 값 (1번째 자릿수의 값 : 4)
	int32 nValue = GetValueOfIndex((EWRQuantityIndex)nIndex);
	// 실제 자릿수의 값 (1번째 자릿수의 실제 값 : 40)
	int32 nRealValue = nValue * (FMath::Pow(10, nIndex));
	// 실제 자릿수를 뺀 값 : 12345 - 40 = 12305
	int32 nQuantityOfRmovedRealValue = Quantity - nRealValue;

	// 증가된 자릿수의 값 (5)
	int32 nUpValue = 0;
	// 증가 전 자릿수 값이 9면 0으로 초기화
	if (nValue == 9)
		nUpValue = 0;
	else
		nUpValue = nValue + 1;
	// 증가 된 실제 자릿수의 값 (50)
	int32 nRealUpValue = nUpValue * (FMath::Pow(10, nIndex));
	// 실제 자릿수를 뺀 값에 증가된 실제 자릿수의 값을 더함 (12305 + 50 = 12355)
	nQuantityOfRmovedRealValue += nRealUpValue;

	// 결과
	Quantity = nQuantityOfRmovedRealValue;

	// Max Check
	if (MaxQuantity == DEF_NON_SETTED_MAX_QUANTITY)
	{
		// 별도로 최대값이 설정되어있지 않으면 최대 자릿수만큼만.
		if (Quantity >= FMath::Pow(10, EWRQuantityIndex::Max))
		{
			Quantity = FMath::Pow(10, EWRQuantityIndex::Max) - 1;
		}
	}
	else
	{
		if (Quantity > MaxQuantity)
		{
			Quantity = MaxQuantity;
		}
		else if (Quantity < MinQuantity)
		{
			Quantity = MinQuantity;
		}
	}

	UpdateQuantityWidget();
	OnQuantityChangeDelegate.ExecuteIfBound(Quantity);

	// Test
	WRTextBlock_QuantityTest->SetText(FString::FormatAsNumber(Quantity));
}

void UWRUISlot_QuantitySelector::OnQuantityDown()
{
	if (LastFocusedButton->IsValidLowLevel() == false)
		return;

	// 예) 12345 : 자릿수는 0부터 시작
	// 0번째 : 5
	// 1번째 : 4

	// 대상 자릿수 (1번째)
	int32 nIndex = LastFocusedButton->GetIndex();
	// 자릿수의 값 (1번째 자릿수의 값 : 4)
	int32 nValue = GetValueOfIndex((EWRQuantityIndex)nIndex);
	// 실제 자릿수의 값 (1번째 자릿수의 실제 값 : 40)
	int32 nRealValue = nValue * (FMath::Pow(10, nIndex));
	// 실제 자릿수를 뺀 값 : 12345 - 40 = 12305
	int32 nQuantityOfRmovedRealValue = Quantity - nRealValue;

	// 감소된 자릿수의 값 (3)
	int32 nDownValue = 0;
	// 감소 전 자릿수 값이 0면 9으로 초기화
	if (nValue == 0)
		nDownValue = 9;
	else
		nDownValue = nValue - 1;
	// 감소 된 실제 자릿수의 값 (30)
	int32 nRealDownValue = nDownValue * (FMath::Pow(10, nIndex));
	// 실제 자릿수를 뺀 값에 감소된 실제 자릿수의 값을 더함 (12305 + 30 = 12335)
	nQuantityOfRmovedRealValue += nRealDownValue;

	// 결과
	Quantity = nQuantityOfRmovedRealValue;

	// Min Check
	if (Quantity < MinQuantity)
	{
		Quantity = MinQuantity;
	}
	else if (Quantity > MaxQuantity)
	{
		Quantity = MaxQuantity;
	}

	UpdateQuantityWidget();
	OnQuantityChangeDelegate.ExecuteIfBound(Quantity);

	// Test
	WRTextBlock_QuantityTest->SetText(FString::FormatAsNumber(Quantity));
}

void UWRUISlot_QuantitySelector::OnQuantityLeft()
{
	int32 nCurrentIndex = LastFocusedButton->GetIndex();
	int32 nLeftIndex = nCurrentIndex;
	if (nCurrentIndex == MaxIndex)
	{
		nLeftIndex = EWRQuantityIndex::One;
	}
	else
	{
		nLeftIndex = nCurrentIndex + 1;
	}

	LastFocusedButton = ElementList[nLeftIndex].FocusButton;
	SetFocusableInputMode(true, LastFocusedButton);
}

void UWRUISlot_QuantitySelector::OnQuantityRight()
{
	int32 nCurrentIndex = LastFocusedButton->GetIndex();
	int32 nRightIndex = nCurrentIndex;
	if (nCurrentIndex == EWRQuantityIndex::One)
	{
		nRightIndex = MaxIndex;
	}
	else
	{
		nRightIndex = nCurrentIndex - 1;
	}

	LastFocusedButton = ElementList[nRightIndex].FocusButton;
	SetFocusableInputMode(true, LastFocusedButton);
}

void UWRUISlot_QuantitySelector::UpdateQuantityWidget(const EWRQuantityIndex IN InIndex /* = EWRQuantityIndex::Max */)
{
	if (InIndex == EWRQuantityIndex::Max)
	{
		for (int32 i = EWRQuantityIndex::One; i < EWRQuantityIndex::Max; ++i)
		{
			if (Quantity == 0)
			{
				ElementList[i].FocusButton->SetContents("0");
			}
			else
			{
				int32 nValue = GetValueOfIndex((EWRQuantityIndex)i);
				ElementList[i].FocusButton->SetContents(FString::FormatAsNumber(nValue));
			}
		}
	}
	else
	{
		int32 nValue = GetValueOfIndex(InIndex);
		ElementList[InIndex].FocusButton->SetContents(FString::FormatAsNumber(nValue));
	}
}

void UWRUISlot_QuantitySelector::SetContents(const FString IN InContents)
{
	WRTextBlock_Contents->SetText(InContents);
	WRTextBlock_Contents->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUISlot_QuantitySelector::SetQuantityChangeDelegate(FWRQuantityChangeDelegate IN InDelegate)
{
	if (OnQuantityChangeDelegate.IsBound())
		OnQuantityChangeDelegate.Unbind();

	OnQuantityChangeDelegate = InDelegate;
}

void UWRUISlot_QuantitySelector::SetMinQuantity(const int32 IN InQuantity)
{
	MinQuantity = InQuantity;
}

void UWRUISlot_QuantitySelector::SetMaxQuantity(const int32 IN InQuantity)
{
	MaxQuantity = InQuantity;

// 	for (int32 i = EWRQuantityIndex::One; i < EWRQuantityIndex::Max; ++i)
// 	{
// 		if (InQuantity < FMath::Pow(10, i))
// 		{
// 			ElementList[i].bUse = false;
// 			ElementList[i].CanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
// 		}
// 		else
// 		{
// 			ElementList[i].bUse = true;
// 			ElementList[i].CanvasPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
// 
// 			MaxIndex = (EWRQuantityIndex)i;
// 		}
// 	}
}

void UWRUISlot_QuantitySelector::OnFocus()
{
	SetFocusableInputMode(true, LastFocusedButton);
}
