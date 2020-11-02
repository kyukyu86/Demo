// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRUISlot_InventoryEquipList.generated.h"


UCLASS()
class WR_API UWRUISlot_InventoryEquipList : public UWRUISlot
{
	GENERATED_BODY()

	friend class WRInventoryManager;

private:
	TArray<class UWRUISlot_InventoryItemSlot*> ItemSlotList;

private:
	class UWRUISlot_InventoryItemSlot* FindSlot(class UWRFocusButton* IN InFocusButton);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);

	void UpdateEquipList(TDoubleLinkedList<class WREquipItemBase*>& IN InEquippedItemList);

	// º±≈√ µ 
	void SetEquipListSlotSelected(const EWRRegistrationSlotType IN InSlotType, const bool IN InSet);
	void SetStorageSlotSelected(const ISN_t IN InISN);

	void OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet);
};
