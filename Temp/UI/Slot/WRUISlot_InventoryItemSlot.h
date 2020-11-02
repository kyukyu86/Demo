// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "WRUISlot_ItemIcon.h"

#include "UI/Base/WRUISlot.h"
#include "UI/CustomWidget/WRFocusButton.h"

#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_BluePrint.h"

#include "Enum/EWRUIEnum.h"

#include "Struct/WRUIStruct.h"

#include "WRUISlot_InventoryItemSlot.generated.h"



UCLASS()
class WR_API UWRUISlot_InventoryItemSlot : public UWRUISlot
{
	GENERATED_BODY()

private:
	UWRUISlot_ItemIcon* Slot_ItemIcon;
	class UWRFocusButton* FocusButton;
	class UImage* Image_Default;
	class UWRTextBlock* WRTextBlock_DefaultText;
	class UImage* Image_Equipped;
	class UImage* Image_New;
	class UImage* Image_Registered;
	class UImage* Image_Selectable;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCanvasPanel* CanvasPanel_Spawned;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		FWRInventorySlotInfo SlotInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		FWRFocusButtonManualInfo FocusButtonManualInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		bool bUseDefaultImage = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true", EditCondition = "bUseDefaultImage"))
		class UTexture2D* DefaultImage;

private:
	void SetEmptySlot();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);

	UFUNCTION(BlueprintCallable)
		void OnClickedSlot();

public:
	void SetItemIcon(FWRItemIcon& IN InInfo);
	void SetItemIcon(const WRTableID IN InTID);
	void SetItemIcon(class WRItemBase* IN InItemBase);
	void SetSelect(const bool IN InSet);
	void SetSelectable(const bool IN InSet);

	void UpdateSlot();
	
	FORCEINLINE void SetSlotIndex(const int32 IN InIndex) { SlotInfo.SlotIndex = InIndex; }
	FORCEINLINE int32 GetSlotIndex() { return SlotInfo.SlotIndex; }
	FORCEINLINE class UWRFocusButton* GetFocusButton() { return FocusButton; }
	FORCEINLINE ISN_t GetISN() { return SlotInfo.ISN; }
	FORCEINLINE const FWRInventorySlotInfo& GetSlotInfo() { return SlotInfo; }
	FORCEINLINE EWRRegistrationSlotType GetRegistrationSlotType() { return SlotInfo.GetRegistrationSlotType(); }
};
