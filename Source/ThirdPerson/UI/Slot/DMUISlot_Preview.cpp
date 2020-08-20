// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_Preview.h"
#include "../../Manager/DMPreviewManager.h"
#include "Components/Image.h"
#include "../../PreviewStudio/DMPreviewStudio.h"


void UDMUISlot_Preview::NativeConstruct()
{
	Super::NativeConstruct();

	bool bCorrect = false;

//	if (Flags1 & ETestFlag1::A)					// 컴파일 에러 (일치하는 & 연산자 없음)
//	if ((ETestFlag1)Flags1 & ETestFlag1::A)		// 컴파일 에러 (일치하는 & 연산자 없음)
	if (Flags1 & (int32)ETestFlag1::A)
	{
		bCorrect = true;
	}
	ETestFlag1 Temp1;
//	Temp1 |= ETestFlag1::A;								// 컴파일 에러 (사용자 정의 연산자 함수 필요)
	Temp1 = ETestFlag1::None;
//	Temp1 = ETestFlag1::B | ETestFlag1::C;				// 컴파일 에러 (일치하는 | 연산자 없음)
//	if ((Temp1 & ETestFlag1::B) != ETestFlag1::None)	// 컴파일 에러 (일치하는 & 연산자 없음)
	{
		bCorrect = true;
	}

	

//	if (Flags2 & ETestFlag2::A)							// 컴파일 에러 (일치하는 & 연산자 없음)
//	if ((ETestFlag2)Flags2 & ETestFlag2::A)				// 컴파일 에러 (일치하는 & 연산자 없음)
	if (Flags2 & (int32)ETestFlag2::A)
	{
		bCorrect = true;
	}
	ETestFlag2 Temp2;
	Temp2 = ETestFlag2::None;
	Temp2 |= ETestFlag2::B;
	Temp2 = ETestFlag2::B | ETestFlag2::C;
//	if (Temp2 & ETestFlag2::B)							// 컴파일 에러 (bool로 변환할 수 있는 형식이여야한다)
	if ((Temp2 & ETestFlag2::B) != ETestFlag2::None)
	{
		bCorrect = true;
	}

	

//	if (Flags3 & ETestFlag3::A)							// 컴파일 에러 (일치하는 & 연산자 없음)
//	if ((ETestFlag3)Flags3 & ETestFlag3::A)				// 컴파일 에러 (일치하는 & 연산자 없음)
	if (Flags3 & (int32)ETestFlag3::A)
	{
		bCorrect = true;
	}
	ETestFlag3 Temp3;
	Temp3 = ETestFlag3::None;
	Temp3 |= ETestFlag3::B;
	Temp3 = ETestFlag3::B | ETestFlag3::C;
//	if (Temp3 & ETestFlag3::B)							// 컴파일 에러 (bool로 변환할 수 있는 형식이여야한다)
	if ((Temp3 & ETestFlag3::B) != ETestFlag3::None)
	{
		bCorrect = true;
	}



//	if (Flags4 & ETestFlag4::A)							// 컴파일 에러 (일치하는 & 연산자 없음)
//	if ((ETestFlag4)Flags4 & ETestFlag4::A)				// 컴파일 에러 (일치하는 & 연산자 없음)
	if (Flags4 & (int32)ETestFlag4::A)
	{
		bCorrect = true;
	}
	ETestFlag4 Temp4;
	Temp4 = ETestFlag4::None;
	Temp4 |= ETestFlag4::B;
	Temp4 = ETestFlag4::B | ETestFlag4::C;
//	if (Temp4 & ETestFlag4::B)							// 컴파일 에러 (bool로 변환할 수 있는 형식이여야한다)
	if ((Temp4 & ETestFlag4::B) != ETestFlag4::None)
	{
		bCorrect = true;
	}
	if ((int32)(Temp4 & ETestFlag4::B) > 0)
	{
		bCorrect = true;
	}


	ETestFlag5 Temp5;
	Temp5 = ETestFlag5::None;
	Temp5 |= ETestFlag5::B;
	Temp5 = ETestFlag5::B | ETestFlag5::C;
	if ((Temp5 & ETestFlag5::B) != ETestFlag5::None)
	{
		bCorrect = true;
	}
	if ((int32)(Temp5 & ETestFlag5::B) > 0)
	{
		bCorrect = true;
	}


	ETestBitFlagForBP TempForBP = ETestBitFlagForBP::A;
	ETestBitFlag::en TempFlag = ETestBitFlag::A;

	
	if (Image_BindTest)
	{
		bCorrect = true;
	}
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
