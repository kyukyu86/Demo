// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_FocusTalk.h"
#include "Manager/WRTargetLocationGuideManager.h"
#include "Actor/Character/WRCharacter.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRVoiceContentsManager.h"

void UWRUISlot_FocusTalk::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_HMDRotation);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Name);
}

void UWRUISlot_FocusTalk::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUISlot_FocusTalk::SetTargetLocationInfo(const FWRTargetLocationInfoBase* IN InTargetLocationInfo)
{
	Super::SetTargetLocationInfo(InTargetLocationInfo);

	if (TargetLocationInfoBase == nullptr)
		return;

	const FWRTargetLocationInfoForActor*  TargetLocationInfoForActor = static_cast<const FWRTargetLocationInfoForActor*>(TargetLocationInfoBase);
	if (TargetLocationInfoForActor == nullptr)
		return;

	if (TargetLocationInfoForActor->TargetActor == nullptr)
		return;

	const AWRCharacter* TargetCharacter = Cast<AWRCharacter>(TargetLocationInfoForActor->TargetActor);
	if (TargetCharacter == nullptr)
		return;

	WRTextBlock_Name->SetText(TargetCharacter->GetCharacterName());
	UpdateUIForHMDRotation(WRVoiceContentsManager::Get()->GetTargetAIDForHMDRotationInFocusTalk());
}

void UWRUISlot_FocusTalk::UpdateUIForHMDRotation(const AID_t IN InTargetAID)
{
	if (TargetLocationInfoBase == nullptr)
		return;

	const FWRTargetLocationInfoForActor*  TargetLocationInfoForActor = static_cast<const FWRTargetLocationInfoForActor*>(TargetLocationInfoBase);
	if (TargetLocationInfoForActor == nullptr)
		return;

	if (TargetLocationInfoForActor->TargetActor == nullptr)
		return;

	const AWRCharacter* TargetCharacter = Cast<AWRCharacter>(TargetLocationInfoForActor->TargetActor);
	if (TargetCharacter == nullptr)
		return;

	if (TargetCharacter->GetActorID() == InTargetAID)
	{
		CanvasPanel_HMDRotation->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		CanvasPanel_HMDRotation->SetVisibility(ESlateVisibility::Collapsed);
	}
}

