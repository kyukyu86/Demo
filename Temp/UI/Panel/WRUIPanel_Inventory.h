// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Enum/EWRUIEnum.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRUIPanel_Inventory.generated.h"


UCLASS()
class WR_API UWRUIPanel_Inventory : public UWRUIPanel
{
	GENERATED_BODY()

	friend class WRInventoryManager;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*			WRTextBlock_Title;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWidgetSwitcher*		WidgetSwitcher_Main;

	// Category
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRUISlot_InventoryCategory*			Slot_InventoryCategory;

	// Contents
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWidgetSwitcher*						WidgetSwitcher_Middle;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRUISlot_InventoryEquipList*			Slot_InventoryEquipList;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRUISlot_InventoryInnerCategory*		Slot_InventoryInnerCategory;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRUISlot_InventoryStorage*			Slot_InventoryStorage;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UOverlay*								Overlay_ItemInfo;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRUISlot_InventoryItemSimpleInfo*	Slot_InventoryItemSimpleInfo;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRUISlot_InventoryPriceInfo*			Slot_InventoryPriceInfo;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel*							CanvasPanel_Gold;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*							WRTextBlock_Gold;

	//class UWRUISlot_InventorySetList* Slot_InventorySetList;		// [ 2020-6-16 : kyu ] 8월 버전에서는 Set없음 (feat.박광님)

	// Bottom
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel*			CanvasPanel_CircleGuide;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*			WRTextBlock_CircleGuide;


	class UWRUISlot_InventoryItemInfo*			Slot_InventoryItemInfo;			// *** Dynamic

private:
	TPair<FString, ISN_t> AsyncInfo;
	float TouchPadStartInputValueX = 0.f;
	float TouchPadLastInputValueX = 0.f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float TouchPadSensitivity = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FVector2D SimpleInfoOffset = FVector2D::ZeroVector;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent);

	void SetInventorySwitcher(const EWRInventoryMainSwitcher IN InSwitcher);
	void SetInventoryMiddleType(const EWRInventoryMiddleType IN InType);
	void SetTitle(const FString& IN InTitle);

	void CloseInventory();

	void OpenItemInformation(const ISN_t IN InISN);
	void CloseItemInformation();

	void UpdateControlGuide();
	void UpdateGoldInfo();

	FVector2D GetSimpleInfoOffset() { return SimpleInfoOffset; }

public:
	FORCEINLINE class UWRUISlot_InventoryCategory*			GetInventoryCategoryUI()		{ return Slot_InventoryCategory; }
	FORCEINLINE class UWRUISlot_InventoryEquipList*			GetInventoryEquipListUI()		{ return Slot_InventoryEquipList; }
	FORCEINLINE class UWRUISlot_InventoryInnerCategory*		GetInventoryInnerCategoryUI()	{ return Slot_InventoryInnerCategory; }
	FORCEINLINE class UWRUISlot_InventoryStorage*			GetInventoryStorageUI()			{ return Slot_InventoryStorage; }
	FORCEINLINE class UWRUISlot_InventoryItemSimpleInfo*	GetInventorySimpleInfoUI()		{ return Slot_InventoryItemSimpleInfo; }
	FORCEINLINE class UWRUISlot_InventoryPriceInfo*			GetInventoryPriceInfoUI()		{ return Slot_InventoryPriceInfo; }
	FORCEINLINE class UWRUISlot_InventoryItemInfo*			GetInventoryItemInfo()			{ return Slot_InventoryItemInfo; }	// Dynamic
};
