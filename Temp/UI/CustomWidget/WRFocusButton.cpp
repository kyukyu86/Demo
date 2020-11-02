// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRFocusButton.h"
#include "WRTextBlock.h"

#include "Define/WRUIDefine.h"

#include "Utility/WRUIUtility.h"
#include "Manager/WRConsoleManager.h"
#include "Engine/Texture2D.h"
#include "Core/WRGameInstance.h"
#include "WidgetBlueprintLibrary.h"
#include "Manager/WRUIManager.h"

#include "Components/ButtonSlot.h"


void UWRFocusButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime())
	{
		if (Image_NotFocus)
		{
			Image_NotFocus->SetBrush(FocusButtonManualInfo.NotFocusBrush);
			if (FocusUIType == EWRFocusUITypeInFocusButton::Change)
			{
				Image_NotFocus->SetVisibility(TestState == EWRFocusButtonState::None ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			}
			else
			{
				Image_NotFocus->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		if (Image_Focus)
		{
			Image_Focus->SetBrush(FocusButtonManualInfo.FocusBrush);
			Image_Focus->SetVisibility(TestState == EWRFocusButtonState::Focus ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
		if (Image_Select)
		{
			Image_Select->SetBrush(FocusButtonManualInfo.SelectBrush);
			Image_Select->SetVisibility(TestState == EWRFocusButtonState::Select ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}

	if (WRTextBlock_Contents)
	{
		WRTextBlock_Contents->SetText(DefaultString);
		WRTextBlock_Contents->SetFontRawName(*FontStyle);
	}

// 	UWRTextBlock* GettedTextblock = Cast<UWRTextBlock>(GetWidgetFromName("WRTextBlock_Contents"));
// 	if (GettedTextblock)
// 	{
// 		GettedTextblock->SetText(DefaultString);
// 		GettedTextblock->SetFontRawName(*FontStyle);
// 		//GettedTextblock->SetIsRichTextBlock(UseRichText);
// 	}
}

void UWRFocusButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Image_Focus)
		Image_Focus->SetBrush(FocusButtonManualInfo.FocusBrush);

	if (Image_NotFocus)
		Image_NotFocus->SetBrush(FocusButtonManualInfo.NotFocusBrush);

	if (Image_Select)
		Image_Select->SetBrush(FocusButtonManualInfo.SelectBrush);

//	BIND_WIDGET_WITH_IMPLEMENTATION(UButton, Button_Contents,
// 		Button_Contents->OnClicked.AddDynamic(this, &UWRFocusButton::OnClickedButton);
// 		Button_Contents->OnHovered.AddDynamic(this, &UWRFocusButton::OnHoveredButton);
// 		Button_Contents->OnUnhovered.AddDynamic(this, &UWRFocusButton::OnUnHoveredButton);
//	);

	CurScaleForZoom = 1.f;
	MaxScaleForZoom = ZoomInRate;
	ZoomSpeed = (MaxScaleForZoom - 1) / ZoomInTime;

	bUseWidgetAnimation = true;

	// Init Image
	if (bNotUsedNotFocusImg == false)
		WRUIUtility::SetVisibilitySafe(Image_NotFocus, ESlateVisibility::SelfHitTestInvisible);
}

void UWRFocusButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRFocusButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bZoomInFocus)
		Tick_Zoom(InDeltaTime);

	if (bUseTick == false)
		return;

	// Focus Change
	if (HasAnyUserFocus() || Button_Contents->HasAnyUserFocus())
	//if (Button_Contents->HasAnyUserFocus()) // if (HasFocusedDescendants())
	{
		if (bFocused == false)
		{
			bFocused = true;
			WRUIUtility::SetVisibilitySafe(Image_Focus, ESlateVisibility::SelfHitTestInvisible);
			WRUIUtility::SetVisibilitySafe(Image_NotFocus, ESlateVisibility::Hidden);

			if (bUseFocusAnim)
			{
				PlayAnimationByName("FocusIN", 0.f, 1, EUMGSequencePlayMode::Forward, FocusAnimPlayBackSpeed);
			}

			//====================================================================================
			// [ 2020-7-14 : kyu ] 
			/*
				FReply SButton::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) 에서
				FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept 로 GamePad_Down(X)가 Press로 입력되고 있음
				SButton, UButton으로 입력이 가고 Handled되어 UI의 KeyDown 이벤트로 오지 않음
				: FocusButton에서 Button에 포커스되면 FocusButton이 포커스를 가져가도록 수정함
				-> 추후 UButton 소유구조에서 상속구조로 변경해야 할듯 함
			*/
			if (UWRGameInstance::GetGameInstance() == nullptr)
				return;

			APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController == nullptr)
				return;

			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, this);
#if WITH_EDITOR || PLATFORM_WINDOWS
			WRUIManager::Get()->SaveFocusedUI(this);
#endif // WITH_EDITOR || PLATFORM_WINDOWS
			//====================================================================================

			OnFocusChanged.ExecuteIfBound(this, true);
		}
	}
	else
	{
		if (bFocused)
		{
			bFocused = false;
						
			if (bNotUsedNotFocusImg == false)
				WRUIUtility::SetVisibilitySafe(Image_NotFocus, ESlateVisibility::SelfHitTestInvisible);

			if (bUseFocusAnim)
			{
				PlayAnimationByName("FocusOUT", 0.f, 1, EUMGSequencePlayMode::Forward, FocusAnimPlayBackSpeed);
			}
			else
			{
				WRUIUtility::SetVisibilitySafe(Image_Focus, ESlateVisibility::Hidden);
			}

			OnFocusChanged.ExecuteIfBound(this, false);
		}
	}
}

