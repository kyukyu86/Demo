// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryItemInfo.h"
#include "Define/WRUIDefine.h"
#include "UI/CustomWidget/WRTextBlock.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "WRUISlot_Preview.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRInventoryManager.h"
#include "Item/WRItem.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"
#include "Table/WREquipmentLookTable.h"
#include "Manager/WRPreviewManager.h"
#include "CanvasPanel.h"
#include "Image.h"
#include "Manager/WRItemManager.h"
#include "Table/WRObjectTable.h"
#include "Table/WRVehicleTable.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WRAccountManager.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUISlot_InventoryItemInfo::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Name);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Description);
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_Preview, Slot_Preview, Slot_Preview->SetVisibility(ESlateVisibility::Collapsed););

	int32 nIndex = 0;
	bool bCreating = true;
	while (bCreating)
	{
		FString WidgetName = FString::Printf(TEXT("CanvasPanel_Ability_%d"), nIndex);
		UCanvasPanel* pCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(FName(*WidgetName)));
		if (pCanvasPanel == nullptr)
		{
			bCreating = false;
		}
		else
		{
			WidgetName = FString::Printf(TEXT("Image_State_%d"), nIndex);
			UImage* pImage = Cast<UImage>(GetWidgetFromName(FName(*WidgetName)));

			WidgetName = FString::Printf(TEXT("WRTextBlock_AbilityName_%d"), nIndex);
			UWRTextBlock* pTextBlockName = Cast<UWRTextBlock>(GetWidgetFromName(FName(*WidgetName)));
						
			WidgetName = FString::Printf(TEXT("WRTextBlock_AbilityValue_%d"), nIndex);
			UWRTextBlock* pTextBlockValue = Cast<UWRTextBlock>(GetWidgetFromName(FName(*WidgetName)));

			FWRInventoryItemInfoAbility Ability;
			Ability.CanvasPanel = pCanvasPanel;
			Ability.Image_State = pImage;
			Ability.WRTextBlock_AbilityName = pTextBlockName;
			Ability.WRTextBlock_AbilityValue = pTextBlockValue;
			AbilityWidgetList.Add(Ability);

			++nIndex;
		}
	}

	SetFocusableInputMode(true, this);
}

void UWRUISlot_InventoryItemInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

FReply UWRUISlot_InventoryItemInfo::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Slot_Preview->NativeOnKeyDown(InGeometry, InKeyEvent);
	if (Reply.IsEventHandled())
		return Reply;
	
	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (ConvertedInputType != EWRInput::None)
	{
		switch (ConvertedInputType)
		{
		case EWRInput::Cross:
		{
			// 돌아가기
			WRInventoryManager::Get()->CloseItemInformation();
			return FReply::Handled();
		}
		break;
		}
	}

	return FReply::Unhandled();
}

FReply UWRUISlot_InventoryItemInfo::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Slot_Preview->NativeOnKeyUp(InGeometry, InKeyEvent);
}

FReply UWRUISlot_InventoryItemInfo::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InAnalogEvent.GetKey());
	switch (ConvertedInputType)
	{
	case EWRInput::RStickX:
	{
		float fValue = InAnalogEvent.GetAnalogValue();
		if (fValue < (RStickSensitivity * -1.f))
		{
			Slot_Preview->OnRotateToLeft();
		}
		else if (fValue > RStickSensitivity)
		{
			Slot_Preview->OnRotateToRight();
		}
		else
		{
			Slot_Preview->OnRotateEnd();
		}
	}
	break;

	case EWRInput::LStickY:
	{
		float fValue = InAnalogEvent.GetAnalogValue();
		if (fValue < (RStickSensitivity * -1.f))
		{
			Slot_Preview->OnZoomOutStart();
		}
		else if (fValue > RStickSensitivity)
		{
			Slot_Preview->OnZoomInStart();
		}
		else
		{
			Slot_Preview->OnZoomEnd();
		}
	}
	break;
	}

	return FReply::Unhandled();
}

void UWRUISlot_InventoryItemInfo::Release()
{
	WRPreviewManager::Get()->ReleasePreview(Slot_Preview, true);
}

