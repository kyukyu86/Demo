// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Network/Share/Define/Define_Type.h"
#include "WRUISlot_InventoryStorage.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_InventoryStorage : public UWRUISlot
{
	GENERATED_BODY()

	friend class WRInventoryManager;
	friend class UWRUIPanel_Inventory;

private:
	class UWRTextBlock* WRTextBlock_Title;
	TArray<class UWRUISlot_InventoryItemSlot*> ItemSlotList;
	class UWRFocusButton* FocusButton_Prev;
	class UWRFocusButton* FocusButton_Next;
	class UWRTextBlock* WRTextBlock_Page;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		uint8 SlotWidth = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float AnalogNavigationDelay = 0.2f;

	float fNavigationElapsedDelayTIme = 0.f;
	bool bAnalogDelayTime = false;

private:
	class UWRUISlot_InventoryItemSlot* FindSlot(class UWRFocusButton* IN InFocusButton);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	//virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent);

	UFUNCTION(BlueprintCallable)
		void OnClicked_Prev();
	UFUNCTION(BlueprintCallable)
		void OnClicked_Next();

	void UpdateStorageTitle();
	void UpdateStorage(TDoubleLinkedList<class WRItemBase*>& IN InItemList);
	void UpdateStorageSlot(const ISN_t IN InISN);
	void UpdatePage();

	void SetStorageSlotSelected(const ISN_t IN InISN);

	void OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet);

	void HideAllSlot();
	
};
