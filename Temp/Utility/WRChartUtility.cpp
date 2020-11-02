// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRChartUtility.h"
#include "Anchors.h"
#include "Image.h"
#include "CanvasPanelSlot.h"


#include "Kismet/KismetMathLibrary.h"
#include "Widget.h"
#include "UI/CustomWidget/Chart/WRChart_Line.h"
#include "Network/Share/Define/Define_Time.h"
#include "Struct/WRUIStruct.h"
#include "UI/CustomWidget/WRTextBlock.h"
#include "WRUIUtility.h"

void WRChartUtility::DrawLineStartToEnd(UImage* InImage, const FVector2D& InStart, const FVector2D&InEnd, const float InTickness)
{
	if (InImage == nullptr)
		return;

	UCanvasPanelSlot* InImageSlot = Cast<UCanvasPanelSlot>(InImage->Slot);
	if (InImageSlot == nullptr)
		return;

	FVector2D LineCenterLocation = InStart + ((InEnd - InStart) / 2.f);
	float InLength = (InEnd - InStart).Size();

	FVector2D InSize = FVector2D(InLength, InTickness);
	float InDegree = GetDegreeBetweenVector2Ds(InStart, InEnd);

	InImageSlot->SetPosition(LineCenterLocation);
	InImageSlot->SetSize(InSize);
	InImageSlot->SetAlignment(FVector2D(0.5f, 0.5f));

	InImage->SetRenderTransformAngle(InDegree);
}

void WRChartUtility::GetAbsoluteMinMaxPosition(UWidget* InWidget, FVector2D& OUT OutMinPosition, FVector2D& OUT OutMaxPosition)
{
	if (InWidget == nullptr)
		return;

	if (InWidget->GetParent() == nullptr)
		return;

	UCanvasPanelSlot* InCanvasSlot = Cast<UCanvasPanelSlot>(InWidget->Slot);
	if (InCanvasSlot == nullptr)
		return;


}

float WRChartUtility::GetDegreeBetweenVector2Ds(const FVector2D& InStart, const FVector2D&InEnd)
{
	FVector2D InDirection = (InEnd - InStart).GetSafeNormal();
	//FVector2D InUpVector = FVector2D(1.f, 0.f);
	if (InStart.Y > InEnd.Y)
	{
		InDirection.X *= -1.f;
	}
	float InAcos = FMath::Acos(FVector2D::DotProduct(InDirection, FVector2D(1.f, 0.f)));

	float DegreeResult = FMath::RadiansToDegrees(InAcos);

	return DegreeResult;
}


