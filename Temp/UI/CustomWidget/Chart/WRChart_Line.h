// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/CustomWidget/Chart/Base/WRChartBase.h"
#include "WRChart_Line.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWRChartImageArray
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<class UImage*> ImageArray;
};

UCLASS()
class WR_API UWRChart_Line : public UWRChartBase
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	   
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartLineInteriorBrush")
	TArray<FWRChartSlateBrush> ChartLineBrushs;
private:
	bool DrawLine();

	class UImage* GetLine(const int32& InLineIndex, const int32 InDataIndex);
protected:
	virtual void InitializeChart() override;

	virtual void UpdateNodes() override;
private:
	TMap<int32, FWRChartImageArray> Lines;
};
