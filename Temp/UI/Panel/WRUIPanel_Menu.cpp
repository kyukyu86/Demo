// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Menu.h"

#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_VoiceCommandText.h"

#include "UI/CustomWidget/WRFocusButton.h"
#include "UI/CustomWidget/WRFocusButtonRich.h"

#include "UI/Component/WRUIComponentFocus.h"

#include "VerticalBox.h"
#include "Button.h"

#include "Table/WRTutorialTable.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRVoiceRecognitionTable.h"

#include "Manager/WRTutorialManager.h"
#include "Manager/UWRVoiceController.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRCharacterManager.h"

#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "CustomAsset/WRMenuAsset.h"

#include "Localization/WRLocalizationProcessor.h"



#define MENU_ASSET_PATH TEXT("/Game/Asset/Data/Asset/Widget/Menu.Menu")

void UWRUIPanel_Menu::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Title);
	BIND_WIDGET(UTextBlock, TextBlock_Page);
	BIND_WIDGET(UVerticalBox, VerticalBox_List);
 	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommand, Slot_VoiceCommand, Slot_VoiceCommand->SetVoiceWaitUIType(UWRUISlot_VoiceCommand::EWRVoiceWaitUIType::Visible);
																				 Slot_VoiceCommand->SetVoiceContentType(EWRVoiceContentType::Menu););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommandText, Slot_VoiceCommandText, Slot_VoiceCommandText->SetJustification(ETextJustify::Center);
																						Slot_VoiceCommandText->SetVoiceWaitUIType(UWRUISlot_VoiceCommandText::EWRVoiceWaitUIType::Visible););
	
	WRUIComponentFocus* UIComponentFocus = AddUIComponent<WRUIComponentFocus>(EWRUIComponentType::Focus);
	if (UIComponentFocus != nullptr)
	{
		int32 nIndex = 0;
		bool bCreating = true;
		while (bCreating)
		{
			FString WidgetName = FString::Printf(TEXT("FocusButtonRich_%d"), nIndex);
			UWRFocusButtonRich* pFocusButton = Cast<UWRFocusButtonRich>(GetWidgetFromName(FName(*WidgetName)));
			if (pFocusButton != nullptr)
			{
				pFocusButton->SetIndex(nIndex);

				// Set Focus Change Delegate
				FOnFocusChanged FocusChangedDelegate;
				FocusChangedDelegate.BindRaw(UIComponentFocus, &WRUIComponentFocus::OnFocusChanged);
				pFocusButton->SetFocusChangedDelegate(FocusChangedDelegate);
				pFocusButton->OnClicked.BindUObject(this, &UWRUIPanel_Menu::OnClicked_List);

				FocusButtonList.Add(pFocusButton);

				if (nIndex == 0)
				{
					UIComponentFocus->SetFocusButton(FocusButtonList[0], true);
				}

				++nIndex;
			}
			else
			{
				bCreating = false;
			}
		}
	}

	
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_First, FocusButton_First->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Menu::OnClicked_First););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_End, FocusButton_End->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Menu::OnClicked_End););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Prev, FocusButton_Prev->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Menu::OnClicked_Prev););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Next, FocusButton_Next->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Menu::OnClicked_Next););

	// Voice
	if (WRInputSystemManager::Get()->IsVoiceRecording() || WRInputSystemManager::Get()->IsChatRecording())
	{
		WRInputSystemManager::Get()->CancelRecording();
	}

	// Load Menu Asset
