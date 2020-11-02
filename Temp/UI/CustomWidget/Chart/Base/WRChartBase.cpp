// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRChartBase.h"
#include "CanvasPanel.h"
#include "Define/WRUIDefine.h"
#include "Image.h"
#include "WidgetTree.h"
#include "PanelWidget.h"
#include "CanvasPanelSlot.h"
#include "Utility/WRChartUtility.h"
#include "Kismet/KismetMathLibrary.h"

bool UWRChartBase::Initialize()
{
	return Super::Initialize();
}

void UWRChartBase::NativeConstruct()
{
	Super::NativeConstruct();
	BIND_WIDGET(UCanvasPanel, GridPanel);
	BIND_WIDGET(UCanvasPanel, XAxisPanel);
	BIND_WIDGET(UCanvasPanel, YAxisPanel);
	GridPanel->SetVisibility(ESlateVisibility::Visible);

	this->InitializeChart();
	//this->SetDummyData();
}

void UWRChartBase::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRChartBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ChartTickProcess();
}

void UWRChartBase::SetDataNodes(const TMap<int32, FWRChartDataNodes>& IN InNodes)
{
	RemoveNodes();
	DataNodes = InNodes;
	ReserveResetNode = true;
}

void UWRChartBase::InitializeChart()
{
	for (int32 XIndex = 0; XIndex < XAxisCount; XIndex++)
	{
		FWRGridNode InNode;

		InitializeGridNode(InNode, EWRChartAxis::X);
		XGridNodes.Add(InNode);
	}

	for (int32 YIndex = 0; YIndex < YAxisCount; YIndex++)
	{
		FWRGridNode InNode;

		/*bool IsCreateText = false;

		if (YIndex == 0 || YIndex == YAxisCount - 1)
			IsCreateText = true;*/

		InitializeGridNode(InNode, EWRChartAxis::Y);// , IsCreateText);
		YGridNodes.Add(InNode);
	}
	//ReserveResetChart = true; Child에서 활성화 필요
}

void UWRChartBase::SetGridNode(const TArray<FName>& IN InXAxisTexts, const TArray<FName>& InYAxisTexts)
{
	for (int32 InXIndex = 0; InXIndex < XGridNodes.Num(); InXIndex++)
	{
		FWRGridNode& InNode = XGridNodes[InXIndex];
		if (InNode.TextBlock != nullptr && InXAxisTexts.IsValidIndex(InXIndex) == true)
		{
			FName InText = InXAxisTexts[InXIndex];
			InNode.TextBlock->SetText(InText);
			InNode.TextBlock->SetVisibility(ESlateVisibility::Visible);
			InNode.TextBlock->SetJustification(ETextJustify::Center);
		}
	}

	for (int32 InYIndex = 0; InYIndex < YGridNodes.Num(); InYIndex++)
	{
		FWRGridNode& InNode = YGridNodes[InYIndex];
		if (InNode.TextBlock != nullptr && InYAxisTexts.IsValidIndex(InYIndex) == true)
		{
			FName InText = InYAxisTexts[InYIndex];
			InNode.TextBlock->SetText(InText);
			InNode.TextBlock->SetVisibility(ESlateVisibility::Visible);
			InNode.TextBlock->SetJustification(ETextJustify::Center);
		}
	}
	//ReserveResetChart = true;
}

void UWRChartBase::ChartTickProcess()
{
	if (ReserveResetChart == true)
		UpdateChart();

	if (ReserveResetNode == true)
		UpdateNodes();
}

void UWRChartBase::UpdateNodes()
{
}

void UWRChartBase::UpdateChart()
{
	if (GridPanel == nullptr || XAxisPanel == nullptr || YAxisPanel == nullptr)
		return;

	ReserveResetChart = !UpdateGridLine();
}