FReply UWRFocusButton::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Handled();

	return FReply::Unhandled();
}

void UWRFocusButton::Tick_Zoom(const float IN InDeltaTime)
{
	if (bFocused)
	{
		if (CurScaleForZoom < MaxScaleForZoom)
		{
			CurScaleForZoom += ZoomSpeed * InDeltaTime;
			if (CurScaleForZoom > MaxScaleForZoom)
				CurScaleForZoom = MaxScaleForZoom;

			SetRenderScale(FVector2D(CurScaleForZoom));
		}
	}
	else
	{
		if (CurScaleForZoom > 1.f)
		{
			CurScaleForZoom -= ZoomSpeed * InDeltaTime;
			if (CurScaleForZoom < 1.f)
				CurScaleForZoom = 1;

			SetRenderScale(FVector2D(CurScaleForZoom));
		}
	}
}

void UWRFocusButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (WRTextBlock_Contents)
	{
		WRTextBlock_Contents->SetText(DefaultString);
		WRTextBlock_Contents->SetFontRawName(*FontStyle);
	}

// 	UWRTextBlock* GettedTextblock = Cast<UWRTextBlock>(GetWidgetFromName("WRTextBlock_Contents"));
// 	if (GettedTextblock)
// 	{
// 		GettedTextblock->SetText(DefaultString);
// 		GettedTextblock->SetFontRawName(*FontStyle);
// 		//GettedTextblock->SetIsRichTextBlock(UseRichText);
// 	}
}

void UWRFocusButton::SetContents(FString IN InContents)
{
	WRUIUtility::SetTextSafe(WRTextBlock_Contents, InContents);
}

void UWRFocusButton::SetSelect(const bool IN InSelect)
{
	if (InSelect)
		WRUIUtility::SetVisibilitySafe(Image_Select, ESlateVisibility::SelfHitTestInvisible);
	else
		WRUIUtility::SetVisibilitySafe(Image_Select, ESlateVisibility::Hidden);
}

void UWRFocusButton::SetFocusImg(UTexture2D* IN InTexture)
{
	Image_Focus->SetBrushFromTexture(InTexture);
	WRUIUtility::SetImageTintColorSafe(Image_Focus, FSlateColor(FLinearColor(1.f, 1.f, 1.f)));
}

