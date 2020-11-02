// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Inventory.h"

#include "UI/Slot/WRUISlot_InventorySetList.h"
#include "UI/Slot/WRUISlot_InventoryItemInfo.h"
#include "UI/Slot/WRUISlot_InventoryStorage.h"
#include "UI/Slot/WRUISlot_InventoryCategory.h"
#include "UI/Slot/WRUISlot_InventoryEquipList.h"
#include "UI/Slot/WRUISlot_InventoryItemSimpleInfo.h"
#include "UI/Slot/WRUISlot_InventoryInnerCategory.h"
#include "UI/Slot/WRUISlot_InventoryPriceInfo.h"

#include "UI/CustomWidget/WRTextBlock.h"

#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Manager/WRInventoryManager.h"
#include "Manager/WRUIPathManager.h"

#include "Define/WRUIDefine.h"

#include "Overlay.h"
#include "WidgetSwitcher.h"
#include "Item/WRItem.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRCurrencyManager.h"
#include "Manager/WRItemManager.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRVehicleManager.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUIPanel_Inventory::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetFocusableInputMode(true, this);

	//WRUIManager::Get()->SetDOF(1);
}

void UWRUIPanel_Inventory::NativeDestruct()
{
	SetFocusableInputMode(false);

	//WRUIManager::Get()->SetDOF(0);

	Super::NativeDestruct();
}

void UWRUIPanel_Inventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUIPanel_Inventory::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation->MovieScene->GetFName() == FName("CloseInvenAnimation"))
	{
		// ´ÝÈû ¿¬Ãâ ÈÄ ²û
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::Inventory);
		SetFocusableInputMode(false);

		WRInventoryManager::Get()->OnCompletedInventoryClose();
	}
	else if (Animation->MovieScene->GetFName() == FName("OpenContentsAnimation"))
	{
		// ¿­¸² ¿¬Ãâ ÈÄ Storage ¿ÀÇÂ ¿¬Ãâ
		Slot_InventoryStorage->PlayAnimationByName("OpenAnimation");
	}
}

FReply UWRUIPanel_Inventory::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
// 	+ ActionMappings = (ActionName = "Cross", bShift = False, bCtrl = False, bAlt = False, bCmd = False, Key = Gamepad_FaceButton_Bottom)
// 	+ ActionMappings = (ActionName = "Triangle", bShift = False, bCtrl = False, bAlt = False, bCmd = False, Key = Gamepad_FaceButton_Top)
// 	+ ActionMappings = (ActionName = "Square", bShift = False, bCtrl = False, bAlt = False, bCmd = False, Key = Gamepad_FaceButton_Left)
// 	+ ActionMappings = (ActionName = "Circle", bShift = False, bCtrl = False, bAlt = False, bCmd = False, Key = Gamepad_FaceButton_Right)
	
	if (InKeyEvent.IsRepeat())
	{
		return FReply::Handled();
	}

 	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	switch (ConvertedInputType)
	{
	case EWRInput::R3:
	{
		WRInventoryManager::Get()->CloseInventory();
		return FReply::Handled();
	}
	break;
	}

	return FReply::Unhandled();
}

FReply UWRUIPanel_Inventory::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	if (InAnalogEvent.GetKey() == EKeys::Gamepad_Special_Left_X)
	{
		float fValue = InAnalogEvent.GetAnalogValue();
		if (fValue != 0.f)
		{			
			if (TouchPadStartInputValueX == 0.f)
			{
				TouchPadStartInputValueX = fValue;
			}
			else
			{
				TouchPadLastInputValueX = fValue;
			}
		}
		else
		{
			// ÀÔ·Â Á¾·á	
			if (FMath::Abs(TouchPadStartInputValueX - TouchPadLastInputValueX) > TouchPadSensitivity)
			{
				if (TouchPadStartInputValueX > TouchPadLastInputValueX)
				{
					// to the left
					WRInventoryManager::Get()->OnInputEvent(EWRInput::TouchPadDragLeftX);
				}
				else
				{
					// to the right
					WRInventoryManager::Get()->OnInputEvent(EWRInput::TouchPadDragRightX);
				}
				TouchPadStartInputValueX = 0.f;
				TouchPadLastInputValueX = 0.f;
			}
		}
	}
	return FReply::Unhandled();
}

void UWRUIPanel_Inventory::SetInventorySwitcher(const EWRInventoryMainSwitcher IN InSwitcher)
{
	WidgetSwitcher_Main->SetActiveWidgetIndex((int32)InSwitcher);

	switch (InSwitcher)
	{
	case EWRInventoryMainSwitcher::Category:
	{
		PlayAnimationByName("OpenCategoryAnimation");				
	}
	break;
	case EWRInventoryMainSwitcher::Contents:
	{
		UpdateGoldInfo();

		// Main Direction
		PlayAnimationByName("OpenContentsAnimation");

		// Middle Direction
		EWRInventoryMiddleType InventoryMiddleType = WRInventoryManager::Get()->GetInventoryMiddleType();
		switch (InventoryMiddleType)
		{
		case EWRInventoryMiddleType::EquipList:
		{
			Slot_InventoryEquipList->PlayAnimationByName("OpenAnimation");	// ÇÔ²² ¿ÀÇÂ ¿¬Ãâ
		}
		break;
		case EWRInventoryMiddleType::InnerCategory:
		{
			Slot_InventoryInnerCategory->PlayAnimationByName("OpenAnimation");	// ÇÔ²² ¿ÀÇÂ ¿¬Ãâ
		}
		break;
		}

		Slot_InventoryStorage->HideAllSlot(); // ¿¬ÃâÀ» À§ÇØ ½½·Ô ¼û±è
	}
	break;
	}
}

