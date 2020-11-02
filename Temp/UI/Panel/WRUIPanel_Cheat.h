// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "UI/Slot/WRUISlot_CheatRightListElement.h"
#include "Utility/WRUIUtility.h"
#include "Table/Base/WRTableManager.h"
#include "Actor/PreviewActor/WRPreviewStudio.h"

#include "WRUIPanel_Cheat.generated.h"


enum class EWRCheatCategory : uint8
{
	None,
	Main,
	Item,
	Monster,
	IO,
	Character,
};

UENUM(BlueprintType)
enum class EWRCheatItemSubCategory : uint8
{
	None,
	Weapon,
	Armor,
	Costume,
	Temporary,
	Vehicle,
	Material,
	Interior,
	Emote,
	Placement,	
	Housing,

	Max,
};

enum class EWRCheatRightForm : uint8
{
	Main,
	List,
};

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_Cheat : public UWRUIPanel
{
	GENERATED_BODY()
	
	//====================================================================================
	// *** 작성 금지 구역
	//====================================================================================
private:
	// Tab
	class UWRFocusButton* FocusButton_Main;
	class UWRFocusButton* FocusButton_Item;
	class UWRFocusButton* FocusButton_Monster;
	class UWRFocusButton* FocusButton_IO;
	class UWRFocusButton* FocusButton_Character;

	class UWidgetSwitcher* WidgetSwitcher_RightContents;
	// 1. Main Type
	class UCanvasPanel* CanvasPanel_RightMain;
	// 2. Sub Category
	TArray<class UWRFocusButton*> SubCategoryList;
	// 3. List Type
	class UVerticalBox* VerticalBox_RightList;
	TArray<class UWRUISlot_CheatRightListElement*> Slot_RightListElement;
	class UWRFocusButton* FocusButton_First;
	class UWRFocusButton* FocusButton_Prev;
	class UWRFocusButton* FocusButton_Next;
	class UWRFocusButton* FocusButton_Last;
	class UTextBlock* TextBlock_Page;
	//class UOverlay* Overlay_Preview;
	class UWRUISlot_Preview* Slot_Preview;
	//====================================================================================
	// *** 작성 금지 구역
	//====================================================================================
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor FocusColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor NotFocusColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> CloseKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> PagePrevKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> PageNextKey;
	//====================================================================================
	// *** 작성 금지 구역
	//====================================================================================
private:
	class UWRFocusButton* LastFocusedButton = nullptr;
	EWRCheatCategory CurCategory = EWRCheatCategory::None;
	EWRCheatItemSubCategory ItemSubCategory = EWRCheatItemSubCategory::None;
	int32 nDataIndex = 0;
	int32 nPage = 1;
	int32 nMaxPage = 1;
	FString strAsyncKey = "";

	//====================================================================================
	// *** 작성 금지 구역
	//====================================================================================
private:
	void BindMain();
	void BindSubCategory();
	void BindList();

	void HideAllList();

	void SwitchCategory(const EWRCheatCategory IN InCategory);
	void SwitchRightForm(const EWRCheatRightForm IN InList);
	void SetSelectCategory(const EWRCheatCategory IN InCategory, const bool IN InSelect);

	void UpdateItemSubCategory(const EWRCheatItemSubCategory IN InItemCategory, const int32 IN InPage = 1);

	void UpdatePage();
	template<typename T>
	void UpdateData(EWRCheatCategory IN InCategory, int32 IN InPage, const EWRCheatItemSubCategory IN InItemSubCategory = EWRCheatItemSubCategory::None);
	EWRCheatRightListType GetListTypeFromCategory(const EWRCheatCategory IN InCategory);	
	//====================================================================================
	// *** 작성 금지 구역
	//====================================================================================
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation);

	void ShowPreview(FWRPreviewInfo& IN InPreviewInfo);
	void OnFocus();

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Main();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Item();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Monster();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_IO();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Character();

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_SubCategory(int32 InIndex);

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_First();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Prev();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Next();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Last();
	

	/*====================================================================================
	
			[ 2019-11-19 : kyu ] ▼▼▼▼ 추가 기능은 이곳에 작성하세요 ▼▼▼▼
	
	====================================================================================*/
public:
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Custom_01();

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Cheat")
		void OnClicked_Custom_02();

private:
	class UWRFocusButton* FocusButton_BasicEquip;
	class UWRFocusButton* FocusButton_Custom_01;
	class UWRFocusButton* FocusButton_Custom_02;








	//====================================================================================
};



template<typename T>
void UWRUIPanel_Cheat::UpdateData(EWRCheatCategory IN InCategory, int32 IN InPage, const EWRCheatItemSubCategory IN InItemSubCategory /* = EWRCheatItemSubCategory::None */)
{
	UDataTable* pTable = WRTableManager::Get()->GetTable<T>();
	if (pTable == nullptr)
	{
		HideAllList();
		return;
	}

	// Init
	nDataIndex = (InPage - 1) * Slot_RightListElement.Num();

	TArray<T*> TempArray;
	pTable->GetAllRows(FString(""), TempArray);
	if (TempArray.Num() == 0)
	{
		HideAllList();
		InPage = 0;
	}
	else
	{
		int32 nSlotIndex = 0;
		for (int32 i = nDataIndex; i < TempArray.Num(); ++i)
		{
			if (TempArray.IsValidIndex(i) == false)
				break;

			if (Slot_RightListElement.IsValidIndex(nSlotIndex) == false)
				break;

			T* pData = TempArray[i];
			Slot_RightListElement[nSlotIndex]->SetData(GetListTypeFromCategory(InCategory), pData);
			Slot_RightListElement[nSlotIndex]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			++nSlotIndex;
		}

		for (int32 i = nSlotIndex; i < Slot_RightListElement.Num(); ++i)
		{
			if (Slot_RightListElement.IsValidIndex(i) == false)
				break;

			Slot_RightListElement[i]->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	nPage = InPage;
	nMaxPage = (TempArray.Num() / Slot_RightListElement.Num()) + 1;

	FString strPage = FString::Printf(TEXT("%d/%d"), nPage, nMaxPage);
	WRUIUtility::SetTextSafe(TextBlock_Page, strPage);

	ItemSubCategory = InItemSubCategory;
}