bool UWRChartBase::UpdateGridLine()
{
	const FGeometry& InGeometry = GridPanel->GetCachedGeometry();
	if (UKismetMathLibrary::IsNearlyZero2D(InGeometry.GetAbsoluteSize()) == true)
		return false;

	FVector2D GridPanelSize = InGeometry.GetLocalSize();

	float XIntervalDistance = GridPanelSize.X / (XGridNodes.Num() - 1);
	for (int32 InXIndex = 0; InXIndex < XGridNodes.Num(); InXIndex++)
	{
		FWRGridNode& XGridNode = XGridNodes[InXIndex];
		if (XGridNode.LineImage == nullptr || XGridNode.TextBlock == nullptr)
			continue;

		FVector2D InStartPos = FVector2D(XIntervalDistance*InXIndex, 0.f);
		FVector2D InEndPos = FVector2D(XIntervalDistance*InXIndex, GridPanelSize.Y);

		WRChartUtility::DrawLineStartToEnd(XGridNode.LineImage, InStartPos, InEndPos, GridLineBrush.LineTickness);

		if (NodeFontName == TEXT("None"))
			continue;

		UCanvasPanelSlot* InTextSlot = Cast<UCanvasPanelSlot>(XGridNode.TextBlock->Slot);
		if (InTextSlot == nullptr)
			return false;

		const FGeometry& InTextGeometry = XAxisPanel->GetCachedGeometry();
		if (UKismetMathLibrary::IsNearlyZero2D(InTextGeometry.GetAbsoluteSize()) == true)
			return false;

		FVector2D TextPanelSize = InTextGeometry.GetLocalSize();

		FVector2D InPosition = FVector2D(XIntervalDistance*InXIndex, TextPanelSize.Y / 2.f);

		InTextSlot->SetPosition(InPosition);
		InTextSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	}

	float YIntervalDistance = GridPanelSize.Y / (YGridNodes.Num() - 1);
	for (int32 InYIndex = 0; InYIndex < YGridNodes.Num(); InYIndex++)
	{
		FWRGridNode& YGridNode = YGridNodes[InYIndex];
		if (YGridNode.LineImage == nullptr)//|| YGridNode.TextBlock == nullptr)
			continue;

		FVector2D InStartPos = FVector2D(0.f, YIntervalDistance*InYIndex);
		FVector2D InEndPos = FVector2D(GridPanelSize.X, YIntervalDistance*InYIndex);

		WRChartUtility::DrawLineStartToEnd(YGridNode.LineImage, InStartPos, InEndPos, GridLineBrush.LineTickness);

		if (YGridNode.TextBlock == nullptr)
			continue;

		if (NodeFontName == TEXT("None"))
			continue;

		UCanvasPanelSlot* InTextSlot = Cast<UCanvasPanelSlot>(YGridNode.TextBlock->Slot);
		if (InTextSlot == nullptr)
			return false;

		const FGeometry& InTextGeometry = YAxisPanel->GetCachedGeometry();
		if (UKismetMathLibrary::IsNearlyZero2D(InTextGeometry.GetAbsoluteSize()) == true)
			return false;

		FVector2D TextPanelSize = InTextGeometry.GetLocalSize();

		FVector2D InPosition = FVector2D(TextPanelSize.X / 2.f, YIntervalDistance*InYIndex);

		InTextSlot->SetPosition(InPosition);
		InTextSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	}
	return true;
}

void UWRChartBase::RemoveAll()
{
	RemoveChart();
	RemoveNodes();
}

void UWRChartBase::RemoveChart()
{

}

void UWRChartBase::RemoveNodes()
{

}

void UWRChartBase::InitializeGridNode(FWRGridNode& InNode, const EWRChartAxis& InEnumAxis, const bool IsCreateTextBlock/* = true*/)
{
	UWidgetTree* InWidgetTree = this->WidgetTree;
	if (InWidgetTree == nullptr)
		return;

	InNode.LineImage = InWidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT(""));

	if (InNode.LineImage == nullptr)
		return;

	InNode.LineImage->SetBrush(GridLineBrush);
	InNode.LineImage->SetVisibility(ESlateVisibility::Visible);
	GridPanel->AddChild(InNode.LineImage);

	if (IsCreateTextBlock == false)
	{
		InNode.TextBlock = nullptr;
		return;
	}
	if (NodeFontName != TEXT("None"))
	{
		InNode.TextBlock = InWidgetTree->ConstructWidget<UWRTextBlock>(UWRTextBlock::StaticClass(), TEXT(""));
		if (InNode.TextBlock == nullptr)
			return;

		InNode.TextBlock->SetText(FName(""));
		InNode.TextBlock->SetIsRichTextBlock(true);
		InNode.TextBlock->SetTextStyleSet(TextStyleSet);
		InNode.TextBlock->SetFontRawName(NodeFontName);
		InNode.TextBlock->SetVisibility(ESlateVisibility::Hidden);
		switch (InEnumAxis)
		{
		case EWRChartAxis::X:
		{
			XAxisPanel->AddChild(InNode.TextBlock);
		}
		break;

		case EWRChartAxis::Y:
		{
			YAxisPanel->AddChild(InNode.TextBlock);
		}
		break;
		}
	}
}

