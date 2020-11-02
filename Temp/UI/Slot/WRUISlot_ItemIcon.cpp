// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_ItemIcon.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "UI/CustomWidget/WRTextBlock.h"
#include "Utility/WRUIUtility.h"
#include "Image.h"
#include "Border.h"

void UWRUISlot_ItemIcon::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Icon);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Count);
	BIND_WIDGET(UBorder, Border_Select);
	BIND_WIDGET(UBorder, Border_Hover);
}

void UWRUISlot_ItemIcon::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUISlot_ItemIcon::SetIcon(FWRItemIcon& IN InInfo)
{
	// Icon
	strImageAsyncLoadKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InInfo.strIconPath, FWRCompleteAsyncLoad::CreateUObject(this, &UWRUISlot_ItemIcon::OnLoadCompletedForIconTexture));
	Image_Icon->SetVisibility(ESlateVisibility::Collapsed);

	// Count
	if (InInfo.bUseCount)
	{
		WRTextBlock_Count->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRTextBlock_Count->SetText(WRUIUtility::ConvertNumberingToKUnit(InInfo.ItemCount));
	}
	else
	{
		WRTextBlock_Count->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWRUISlot_ItemIcon::SetSelect(const bool IN InSet)
{
	if (InSet)
	{
		Border_Select->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Border_Select->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWRUISlot_ItemIcon::SetHover(const bool IN InSet)
{
	if (InSet)
	{
		Border_Hover->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Border_Hover->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWRUISlot_ItemIcon::OnLoadCompletedForIconTexture(UObject* IN InIconTexture, FString IN InHash)
{
	strImageAsyncLoadKey.Empty();
	if (InIconTexture == nullptr)
	{
		Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	WRUIUtility::SetResourceObject(Image_Icon, InIconTexture);
	Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
