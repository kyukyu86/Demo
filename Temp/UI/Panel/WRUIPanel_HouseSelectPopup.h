// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_HouseSelectPopup.generated.h"


// Test
USTRUCT(BlueprintType)
struct FWRHouseSelectPopupData
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ImagePath;
};

UCLASS()
class WR_API UWRUIPanel_HouseSelectPopup : public UWRUIPanel
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRSlideList* SlideList;
	   
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test", meta = (AllowPrivateAccess = "true", Tooltip = "For test"))
		TArray<FWRHouseSelectPopupData> TestDataList;

	TArray<FWRHouseSelectPopupData> DataList;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	void SetDataList(TArray<FWRHouseSelectPopupData>& IN InDataList);
	TArray<FWRHouseSelectPopupData>& GetDataList() { return DataList; }

};