void UWRFocusButton::SetNotFocusImg(UTexture2D* IN InTexture)
{
	Image_NotFocus->SetBrushFromTexture(InTexture);
	WRUIUtility::SetImageTintColorSafe(Image_NotFocus, FSlateColor(FLinearColor(1.f, 1.f, 1.f)));

	if (bNotUsedNotFocusImg == false && InTexture != nullptr)
	{
		Image_NotFocus->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWRFocusButton::SetSelectImg(UTexture2D* IN InTexture)
{
	Image_Select->SetBrushFromTexture(InTexture);
	WRUIUtility::SetImageTintColorSafe(Image_Select, FSlateColor(FLinearColor(1.f, 1.f, 1.f)));
}

void UWRFocusButton::SetFocusChangedDelegate(FOnFocusChanged& IN InDelegate)
{
	OnFocusChanged = InDelegate;
}

void UWRFocusButton::SetButtonContentAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	UPanelSlot* Slot = Button_Contents->GetContentSlot();
	CastChecked<UButtonSlot>(Slot)->SetHorizontalAlignment(InHorizontalAlignment);
}

void UWRFocusButton::SetFocusBrush(const FSlateBrush& IN InBrush)
{
	FocusButtonManualInfo.FocusBrush = InBrush;
	UImage* GettedImage = Cast<UImage>(GetWidgetFromName("Image_Focus"));
	if (GettedImage)
	{
		GettedImage->SetBrush(FocusButtonManualInfo.FocusBrush);
	}
}

void UWRFocusButton::SetNotFocusBrush(const FSlateBrush& IN InBrush)
{
	FocusButtonManualInfo.NotFocusBrush = InBrush;
	UImage* GettedImage = Cast<UImage>(GetWidgetFromName("Image_NotFocus"));
	if (GettedImage)
	{
		GettedImage->SetBrush(FocusButtonManualInfo.NotFocusBrush);
	}
}

void UWRFocusButton::SetSelectBrush(const FSlateBrush& IN InBrush)
{
	FocusButtonManualInfo.SelectBrush = InBrush;
	UImage* GettedImage = Cast<UImage>(GetWidgetFromName("Image_Select"));
	if (GettedImage)
	{
		GettedImage->SetBrush(FocusButtonManualInfo.SelectBrush);
	}
}

void UWRFocusButton::OnClickedButton()
{
	GetButton()->OnClicked.Broadcast();

	OnClicked.ExecuteIfBound(nIndex);
}

void UWRFocusButton::OnHoveredButton()
{
	// to WidgetInteraction
	if (bFocused == false)
	{
		bFocused = true;
		if (FocusUIType == EWRFocusUITypeInFocusButton::Change)
		{
			WRUIUtility::SetVisibilitySafe(Image_Focus, ESlateVisibility::SelfHitTestInvisible);
			WRUIUtility::SetVisibilitySafe(Image_NotFocus, ESlateVisibility::Hidden);
		}
		else if (FocusUIType == EWRFocusUITypeInFocusButton::Add)
		{
			WRUIUtility::SetVisibilitySafe(Image_Focus, ESlateVisibility::SelfHitTestInvisible);

			if (bNotUsedNotFocusImg == false)
			{
				WRUIUtility::SetVisibilitySafe(Image_NotFocus, ESlateVisibility::SelfHitTestInvisible);
			}
		}

		if (bUseFocusAnim)
		{
			PlayAnimationByName("FocusIN", 0.f, 1, EUMGSequencePlayMode::Forward, FocusAnimPlayBackSpeed);
		}
	}
}
void UWRFocusButton::OnUnHoveredButton()
{
	// to WidgetInteraction
	if (bFocused)
	{
		bFocused = false;
		
		if (bNotUsedNotFocusImg == false && Image_NotFocus->Brush.GetResourceObject()->IsValidLowLevel())
		{
			WRUIUtility::SetVisibilitySafe(Image_NotFocus, ESlateVisibility::SelfHitTestInvisible);
		}
		
		if (bUseFocusAnim)
		{
			PlayAnimationByName("FocusOUT", 0.f, 1, EUMGSequencePlayMode::Forward, FocusAnimPlayBackSpeed);
		}
		else
		{
			WRUIUtility::SetVisibilitySafe(Image_Focus, ESlateVisibility::Hidden);
		}
	}
}
