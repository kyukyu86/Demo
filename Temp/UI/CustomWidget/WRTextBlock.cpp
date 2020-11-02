// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRTextBlock.h"
#include "SlateStyle.h"
#include "Utility/WREnumUtility.h"
#include "Utility/WRUIUtility.h"
#include "UObjectGlobals.h"


TSharedRef<SWidget> UWRTextBlock::RebuildWidget()
{
	if (bRichTextBlock == false)
	{
		MyTextBlock =
			SNew(STextBlock)
			.SimpleTextMode(false);

		return MyTextBlock.ToSharedRef();
	}
	else
	{
		return URichTextBlock::RebuildWidget();
	}
}

void UWRTextBlock::Tick_TypingEffect(const float IN InDeltaTime)
{
	if (bTypingEffect == false)
		return;

	int32 PreviousTextNum = TypingEffectTime / TypingEffectSpeed;
	TypingEffectTime += InDeltaTime;
	int32 CurrentTextNum = TypingEffectTime / TypingEffectSpeed;

	if (PreviousTextNum < CurrentTextNum)
	{
		 SetText(TypingEffectContents.Mid(0, CurrentTextNum));
	}

	if (TypingEffectContents.Len() <=  CurrentTextNum)
	{
		bTypingEffect = false;
		TypingEffectSpeed = 0.f;
		TypingEffectTime = 0.f;
		TypingEffectContents.Empty();
		TypingEffectCompletedDelegate.ExecuteIfBound();
		TypingEffectCompletedDelegate = nullptr;
	}
}

void UWRTextBlock::SynchronizeProperties()
{
	if (bRichTextBlock == false)
	{
		if (MyTextBlock.IsValid() == false)
		{
			return;
		}

		UTextLayoutWidget::SynchronizeProperties();

		MyTextBlock->SetText(Text);
		//MyTextBlock->SetFont(Font);
		//MyTextBlock->SetStrikeBrush(&StrikeBrush);
		//MyTextBlock->SetColorAndOpacity(DefaultTextStyleOverride.ColorAndOpacity);
		//MyTextBlock->SetShadowOffset(ShadowOffset);
		//MyTextBlock->SetShadowColorAndOpacity(DefaultTextStyleOverride.ShadowColorAndOpacity);
		MyTextBlock->SetMinDesiredWidth(MinDesiredWidth);

		UpdateTextBlockStyle();
		UTextLayoutWidget::SynchronizeTextLayoutProperties(*MyTextBlock);
	}
	else
	{
		if (MyRichTextBlock.IsValid() == false)
		{
			return;
		}

		Super::SynchronizeProperties();

		MyRichTextBlock->SetText(Text);
		UpdateTextBlockStyle();

		Super::SynchronizeTextLayoutProperties(*MyRichTextBlock);
	}
}

void UWRTextBlock::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyTextBlock.Reset();
}

FText UWRTextBlock::GetText() const
{
	if (MyTextBlock.IsValid())
	{
		return MyTextBlock->GetText();
	}
	else if (MyRichTextBlock.IsValid())
	{
		return MyRichTextBlock->GetText();
	}

	return Text;

}

void UWRTextBlock::SetJustification(ETextJustify::Type InJustification)
{
	UTextLayoutWidget::SetJustification(InJustification);

	if (MyTextBlock.IsValid())
	{
		MyTextBlock->SetJustification(InJustification);
	}
	else if (MyRichTextBlock.IsValid())
	{
		MyRichTextBlock->SetJustification(InJustification);
	}
}

void UWRTextBlock::SetText(const FText& InText)
{
	Text = InText;
	if (MyTextBlock.IsValid())
	{
		MyTextBlock->SetText(InText);
	}
	else if (MyRichTextBlock.IsValid())
	{
		MyRichTextBlock->SetText(InText);
	}

	if (AutoWrapText)
	{
		if (MyTextBlock.IsValid())
		{
			MyTextBlock->ComputeDesiredSize(1.f);
		}
	}
}

void UWRTextBlock::SetText(const FString& InText)
{
	SetText(FText::FromString(InText));
}

