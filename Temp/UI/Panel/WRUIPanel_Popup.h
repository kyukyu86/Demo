// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
// [ 2019-8-16 : kyu ] Created

#pragma once

#include "CoreMinimal.h"
#include "../Base/WRUIPanel.h"

#include "Button.h"
#include "SizeBox.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Struct/WRUIStruct.h"
#include "Enum/EWRCharacter.h"


#include "WRUIPanel_Popup.generated.h"

/**
 * 
 */

UCLASS()
class WR_API UWRUIPanel_Popup : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	class UWRTextBlock* WRTextBlock_Title;
	class UWRTextBlock* WRTextBlock_Contents;

	class UCanvasPanel* CanvasPanel_FirstButton;
	class UImage* Image_FirstButton;
	class UWRTextBlock* WRTextBlock_FirstButton;

	class UCanvasPanel* CanvasPanel_SecondButton;
	class UImage* Image_SecondButton;
	class UWRTextBlock* WRTextBlock_SecondButton;

	class UOverlay*	Overlay_Quantity;
	class UWRUISlot_QuantitySelector* Slot_Quantity;	// Dynamic

private:
	FString strQuantityAsyncKey;
	FWRPopupInfo PopupInfo;
	
private:
	void SetText(class UWRTextBlock* IN InTextBlock, const FString IN InText);
	void ClosePopup();

	void MakeQuantity();
	void DestroyQuantity();

	void UpdateOption();

	void SetButtonVisibility(const bool IN InFirst, const bool IN InSecond);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Popup")
		void OnButtonClicked_First();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Popup")
		void OnButtonClicked_Second();

	void OnQuantityChanged(const int32 IN InQuantity);

public:
	void SetPopupInfo(const FWRPopupInfo& IN InInfo);
};