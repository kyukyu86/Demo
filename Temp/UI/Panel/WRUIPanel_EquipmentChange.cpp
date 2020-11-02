// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIPanel_EquipmentChange.h"

#include "Utility/WRUIUtility.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRInventoryManager.h"
#include "Table/WRItemTable.h"
#include "Table/Base/WRTableManager.h"
#include "Manager/WRItemManager.h"


void UWRUIPanel_EquipmentChange::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_CurrentHook);
	BIND_WIDGET(UImage, Image_NextHook);
	BIND_WIDGET(UImage, Image_CurrentLeftHand);
	BIND_WIDGET(UImage, Image_NextLeftHand);
	BIND_WIDGET(UImage, Image_CurrentRightHand);
	BIND_WIDGET(UImage, Image_NextRightHand);

	//PlayAnimation(FadeInAnimation);
}

void UWRUIPanel_EquipmentChange::NativeDestruct()
{
	SetFocusableInputMode(false);

	Super::NativeDestruct();
}

void UWRUIPanel_EquipmentChange::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

UTexture2D* UWRUIPanel_EquipmentChange::GetItemImage(class WREquipItemBase* IN InItem)
{
	if (InItem == nullptr)
		return nullptr;

	FWRWeaponItemTable* FoundWeaponTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, InItem->GetTID());

	if (FoundWeaponTable == nullptr)
		return nullptr;

	return Cast<UTexture2D>(WRUIUtility::LoadImageObject(FoundWeaponTable->IconPath));
}

void UWRUIPanel_EquipmentChange::SetItemImage(UImage* IN InImageWidget, UTexture2D* IN InTexture)
{
	if (InImageWidget == nullptr)
		return;

	if (InTexture)
	{
		WRUIUtility::SetResourceObject(InImageWidget, InTexture);
		WRUIUtility::SetVisibilitySafe(InImageWidget, ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		WRUIUtility::SetVisibilitySafe(InImageWidget, ESlateVisibility::Collapsed);
	}
}

void UWRUIPanel_EquipmentChange::OnAnimationFinished_Implementation(const UWidgetAnimation* IN InAnimation)
{
	if (InAnimation->MovieScene->GetFName() == FName("FadeOutAnimation"))
	{
		if (bCheckClose)
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::EquipmentChange);
		}
	}
	else if (InAnimation->MovieScene->GetFName() == FName("LeftHandChangedAnimation")
		|| InAnimation->MovieScene->GetFName() == FName("RightHandChangedAnimation"))
	{
		bCheckClose = true;
		PlayAnimationByName("FadeOutAnimation");
	}
}

void UWRUIPanel_EquipmentChange::OnRightHandChanged(class WREquipItemBase* IN InSourceItem, class WREquipItemBase* IN InTargetItem)
{
	bCheckClose = false;
	
	// for Right Hand
	SetItemImage(Image_CurrentRightHand, GetItemImage(InSourceItem));
	SetItemImage(Image_NextRightHand, GetItemImage(InTargetItem));

	SetItemImage(Image_CurrentLeftHand, GetItemImage(WRItemManager::Get()->FindEquippedSubWeapon()));
	Image_NextLeftHand->SetVisibility(ESlateVisibility::Collapsed);

// 	if (IsAnimationPlayingByName("FadeOutAnimation"))
// 	{
// 		StopAnimationByName("FadeOutAnimation");
// 	}
	StopAllAnimations();
	PlayAnimationByName("RightHandChangedAnimation");
	//StopAnimationByName("LeftHandChangedAnimation");
}

void UWRUIPanel_EquipmentChange::OnLeftHandChanged(class WREquipItemBase* IN InSourceItem, class WREquipItemBase* IN InTargetItem)
{
	bCheckClose = false;

	// for Left Hand
	SetItemImage(Image_CurrentLeftHand, GetItemImage(InSourceItem));
	SetItemImage(Image_NextLeftHand, GetItemImage(InTargetItem));

	SetItemImage(Image_CurrentRightHand, GetItemImage(WRItemManager::Get()->FindEquippedMainWeapon()));
	Image_NextRightHand->SetVisibility(ESlateVisibility::Collapsed);

// 	if (IsAnimationPlayingByName("FadeOutAnimation"))
// 	{
// 		StopAnimationByName("FadeOutAnimation");
// 	}
	StopAllAnimations();
	PlayAnimationByName("LeftHandChangedAnimation");
	//StopAnimationByName("RightHandChangedAnimation");
}