// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_TargetLocationGuide.h"
#include "CanvasPanel.h"
#include "UI/Base/WRUISlot.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRTargetLocationGuideManager.h"
#include "Manager/WRUIManager.h"
#include "UI/Slot/WRUISlot_TargetLocationGuide.h"
#include "CanvasPanelSlot.h"
#include "UI/Slot/WRUISlot_FocusTalk.h"

void UWRUIPanel_TargetLocationGuide::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_List);
}

void UWRUIPanel_TargetLocationGuide::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUIPanel_TargetLocationGuide::AddTargetLocationInfo(const WRTargetLocationGuideHandle IN InHandle, const FWRTargetLocationInfoBase* IN InTargetLocationInfoBase)
{
	if (InTargetLocationInfoBase == nullptr)
		return;

	if (TargetLcationSlots.Contains(InHandle))
		return;

	if (CreatingTargetLocationGuideSlots.Contains(InHandle))
		return;

	FWRSlotUILoadCompletedDelegate Delegate = FWRSlotUILoadCompletedDelegate::CreateLambda([=](UWRUISlot* IN InCreatedSlot, const FString& IN InKey)
	{
		CreatingTargetLocationGuideSlots.Remove(InHandle);

		if (InCreatedSlot)
		{
			UWRUISlot_TargetLocationGuide* CreatedTargetLocationGuideSlot = Cast<UWRUISlot_TargetLocationGuide>(InCreatedSlot);
			if (CreatedTargetLocationGuideSlot == nullptr)
				return;

			UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_List->AddChildToCanvas(CreatedTargetLocationGuideSlot);
			if (CanvasPanelSlot)
			{
				CanvasPanelSlot->SetAnchors(FAnchors(0.5f));
				CanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				CanvasPanelSlot->SetAutoSize(true);
			}
			CreatedTargetLocationGuideSlot->SetTargetLocationInfo(InTargetLocationInfoBase);

			CreatedTargetLocationGuideSlot->SetRadius(Radius);
			TargetLcationSlots.Add(InHandle, CreatedTargetLocationGuideSlot);
		}
	});

	FString AsyncKey = WRUIManager::Get()->CreateUISlotAsync(InTargetLocationInfoBase->UISlotKind, Delegate);
	if(AsyncKey.IsEmpty() == false)
		CreatingTargetLocationGuideSlots.Add(InHandle, AsyncKey);

	WRUIManager::Get()->CreateUISlotAsync(InTargetLocationInfoBase->UISlotKind, nullptr);
}

void UWRUIPanel_TargetLocationGuide::RemoveTargetLocationInfo(const WRTargetLocationGuideHandle IN InHandle)
{
	if (CreatingTargetLocationGuideSlots.Contains(InHandle))
	{
		FString AsyncKey = CreatingTargetLocationGuideSlots.FindRef(InHandle);
		WRUIManager::Get()->CancelUIAsync(AsyncKey);
		CreatingTargetLocationGuideSlots.Remove(InHandle);
		return;
	}

	if (TargetLcationSlots.Contains(InHandle) == false)
		return;

	UWRUISlot* FoundSlot = TargetLcationSlots.FindRef(InHandle);
	if (FoundSlot == nullptr)
		return;

	CanvasPanel_List->RemoveChild(FoundSlot);
	TargetLcationSlots.Remove(InHandle);
}

void UWRUIPanel_TargetLocationGuide::UpdateUIForHMDRotationInFocusTalk(const AID_t IN InTargetAID)
{
	for (auto& Iter : TargetLcationSlots)
	{
		UWRUISlot_FocusTalk* Slot_FocusTalk = Cast< UWRUISlot_FocusTalk>(Iter.Value);
		if(Slot_FocusTalk == nullptr)
			continue;

		Slot_FocusTalk->UpdateUIForHMDRotation(InTargetAID);
	}
}