void UWRTextBlock::SetText(const FName& InText)
{
	SetText(InText.ToString());
}

void UWRTextBlock::UpdateTextBlockStyle()
{
	if (bOverrideDefaultStyle)
	{
		SetTextBlockStyle(DefaultTextStyleOverride);
		return;
	}
	
	//UpdateTextStyleEnum();
	if (TextStyleSet->IsValidLowLevel() == false)
		return;

	/*if (DefaultFontName == EWRTextStyleRowName::MAX)
		return;*/

	//FontRowName = *(WREnumUtility::EnumToString("EWRTextStyleRowName", DefaultFontName));
	FRichTextStyleRow* FoundRichTestStyleRow= TextStyleSet->FindRow<FRichTextStyleRow>(FontRowName, TEXT("WRTextBlock"));
	if (FoundRichTestStyleRow == nullptr)
		return;

	SetTextBlockStyle(FoundRichTestStyleRow->TextStyle);
}

void UWRTextBlock::UpdateTextStyleEnum()
{
	if (TextStyleSet == nullptr)
		return;

	TArray<TPair<FName, int64>> EnumNames;
	const TMap<FName, uint8*>& TextStyleSetMap = TextStyleSet->GetRowMap();
	
	int32 Count = 0;
	for (auto& Data : TextStyleSetMap)
	{
		FName EnumName = *(TEXT("EWRTextStyleRowName::") + WRUIUtility::ConvertFNameToFString(Data.Key));
		EnumNames.Add(TPair<FName, int64>(EnumName, Count++));
	}

	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWRTextStyleRowName"), true);
	if (EnumPtr)
	{
		EnumPtr->SetEnums(EnumNames, EnumPtr->GetCppForm());
	}

	/*if(DefaultFontName == EWRTextStyleRowName::MAX)
		DefaultFontName = EWRTextStyleRowName(WREnumUtility::StringToEnum(TEXT("EWRTextStyleRowName"), WRUIUtility::ConvertFNameToFString(FontRowName)));*/
}

void UWRTextBlock::SetTextBlockStyle(FTextBlockStyle IN InTextBlockStyle)
{
	if (bFontSizeOverride)
	{
		InTextBlockStyle.SetFontSize(FontSize);
	}
	
	if (MyTextBlock.IsValid())
	{
		MyTextBlock->SetTextStyle(&InTextBlockStyle);
		MyTextBlock->SetFont(InTextBlockStyle.Font);
	}
	else if (MyRichTextBlock.IsValid())
	{
		DefaultTextStyle = InTextBlockStyle;
		if (StyleInstance)
			StyleInstance->Set(FontRowName, InTextBlockStyle);

		MyRichTextBlock->SetDecoratorStyleSet(StyleInstance.Get());
		MyRichTextBlock->SetTextStyle(DefaultTextStyle);
	}
}

void UWRTextBlock::SetColorAndOpacity(const FSlateColor& IN InColorAndOpacity)
{
	if (MyTextBlock.IsValid())
	{
		if (MyTextBlock.IsValid())
		{
			MyTextBlock->SetColorAndOpacity(InColorAndOpacity);
		}
	}
	else if (MyRichTextBlock.IsValid())
	{
		SetDefaultColorAndOpacity(InColorAndOpacity);
	}
}

void UWRTextBlock::SetIsRichTextBlock(const bool InbRichTextBlock)
{
	bRichTextBlock = InbRichTextBlock;
	RebuildWidget();
	SynchronizeProperties();
}

void UWRTextBlock::SetFontRawName(const FName InName)
{
	FontRowName = InName;
	UpdateTextBlockStyle();
}

void UWRTextBlock::PlayTypingEffect(const float IN InSpeed /*= 0.2f*/, FWROnTypingEffectCompleted IN InDelegate /*= nullptr*/)
{
	if (bTypingEffect)
		return;

	if (InSpeed <= 0.f)
		return;

	bTypingEffect = true;
	TypingEffectSpeed = InSpeed;
	TypingEffectTime = 0.f;
	TypingEffectContents = GetText().ToString();
	SetText(FText::FromString(TEXT("")));
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	TypingEffectCompletedDelegate = InDelegate;
}
