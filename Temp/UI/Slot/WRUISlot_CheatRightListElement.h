// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRUISlot_CheatRightListElement.generated.h"


enum class EWRCheatRightListType : uint8
{
	None,
	Item,
	Monster,
	IO,
	Character,
};


UCLASS()
class WR_API UWRUISlot_CheatRightListElement : public UWRUISlot
{
	GENERATED_BODY()

private:
	class UWRFocusButton* FocusButton_Name;
	class UWRFocusButton* FocusButton_Minus;
	class UWRFocusButton* FocusButton_Plus;
	class UWRFocusButton* FocusButton_Preview;
	class UTextBlock* TextBlock_Count;

	EWRCheatRightListType eListType;
	struct FWRTableRow* TableData;
	uint32 nCount = 1;

	class UWRFocusButton* LastFocusedButton = nullptr;
	   
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);
	
	UFUNCTION(BlueprintCallable, Category = "UWRUISlot_CheatRightListElement")
		void OnClickedName();	
	UFUNCTION(BlueprintCallable, Category = "UWRUISlot_CheatRightListElement")
		void OnClickedMinus();
	UFUNCTION(BlueprintCallable, Category = "UWRUISlot_CheatRightListElement")
		void OnClickedPlus();
	UFUNCTION(BlueprintCallable, Category = "UWRUISlot_CheatRightListElement")
		void OnClickedPreview();

	UFUNCTION(BlueprintCallable, Category = "UWRUISlot_CheatRightListElement")
		class UButton* GetFirstButton();

	void SetData(EWRCheatRightListType IN InType, struct FWRTableRow* IN InTable);

private:
	FTransform GetMyFrontTransform();

};
