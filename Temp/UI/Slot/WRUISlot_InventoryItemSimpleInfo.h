// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Enum/EWRUIEnum.h"
#include "WRUISlot_InventoryItemSimpleInfo.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_InventoryItemSimpleInfo : public UWRUISlot
{
	GENERATED_BODY()

	friend class WRInventoryManager;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_Name;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_Description;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_Quantity;
	
	EWRInventorySlotGroup SlotGroup = EWRInventorySlotGroup::None;

private:
	void SetEmptyInfo();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetSimpleInfo(const ISN_t IN InISN, const EWRInventorySlotGroup IN InSlotGroup);
	void SetCloseInfo();
	EWRInventorySlotGroup GetCurrentSlotGroup() { return SlotGroup; }
};
