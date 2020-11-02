// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_SelectDialogue.h"
#include "CanvasPanel.h"
#include "TextBlock.h"
#include "Engine/Texture2D.h"
#include "Image.h"
#include "Button.h"
#include "WRUIPanel_Dialogue.h"
#include "WRUIPanel_CrossHair.h"
#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_VoiceCommandText.h"
#include "UI/CustomWidget/WRFocusButton.h"

#include "Table/WRDialogueTable.h"
#include "Table/Base/WRTableManager.h"

#include "Manager/WRQuestManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRCheersManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRPopupManager.h"

#include "Utility/WRUIUtility.h"

#include "Enum/EWRConsole.h"
#include "Table/WRVoiceRecognitionTable.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Define/WRUIDefine.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WRItemManager.h"
#include "Manager/WRVoiceContentsManager.h"

#include "Localization/WRLocalizationProcessor.h"


#define MAX_SELECT_DIALOGUE__NUM 4
void UWRUIPanel_SelectDialogue::NativeConstruct()
{
	Super::NativeConstruct();

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);

	for (int32 i = 0; i < MAX_SELECT_DIALOGUE__NUM; i++)
	{
		NextDialogueInfos.Add(TPair<EWRDialogueType, WRTableID>(EWRDialogueType::None, INVALID_TABLE_TID));

		FString WigetName = FString::Printf(TEXT("CanvasPanel_Set%d"), i);
		UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(*WigetName));
		if(CanvasPanel == nullptr)
			continue;

		WigetName = FString::Printf(TEXT("FocusButtonRich_Set%d"), i);
		UWRFocusButton* FocusButton = Cast<UWRFocusButton>(GetWidgetFromName(*WigetName));
		if (FocusButton == nullptr)
			continue;

		WigetName = FString::Printf(TEXT("Image_Icon%d"), i);
		UImage* Image_Icon = Cast<UImage>(GetWidgetFromName(*WigetName));
		if (Image_Icon == nullptr)
			continue;

		WigetName = FString::Printf(TEXT("Image_Focus%d"), i);
		UImage* Image_Focus = Cast<UImage>(GetWidgetFromName(*WigetName));
		if (Image_Focus == nullptr)
			continue;

		FocusButton->SetTickFocusCheck(false);
		FocusButton->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_SelectDialogue::OnClickedDialogueSetButton);
		FocusButton->GetButton()->OnHovered.AddDynamic(this, &UWRUIPanel_SelectDialogue::OnHoveredDialogueSetButton);
		FocusButton->GetButton()->OnUnhovered.AddDynamic(this, &UWRUIPanel_SelectDialogue::OnUnHoveredDialogueSetButton);
		FocusButton->SetButtonContentAlignment(HAlign_Left);

		DialogueSetInfos.Add(FWRDialogueSetInfo(CanvasPanel, FocusButton, Image_Icon, Image_Focus));
	}

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommand, Slot_VoiceCommand, Slot_VoiceCommand->SetVoiceWaitUIType(UWRUISlot_VoiceCommand::EWRVoiceWaitUIType::Visible);
																				Slot_VoiceCommand->SetVoiceContentType(EWRVoiceContentType::SelectDialogue););

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommandText, Slot_VoiceCommandText, Slot_VoiceCommandText->SetJustification(ETextJustify::Center);
																						Slot_VoiceCommandText->SetVoiceWaitUIType(UWRUISlot_VoiceCommandText::EWRVoiceWaitUIType::Visible););

	// Voice
	if (WRInputSystemManager::Get()->IsVoiceRecording() || WRInputSystemManager::Get()->IsChatRecording())
		WRInputSystemManager::Get()->CancelRecording();

	WRInputSystemManager::Get()->SetLocationLock(true);

	MaxScaleForZoom = WRConsoleManager::Get()->GetFloat(EWRConsole::UI_MaxZoomScale_FocusButton);
	ZoomSpeed = (MaxScaleForZoom - 1) / WRConsoleManager::Get()->GetFloat(EWRConsole::UI_ZoomTime_FocusButton);

	// + UI Exception
	if (WRCheersManager::Get()->PauseCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 해제
	}

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::CostumeChange);
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn() == false && DanceModeComponent->IsEnterDanceArea())
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceMode);
	}

	WRPopupManager::Get()->ClosePopup();
}

