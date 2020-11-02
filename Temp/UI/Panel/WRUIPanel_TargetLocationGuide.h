// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_TargetLocationGuide.generated.h"

class UWRUISlot;
class UCanvasPanel;
struct FWRTargetLocationInfoBase;
UCLASS()
class WR_API UWRUIPanel_TargetLocationGuide : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	void AddTargetLocationInfo(const WRTargetLocationGuideHandle IN InHandle, const FWRTargetLocationInfoBase* IN InTargetLocationInfoBase);
	void RemoveTargetLocationInfo(const WRTargetLocationGuideHandle IN InHandle);

	void UpdateUIForHMDRotationInFocusTalk(const AID_t IN InTargetAID);

	void OnTargetLocationGuideSlotCreated();
private:
	UPROPERTY(EditAnywhere)
		float Radius;
	UCanvasPanel* CanvasPanel_List;
	TMap<WRTargetLocationGuideHandle, UWRUISlot*> TargetLcationSlots;
	TMap<WRTargetLocationGuideHandle, FString> CreatingTargetLocationGuideSlots;
};
