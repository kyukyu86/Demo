// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryItemSlot.h"
#include "WRUISlot_ItemIcon.h"
#include "Define/WRUIDefine.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"
#include "Item/WRItem.h"

#include "Manager/WRInventoryManager.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRVehicleManager.h"
#include "Manager/WRItemManager.h"



void UWRUISlot_InventoryItemSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

	// for Designer
	UImage* GettedImage = Cast<UImage>(GetWidgetFromName("Image_Default"));
	if (GettedImage)
	{
		GettedImage->SetBrushFromTexture(DefaultImage);
	}
	// for Designer
	UWRFocusButton* GettedFocusButton = Cast<UWRFocusButton>(GetWidgetFromName("FocusButton"));
	if (GettedFocusButton)
	{
		GettedFocusButton->SetFocusBrush(FocusButtonManualInfo.FocusBrush);
		GettedFocusButton->SetNotFocusBrush(FocusButtonManualInfo.NotFocusBrush);
		GettedFocusButton->SetSelectBrush(FocusButtonManualInfo.SelectBrush);
	}
}

void UWRUISlot_InventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	BIND_WIDGET(UWRUISlot_ItemIcon, Slot_ItemIcon);
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton, 
		FocusButton->GetButton()->OnClicked.Clear();
		FocusButton->SetFocusBrush(FocusButtonManualInfo.FocusBrush);
		FocusButton->SetNotFocusBrush(FocusButtonManualInfo.NotFocusBrush);
		FocusButton->SetSelectBrush(FocusButtonManualInfo.SelectBrush);
	);
	BIND_WIDGET(UImage, Image_Default);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_DefaultText);
	BIND_WIDGET(UImage, Image_Equipped);
	BIND_WIDGET(UImage, Image_New);
	BIND_WIDGET(UImage, Image_Registered);
	BIND_WIDGET_WITH_IMPLEMENTATION(UImage, Image_Selectable, Image_Selectable->SetVisibility(ESlateVisibility::Collapsed););
	
	if (bUseDefaultImage)
	{
		Image_Default->SetBrushFromTexture(DefaultImage);		
	}
	else
	{
		Image_Default->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

void UWRUISlot_InventoryItemSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

FReply UWRUISlot_InventoryItemSlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Handled();

	if (WRInventoryManager::Get()->OnInputEventInventorySlot(InKeyEvent.GetKey(), SlotInfo))
		return FReply::Handled();

	return FReply::Unhandled();
}

void UWRUISlot_InventoryItemSlot::SetEmptySlot()
{
	Slot_ItemIcon->SetVisibility(ESlateVisibility::Collapsed);

	// Default Image
	if (bUseDefaultImage)
		Image_Default->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else
		Image_Default->SetVisibility(ESlateVisibility::Collapsed);

	// Default Name
	EWRInventoryCategoryType MainCategoryType = WRInventoryManager::Get()->GetCurrentInventoryMainCategory();
	switch (MainCategoryType)
	{
	case EWRInventoryCategoryType::Equipment:
	{
		WRTextBlock_DefaultText->SetText(SlotInfo.EquipmentSlotName);
	}
	break;
	case EWRInventoryCategoryType::Costume:
	{
		WRTextBlock_DefaultText->SetText(SlotInfo.CostuneSlotName);
	}
	break;
	}

	WRTextBlock_DefaultText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// Other Icons
	Image_Equipped->SetVisibility(ESlateVisibility::Collapsed);
	Image_New->SetVisibility(ESlateVisibility::Collapsed);
	Image_Registered->SetVisibility(ESlateVisibility::Collapsed);
	//Image_Selectable->SetVisibility(ESlateVisibility::Collapsed); 
	CanvasPanel_Spawned->SetVisibility(ESlateVisibility::Collapsed);
	
	SlotInfo.bEmptySlot = true;
	SlotInfo.ISN = INVALID_ISN;
	SlotInfo.TID = INVALID_TID;
}

