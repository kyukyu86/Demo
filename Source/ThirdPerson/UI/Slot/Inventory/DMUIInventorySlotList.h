// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Base/DMUISlot.h"
#include "../../Data/DMInventoryItemSlotData.h"
#include "DMUIInventorySlotList.generated.h"

class UTileView;

UCLASS()
class THIRDPERSON_API UDMUIInventorySlotList : public UDMUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UTileView* TileView_List;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|TileView", meta = (AllowPrivateAccess = "true"))
		int32 MinTileNum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|TileView", meta = (AllowPrivateAccess = "true"))
		TArray<FDMInventoryItemSlotData> TestDataList;

private:
	int32 SelectionSlotID = DEF_INVENTORY_SLOT_INVALID_ID;
	TMap<int32, UDMInventoryItemSlotDataObject*> DataObjectList;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// for Test
	UDMInventoryItemSlotDataObject* GetDataObject(const int32 IN InFindID);

public:
// 	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
// 	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void SetFocusThis() override;

	void OnItemScrolledIntoView(UObject* InObject, UUserWidget& InUserWidget);
	void OnItemSelectionChanged(UObject* InObject);
	void OnItemClicked(UObject* InObject);
	void OnItemHoveredChanged(UObject* InObject, bool InState);

};