// 	FStringAssetReference MenuStrAsset = FStringAssetReference("/Game/Asset/Data/Asset/Widget/Menu.Menu");
// 	MenuAsset = Cast<UWRMenuAsset>(MenuStrAsset.TryLoad());
#ifdef DEF_ASYNC_LOAD_MENU_ASSET_KYU
	auto AsyncLoadComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		MenuAsset = Cast<UWRMenuAsset>(InObject);
		checkf(MenuAsset != nullptr, TEXT("Null MenuAsset : %s"), MENU_ASSET_PATH)
		MenuAsset->AddToRoot();

		// Set UI
		if (FocusButtonList.IsValidIndex(0))
		{
			SetFocusableInputMode(true, Cast<UWidget>(FocusButtonList[0]->GetButton()));
		}
		SetCategory(CurCategory); // to menu	

		strMenuAssetAsyncKey = "";
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	});

	strMenuAssetAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MENU_ASSET_PATH, AsyncLoadComplete);

	SetVisibility(ESlateVisibility::Hidden);
#else//DEF_ASYNC_LOAD_MENU_ASSET_KYU
	MenuAsset = LoadObject<UWRMenuAsset>(nullptr, MENU_ASSET_PATH);
	checkf(MenuAsset != nullptr, TEXT("Null MenuAsset : %s"), MENU_ASSET_PATH)
	MenuAsset->AddToRoot();

	// Set UI
	if (FocusButtonList.IsValidIndex(0))
	{
		SetFocusableInputMode(true, Cast<UWidget>(FocusButtonList[0]->GetButton()));
	}
	SetCategory(EWRMenuCategory::Main); // to menu	
#endif//DEF_ASYNC_LOAD_MENU_ASSET_KYU
}

void UWRUIPanel_Menu::NativeDestruct()
{
	if (MenuAsset->IsValidLowLevel() == true)
		MenuAsset->RemoveFromRoot();

	SetFocusableInputMode(false);
	if (WRInputSystemManager::Get()->IsVoiceRecording())
		WRInputSystemManager::Get()->CancelRecording();
	
	Super::NativeDestruct();
}

FReply UWRUIPanel_Menu::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// 닫음 키입력 추가
	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	switch (ConvertedInputType)
	{
	case EWRInput::Circle:
	{
		OnClickedFocusedButton();
		return FReply::Handled();
	}
	break;
	case EWRInput::Option:
	{
		if (CurCategory == EWRMenuCategory::Main)
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::Menu);
		}
		else
		{
			SetCategory(EWRMenuCategory::Main); // to Main
		}
		return FReply::Handled();
	}
	break;
	case EWRInput::DpadDown:
	{
		if (WRInputSystemManager::Get()->IsVoiceRecording() == false)
		{
			StartVoiceRecognition();
			return FReply::Handled();
		}
		else if (WRInputSystemManager::Get()->GetVoiceContentType() == EWRVoiceContentType::Menu)
		{
			WRVoiceContentsManager::Get()->CancelRecording();
			return FReply::Handled();
		}
	}
	break;
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

//====================================================================================

void UWRUIPanel_Menu::OnClicked_First()
{
	nPage = 1;
	UpdateList();
}

void UWRUIPanel_Menu::OnClicked_End()
{
	nPage = nMaxPage;
	UpdateList();
}

void UWRUIPanel_Menu::OnClicked_Prev()
{
	if (nPage > 1)
	{
		--nPage;
		UpdateList();
	}
}

void UWRUIPanel_Menu::OnClicked_Next()
{
	if (nPage < nMaxPage)
	{
		++nPage;
		UpdateList();
	}
}

void UWRUIPanel_Menu::OnClicked_List(int32 InIndex)
{
	if (FocusButtonList.IsValidIndex(InIndex))
	{
		int32 nValue = FocusButtonList[InIndex]->GetValue();

		switch (CurCategory)
		{
		case EWRMenuCategory::Main:
		{
			if ((EWRMenuCategory)nValue == EWRMenuCategory::Cheat)
			{
				FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Cheat, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr, Cast<AActor>(WRCharacterManager::Get()->GetMyCharacter()));
				WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

				WRUIManager::Get()->CloseUI(EWRUIPanelKind::Menu);
			}
			else if ((EWRMenuCategory)nValue == EWRMenuCategory::Inven)
			{
				FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Inventory, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr, Cast<AActor>(WRCharacterManager::Get()->GetMyCharacter()));
				WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

				WRUIManager::Get()->CloseUI(EWRUIPanelKind::Menu);
			}
			else
			{
				// 일반(메뉴목록)에서는 카테고리 선택
				SetCategory((EWRMenuCategory)nValue);
			}
		}
		break;
		case EWRMenuCategory::Tutorial:
		{
			WRTableID TableID = (WRTableID)nValue;
			if (TableID != INVALID_TABLE_TID)
			{
				WRTutorialManager::Get()->OnNewTutorial(TableID, true);
				SetVisibilityUI(false);
			}
		}
		break;
		}
	}
}