void UWRUIPanel_SelectDialogue::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Tick_Zoom(InDeltaTime);
}

void UWRUIPanel_SelectDialogue::NativeDestruct()
{
	if(AlreadyRelaseMoveLock == false)
		WRInputSystemManager::Get()->SetLocationLock(false);

	UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
	if (Panel_CrossHair)
		Panel_CrossHair->ClosePressCircleButton();

	if (WRInputSystemManager::Get()->IsVoiceRecording())
		WRInputSystemManager::Get()->CancelRecording();

	// + UI Exception
	if (WRCheersManager::Get()->AttemptToOpenCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 설정
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn() == false && DanceModeComponent->IsEnterDanceArea())
			DanceModeComponent->ShowStartDanceUI();
	}
		
	Super::NativeDestruct();
}

void UWRUIPanel_SelectDialogue::Tick_Zoom(const float IN InDeltaTime)
{
	for (int32 i = 0; i < MAX_SELECT_DIALOGUE__NUM; i++)
	{
		bool bFocused = DialogueSetInfos[i].FocusButton->IsHovered();

		if (bFocused)
		{
			if (DialogueSetInfos[i].CurScaleForZoom < MaxScaleForZoom)
			{
				DialogueSetInfos[i].CurScaleForZoom += ZoomSpeed * InDeltaTime;
				if (DialogueSetInfos[i].CurScaleForZoom > MaxScaleForZoom)
					DialogueSetInfos[i].CurScaleForZoom = MaxScaleForZoom;

				DialogueSetInfos[i].CanvasPanel->SetRenderScale(FVector2D(DialogueSetInfos[i].CurScaleForZoom));
			}
		}
		else
		{
			if (DialogueSetInfos[i].CurScaleForZoom > 1.f)
			{
				DialogueSetInfos[i].CurScaleForZoom -= ZoomSpeed * InDeltaTime;
				if (DialogueSetInfos[i].CurScaleForZoom < 1.f)
					DialogueSetInfos[i].CurScaleForZoom = 1;

				DialogueSetInfos[i].CanvasPanel->SetRenderScale(FVector2D(DialogueSetInfos[i].CurScaleForZoom));
			}
		}
	}
}

void UWRUIPanel_SelectDialogue::SetDialogueSetGroupData(const WRTableID IN InDialogueSetGroupTID)
{
	if (InDialogueSetGroupTID == INVALID_TABLE_TID)
		return;

	FWRDialogueSetGroupTable* DialogueSetGroupTable = WRTableManager::Get()->FindRow<FWRDialogueSetGroupTable>(InDialogueSetGroupTID);
	if (DialogueSetGroupTable == nullptr)
		return;

	SetDialogueSetData(0, DialogueSetGroupTable->DialogueSet01Tid, DialogueSetGroupTable->DialogueSet01Description);
	SetDialogueSetData(1, DialogueSetGroupTable->DialogueSet02Tid, DialogueSetGroupTable->DialogueSet02Description);
	SetDialogueSetData(2, DialogueSetGroupTable->DialogueSet03Tid, DialogueSetGroupTable->DialogueSet03Description);
	SetDialogueSetData(3, DialogueSetGroupTable->DialogueSet04Tid, DialogueSetGroupTable->DialogueSet04Description);

	
	int32 Count = 0;
	for (int32 i = 0; i < DialogueSetInfos.Num(); ++i)
	{
		if (DialogueSetInfos[i].CanvasPanel->IsVisible())
			break;

		++Count;
	}

	if (Count + 1 == DialogueSetInfos.Num())
	{
		FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::Dialogue);
		if (CombinedPanelData)
		{
			UWRUIPanel_Dialogue* Panel_Dialogue = Cast<UWRUIPanel_Dialogue>(CombinedPanelData->UIPanel);
			if (Panel_Dialogue)
				Panel_Dialogue->StopDialogue();
		}

		WRUIManager::Get()->CloseUI(EWRUIPanelKind::SelectDialogue);
	}

	StartVoiceRecognition();
}