void WRChartUtility::ConvertChartDataNode(UWRChart_Line* InChart, TMap<int32, FWRChartDataNodes>& OUT OutItemInfoMap, std::list<stSekaiItemInfo>* IN InItemInfoList)
{
	if (InChart == nullptr)
		return;

	OutItemInfoMap.Empty(InItemInfoList->size());

	TArray<FName> XGridText;
	TArray<FName> YGridText;


	std::list<stSekaiItemInfo>::const_iterator Iter = InItemInfoList->begin();

	GAME_MONEY_t MostExpensiveMoney = 0;

	TArray<stSekaiItemInfo> InInfoArray;

	for (; Iter != InItemInfoList->end(); ++Iter)
	{
		stSekaiItemInfo Indata = *Iter;

		InInfoArray.Add(Indata);
	}

	stSmallDateTime CurrentDate;

	for (int32 InIndex = 0; InIndex < InInfoArray.Num(); InIndex++)
	{
		stSekaiItemInfo& Indata = InInfoArray[InIndex];

		GAME_MONEY_t TempMoney = FMath::Max3(Indata.commissionPurchasePrice, Indata.commissionSellingPrice, Indata.floridaPurchasePrice);
		MostExpensiveMoney = FMath::Max(MostExpensiveMoney, TempMoney);

		if (InIndex >= InItemInfoList->size() - InChart->GetXCount())
		{
			//XGridText

			FString InMonth, InDay;

			FString InDate;

			if (Indata.date.month < 10)
			{
				InMonth += TEXT("0");
			}
			if (Indata.date.day < 10)
			{
				InDay += TEXT("0");
			}
			InMonth.AppendInt(Indata.date.month);
			InDay.AppendInt(Indata.date.day);

			InDate = InMonth + TEXT("/") + InDay;
			FName InTextName = FName(*InDate);
			XGridText.Add(InTextName);
		}
		if (InIndex == InInfoArray.Num() - 1)
			CurrentDate = Indata.date;
	}

	FString MaxCount;
	MaxCount.AppendInt(MostExpensiveMoney);// (MostExpensiveMoney);
	int32 InDigit = MaxCount.Len();

	FString TempStr = MaxCount.Left(1);
	int32 FirstCount = FCString::Atoi(*TempStr);
	FirstCount++;
	MaxCount.Empty();
	MaxCount.AppendInt(FirstCount);

	for (int32 InIndex = 0; InIndex < InDigit - 1; InIndex++)
	{
		MaxCount += TEXT("0");
	}

	int32 YCount = FCString::Atoi(*MaxCount);
	YCount = YCount / InChart->GetYCount();

	for (int32 InIndex = 0; InIndex < InChart->GetYCount(); InIndex++)
	{
		if (InIndex == InChart->GetYCount() - 1)
		{
			YGridText.Add(TEXT("0"));
		}
		else
		{
			FString CountText;
			CountText.AppendInt(YCount*(InChart->GetYCount() - InIndex));
			YGridText.Add(*CountText);
		}
	}

	/*for (int32 InIndex = 0; InIndex < InChart->GetYCount(); InIndex++)
	{
		if (InIndex == InChart->GetYCount() - 1)
		{
			YGridText.Add(TEXT("0"));
		}
		else
		{
			YGridText.Add(*MaxCount);
		}
	}*/
	float MaxCountFloat = FCString::Atof(*MaxCount);

	FWRChartDataNodes purchasePriceNodes, sellingPriceNodes, FloridaNodes;

	purchasePriceNodes.DataNodes.Empty(InChart->GetXCount());
	sellingPriceNodes.DataNodes.Empty(InChart->GetXCount());
	FloridaNodes.DataNodes.Empty(InChart->GetXCount());

	float ForCalcXRatio = 0.f;
	int32 SumResult = InChart->GetXCount() - 1;
	float SumCalcXRatio = SumResult;
	SumCalcXRatio = 1.f / SumCalcXRatio;
	float XCount = InChart->GetXCount();

	for (int32 InIndex = InInfoArray.Num() - InChart->GetXCount(); InIndex < InInfoArray.Num(); InIndex++)
	{
		if (InInfoArray.IsValidIndex(InIndex) == false)
			continue;

		stSekaiItemInfo& Indata = InInfoArray[InIndex];

		FWRChartDataNode purchasePriceNode, sellingPriceNode, FloridaNode;

		float commissionPurchasePrice = Indata.commissionPurchasePrice;
		float commissionSellingPrice = Indata.commissionSellingPrice;
		float floridaPurchasePrice = Indata.floridaPurchasePrice;

		purchasePriceNode.Xratio = ForCalcXRatio;
		purchasePriceNode.Yratio = (MaxCountFloat - commissionPurchasePrice) / MaxCountFloat;


		sellingPriceNode.Xratio = ForCalcXRatio;
		sellingPriceNode.Yratio = (MaxCountFloat - commissionSellingPrice) / MaxCountFloat;

		FloridaNode.Xratio = ForCalcXRatio;
		FloridaNode.Yratio = (MaxCountFloat - floridaPurchasePrice) / MaxCountFloat;

		purchasePriceNodes.DataNodes.Add(purchasePriceNode);
		sellingPriceNodes.DataNodes.Add(sellingPriceNode);
		FloridaNodes.DataNodes.Add(FloridaNode);
		ForCalcXRatio += SumCalcXRatio;//1.f;
	}

	OutItemInfoMap.Add(0, purchasePriceNodes);
	OutItemInfoMap.Add(1, sellingPriceNodes);
	OutItemInfoMap.Add(2, FloridaNodes);

	InChart->SetGridNode(XGridText, YGridText);
	InChart->SetDataNodes(OutItemInfoMap);
}

