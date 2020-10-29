// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIInventoryDetailInfo.h"
#include "../../../Manager/DMUIManager.h"
#include "../../../Actor/WidgetActor/DMWidgetActorInventory.h"
#include "../../Custom/FocusButton/DMUIFocusButton.h"


void UDMUIInventoryDetailInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDMUIInventoryDetailInfo::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDMUIInventoryDetailInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UDMUIInventoryDetailInfo::SetFocusThis()
{
	SetFocusableInputMode(true, FocusButton_Equip);
}

FReply UDMUIInventoryDetailInfo::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply RetEvent = FReply::Unhandled();
	ADMWidgetActorInventory* WidgetActorInventory = DMUIManager::Get()->FindWidgetActor<ADMWidgetActorInventory>(EDMPanelKind::Inventory);
	if (WidgetActorInventory)
	{
		RetEvent = WidgetActorInventory->OnKeyEvent((int32)EDMWidgetActorInventory::DetailInfo, InKeyEvent.GetKey());
	}

	if (RetEvent.IsEventHandled())
		return RetEvent;
	else
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UDMUIInventoryDetailInfo::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
