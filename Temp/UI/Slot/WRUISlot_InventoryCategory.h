// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WidgetAnimation.h"
#include "Enum/EWRUIEnum.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRUISlot_InventoryCategory.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_InventoryCategory : public UWRUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRFocusButton* FocusButton_Equipment;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRFocusButton* FocusButton_Costume;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRFocusButton* FocusButton_Vehicle;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRFocusButton* FocusButton_Consume;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRFocusButton* FocusButton_VideoChat;

private:
	EWRInventoryCategoryType SelectedCategory = EWRInventoryCategoryType::MAX;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation);

	void OnFocusChanged(class UWRFocusButton* IN InFocusButton, const bool IN InSet);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void OnClickedEquipment();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void OnClickedCostume();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void OnClickedVehicle();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void OnClickedConsume();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void OnClickedVideoChat();

public:
	void OnOpening();
	void OnCategorySelected(const EWRInventoryCategoryType IN InCategory);
};