void WRChartUtility::CalculatePriceInfo(FWRFluctuationateInfo& IN InFluctuationInfo, const std::list<stSekaiItemInfo>* InItemInfoList, const EWRClassificationForTrade& InClassification)
{
	std::list<stSekaiItemInfo>::const_iterator Iter = InItemInfoList->end();

	stSekaiItemInfo InCurrentInfo = *(--Iter);

	FString PriceStr, FluctuationStr;
	

	if (InClassification == EWRClassificationForTrade::Purchase)
	{
		PriceStr.AppendInt(InCurrentInfo.commissionPurchasePrice);
	}
	else
	{
		PriceStr.AppendInt(InCurrentInfo.commissionSellingPrice);
	}

	//InFluctuationInfo.Price->sett
	if (InItemInfoList->size() <= 1)
	{
		FluctuationStr = TEXT("0.0%");

		InFluctuationInfo.Price->SetText(PriceStr);
		InFluctuationInfo.Fluctuationate->SetText(FluctuationStr);
		InFluctuationInfo.UpDownTriangle->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	stSekaiItemInfo InYesterdayInfo = *(--Iter);

	float CurrentPrice, YesterdayPrice;
	
	if (InClassification == EWRClassificationForTrade::Purchase)
	{
		if (InCurrentInfo.commissionPurchasePrice == InYesterdayInfo.commissionPurchasePrice)
		{
			FluctuationStr = TEXT("0.0%");

			InFluctuationInfo.Fluctuationate->SetText(FluctuationStr);
			InFluctuationInfo.UpDownTriangle->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}

		CurrentPrice = InCurrentInfo.commissionPurchasePrice;
		YesterdayPrice = InYesterdayInfo.commissionPurchasePrice;
	}
	else
	{
		if (InCurrentInfo.commissionSellingPrice == InYesterdayInfo.commissionSellingPrice)
		{
			FluctuationStr = TEXT("0.0%");

			InFluctuationInfo.Fluctuationate->SetText(FluctuationStr);
			InFluctuationInfo.UpDownTriangle->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}

		CurrentPrice = InCurrentInfo.commissionSellingPrice;
		YesterdayPrice = InYesterdayInfo.commissionSellingPrice;
	}
	
	InFluctuationInfo.fFluctuationate = CurrentPrice - YesterdayPrice;
	InFluctuationInfo.fFluctuationate = InFluctuationInfo.fFluctuationate / YesterdayPrice * 100.f;

	int32 FluctuationInt = InFluctuationInfo.fFluctuationate;
	int32 DecimalPoint = (InFluctuationInfo.fFluctuationate - FluctuationInt)*100.f;

	if (FluctuationInt > 0)
	{
		FluctuationStr.Append(TEXT("+ "));
	}
	else
	{
		FluctuationStr.Append(TEXT("- "));
	}
	FluctuationStr.AppendInt(FMath::Abs(FluctuationInt));
	FluctuationStr.Append(TEXT("."));
	FluctuationStr.AppendInt(FMath::Abs(DecimalPoint));
	FluctuationStr.Append(TEXT("%"));

	InFluctuationInfo.Price->SetText(PriceStr);
	InFluctuationInfo.Fluctuationate->SetText(FluctuationStr);
	InFluctuationInfo.UpDownTriangle->SetVisibility(ESlateVisibility::Visible);

	if (InFluctuationInfo.fFluctuationate > 0.f)
	{
		InFluctuationInfo.UpDownTriangle->SetRenderTransformAngle(0.f);
	}
	else
	{
		InFluctuationInfo.UpDownTriangle->SetRenderTransformAngle(180.f);
	}

}

