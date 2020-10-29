// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIInventoryItemSlot.h"
#include "../../../Manager/DMUIManager.h"
#include "../../../Actor/WidgetActor/DMWidgetActorInventory.h"
#include "../../Data/DMInventoryItemSlotData.h"
#include "../../../Util/DMUIUtil.h"

void UDMUIInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDMUIInventoryItemSlot::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDMUIInventoryItemSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UDMUIInventoryItemSlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply RetEvent = FReply::Unhandled();
	ADMWidgetActorInventory* WidgetActorInventory = DMUIManager::Get()->FindWidgetActor<ADMWidgetActorInventory>(EDMPanelKind::Inventory);
	if (WidgetActorInventory)
	{
		RetEvent = WidgetActorInventory->OnKeyEvent((int32)EDMWidgetActorInventory::SlotList, InKeyEvent.GetKey());
	}

	if (RetEvent.IsEventHandled())
		return RetEvent;
	else
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UDMUIInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UDMUIInventoryItemSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UDMInventoryItemSlotDataObject* CastedDataObject = Cast<UDMInventoryItemSlotDataObject>(ListItemObject);
	if (CastedDataObject == nullptr)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (CastedDataObject->bIsEmpty)
	{
		Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		DMUIUtil::SetResourceObject(Image_Icon, CastedDataObject->Data.IconPath);
	}

	SetFocusState(false);
}

void UDMUIInventoryItemSlot::SetFocusState(const bool IN InSet)
{
	Image_Focus->SetVisibility(InSet ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}
