// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIPathManager.h"
#include "WRPathManager.h"
#include "Utility/WREnumUtility.h"
#include "UObjectGlobals.h"

void WRUIPathManager::OnInitialize()
{
	// [ 2019-8-19 : vuvle ] UI Resource(Panel, Slot) 컨테이너 Insert 구문

	//FString UIFolderPath = WRPathManager::Get()->GetPath(EWRPath::Blueprint_UI_Panel);
	FString UIFolderPath = WRPathManager::Get()->GetPath(EWRPath::UI_Panel);
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWRUIPanelKind"), true);
	if (EnumPtr)
	{
		int32 MaxNum = EnumPtr->NumEnums();
		for (int32 i = 0; i < MaxNum; ++i)
		{
			const FString BPName = GetPanelBPName(EnumPtr->GetNameStringByIndex(i));
			if (BPName.IsEmpty())
				continue;

			PanelPaths.Add(EWRUIPanelKind(EnumPtr->GetValueByIndex(i)), UIFolderPath + BPName);
		}
	}

	//UIFolderPath = WRPathManager::Get()->GetPath(EWRPath::Blueprint_UI_Slot);
	UIFolderPath = WRPathManager::Get()->GetPath(EWRPath::UI_Slot);
	EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWRUISlotKind"), true);
	if (EnumPtr)
	{
		int32 MaxNum = EnumPtr->NumEnums();
		for (int32 i = 0; i < MaxNum; ++i)
		{
			const FString BPName = GetSlotBPName(EnumPtr->GetNameStringByIndex(i));
			if (BPName.IsEmpty())
				continue;

			SlotPaths.Add(EWRUISlotKind(EnumPtr->GetValueByIndex(i)), UIFolderPath + BPName);
		}
	}

	/*for (uint8 i = 0; i < uint8(EWRUIPanelKind::MAX); ++i)
	{
		const FString BPName = GetPanelBPName(EWRUIPanelKind(i));
		if (BPName.IsEmpty())
			continue;

		PanelPaths.Add(EWRUIPanelKind(i), UIFolderPath + BPName);
	}

	UIFolderPath = WRPathManager::Get()->GetPath(EWRPath::Blueprint_UI_Slot);
	for (uint8 i = 0; i < uint8(EWRUISlotKind::MAX); ++i)
	{
		const FString BPName = GetSlotBPName(EWRUISlotKind(i));
		if (BPName.IsEmpty())
			continue;

		SlotPaths.Add(EWRUISlotKind(i), UIFolderPath + BPName);
	}*/
}

void WRUIPathManager::OnShutdown()
{
	PanelPaths.Empty();
	//SlotPath.Empty();
}

FString WRUIPathManager::GetPanelBPName(const FString& IN InBPName)
{
	//FString BPName = WREnumUtility::EnumToString("EWRUIPanelKind", InUIPanelKind);  // EWRUIPanelKind::GetStr(InUIPanelKind);
	if (InBPName.IsEmpty())
		return InBPName;

	return FString::Printf(TEXT("Panel_%s_WB.Panel_%s_WB_C"), *InBPName, *InBPName);
}

FString WRUIPathManager::GetSlotBPName(const FString& IN InBPName)
{
	if (InBPName.IsEmpty())
		return InBPName;

	return FString::Printf(TEXT("Slot_%s_WB.Slot_%s_WB_C"), *InBPName, *InBPName);
}

const FString& WRUIPathManager::GetUIPath(const EWRUIPanelKind IN InPanelKind) const
{
	static FString EmptyString = "Empty";
	if (PanelPaths.Contains(InPanelKind) == false)
		return EmptyString;

	return 	PanelPaths[InPanelKind];
}

const FString& WRUIPathManager::GetUIPath(const EWRUISlotKind IN InSlotKind) const
{
	static FString EmptyString = "Empty";
	if (SlotPaths.Contains(InSlotKind) == false)
		return EmptyString;

	return 	SlotPaths[InSlotKind];
}
