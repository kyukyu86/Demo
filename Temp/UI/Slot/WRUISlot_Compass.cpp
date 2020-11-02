// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_Compass.h"
#include "Overlay.h"
#include "Image.h"
#include "OverlaySlot.h"
#include "NoExportTypes.h"
#include "WRUISlot_CompassMark.h"

#include "Manager/WRCompassManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"

#include "Utility/WRUIUtility.h"
#include "Utility/WRActorUtility.h"

#include "Actor/Character/WRCharacterMine.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Define/WRUIDefine.h"

void UWRUISlot_Compass::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UOverlay, Overlay_Marks);
	BIND_WIDGET_WITH_IMPLEMENTATION(UImage, Image_Compass, DynamicMaterial_Compass = WRUIUtility::GetDynamicMaterial(Image_Compass););
}

void UWRUISlot_Compass::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateCompassUI();
}

void UWRUISlot_Compass::InitCompassUI()
{
	const TMap<WRCompassMarkHandle, FWRCompassMarkInfoBase*>* CompassMarkMap = WRCompassManager::Get()->GetCompassList();
	if (CompassMarkMap == nullptr)
		return;

	for (auto Iter : *CompassMarkMap)
	{
		AddMark(Iter.Key, Iter.Value);
	}
}

void UWRUISlot_Compass::ClearCompassUI()
{
	Overlay_Marks->ClearChildren();
}

void UWRUISlot_Compass::AddMark(const WRCompassMarkHandle IN InCompassMarkHandle, const FWRCompassMarkInfoBase* IN InCompassMarkInfo)
{
	if (InCompassMarkInfo == nullptr)
		return;

	UWRUISlot_CompassMark* Slot_CompassMark = Cast<UWRUISlot_CompassMark>(WRUIManager::Get()->CreateUISlot(EWRUISlotKind::CompassMark));
	if (Slot_CompassMark == nullptr)
		return;

	UOverlaySlot* OverlaySlot = Overlay_Marks->AddChildToOverlay(Slot_CompassMark);
	if (OverlaySlot == nullptr)
		return;

	OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	Slot_CompassMark->SetCompassMarkInfo(InCompassMarkHandle, InCompassMarkInfo);
}

void UWRUISlot_Compass::RemoveMark(const WRCompassMarkHandle IN InCompassMarkHandle)
{
	if (InCompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
		return;

	UWRUISlot_CompassMark* FoundCompassMarkSlot = FindCompassMark(InCompassMarkHandle);
	if (FoundCompassMarkSlot == nullptr)
		return;

	Overlay_Marks->RemoveChild(FoundCompassMarkSlot);
}

void UWRUISlot_Compass::UpdateCompassUI()
{
	if (DynamicMaterial_Compass == nullptr)
		return;

	FVector FowardVector = WRInputSystemManager::Get()->GetHMDRotation().Vector();
	FowardVector.Z = 0;

	DynamicMaterial_Compass->SetScalarParameterValue("Offset", WRActorUtility::GetDegreesBetween2Vectors(FVector(0, -1, 0), FowardVector) / 360.f);
}

UWRUISlot_CompassMark* UWRUISlot_Compass::FindCompassMark(const WRCompassMarkHandle IN InCompassMarkHandle)
{
	if (InCompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
		return nullptr;

	int32 MaxNum = Overlay_Marks->GetChildrenCount();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		UWRUISlot_CompassMark* SlotCompassMark = Cast<UWRUISlot_CompassMark>(Overlay_Marks->GetChildAt(i));
		if(SlotCompassMark == nullptr)
			continue;

		if (InCompassMarkHandle != SlotCompassMark->GetCompassMarkHandle())
			continue;

		return SlotCompassMark;
	}

	return nullptr;
}
