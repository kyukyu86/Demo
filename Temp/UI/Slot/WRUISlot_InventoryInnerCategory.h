// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRUISlot_InventoryInnerCategory.generated.h"

USTRUCT(BlueprintType)
struct FWRInventoryInnerCategoryInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FText> Names;
};

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_InventoryInnerCategory : public UWRUISlot
{
	GENERATED_BODY()

	friend class WRInventoryManager;

private:
	TArray<class UWRFocusButton*> FocusButtonList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TMap<EWRInventoryCategoryType, FWRInventoryInnerCategoryInfo> InnerCategoryInfos;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);

	void OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet);

	UFUNCTION(BlueprintCallable)
		void OnClicked_List(int32 InIndex);

	void UpdateInnerCategory();

	void SetSelectInnerCategory(const int32 IN InIndex, const bool IN InSet);
};
