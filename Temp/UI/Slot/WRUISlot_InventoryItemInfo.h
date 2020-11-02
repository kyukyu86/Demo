// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Struct/WRPreviewStruct.h"
#include "Network/Share/Define/Define_Type.h"
#include "WRUISlot_InventoryItemInfo.generated.h"

struct FWRInventoryItemInfoAbility
{
	class UCanvasPanel* CanvasPanel;	
	class UImage* Image_State;
	class UWRTextBlock* WRTextBlock_AbilityName;
	class UWRTextBlock* WRTextBlock_AbilityValue;
};

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_InventoryItemInfo : public UWRUISlot
{
	GENERATED_BODY()

	friend class WRInventoryManager;
	friend class UWRUIPanel_Inventory;
	
private:
	class UWRTextBlock* WRTextBlock_Name;
	class UWRTextBlock* WRTextBlock_Description;
	class UWRUISlot_Preview* Slot_Preview;
	TArray<FWRInventoryItemInfoAbility> AbilityWidgetList;

	FString strImageAsyncLoadKey;
	float RStickInputValueX = 0.f;
	float RStickInputValueY = 0.f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float RStickSensitivity = 0.9f;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent);

	void Release();
	void SetInfo(const ISN_t IN InISN);
	void SetOption();

	void SetInfoAttached(const ISN_t IN InISN);
	void OnActorSpawnCompleted(class AActor* IN InActor);
};
