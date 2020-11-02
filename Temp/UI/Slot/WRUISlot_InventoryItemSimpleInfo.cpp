// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryItemSimpleInfo.h"
#include "Manager/WRInventoryManager.h"
#include "Item/WRItem.h"
#include "Define/WRUIDefine.h"
#include "UI/CustomWidget/WRTextBlock.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"
#include "Utility/WREnumUtility.h"
#include "Manager/WRItemManager.h"
#include "Utility/WRUIUtility.h"

#include "Localization/WRLocalizationProcessor.h"

void UWRUISlot_InventoryItemSimpleInfo::NativeConstruct()
{
	Super::NativeConstruct();

}

void UWRUISlot_InventoryItemSimpleInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUISlot_InventoryItemSimpleInfo::SetSimpleInfo(const ISN_t IN InISN, const EWRInventorySlotGroup IN InSlotGroup)
{
	SlotGroup = InSlotGroup;

	if (InISN == INVALID_ISN)
	{
		SetEmptyInfo();
		return;
	}

	WRItemBase* FoundItemBase = WRItemManager::Get()->FindItem(InISN);
	if (FoundItemBase == nullptr)
	{
		SetEmptyInfo();
		return;
	}

	FWRItemTableBase* ItemTableBase = FoundItemBase->GetTable();
	if (ItemTableBase == nullptr)
	{
		SetEmptyInfo();
		return;
	}

	FString NameLocalKey = WRUIUtility::ConvertFNameToFString(ItemTableBase->NameLocalKey);
	FText ItemName = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, NameLocalKey);
	WRTextBlock_Name->SetText(ItemName);

	FText ItemMainDescription = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, ItemTableBase->MainDescription);
	WRTextBlock_Description->SetText(ItemMainDescription);

	WRTextBlock_Quantity->SetText(WRUIUtility::ConvertNumberingToKUnit(FoundItemBase->GetQuantity()));

	WRTextBlock_Name->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRTextBlock_Description->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRTextBlock_Quantity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUISlot_InventoryItemSimpleInfo::SetCloseInfo()
{
	SlotGroup = EWRInventorySlotGroup::None;
}

void UWRUISlot_InventoryItemSimpleInfo::SetEmptyInfo()
{
	WRTextBlock_Name->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRTextBlock_Name->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_ItemNameKey"));
	WRTextBlock_Description->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRTextBlock_Name->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_ItemSelectEmptyKey"));
	WRTextBlock_Quantity->SetVisibility(ESlateVisibility::Collapsed);
}
