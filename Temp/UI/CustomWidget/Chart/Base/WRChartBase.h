// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlateBrush.h"
#include "SharedPointer.h"
#include "Struct/WRUIStruct.h"
#include "WRChartBase.generated.h"


class UImage;
class UWRTextBlock;

USTRUCT(BlueprintType)
struct FWRChartDataNode
{
	GENERATED_USTRUCT_BODY()
public:
	float Xratio = 0.f;
	float Yratio = 0.f;
};

USTRUCT(BlueprintType)
struct FWRChartDataNodes
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<FWRChartDataNode> DataNodes;
};

struct FWRChartAxisMinMax
{
public:
	float XAxisMin = 0.f;
	float YAxisMin = 0.f;
	float XAxisMax = 1.f;
	float YAxisMax = 1.f;
};

enum EWRChartAxis
{
	X,
	Y,
};

USTRUCT(BlueprintType)
struct FWRGridNode
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AxisNode)
		UWRTextBlock* TextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AxisNode)
		UImage* LineImage;
};

/**
 *
 */
UCLASS()
class WR_API UWRChartBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorBrush")
		FWRChartSlateBrush GridLineBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorText")
		FName NodeFontName = TEXT("None");
protected:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorBrush", meta = (AllowPrivateAccess = "true"))
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorBrush", meta = (AllowPrivateAccess = "true"))
		float ChartTickness = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorBrush", meta = (AllowPrivateAccess = "true"))
		int32 XAxisCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorBrush", meta = (AllowPrivateAccess = "true"))
		int32 YAxisCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorBrush", meta = (AllowPrivateAccess = "true"))
		int32 MaxNodeCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "WRChartBarNodeInfo", meta = (AllowPrivateAccess = "true"))
		TArray<FWRGridNode> XGridNodes;
	UPROPERTY(BlueprintReadOnly, Category = "WRChartBarNodeInfo", meta = (AllowPrivateAccess = "true"))
		TArray<FWRGridNode> YGridNodes;
	UPROPERTY(BlueprintReadOnly, Category = "WRChartBarNodeInfo", meta = (AllowPrivateAccess = "true"))
		TMap<int32, FWRChartDataNodes> DataNodes;//TArray<FWRChartDataNode> DataNodes;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartBarInteriorText", meta = (AllowPrivateAccess = "true"))
		class UDataTable* TextStyleSet;

	FWRChartAxisMinMax AxisInfo;

	class UCanvasPanel* GridPanel;
	class UCanvasPanel* XAxisPanel;
	class UCanvasPanel* YAxisPanel;

	bool ReserveResetChart = false;
	bool ReserveResetNode = false;

public:
	FORCEINLINE int32 GetXCount() { return XGridNodes.Num();}
	FORCEINLINE int32 GetYCount() { return YGridNodes.Num(); }
	void SetDataNodes(const TMap<int32, FWRChartDataNodes>& IN InNodes);

	

	void SetGridNode(const TArray<FName>& IN InXAxisTexts, const TArray<FName>& InYAxisTexts);

	//FVector2D GetPositionOnGrid();

protected:
	virtual void InitializeChart();

	virtual void UpdateNodes();
	virtual void UpdateChart();

private:
	void ChartTickProcess();


	bool UpdateGridLine();

	void RemoveAll();

	void RemoveChart();
	void RemoveNodes();

	void InitializeGridNode(FWRGridNode& InNode, const EWRChartAxis& InEnumAxis, const bool IsCreateTextBlock = true);

	void SetDummyData();
};
