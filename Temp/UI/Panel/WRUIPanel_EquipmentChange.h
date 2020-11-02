// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Define/WRUIDefine.h"
#include "WRUIPanel_EquipmentChange.generated.h"

UCLASS()
class WR_API UWRUIPanel_EquipmentChange : public UWRUIPanel
{
	GENERATED_BODY()

private:	
	UImage* Image_CurrentHook;
	UImage* Image_NextHook;
	UImage* Image_CurrentLeftHand;
	UImage* Image_NextLeftHand;
	UImage* Image_CurrentRightHand;
	UImage* Image_NextRightHand;

	bool bCheckClose = false;

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* IN InAnimation) override;

	UTexture2D*	GetItemImage(class WREquipItemBase* IN InItem);
	void SetItemImage(UImage* IN InImageWidget, UTexture2D* IN InTexture);

public:
	void OnRightHandChanged(class WREquipItemBase* IN InSourceItem, class WREquipItemBase* IN InTargetItem);
	void OnLeftHandChanged(class WREquipItemBase* IN InSourceItem, class WREquipItemBase* IN InTargetItem);
};