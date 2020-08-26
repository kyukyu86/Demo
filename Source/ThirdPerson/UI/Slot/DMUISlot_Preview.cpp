// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_Preview.h"
#include "../../Manager/DMPreviewManager.h"
#include "Components/Image.h"
#include "../../PreviewStudio/DMPreviewStudio.h"


void UDMUISlot_Preview::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UDMUISlot_Preview::NativeDestruct()
{
	DMPreviewManager::Get()->ReleasePreview(this, true);

	Super::NativeDestruct();
}

void UDMUISlot_Preview::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UDMUISlot_Preview::OnExit()
{
	DMPreviewManager::Get()->ReleasePreview(this, false);

	SetVisibility(ESlateVisibility::Collapsed);
}

FReply UDMUISlot_Preview::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::S)
	{
		if (PreviewStudio->OnZoomOut())
		{
			return FReply::Handled();
		}
	}
	else if (InKeyEvent.GetKey() == EKeys::W)
	{
		if (PreviewStudio->OnZoomIn())
		{
			return FReply::Handled();
		}
	}
	else if (InKeyEvent.GetKey() == EKeys::A)
	{
		if (PreviewStudio->OnRotateStart(true))
		{
			return FReply::Handled();
		}
	}
	else if (InKeyEvent.GetKey() == EKeys::D)
	{
		if (PreviewStudio->OnRotateStart(false))
		{
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UDMUISlot_Preview::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::S)
	{
		if (PreviewStudio->OnZoomEnd())
		{
			return FReply::Handled();
		}
	}
	else if (InKeyEvent.GetKey() == EKeys::W)
	{
		if (PreviewStudio->OnZoomEnd())
		{
			return FReply::Handled();
		}
	}
	else if (InKeyEvent.GetKey() == EKeys::A)
	{
		if (PreviewStudio->OnRotateEnd())
		{
			return FReply::Handled();
		}
	}
	else if (InKeyEvent.GetKey() == EKeys::D)
	{
		if (PreviewStudio->OnRotateEnd())
		{
			return FReply::Handled();
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UDMUISlot_Preview::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return FReply::Unhandled();

	FVector2D vStart = InMouseEvent.GetScreenSpacePosition();
	PreviewStudio->OnInputStart(vStart);

	return FReply::Handled();
}

FReply UDMUISlot_Preview::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return FReply::Unhandled();

	PreviewStudio->OnInputEnd();

	return FReply::Handled();
}

FReply UDMUISlot_Preview::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return FReply::Unhandled();

	FVector2D vMove = InMouseEvent.GetScreenSpacePosition();
	if (PreviewStudio->OnInputMove(vMove) == false)
		return FReply::Unhandled();

	return FReply::Handled();
}

void UDMUISlot_Preview::OnShow()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (PreviewData.UseZoom || PreviewData.UseRotate)
	{
		SetFocusableInputMode(true, this);
	}
}

void UDMUISlot_Preview::OnRotateToLeft()
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnRotateStart(true);
}

void UDMUISlot_Preview::OnRotateToRight()
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnRotateStart(false);
}

void UDMUISlot_Preview::OnRotateEnd()
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnRotateEnd();
}

void UDMUISlot_Preview::OnZoomInStart()
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnZoomIn();
}

void UDMUISlot_Preview::OnZoomOutStart()
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnZoomOut();
}

void UDMUISlot_Preview::OnZoomEnd()
{
	ADMPreviewStudio* PreviewStudio = DMPreviewManager::Get()->GetPreviewStudio(this);
	if (PreviewStudio == nullptr)
		return;

	PreviewStudio->OnZoomEnd();
}
