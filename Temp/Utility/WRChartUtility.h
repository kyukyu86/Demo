// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include <list>
#include "Network/Share/Define/Define_Sekai.h"
#include "UI/CustomWidget/Chart/Base/WRChartBase.h"
#include "Enum/EWRItem.h"
#include "Struct/WRUIStruct.h"


class WR_API WRChartUtility
{
public:
	//====================================================================================
	//	Resource
	//====================================================================================
	static void DrawLineStartToEnd(class UImage* InImage, const FVector2D& InStart, const FVector2D&InEnd, const float InTickness = 5.f);

	static void GetAbsoluteMinMaxPosition(class UWidget* InWidget, FVector2D& OUT OutMinPosition, FVector2D& OUT OutMaxPosition);

	static float GetDegreeBetweenVector2Ds(const FVector2D& InStart, const FVector2D&InEnd);

	static void ConvertChartDataNode(class UWRChart_Line* InChart, TMap<int32, FWRChartDataNodes>& OUT OutItemInfoMap,std::list<stSekaiItemInfo>* IN InItemInfoList);

	static void CalculatePriceInfo(FWRFluctuationateInfo& IN InFluctuationInfo, const std::list<stSekaiItemInfo>* InItemInfoList, const EWRClassificationForTrade& InClassification);
};