void UWRUIPanel_Inventory::SetInventoryMiddleType(const EWRInventoryMiddleType IN InType)
{
	WidgetSwitcher_Middle->SetActiveWidgetIndex((int32)InType);

}

void UWRUIPanel_Inventory::SetTitle(const FString& IN InTitle)
{
	WRTextBlock_Title->SetText(InTitle);
}

void UWRUIPanel_Inventory::CloseInventory()
{
	if (IsAnimationPlayingByName("CloseInvenAnimation") == false)
	{
		PlayAnimationByName("CloseInvenAnimation");
	}
}

void UWRUIPanel_Inventory::OpenItemInformation(const ISN_t IN InISN)
{
	if (InISN == INVALID_ISN)
		return;

	if (Slot_InventoryItemInfo->IsValidLowLevel())
	{
		Slot_InventoryItemInfo->SetInfo(InISN);
	}
	else
	{
		auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
		{
			AsyncInfo.Key = "";

			if (Overlay_ItemInfo == nullptr)
				return;

			Slot_InventoryItemInfo = Cast<UWRUISlot_InventoryItemInfo>(InObject);
			if (Slot_InventoryItemInfo == nullptr)
				return;

			Overlay_ItemInfo->AddChildToOverlay(Slot_InventoryItemInfo);
			Overlay_ItemInfo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			Slot_InventoryItemInfo->SetInfo(AsyncInfo.Value);
			//Slot_InventoryItemInfo->SetInfoAttached(AsyncInfo.Value);

			AsyncInfo.Value = INVALID_ISN;

			WRInventoryManager::Get()->PlaySound(EWRInventorySoundType::OpenItemInfo);
		});
		AsyncInfo.Key = WRAsyncLoadUObjectManager::Get()->AsyncCreateWidget(WRUIPathManager::Get()->GetUIPath(EWRUISlotKind::InventoryItemInfo), AsyncCreateComplete);
		AsyncInfo.Value = InISN;
	}	
	
	// Hide List
	WidgetSwitcher_Middle->SetVisibility(ESlateVisibility::Collapsed);
	Slot_InventoryStorage->SetVisibility(ESlateVisibility::Collapsed);
	Slot_InventoryItemSimpleInfo->SetVisibility(ESlateVisibility::Collapsed);
	Slot_InventoryPriceInfo->SetVisibility(ESlateVisibility::Collapsed);
	CanvasPanel_Gold->SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_Inventory::CloseItemInformation()
{
	if (AsyncInfo.Key.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncInfo.Key);
	}
	else
	{
		if (Slot_InventoryItemInfo->IsValidLowLevel())
		{
			Slot_InventoryItemInfo->Release();
			Overlay_ItemInfo->RemoveChild(Slot_InventoryItemInfo);
			Overlay_ItemInfo->SetVisibility(ESlateVisibility::Collapsed);
			Slot_InventoryItemInfo = nullptr;

			WRInventoryManager::Get()->PlaySound(EWRInventorySoundType::CloseItemInfo);
		}
	}

	// Show List
	WidgetSwitcher_Middle->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Slot_InventoryStorage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Slot_InventoryItemSimpleInfo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Slot_InventoryPriceInfo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_Gold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUIPanel_Inventory::UpdateControlGuide()
{
	EWRInventoryMainSwitcher MainSwitcher = WRInventoryManager::Get()->GetReserveInventoryMainSwitcher();
	if (MainSwitcher != EWRInventoryMainSwitcher::Contents)
		return;

	CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::Collapsed);

	EWRInventorySlotAction CurSlotAction = WRInventoryManager::Get()->GetInventorySlotAction();
	switch (CurSlotAction)
	{
	case EWRInventorySlotAction::RegisterToSelectedRegistrationSlot:
	case EWRInventorySlotAction::RegisterWhenStorageSelecting:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_RegistKey"));
	}
	break;
	case EWRInventorySlotAction::UnRegister:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_UnregistKey"));
	}
	break;
	case EWRInventorySlotAction::Equip:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_EquipKey"));
	}
	break;
	case EWRInventorySlotAction::UnEquip:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_UnequipKey"));
	}
	break;
	case EWRInventorySlotAction::SpawnVehicle:
	case EWRInventorySlotAction::SpawnInterior:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SpawnKey"));
	}
	break;
	case EWRInventorySlotAction::UnSpawnVehicle:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_UnspawnKey"));
	}
	break;
	case EWRInventorySlotAction::Use:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_UseKey"));
	}
	break;
	case EWRInventorySlotAction::GotoStorage:
	case EWRInventorySlotAction::GotoEquipList:
	{
		CanvasPanel_CircleGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_CircleGuide->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey"));
	}
	break;
	}
}

void UWRUIPanel_Inventory::UpdateGoldInfo()
{
	// + Money Info
	GAME_MONEY_t CurrentGold = WRCurrencyManager::Get()->FindGameMoneyCurrencyValue(EWRCurrencyType::Gold);
	WRTextBlock_Gold->SetText(FString::FormatAsNumber((int32)CurrentGold));
}