void UWRUISlot_InventoryItemSlot::SetItemIcon(FWRItemIcon& IN InInfo)
{
	// Hide Default
	Image_Default->SetVisibility(ESlateVisibility::Collapsed);
	WRTextBlock_DefaultText->SetVisibility(ESlateVisibility::Collapsed);

	// Set Info
	Slot_ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Slot_ItemIcon->SetIcon(InInfo);

	CanvasPanel_Spawned->SetVisibility(InInfo.bSpawned ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UWRUISlot_InventoryItemSlot::SetItemIcon(const WRTableID IN InTID)
{
	if (InTID == INVALID_TID)
	{
		SetEmptySlot();
	}
	else
	{
	/*	FWRItemTable* FoundItemTable = WRTableManager::Get()->FindRow<FWRItemTable>(InTID);
		if (FoundItemTable == nullptr)
			return;

		FWRItemIcon ItemIconInfo;
		ItemIconInfo.strIconPath = FoundItemTable->IconPath;

		SetItemIcon(ItemIconInfo);

		// Save Slot Info
		SlotInfo.bEmptySlot = false;
		SlotInfo.ISN = INVALID_ISN;
		SlotInfo.TID = InTID;*/
	}
}

void UWRUISlot_InventoryItemSlot::SetItemIcon(class WRItemBase* IN InItemBase)
{
	if (InItemBase == nullptr)
	{
		SetEmptySlot();
	}
	else
	{
		FWRItemTableBase* FoundItemTable = WRTableManager::Get()->FindRowInItemTable(InItemBase->GetTID());

		if (FoundItemTable == nullptr)
			return;

		FWRItemIcon ItemIconInfo;
		ItemIconInfo.strIconPath = FoundItemTable->IconPath;
		if (InItemBase->IsConsume())
		{
			WRConsumeItem* CastedConsumeItem = static_cast<WRConsumeItem*>(InItemBase);
			if (CastedConsumeItem)
			{
				ItemIconInfo.bUseCount = true;
				ItemIconInfo.ItemCount = CastedConsumeItem->GetQuantity();
			}
		}
		if (InItemBase->IsVehicle())
		{
			if (WRVehicleManager::Get()->IsValidSapwnedVehicleISN(InItemBase->GetISN()))
			{
				ItemIconInfo.bSpawned = true;
			}
		}
		SetItemIcon(ItemIconInfo);

		// Equip State
		if (InItemBase->IsEquipped())
		{
			Image_Equipped->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			Image_Equipped->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		// Regist State
		bool bRegistered = false;
		if (WRInventoryManager::Get()->IsRegisteredItem(InItemBase->GetISN()))
		{
			Image_Registered->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			bRegistered = true;
		}
		else
		{
			Image_Registered->SetVisibility(ESlateVisibility::Collapsed);
		}

		// New State
		if (bRegistered == false && WRInventoryManager::Get()->IsNewItem(InItemBase->GetISN()))
		{
			Image_New->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			Image_New->SetVisibility(ESlateVisibility::Collapsed);
		}

		// Save Slot Info
		SlotInfo.bEmptySlot = false;
		SlotInfo.ISN = InItemBase->GetISN();
		SlotInfo.TID = InItemBase->GetTID();
	}	
}

void UWRUISlot_InventoryItemSlot::SetSelect(const bool IN InSet)
{
	FocusButton->SetSelect(InSet);
}

void UWRUISlot_InventoryItemSlot::SetSelectable(const bool IN InSet)
{
	if (Image_Selectable)
	{
		if (InSet)
		{
			Image_Selectable->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			PlayAnimationByName("Selectable", 0.f, 0);
		}
		else
		{
			Image_Selectable->SetVisibility(ESlateVisibility::Collapsed);
			StopAnimationByName("Selectable");
		}
	}
}

void UWRUISlot_InventoryItemSlot::UpdateSlot()
{
	WRItemBase* FoundItemBase = WRItemManager::Get()->FindItem(GetISN());
	SetItemIcon(FoundItemBase);
}

void UWRUISlot_InventoryItemSlot::OnClickedSlot()
{
	WRInventoryManager::Get()->OpenItemInformation(SlotInfo.ISN);
}
