// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Struct/WRUIStruct.h"
#include "UI/Base/WRUISlot.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Struct/WRUIStruct.h"
#include "WRUISlot_InventoryPriceInfo.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_InventoryPriceInfo : public UWRUISlot
{
	GENERATED_BODY()
	
private:
	class UCanvasPanel* CanvasPanel_Description;
	class UWRTextBlock* WRTextBlock_Notice;
	class UWRChart_Line* WRChart_Line;

	FWRFluctuationateInfo PurchaseInfo;
	FWRFluctuationateInfo SellInfo;
private:
	void SetEmptyInfo();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	void SetPriceInfo(const ISN_t IN InISN);
	void SetPriceNotice(const FString IN InNotice);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartLineInteriorBrush")
		FSlateColor UpColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRChartLineInteriorBrush")
		FSlateColor DownColor;
};
