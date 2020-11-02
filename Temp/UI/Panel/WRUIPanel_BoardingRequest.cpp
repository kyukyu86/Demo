// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_BoardingRequest.h"
#include "Define/WRUIDefine.h"
#include "UI/CustomWidget/WRTextBlock.h"
#include "CanvasPanel.h"
#include "UI/CustomWidget/WRProgressBar.h"
#include "Manager/WRUIManager.h"

void UWRUIPanel_BoardingRequest::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Comment);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Accept);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Reject);
	BIND_WIDGET(UWRProgressBar, ProgressBar_Timer);
}

void UWRUIPanel_BoardingRequest::NativeDestruct()
{

	Super::NativeDestruct();
}

void UWRUIPanel_BoardingRequest::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ProgressBar_Timer)
		ProgressBar_Timer->OnTick(InDeltaTime);
}

void UWRUIPanel_BoardingRequest::SetBoardingType(const EWRBoardingRequestType IN InType, FWROnFinishedBoardingRequestTime IN InDelegate, const AID_t IN InPassengerAID /*= INVALID_AID*/, AID_t IN InVehicleAID /*= INVALID_AID*/, int8 IN InSeatIndex /*= INVALID_SEAT_INDEX*/)
{
	TimeFinishedDelegate = InDelegate;
	PassengerAID = InPassengerAID;
	VehicleAID = InVehicleAID;
	SeatIndex = InSeatIndex;

	FWROnProgressCompleted Delegate;
	Delegate.BindUObject(this, &UWRUIPanel_BoardingRequest::OnCompleteProgress);
	ProgressBar_Timer->SetTimer(RequestTime, Delegate);

	switch (InType)
	{
	case EWRBoardingRequestType::Driver:
	{
		SetBoardingContents(DriverContents);
	}
	break;
	case EWRBoardingRequestType::Passenger:
	{
		SetBoardingContents(PassengerContents);

		CanvasPanel_Accept->SetVisibility(ESlateVisibility::Collapsed);
		CanvasPanel_Reject->SetVisibility(ESlateVisibility::Collapsed);
	}
	break;
	}
}

void UWRUIPanel_BoardingRequest::SetBoardingContents(const FText IN InContents)
{
	WRTextBlock_Comment->SetText(InContents);
}

void UWRUIPanel_BoardingRequest::OnCompleteProgress()
{
	if (TimeFinishedDelegate.IsBound())
	{
		TimeFinishedDelegate.ExecuteIfBound(PassengerAID, VehicleAID, SeatIndex);
	}
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::BoardingRequest);
}