void UWRUIPanel_SelectDialogue::OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID)
{
	// [SeungYeop 2020/07/22] : 청자 기준 번역이 들어가면서 음성명령 종료시 번역을 켜줄건지 판단해야한다.
	WRVoiceContentsManager::Get()->StartChatRecording();

	WRTableID VoiceSubCommandTID = WRConsoleManager::Get()->GetInt(EWRConsole::Cheat_VoiceTID);
	if(VoiceSubCommandTID == 0)
	{
		VoiceSubCommandTID = InVoiceSubCommandTID;
	}

	if (VoiceSubCommandTID == INVALID_TABLE_TID)
		return;

	int32 Index = -1;
	for (int32 i = 0; i < MAX_SELECT_DIALOGUE__NUM; ++i)
	{
		if (DialogueSetInfos[i].VoiceSubCommandTIDs.Contains(InVoiceSubCommandTID) == false)
			continue;

		Index = i;
		break;
	}

	if (Index == -1)
		return;

	if (Index >= DialogueSetInfos.Num())
		return;

	if (Index >= NextDialogueInfos.Num())
		return;


	WRInputSystemManager::Get()->SetLocationLock(false);
	AlreadyRelaseMoveLock = true;

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::SelectDialogue);

	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::Dialogue);
	if (CombinedPanelData)
	{
		UWRUIPanel_Dialogue* Panel_Dialogue = Cast<UWRUIPanel_Dialogue>(CombinedPanelData->UIPanel);
		if (Panel_Dialogue)
		{
			Panel_Dialogue->OnDialogueSetSelected(NextDialogueInfos[Index].Key, NextDialogueInfos[Index].Value);
		}
	}
}

void UWRUIPanel_SelectDialogue::ForcedStopSelectDialogue()
{
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::SelectDialogue);

	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::Dialogue);
	if (CombinedPanelData)
	{
		UWRUIPanel_Dialogue* Panel_Dialogue = Cast<UWRUIPanel_Dialogue>(CombinedPanelData->UIPanel);
		if (Panel_Dialogue)
		{
			Panel_Dialogue->ForcedFinishDialogue();
		}
	}
}

void UWRUIPanel_SelectDialogue::OnClickedDialogueSetButton()
{
	int32 Index = -1;
	for (int32 i = 0; i < MAX_SELECT_DIALOGUE__NUM; ++i)
	{
		if ((DialogueSetInfos[i].FocusButton)->GetButton()->HasAnyUserFocus() == false)
			continue;

		Index = i;
		break;
	}

	if (Index == -1)
		return;

	if (Index >= DialogueSetInfos.Num())
		return;

	if (Index >= NextDialogueInfos.Num())
		return;

	WRInputSystemManager::Get()->SetLocationLock(false);
	AlreadyRelaseMoveLock = true;

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::SelectDialogue);

	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::Dialogue);
	if (CombinedPanelData)
	{
		UWRUIPanel_Dialogue* Panel_Dialogue = Cast<UWRUIPanel_Dialogue>(CombinedPanelData->UIPanel);
		if (Panel_Dialogue)
		{
			Panel_Dialogue->OnDialogueSetSelected(NextDialogueInfos[Index].Key, NextDialogueInfos[Index].Value);
		}
	}
}

void UWRUIPanel_SelectDialogue::OnHoveredDialogueSetButton()
{
	for (int32 i = 0; i < MAX_SELECT_DIALOGUE__NUM; ++i)
	{
		if ((DialogueSetInfos[i].FocusButton)->GetButton()->IsHovered() == false)
			continue;

		DialogueSetInfos[i].Image_Focus->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	}
	
	/*UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
	if (Panel_CrossHair)
		Panel_CrossHair->OpenPressCircleButton();*/
}

void UWRUIPanel_SelectDialogue::OnUnHoveredDialogueSetButton()
{
	int32 Index = -1;
	for (int32 i = 0; i < MAX_SELECT_DIALOGUE__NUM; ++i)
	{
		DialogueSetInfos[i].Image_Focus->SetVisibility(ESlateVisibility::Collapsed);
	}

	/*UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
	if (Panel_CrossHair)
		Panel_CrossHair->ClosePressCircleButton();*/
}