//====================================================================================

void UWRUIPanel_Menu::SetVisibilityUI(const bool IN InVisible)
{
	if (InVisible)
	{
		// 리포커싱
		if (FocusButtonList.IsValidIndex(0))
		{
			SetFocusableInputMode(true, FocusButtonList[0]);
		}
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (WRInputSystemManager::Get()->IsVoiceRecording() == false)
			StartVoiceRecognition();
	}
	else
	{
		SetFocusableInputMode(false); // 포커스 잠시 멈추고
		SetVisibility(ESlateVisibility::Collapsed);
		if (WRInputSystemManager::Get()->IsVoiceRecording())
		{
			WRInputSystemManager::Get()->CancelRecording();
		}
			
	}
}

void UWRUIPanel_Menu::StartVoiceRecognition()
{
	if (WRInputSystemManager::Get()->IsImpossibleVoiceRecording())
		return;

	TArray<FWRSubCommandData> SubCommandDatas;	
	for (auto& Iter : VoiceSubCommandMap)
	{
		if (Iter.Value == INVALID_TABLE_TID)
			continue;

		FWRVoiceSubCommandTable* VoiceSubCommandTable = WRTableManager::Get()->FindRow<FWRVoiceSubCommandTable>(Iter.Value);
		if (VoiceSubCommandTable == nullptr)
			continue;

		FWRSubCommandData TempData;
		TempData.Tid = Iter.Value;
		TempData.NounWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->NounWord);
		TempData.VerbWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->VerbWord);
		SubCommandDatas.Add(TempData);
	}
	if (SubCommandDatas.Num() == 0)
		return;

	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::Menu, SubCommandDatas, FWRVoiceEventDelegate::CreateUObject(this, &UWRUIPanel_Menu::OnResultVoiceRecognitionReceived));
}

void UWRUIPanel_Menu::OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID)
{
	// [SeungYeop 2020/07/22] : 청자 기준 번역이 들어가면서 음성명령 종료시 번역을 켜줄건지 판단해야한다.
	WRVoiceContentsManager::Get()->StartChatRecording();

	WRTableID VoiceSubCommandTID = WRConsoleManager::Get()->GetInt(EWRConsole::Cheat_VoiceTID);
	if (VoiceSubCommandTID == 0)
	{
		VoiceSubCommandTID = InVoiceSubCommandTID;
	}

	if (VoiceSubCommandTID == INVALID_TABLE_TID)
		return;

	int32 Index = -1;
	for (auto& Iter : VoiceSubCommandMap)
	{
		if (Iter.Value != VoiceSubCommandTID)
			continue;

		Index = Iter.Key;
		break;
	}

	if (Index == -1)
		return;

	OnClicked_List(Index);
}

void UWRUIPanel_Menu::SetCategory(EWRMenuCategory IN InCategory)
{
	CurCategory = InCategory;
	UpdateList(true);
}

void UWRUIPanel_Menu::SetPage(int32 IN InPage, int32 IN InMaxPage)
{
	nPage = InPage;	
	nMaxPage = InMaxPage;
	FString strPage = FString::Printf(TEXT("%d/%d"), nPage, nMaxPage);
	WRUIUtility::SetTextSafe(TextBlock_Page, strPage);
}

