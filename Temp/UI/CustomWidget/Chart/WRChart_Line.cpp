// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRChart_Line.h"
#include "CanvasPanel.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetTree.h"
#include "Image.h"
#include "Utility/WRChartUtility.h"

void UWRChart_Line::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWRChart_Line::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRChart_Line::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}


void UWRChart_Line::InitializeChart()
{
	Super::InitializeChart();

	ReserveResetChart = true;
}

void UWRChart_Line::UpdateNodes()
{
	Super::UpdateNodes();

	if (this->DrawLine() == true)
		ReserveResetNode = false;
}

bool UWRChart_Line::DrawLine()
{
	UWidgetTree* InWidgetTree = this->WidgetTree;
	if (InWidgetTree == nullptr)
		return false;

	const FGeometry& InGeometry = GridPanel->GetCachedGeometry();
	if (UKismetMathLibrary::IsNearlyZero2D(InGeometry.GetAbsoluteSize()) == true)
		return false;

	FVector2D GridPanelSize = InGeometry.GetLocalSize();

	TArray<FWRChartDataNodes> InDataArr;
	DataNodes.GenerateValueArray(InDataArr);

	for (int32 LineIndex = 0; LineIndex < InDataArr.Num(); LineIndex++)
	{
		FWRChartDataNodes InDataNode = InDataArr[LineIndex];

		for (int32 DataIndex = 1; DataIndex < InDataNode.DataNodes.Num(); DataIndex++)// (FWRChartDataNode& Indata : InDataArr[LineIndex].DataNodes)
		{
			if (InDataNode.DataNodes.IsValidIndex(DataIndex - 1) == false || InDataNode.DataNodes.IsValidIndex(DataIndex) == false)
				continue;

			FWRChartDataNode InFirstNode = InDataNode.DataNodes[DataIndex - 1];
			FWRChartDataNode InSecondNode = InDataNode.DataNodes[DataIndex];

			FVector2D FirstPosition = FVector2D(GridPanelSize.X*InFirstNode.Xratio, GridPanelSize.Y*InFirstNode.Yratio);
			FVector2D SecondPosition = FVector2D(GridPanelSize.X*InSecondNode.Xratio, GridPanelSize.Y*InSecondNode.Yratio);

			UImage* InLine = this->GetLine(LineIndex, DataIndex - 1);

			if (InLine == nullptr)
				return false;


			WRChartUtility::DrawLineStartToEnd(InLine, FirstPosition, SecondPosition, ChartLineBrushs[LineIndex].LineTickness);
		}
	}
	return true;
}

class UImage* UWRChart_Line::GetLine(const int32& InLineIndex, const int32 InDataIndex)
{
	UWidgetTree* InWidgetTree = this->WidgetTree;
	if (InWidgetTree == nullptr)
		return nullptr;

	if (Lines.Contains(InLineIndex) == true)
	{
		FWRChartImageArray* InArrayStr = Lines.Find(InLineIndex);
		if (InArrayStr == nullptr)
			return nullptr;
		if (InArrayStr->ImageArray.IsValidIndex(InDataIndex) == true)
		{
			return InArrayStr->ImageArray[InDataIndex];
		}
		else
		{
			UImage* InLine = InWidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT(""));

			if (ChartLineBrushs.IsValidIndex(InLineIndex) == true)
				InLine->SetBrush(ChartLineBrushs[InLineIndex]);

			InLine->SetVisibility(ESlateVisibility::Visible);
			GridPanel->AddChild(InLine);

			InArrayStr->ImageArray.Add(InLine);

			return InLine;
		}
	}

	FWRChartImageArray InArrayStr;

	UImage* InLine = InWidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT(""));

	if (ChartLineBrushs.IsValidIndex(InLineIndex) == true)
		InLine->SetBrush(ChartLineBrushs[InLineIndex]);

	InLine->SetVisibility(ESlateVisibility::Visible);
	GridPanel->AddChild(InLine);

	InArrayStr.ImageArray.Add(InLine);
	Lines.Add(InLineIndex, InArrayStr);

	return InLine;
}