void UWRUIPanel_SelectDialogue::SetDialogueSetData(const int8 IN InIndex, const WRTableID IN InDialogueSetTID, const FName& IN InDescriptionKey)
{
	if (InIndex > DialogueSetInfos.Num())
		return;

	if (InDialogueSetTID == INVALID_TABLE_TID)
	{
		(DialogueSetInfos[InIndex].CanvasPanel)->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FWRDialogueSetTable* DialogueSetTable = WRTableManager::Get()->FindRow<FWRDialogueSetTable>(InDialogueSetTID);
	if (DialogueSetTable == nullptr)
	{
		(DialogueSetInfos[InIndex].CanvasPanel)->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	int32 MaxNum = DialogueSetTable->Conditions.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		const FWRDialogueSetCondition& Condition = DialogueSetTable->Conditions[i];
		if (Condition.Type == EWRDialogueSetCoditionType::None)
			break;

		if (CheckDialogueSetCondition(Condition) == false)
		{
			(DialogueSetInfos[InIndex].CanvasPanel)->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}
	}

	(DialogueSetInfos[InIndex].CanvasPanel)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	FString DescriptionKey = InDescriptionKey.ToString();
	(DialogueSetInfos[InIndex].FocusButton)->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::NPCStringTable, DescriptionKey).ToString());

	DialogueSetInfos[InIndex].VoiceSubCommandTIDs = DialogueSetTable->VoiceSubCommandTIDs;

	NextDialogueInfos[InIndex].Key = DialogueSetTable->NextDialogueType;
	NextDialogueInfos[InIndex].Value = DialogueSetTable->NextDialogueTID;

	if (DialogueSetTable->DialogueSetIcon.IsNone())
	{
		DialogueSetInfos[InIndex].Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		DialogueSetInfos[InIndex].Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *WRUIUtility::ConvertFNameToFString(DialogueSetTable->DialogueSetIcon));
		DialogueSetInfos[InIndex].Image_Icon->SetBrushFromTexture(Texture);
	}
}


bool UWRUIPanel_SelectDialogue::CheckDialogueSetCondition(const FWRDialogueSetCondition& IN InDialogueSetCondition)
{
	if (InDialogueSetCondition.Type == EWRDialogueSetCoditionType::CompletedQuest)
	{
		return  WRQuestManager::Get()->IsCompletedQuest(InDialogueSetCondition.Value01);
	}

	if (InDialogueSetCondition.Type == EWRDialogueSetCoditionType::ProgressingQuest)
	{
		return  WRQuestManager::Get()->IsProgressQuest(InDialogueSetCondition.Value01);
	}

	if (InDialogueSetCondition.Type == EWRDialogueSetCoditionType::HaveItem)
	{
	 	ITEM_COUNT_t ItemQuantity = WRItemManager::Get()->FindItemTotalQuantity(InDialogueSetCondition.Value01);
		if (ItemQuantity < InDialogueSetCondition.Value02)
			return false;

		//return  WRQuestManager::Get()->IsProgressQuest(InDialogueSetCondition.Value01);
	}

	return true;
}

void UWRUIPanel_SelectDialogue::StartVoiceRecognition()
{
	if (WRInputSystemManager::Get()->IsImpossibleVoiceRecording())
		return;

	TArray<FWRSubCommandData> SubCommandDatas;
	for (int32 i = 0; i < MAX_SELECT_DIALOGUE__NUM; ++i)
	{
		int32 MaxNum = DialogueSetInfos[i].VoiceSubCommandTIDs.Num();
		if (MaxNum == 0)
			continue;

		for (int32 j = 0; j < MaxNum; ++j)
		{
			FWRVoiceSubCommandTable* VoiceSubCommandTable = WRTableManager::Get()->FindRow<FWRVoiceSubCommandTable>(DialogueSetInfos[i].VoiceSubCommandTIDs[j]);
			if (VoiceSubCommandTable == nullptr)
				continue;

			FWRSubCommandData TempData;
			TempData.Tid = DialogueSetInfos[i].VoiceSubCommandTIDs[j];
			TempData.NounWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->NounWord);
			TempData.VerbWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->VerbWord);
			SubCommandDatas.Add(TempData);
		}
	}
	if (SubCommandDatas.Num() == 0)
		return;

	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::SelectDialogue, SubCommandDatas, FWRVoiceEventDelegate::CreateUObject(this, &UWRUIPanel_SelectDialogue::OnResultVoiceRecognitionReceived));
}
