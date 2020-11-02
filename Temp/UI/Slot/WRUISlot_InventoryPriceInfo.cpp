// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventoryPriceInfo.h"
#include "Define/WRUIDefine.h"
#include "UI/CustomWidget/WRTextBlock.h"
#include "Item/WRItem.h"
#include "Manager/WRItemManager.h"
#include "UI/CustomWidget/Chart/WRChart_Line.h"
#include "Manager/WRInventoryManager.h"
#include "Utility/WRChartUtility.h"
#include "Utility/WRUIUtility.h"
#include "Enum/EWRItem.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUISlot_InventoryPriceInfo::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Description);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Notice);
	BIND_WIDGET(UWRChart_Line, WRChart_Line);

	PurchaseInfo.Price = Cast<UWRTextBlock>(GetWidgetFromName(FName("WRTextBlock_PurchasePrice")));
	PurchaseInfo.Fluctuationate = Cast<UWRTextBlock>(GetWidgetFromName(FName("WRTextBlock_PurchaseFluctuationate")));
	PurchaseInfo.UpDownTriangle = Cast<UImage>(GetWidgetFromName(FName("Image_PurchaseUpDownTri")));

	SellInfo.Price = Cast<UWRTextBlock>(GetWidgetFromName(FName("WRTextBlock_SellPrice")));
	SellInfo.Fluctuationate = Cast<UWRTextBlock>(GetWidgetFromName(FName("WRTextBlock_SellFluctuationate")));
	SellInfo.UpDownTriangle = Cast<UImage>(GetWidgetFromName(FName("Image_SellUpDownTri")));

}

void UWRUISlot_InventoryPriceInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWRUISlot_InventoryPriceInfo::SetPriceInfo(const ISN_t IN InISN)
{
	if (InISN == INVALID_ISN)
	{
		SetEmptyInfo();
		return;
	}

	WRTextBlock_Notice->SetVisibility(ESlateVisibility::Collapsed);

	// + 시세정보 출력

	WRItemBase* FoundItem = WRItemManager::Get()->FindItem(InISN);
	if (FoundItem == nullptr)
	{
		SetEmptyInfo();
		return;
	}

	if (CanvasPanel_Description != nullptr)
		CanvasPanel_Description->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (WRChart_Line != nullptr)
	{
		TID_t InTID = FoundItem->GetTID();
		std::list<stSekaiItemInfo>* InItemInfoList = WRInventoryManager::Get()->GetItemInfoList(InTID);

		if (InItemInfoList == nullptr)
		{
			SetEmptyInfo();
			return;
		}

		if (InItemInfoList->size() <= INVALID_RESULT)
		{
			SetEmptyInfo();
			return;
		}
		WRChart_Line->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TMap<int32, FWRChartDataNodes> InNodeMap;
		WRChartUtility::ConvertChartDataNode(WRChart_Line, InNodeMap, InItemInfoList);
		WRChartUtility::CalculatePriceInfo(PurchaseInfo, InItemInfoList, EWRClassificationForTrade::Purchase);
		WRChartUtility::CalculatePriceInfo(SellInfo, InItemInfoList, EWRClassificationForTrade::Sell);

		// ez - 하드코딩됨 데이터테이블 추가하고 변경될예정
		if (PurchaseInfo.fFluctuationate > 0.f)
		{
			WRUIUtility::SetImageTintColorSafe(PurchaseInfo.UpDownTriangle, UpColor);
			PurchaseInfo.UpDownTriangle->SetRenderTransformAngle(0.f);
		}
		else 
		{
			WRUIUtility::SetImageTintColorSafe(PurchaseInfo.UpDownTriangle, DownColor);
			PurchaseInfo.UpDownTriangle->SetRenderTransformAngle(180.f);
		}
		if (SellInfo.fFluctuationate > 0.f)
		{
			WRUIUtility::SetImageTintColorSafe(SellInfo.UpDownTriangle, UpColor);
			SellInfo.UpDownTriangle->SetRenderTransformAngle(0.f);
		}
		else
		{
			WRUIUtility::SetImageTintColorSafe(SellInfo.UpDownTriangle, DownColor);
			SellInfo.UpDownTriangle->SetRenderTransformAngle(180.f);
		}
	}
	// FoundItem->GetTID()

}

void UWRUISlot_InventoryPriceInfo::SetPriceNotice(const FString IN InNotice)
{
	WRTextBlock_Notice->SetText(InNotice);
}

void UWRUISlot_InventoryPriceInfo::SetEmptyInfo()
{
	WRTextBlock_Notice->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRTextBlock_Notice->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_ItemSelectEmptyKey"));
	if (CanvasPanel_Description != nullptr)
		CanvasPanel_Description->SetVisibility(ESlateVisibility::Collapsed);
	if (WRChart_Line != nullptr)
		WRChart_Line->SetVisibility(ESlateVisibility::Collapsed);
}