void UWRUIPanel_Menu::SetList(int32 IN InIndex, FString& IN InString, int32 IN InValue /* = 0 */, WRTableID IN InVoiceSubCommand /*= INVALID_TABLE_TID*/)
{
	if (FocusButtonList.IsValidIndex(InIndex))
	{
		FocusButtonList[InIndex]->SetIndex(InIndex);
		FocusButtonList[InIndex]->SetContents(InString);
		FocusButtonList[InIndex]->SetValue(InValue);
		if(InVoiceSubCommand != INVALID_TABLE_TID)
			VoiceSubCommandMap.Add(InIndex, InVoiceSubCommand);
	}
}

void UWRUIPanel_Menu::SetVisibilityList(int32 IN InIndex, bool IN InVisible)
{
	if (FocusButtonList.IsValidIndex(InIndex))
	{
		if (InVisible)
		{
			FocusButtonList[InIndex]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			FocusButtonList[InIndex]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWRUIPanel_Menu::SetVisibilityListAll(bool IN InVisible)
{
	WRUIUtility::SetVisibilitySafe(VerticalBox_List, InVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

FString UWRUIPanel_Menu::GetMenuTitle(const EWRMenuCategory IN InCategory)
{
	if (MenuAsset == nullptr)
		return "";

	FWRMenuForm* pFound = MenuAsset->Title.Find(InCategory);
	if (pFound == nullptr)
		return "";

	return pFound->MainTitle;
}

FString UWRUIPanel_Menu::GetListTitle(const EWRMenuCategory IN InCategory)
{
	if (MenuAsset == nullptr)
		return "";

	FWRMenuForm* pFound = MenuAsset->Title.Find(InCategory);
	if (pFound == nullptr)
		return "";

	return pFound->ListTitle;
}

WRTableID UWRUIPanel_Menu::GetVoiceSubCommandTID(const EWRMenuCategory IN InCategory)
{
	if (MenuAsset == nullptr)
		return INVALID_TABLE_TID;

	FWRMenuForm* pFound = MenuAsset->Title.Find(InCategory);
	if (pFound == nullptr)
		return INVALID_TABLE_TID;

	return pFound->VoiceSubCommandTID;
}

//====================================================================================

void UWRUIPanel_Menu::UpdateTitle()
{
	FString strTitle = GetMenuTitle(CurCategory);
	WRUIUtility::SetTextSafe(WRTextBlock_Title, strTitle);// "<60Font_Green>FL</><60Font>ORIDA OPTIONS</>");
}

void UWRUIPanel_Menu::UpdateList(bool IN InNew /* = false */)
{
	VoiceSubCommandMap.Empty();
	if (WRInputSystemManager::Get()->IsVoiceRecording())
	{
		WRInputSystemManager::Get()->CancelRecording();
	}

	if (InNew)
	{
		SetPage(1, nMaxPage); // 일단 첫 페이지
		if (FocusButtonList.IsValidIndex(0))
		{
			SetFocusableInputMode(true, FocusButtonList[0]); // 첫 버튼 포커싱
		}
	}

	switch (CurCategory)
	{
	case EWRMenuCategory::Main:
	{
		SetVisibilityListAll(true);
		UpdateMainMenu();
		StartVoiceRecognition();
	}
	break;
	case EWRMenuCategory::Tutorial:
	{		
		SetVisibilityListAll(true);
		UpdateTutorialList(InNew);
		StartVoiceRecognition();
	}
	break;
	case EWRMenuCategory::Option:
	{
		SetVisibilityListAll(false); // [ 2020-1-28 : kyu ] 옵션창은 리스트를 쓰는 다른 창과는 다를것이다?
		UpdateOptions();
	}
	break;
	}

	UpdateTitle();
}

void UWRUIPanel_Menu::UpdateMainMenu()
{
	// [ 2020-1-28 : kyu ] 임시
	FString strContents = GetListTitle(EWRMenuCategory::Tutorial);
	WRTableID VoiceSubCommandTID = GetVoiceSubCommandTID(EWRMenuCategory::Tutorial);
	SetList(0, strContents, (int32)EWRMenuCategory::Tutorial, VoiceSubCommandTID);
	SetVisibilityList(0, true);

	// [ 2020-02-19 : ysy ] 임시
	strContents = GetListTitle(EWRMenuCategory::Cheat);
	VoiceSubCommandTID = GetVoiceSubCommandTID(EWRMenuCategory::Cheat);
	SetList(1, strContents, (int32)EWRMenuCategory::Cheat, VoiceSubCommandTID);
	SetVisibilityList(1, true);


	SetVisibilityList(2, false);
	SetVisibilityList(3, false);
	SetVisibilityList(4, false);
	SetVisibilityList(5, false);
	SetVisibilityList(6, false);
	SetVisibilityList(7, false);

	// [ 2020-1-29 : kyu ] 임시
	SetPage(1, 1);
}

void UWRUIPanel_Menu::UpdateTutorialList(bool IN InNew /* = false */)
{
	UDataTable* pTable = WRTableManager::Get()->GetTable<FWRTutorialTable>();
	if (pTable == nullptr)
	{
		// Empty List
		SetVisibilityListAll(true);
		SetPage(0, 0);
		return;
	}

	// Init
	nDataIndex = (nPage - 1) * FocusButtonList.Num();
	
	TArray<FWRTutorialTable*> TempArray;
	pTable->GetAllRows(FString(""), TempArray);
	if (TempArray.Num() == 0)
	{
		// Empty List
		SetVisibilityListAll(true);
		SetPage(0, 0);
	}
	else
	{
		if (InNew)
		{			
			// 새로 설정시 실제 최대 갯수를 구함
			int32 nRealNum = 0;
			for (int32 i = nDataIndex; i < TempArray.Num(); ++i)
			{
				if (TempArray.IsValidIndex(i) == false)
					break;

				FWRTutorialTable* pData = TempArray[i];
				if (pData == nullptr)
					continue;

				if (pData->Menu == false)
					continue;

				++nRealNum;
			}

			if (nRealNum == FocusButtonList.Num())
			{
				nMaxPage = 1;
			}
			else
			{
				nMaxPage = (nRealNum / FocusButtonList.Num()) + 1;
			}
		}

		int32 nSlotIndex = 0;
		for (int32 i = nDataIndex; i < TempArray.Num(); ++i)
		{
			if (TempArray.IsValidIndex(i) == false)
				break;

			if (FocusButtonList.IsValidIndex(nSlotIndex) == false)
				break;

			FWRTutorialTable* pData = TempArray[i];
			if(pData == nullptr)
				continue;

			if (pData->Menu == false)
				continue;

			WRTableID TID = FCString::Atoi(*pData->GetRowName().ToString());
			/*FocusButtonList[nSlotIndex]->SetContents(pData->Title);
			FocusButtonList[nSlotIndex]->SetIndex(nSlotIndex);
			FocusButtonList[nSlotIndex]->SetValue(TID);*/

			FString titleText = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::TutorialStringTable, pData->Title).ToString();
			SetList(nSlotIndex, titleText, TID, pData->VoiceSubCommandTID);
			FocusButtonList[nSlotIndex]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			
			++nSlotIndex;
		}

		for (int32 i = nSlotIndex; i < FocusButtonList.Num(); ++i)
		{
			if (FocusButtonList.IsValidIndex(i) == false)
				break;

			FocusButtonList[i]->SetVisibility(ESlateVisibility::Hidden);
		}

		SetPage(nPage, nMaxPage);
	}	
}

void UWRUIPanel_Menu::UpdateOptions()
{
	// [ 2020-1-28 : kyu ] 옵션 셋팅하세요
}