void UWRChartBase::SetDummyData()
{
	TArray<FName> testX;
	TArray<FName> testY;

	testX.Add(FName(TEXT("aaaa")));
	testX.Add(FName(TEXT("aaaa")));
	testX.Add(FName(TEXT("aaaa")));
	testX.Add(FName(TEXT("aaaa")));
	testX.Add(FName(TEXT("aaaa")));

	testY.Add(FName(TEXT("bbb")));
	testY.Add(FName(TEXT("bbb")));
	testY.Add(FName(TEXT("bbb")));
	testY.Add(FName(TEXT("bbb")));
	testY.Add(FName(TEXT("bbb")));
	testY.Add(FName(TEXT("bbb")));
	testY.Add(FName(TEXT("bbb")));
	SetGridNode(testX, testY);

	FWRChartDataNode Intest10, Intest11, Intest12, Intest13, Intest14;
	FWRChartDataNode Intest20, Intest21, Intest22, Intest23, Intest24;
	FWRChartDataNode Intest30, Intest31, Intest32, Intest33, Intest34;

	Intest10.Xratio = 0.0f;
	Intest10.Yratio = 0.1f;

	Intest11.Xratio = 0.25f;
	Intest11.Yratio = 0.3f;

	Intest12.Xratio = 0.5f;
	Intest12.Yratio = 0.5f;

	Intest13.Xratio = 0.75f;
	Intest13.Yratio = 0.9f;

	Intest14.Xratio = 1.f;
	Intest14.Yratio = 0.9f;

	Intest20.Xratio = 0.0f;
	Intest20.Yratio = 0.2f;

	Intest21.Xratio = 0.25f;
	Intest21.Yratio = 0.5f;

	Intest22.Xratio = 0.5f;
	Intest22.Yratio = 0.2f;

	Intest23.Xratio = 0.75f;
	Intest23.Yratio = 0.25f;

	Intest24.Xratio = 1.f;
	Intest24.Yratio = 0.8f;

	Intest30.Xratio = 0.0f;
	Intest30.Yratio = 0.5f;

	Intest31.Xratio = 0.25f;
	Intest31.Yratio = 0.5f;

	Intest32.Xratio = 0.5f;
	Intest32.Yratio = 0.6f;

	Intest33.Xratio = 0.75f;
	Intest33.Yratio = 0.45f;

	Intest34.Xratio = 1.f;
	Intest34.Yratio = 0.5f;

	FWRChartDataNodes IntestNodes, IntestNodes1, IntestNodes2;

	IntestNodes.DataNodes.Add(Intest10);
	IntestNodes.DataNodes.Add(Intest11);
	IntestNodes.DataNodes.Add(Intest12);
	IntestNodes.DataNodes.Add(Intest13);
	IntestNodes.DataNodes.Add(Intest14);

	IntestNodes1.DataNodes.Add(Intest20);
	IntestNodes1.DataNodes.Add(Intest21);
	IntestNodes1.DataNodes.Add(Intest22);
	IntestNodes1.DataNodes.Add(Intest23);
	IntestNodes1.DataNodes.Add(Intest24);

	IntestNodes2.DataNodes.Add(Intest30);
	IntestNodes2.DataNodes.Add(Intest31);
	IntestNodes2.DataNodes.Add(Intest32);
	IntestNodes2.DataNodes.Add(Intest33);
	IntestNodes2.DataNodes.Add(Intest34);

	TMap<int32, FWRChartDataNodes> IntestNodesMap;
	IntestNodesMap.Add(0, IntestNodes);
	IntestNodesMap.Add(1, IntestNodes1);
	IntestNodesMap.Add(2, IntestNodes2);
	SetDataNodes(IntestNodesMap);
}
