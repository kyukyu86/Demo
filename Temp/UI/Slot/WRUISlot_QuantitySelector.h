// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRUISlot_QuantitySelector.generated.h"

DECLARE_DELEGATE_OneParam(FWRQuantityChangeDelegate, int32);

#define DEF_NON_SETTED_MAX_QUANTITY 0

enum EWRQuantityIndex : uint8
{
	One,				// 1
	Ten,				// 10
	Hundred,			// 100
	Thousand,			// 1,000
	TenThousand,		// 10,000
	HundredThousand,	// 100,000

	Max,
};

struct FWRPopupQuantityElenent
{
	class UCanvasPanel* CanvasPanel;
	class UWRFocusButton* FocusButton;

	int32 Value = 0;
	bool bUse = false;
};

UCLASS()
class WR_API UWRUISlot_QuantitySelector : public UWRUISlot
{
	GENERATED_BODY()
	
private:
	FWRPopupQuantityElenent ElementList[EWRQuantityIndex::Max];
	class UWRTextBlock* WRTextBlock_Contents;
	class UWRTextBlock* WRTextBlock_QuantityTest;

	FWRQuantityChangeDelegate OnQuantityChangeDelegate;
	int32 Quantity = 0;
	int32 MinQuantity = 0;
	int32 MaxQuantity = DEF_NON_SETTED_MAX_QUANTITY;
	EWRQuantityIndex MaxIndex = (EWRQuantityIndex)((int32)EWRQuantityIndex::Max - 1);

	class UWRFocusButton* LastFocusedButton = nullptr;

private:
	int32 GetValueOfIndex(const EWRQuantityIndex IN InIndex);
	void OnQuantityUp();
	void OnQuantityDown();
	void OnQuantityLeft();
	void OnQuantityRight();
	void UpdateQuantityWidget(const EWRQuantityIndex IN InIndex = EWRQuantityIndex::Max);

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);

	void SetContents(const FString IN InContents);
	void SetQuantityChangeDelegate(FWRQuantityChangeDelegate IN InDelegate);
	void SetMinQuantity(const int32 IN InQuantity);
	void SetMaxQuantity(const int32 IN InQuantity);

	void OnFocus();
};
