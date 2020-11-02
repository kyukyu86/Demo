// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_Preview.h"

#include "Core/WRGameInstance.h"
#include "WidgetBlueprintLibrary.h"

#include "Define/WRUIDefine.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Manager/WRPreviewManager.h"
#include "Utility/WRUIUtility.h"
#include "Enum/EWRCharacter.h"


void UWRUISlot_Preview::NativeConstruct()
{
	Super::NativeConstruct();
		
}

void UWRUISlot_Preview::NativeDestruct()
{
	WRPreviewManager::Get()->ReleasePreview(this, true);

	Super::NativeDestruct();
}

void UWRUISlot_Preview::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUISlot_Preview::OnExit()
{
	WRPreviewManager::Get()->ReleasePreview(this, false);

	SetVisibility(ESlateVisibility::Collapsed);
}

FReply UWRUISlot_Preview::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (ConvertedInputType != EWRInput::None)
	{
		switch (ConvertedInputType)
		{
		case EWRInput::RStickRight:
		{
			if (PreviewStudio->OnRotateStart(false))
			{
				return FReply::Handled();
			}
		}
		break;
		case EWRInput::RStickLeft:
		{
			if (PreviewStudio->OnRotateStart(true))
			{
				return FReply::Handled();
			}
		}
		break;
		case EWRInput::LStickUp:
		{
			if (PreviewStudio->OnZoomIn())
			{
				return FReply::Handled();
			}
		}
		break;
		case EWRInput::LStickDown:
		{
			if (PreviewStudio->OnZoomOut())
			{
				return FReply::Handled();
			}
		}
		break;
// 		case EWRInput::Option:
// 		{
// 			if (PreviewStudio->OnZoomOut())
// 			{
// 				return FReply::Handled();
// 			}
// 		}
// 		break;
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UWRUISlot_Preview::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (ConvertedInputType != EWRInput::None)
	{
		switch (ConvertedInputType)
		{
		case EWRInput::RStickRight:
		{
			if (PreviewStudio->OnRotateEnd())
			{
				return FReply::Handled();
			}
		}
		break;
		case EWRInput::RStickLeft:
		{
			if (PreviewStudio->OnRotateEnd())
			{
				return FReply::Handled();
			}
		}
		break;
		case EWRInput::LStickUp:
		{
			if (PreviewStudio->OnZoomEnd())
			{
				return FReply::Handled();
			}
		}
		break;
		case EWRInput::LStickDown:
		{
			if (PreviewStudio->OnZoomEnd())
			{
				return FReply::Handled();
			}
		}
		break;
// 		case EWRInput::Option:
// 		{
// 			if (PreviewStudio->OnZoomOut())
// 			{
// 				return FReply::Handled();
// 			}
// 		}
// 		break;
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UWRUISlot_Preview::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return FReply::Unhandled();

	FVector2D vStart = InMouseEvent.GetScreenSpacePosition();
	PreviewStudio->OnInputStart(vStart);

	return FReply::Handled();
}

FReply UWRUISlot_Preview::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return FReply::Unhandled();

	PreviewStudio->OnInputEnd();

	return FReply::Handled();
}

FReply UWRUISlot_Preview::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return FReply::Unhandled();

	FVector2D vMove = InMouseEvent.GetScreenSpacePosition();
	if(PreviewStudio->OnInputMove(vMove) == false)
		return FReply::Unhandled();

	return FReply::Handled();
}

void UWRUISlot_Preview::OnShow()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (PreviewData.UseZoom || PreviewData.UseRotate)
	{
		SetFocusableInputMode(true, this);
	}
}

void UWRUISlot_Preview::OnRotateToLeft()
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnRotateStart(true);
}

void UWRUISlot_Preview::OnRotateToRight()
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnRotateStart(false);
}

void UWRUISlot_Preview::OnRotateEnd()
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnRotateEnd();
}

void UWRUISlot_Preview::OnZoomInStart()
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnZoomIn();
}

void UWRUISlot_Preview::OnZoomOutStart()
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnZoomOut();
}

void UWRUISlot_Preview::OnZoomEnd()
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnZoomEnd();
}
