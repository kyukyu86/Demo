// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRPreviewStruct.h"
#include "UI/Slot/WRUISlot_Preview.h"
#include "Table/WRItemTable.h"
#include "Table/Base/WRTableManager.h"

void FWRPreviewInfo::SetPreviewWidget(class UWRUISlot_Preview* IN InWidget)
{
	if (InWidget == nullptr)
		return;

	PreviewWidget = InWidget;
	PreviewData = InWidget->GetPreviewData();
}

void FWRPreviewInfo::Release(const bool IN InParentDestroy /*= false*/)
{
	Type = EWRPreviewType::Static;
	BPPath = "";
	Anim = nullptr;
	PreviewWidget = nullptr;

	if (PreviewStudio->IsValidLowLevel())
	{
		PreviewStudio->Release();
		PreviewStudio->Destroy();
	}
	PreviewStudio = nullptr;

	if (InParentDestroy == false)
	{
		PreviewExitDelegate.ExecuteIfBound();
	}
	PreviewExitDelegate.Unbind();

	PreviewData = FWRPreviewDataWidget();
}

void FWRPreviewInfo::ModifyBaseData(struct FWRPreviewDataBase& IN InPreviewTablData)
{
	PreviewData = InPreviewTablData;
}

void FWRPreviewCustomActorData::AddItem(const WRTableID IN InItemTID)
{
	FWRItemTableBase* FoundItemTableBase = WRTableManager::Get()->FindRowInItemTable(InItemTID);
	if (FoundItemTableBase == nullptr)
		return;
		
	switch (FoundItemTableBase->MainType)
	{
	case EWRItemMainType::Weapon:
	case EWRItemMainType::Temporary:
	{
		FWREquipmentItemTableBase* EquipItemTableBase = static_cast<FWREquipmentItemTableBase*>(FoundItemTableBase);
		if (EquipItemTableBase == nullptr)
			return;
		
		if (EquipItemTableBase->EquippableSlot == EWRItemEquippableSlot::LeftWeapon)
		{
			LeftWeaponTID = InItemTID;
		}
		else if (EquipItemTableBase->EquippableSlot == EWRItemEquippableSlot::RightWeapon)
		{
			RightWeaponTID = InItemTID;
		}
		else if (EquipItemTableBase->EquippableSlot == EWRItemEquippableSlot::Temporary)
		{
			TemporaryTID = InItemTID;
		}
	}
	break;

	case EWRItemMainType::Costume:
	{
		CostumeList.Add(InItemTID);
	}
	break;
	}
}