void UWRUISlot_InventoryItemInfo::SetInfo(const ISN_t IN InISN)
{
	if (InISN == INVALID_ISN)
		return;

	WRItemBase* FoundItem = WRItemManager::Get()->FindItem(InISN);
	if (FoundItem == nullptr)
		return;

	FWRItemTableBase* FoundItemTableBase = WRTableManager::Get()->FindRowInItemTable(FoundItem->GetTID());
	if (FoundItemTableBase == nullptr)
		return;

	// + Description
	FString NameLocalKey = WRUIUtility::ConvertFNameToFString(FoundItemTableBase->NameLocalKey);
	WRTextBlock_Name->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, NameLocalKey));

	FString MainDescription = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, FoundItemTableBase->MainDescription).ToString();
	FString SubDescription = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, FoundItemTableBase->SubDescription).ToString();
	FString StrDescription = MainDescription + "\n" + SubDescription;
	WRTextBlock_Description->SetText(StrDescription);

	// Option
	SetOption();

	// Preview
	FWRPreviewInfo PreviewInfo;

	switch (FoundItemTableBase->MainType)
	{
	case EWRItemMainType::Weapon:
	case EWRItemMainType::Costume:
	{
		FWREquipmentLookTable* FoundEquipLookTable = nullptr;
		if (FoundItemTableBase->MainType == EWRItemMainType::Weapon)
		{
			FWRWeaponItemTable* CastedItemTable = static_cast<FWRWeaponItemTable*>(FoundItemTableBase);
			FoundEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(CastedItemTable->EquipmentLookTID);			
		}
		else if (FoundItemTableBase->MainType == EWRItemMainType::Costume)
		{
			FWRCostumeItemTable* CastedItemTable = static_cast<FWRCostumeItemTable*>(FoundItemTableBase);
			FoundEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(CastedItemTable->EquipmentLookTID);
		}

		if (FoundEquipLookTable == nullptr)
			return;

		if (FoundEquipLookTable->StaticMesh.IsEmpty() == false)
		{
			PreviewInfo.Type = EWRPreviewType::Static;
			PreviewInfo.BPPath = FoundEquipLookTable->StaticMesh;
		}
		else if (FoundEquipLookTable->SkeletalMesh.IsEmpty() == false)
		{
			PreviewInfo.Type = EWRPreviewType::Skeletal;
			PreviewInfo.BPPath = FoundEquipLookTable->SkeletalMesh;
		}
	}
	break;
	case EWRItemMainType::Material:
	{
		// 쓰레기
	}
	break;
	case EWRItemMainType::Interior:
	{
		FWRInteriorItemTable* CastedItemTable = static_cast<FWRInteriorItemTable*>(FoundItemTableBase);
		FWRInteractionObjectTable* FoundInteractionObjectTable = WRTableManager::Get()->FindRow<FWRInteractionObjectTable>(CastedItemTable->ObjectTID);
		if (FoundInteractionObjectTable == nullptr)
			return;

		PreviewInfo.Type = EWRPreviewType::DefaultActor;
		PreviewInfo.BPPath = FoundInteractionObjectTable->MainBPPath;
	}
	break;
	case EWRItemMainType::Vehicle:
	{
		FWRVehicleItemTable* CastedItemTable = static_cast<FWRVehicleItemTable*>(FoundItemTableBase);
		FWRVehicleTable* VehicleTable = WRTableManager::Get()->FindRow<FWRVehicleTable>(CastedItemTable->VehicleTID);
		if (VehicleTable == nullptr)
			return;

		PreviewInfo.Type = EWRPreviewType::DefaultActor;
		PreviewInfo.BPPath = VehicleTable->BlueprinTid;
	}
	break;
	}

	PreviewInfo.SetPreviewWidget(Slot_Preview);
	PreviewInfo.ModifyBaseData(FoundItemTableBase->Preview);
	//PreviewInfo.PreviewExitDelegate = FWRPreviewExitDelegate::CreateUObject(this, &UWRUISlot_InventoryItemInfo::OnFocus);
	WRPreviewManager::Get()->SetPreview(PreviewInfo);
}

void UWRUISlot_InventoryItemInfo::SetOption()
{
	for (auto& AbilityWidget : AbilityWidgetList)
	{

	}
}

void UWRUISlot_InventoryItemInfo::SetInfoAttached(const ISN_t IN InISN)
{
	if (InISN == INVALID_ISN)
		return;

	WRItemBase* FoundItem = WRItemManager::Get()->FindItem(InISN);
	if (FoundItem == nullptr)
		return;

	FWRItemTableBase* FoundItemTableBase = WRTableManager::Get()->FindRowInItemTable(FoundItem->GetTID());
	if (FoundItemTableBase == nullptr)
		return;

	AWRCharacterMine* Mine = WRCharacterManager::Get()->GetMyCharacter();
	if (Mine == nullptr)
		return;
		
	// Preview Info
	FWRPreviewInfo PreviewInfo;
	PreviewInfo.Type = EWRPreviewType::CustomActor;
	PreviewInfo.SetPreviewWidget(Slot_Preview);
	PreviewInfo.PreviewLoadCompletedDelegate = FWRPreviewLoadCompletedDelegate::CreateUObject(this, &UWRUISlot_InventoryItemInfo::OnActorSpawnCompleted);
	// Preview Base Data Set
	FWRPreviewDataBase PreviewBaseData;
	PreviewBaseData.UseRotate = true;
	PreviewBaseData.UseZoom = true;
	PreviewBaseData.ActorRotationOffset = FRotator(0, 90, 0);
	PreviewBaseData.CameraOffset = FVector(0, -80, 0);
	PreviewBaseData.CameraOffsetPivotSocket = FName("head");
	PreviewInfo.ModifyBaseData(PreviewBaseData);
	// + Custom Actor Data
	FWRPreviewCustomActorData CustomActorData;
	CustomActorData.PresetTID = WRAccountManager::Get()->GetMyCharacterPresetTID();
	CustomActorData.AddItem(FoundItem->GetTID());
	PreviewInfo.SetCustomActorData(CustomActorData);
	WRPreviewManager::Get()->SetPreview(PreviewInfo);
}

void UWRUISlot_InventoryItemInfo::OnActorSpawnCompleted(class AActor* IN InActor)
{
	if (InActor == nullptr)
		return;

	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(InActor);
	if (CastedPlayer == nullptr)
		return;

	
}
