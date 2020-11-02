// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Enum/EWRUIEnum.h"
#include "WRUIPanel_Menu.generated.h"

#define DEF_ASYNC_LOAD_MENU_ASSET_KYU

UCLASS()
class WR_API UWRUIPanel_Menu : public UWRUIPanel
{
	GENERATED_BODY()
	
private:
	class UWRTextBlock* WRTextBlock_Title;
	class UVerticalBox* VerticalBox_List;
	TArray<class UWRFocusButtonRich*> FocusButtonList;
	class UTextBlock* TextBlock_Page;

	class UWRFocusButton* FocusButton_First;
	class UWRFocusButton* FocusButton_End;
	class UWRFocusButton* FocusButton_Prev;
	class UWRFocusButton* FocusButton_Next;
	class UWRUISlot_VoiceCommand* Slot_VoiceCommand;
	class UWRUISlot_VoiceCommandText* Slot_VoiceCommandText;
	
private:
	EWRMenuCategory CurCategory = EWRMenuCategory::Main;
	int32 nDataIndex = 0;
	int32 nPage = 1;
	int32 nMaxPage = 1;
	
	class UWRMenuAsset* MenuAsset = nullptr;
#ifdef DEF_ASYNC_LOAD_MENU_ASSET_KYU
	// Async
	FString strMenuAssetAsyncKey = "";
#else//DEF_ASYNC_LOAD_MENU_ASSET_KYU
#endif//DEF_ASYNC_LOAD_MENU_ASSET_KYU

	TMap<int32, WRTableID> VoiceSubCommandMap;	// <index, VoiceSubCommandTID>

private:
	void SetPage(int32 IN InPage, int32 IN InMaxPage);

	void SetList(int32 IN InIndex, FString& IN InString, int32 IN InValue = 0, WRTableID IN InVoiceSubCommand = INVALID_TABLE_TID);
	void SetVisibilityList(int32 IN InIndex, bool IN InVisible);
	void SetVisibilityListAll(bool IN InVisible);

	FString GetMenuTitle(const EWRMenuCategory IN InCategory);
	FString GetListTitle(const EWRMenuCategory IN InCategory);
	WRTableID GetVoiceSubCommandTID(const EWRMenuCategory IN InCategory);

	void UpdateTitle();
	void UpdateList(bool IN InNew = false);
	void UpdateMainMenu();
	void UpdateTutorialList(bool IN InNew = false);
	void UpdateOptions();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Menu")
		void OnClicked_First();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Menu")
		void OnClicked_End();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Menu")
		void OnClicked_Prev();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Menu")
		void OnClicked_Next();
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_Menu")
		void OnClicked_List(int32 InIndex);

	void SetVisibilityUI(const bool IN InVisible);
	void SetCategory(EWRMenuCategory IN InCategory);

	// Voice
	class UWRUISlot_VoiceCommand* GetSlotVoiceCommand() { return Slot_VoiceCommand; }
	class UWRUISlot_VoiceCommandText* GetSlotVoiceCommandText() { return Slot_VoiceCommandText; }
	void StartVoiceRecognition();
	void OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID);	// 음성인식 결과 받음
	

};