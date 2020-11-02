// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_VideoChatUserList.h"
#include "UI/Slot/WRUISlot_VideoChatUserInfo.h"

#include "Define/WRUIDefine.h"
#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRUIManager.h"

#include "Utility/WRUIUtility.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/UWRVoiceController.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRVoiceRecognitionTable.h"
#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_VoiceCommandText.h"
#include "WRUIPanel_CrossHair.h"

void UWRUIPanel_VideoChatUserList::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRUISlot_VideoChatUserInfo, Slot_VideoChatUserInfo);
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommand, Slot_VoiceCommand, Slot_VoiceCommand->SetVoiceWaitUIType(UWRUISlot_VoiceCommand::EWRVoiceWaitUIType::Visible);
																				Slot_VoiceCommand->SetVoiceContentType(EWRVoiceContentType::SocialMenu););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommandText, Slot_VoiceCommandText, Slot_VoiceCommandText->SetJustification(ETextJustify::Center);
																						Slot_VoiceCommandText->SetVoiceWaitUIType(UWRUISlot_VoiceCommandText::EWRVoiceWaitUIType::Visible););
	BIND_WIDGET(UWRTextBlock, WRTextBlock_UserCount);

	SetFocusableInputMode(true, this);

	InitVideoChatUserList();

	PlayAnimationByName("OpenAnimation");

	if (WRInputSystemManager::Get()->IsVoiceRecording() || WRInputSystemManager::Get()->IsChatRecording())
		WRInputSystemManager::Get()->CancelRecording();

	StartVoiceRecognition();

	// Show Cross Hair
	UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
	if (Panel_CrossHair)
		Panel_CrossHair->HideCrossHairPanel();
}

void UWRUIPanel_VideoChatUserList::NativeDestruct()
{
	SetFocusableInputMode(false);
	if (WRInputSystemManager::Get()->IsVoiceRecording() && WRInputSystemManager::Get()->GetVoiceContentType() == EWRVoiceContentType::VideoChatUserList)
		WRInputSystemManager::Get()->CancelRecording();

	// Show Cross Hair
	UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
	if (Panel_CrossHair)
		Panel_CrossHair->ShowCrossHairPanel();

	Super::NativeDestruct();
}

void UWRUIPanel_VideoChatUserList::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation->MovieScene->GetFName() == FName("CloseAnimation"))
	{
		// 닫힘 연출 후 끔
		WRUIManager::Get()->CloseUI(UIHandle, true);
	}
	else if (Animation->MovieScene->GetFName() == FName("ChangeNextList01Animation"))
	{
		SetPage(VideoChatUserDataIndex);
		PlayAnimationByName("ChangeNextList02Animation");
	}
	else if (Animation->MovieScene->GetFName() == FName("ChangePrevList01Animation"))
	{
		SetPage(VideoChatUserDataIndex);
		PlayAnimationByName("ChangePrevList02Animation");
	}
	else if (Animation->MovieScene->GetFName() == FName("CallingAnimation"))
	{
		if (ShortcutNumber != INVALID_SHORTCUT_NUMBER)
		{
			WRVoiceContentsManager::Get()->GetVideoChatController().CallVideoChatByShortcutNumber(ShortcutNumber);
		}
		else if (NickName.IsEmpty() == false)
		{
			WRVoiceContentsManager::Get()->GetVideoChatController().CallVideoChatByNickName(NickName);
		}
		else
		{
			WRVoiceContentsManager::Get()->GetVideoChatController().RequestBeginVideoChat(VideoChatUserDataArray[VideoChatUserDataIndex].PlayerAID);
		}
		ShortcutNumber = INVALID_SHORTCUT_NUMBER;
		NickName.Empty();
	}
	else if (Animation->MovieScene->GetFName() == FName("RejectAnimation"))
	{
		CloseUIAfterAnimation();
	}
}

FReply UWRUIPanel_VideoChatUserList::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
	{
		return FReply::Handled();
	}

	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (ConvertedInputType == EWRInput::DpadLeft)
	{
		NextPage(); 
	}
	else if (ConvertedInputType == EWRInput::DpadRight)
	{
		PrevPage();
	}
	else if (ConvertedInputType == EWRInput::Circle)
	{
		if (VideoChatUserDataIndex >= 0 && VideoChatUserDataIndex < VideoChatUserDataArray.Num())
		{
			PlayAnimationByName("CallingAnimation");
		}
	}
	else if (ConvertedInputType == EWRInput::Cross)
	{
		PlayAnimationByName("RejectAnimation");
	}
	else if (ConvertedInputType == EWRInput::DpadDown)
	{
		if (WRInputSystemManager::Get()->IsVoiceRecording() == false)
		{
			StartVoiceRecognition();
		}
		else if (WRInputSystemManager::Get()->GetVoiceContentType() == EWRVoiceContentType::VideoChatUserList)
		{
			WRVoiceContentsManager::Get()->CancelRecording();
		}
	}


	return FReply::Unhandled();
}

void UWRUIPanel_VideoChatUserList::UpdateVideoChatUserList()
{
	InitVideoChatUserList();
}

void UWRUIPanel_VideoChatUserList::OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID)
{
	// [SeungYeop 2020/07/22] : 청자 기준 번역이 들어가면서 음성명령 종료시 번역을 켜줄건지 판단해야한다.
	WRVoiceContentsManager::Get()->StartChatRecording();

	if (InVoiceSubCommandTID == INVALID_TABLE_TID)
		return;

	const FWRVideoChatVoiceSubCommandTable* FoundVideoChatVoiceSubCommandTable = FWRVideoChatVoiceSubCommandTable::FindVideoChatVoiceSubCommandTable(InVoiceSubCommandTID);
	if (FoundVideoChatVoiceSubCommandTable == nullptr)
		return;

	if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::ShortcutNumber)
	{
		ShortcutNumber = FCString::Atoi(*WRUIUtility::ConvertFNameToFString(FoundVideoChatVoiceSubCommandTable->ActionValue));
		PlayAnimationByName("CallingAnimation");
	}
	else if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::NickName)
	{
		NickName = WRUIUtility::ConvertFNameToFString(FoundVideoChatVoiceSubCommandTable->ActionValue);
		PlayAnimationByName("CallingAnimation");
	}
	else if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::DirectCall)
	{
		if (VideoChatUserDataIndex >= 0 && VideoChatUserDataIndex < VideoChatUserDataArray.Num())
		{
			PlayAnimationByName("CallingAnimation");
		}
	}
}

void UWRUIPanel_VideoChatUserList::CloseUIAfterAnimation()
{
	if (WRInputSystemManager::Get()->IsVoiceRecording() && WRInputSystemManager::Get()->GetVoiceContentType() == EWRVoiceContentType::VideoChatUserList)
		WRInputSystemManager::Get()->CancelRecording();

	PlayAnimationByName("CloseAnimation");
}

void UWRUIPanel_VideoChatUserList::InitVideoChatUserList()
{
	WRVoiceContentsManager::Get()->GetVideoChatController().GetVideChatUserDataArray(VideoChatUserDataArray);

	if (VideoChatUserDataArray.Num() <= 0)
		VideoChatUserDataIndex = -1;
	else
		VideoChatUserDataIndex = 0;

	WRTextBlock_UserCount->SetText(FText::AsNumber(VideoChatUserDataArray.Num()));
	SetPage(VideoChatUserDataIndex);
}

void UWRUIPanel_VideoChatUserList::NextPage()
{
	if (IsPlayingAnimation())
		return;

	if (VideoChatUserDataArray.Num() <= 0)
	{
		VideoChatUserDataIndex = -1;
	}
	else
	{
		++VideoChatUserDataIndex;
		if (VideoChatUserDataIndex >= VideoChatUserDataArray.Num())
			VideoChatUserDataIndex = 0;
	}

	PlayAnimationByName("ChangeNextList01Animation");
}


void UWRUIPanel_VideoChatUserList::PrevPage()
{
	if (IsPlayingAnimation())
		return;

	if (VideoChatUserDataArray.Num() <= 0)
	{
		VideoChatUserDataIndex = -1;
	}
	else
	{
		--VideoChatUserDataIndex;
		if (VideoChatUserDataIndex < 0)
			VideoChatUserDataIndex = VideoChatUserDataArray.Num() - 1;
	}

	PlayAnimationByName("ChangePrevList01Animation");
}

void UWRUIPanel_VideoChatUserList::SetPage(const int32 IN InIndex)
{
	if (VideoChatUserDataIndex >= 0)
		Slot_VideoChatUserInfo->SetUserInfo(VideoChatUserDataArray[VideoChatUserDataIndex]);
}

void UWRUIPanel_VideoChatUserList::StartVoiceRecognition()
{
	if (WRInputSystemManager::Get()->IsImpossibleVoiceRecording())
		return;
	
	UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatVoiceSubCommandTable>();
	if (DataTable == nullptr)
		return ;

	TArray<FWRSubCommandData> SubCommandDatas;
	const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

	for (auto& iter : CurrencyData)
	{
		FWRVideoChatVoiceSubCommandTable* VideoChatVoiceSubCommandTable = reinterpret_cast<FWRVideoChatVoiceSubCommandTable*>(iter.Value);
		if (VideoChatVoiceSubCommandTable == nullptr)
			continue;

		if(VideoChatVoiceSubCommandTable->ConditionType != EWRVideoChatSubCommandConditionType::VideoChatList)
			continue;

		FWRVoiceSubCommandTable* VoiceSubCommandTable = WRTableManager::Get()->FindRow<FWRVoiceSubCommandTable>(VideoChatVoiceSubCommandTable->VoiceSubCommandTID);
		if (VoiceSubCommandTable == nullptr)
			continue;

		FWRSubCommandData TempData;
		TempData.Tid = VideoChatVoiceSubCommandTable->VoiceSubCommandTID;
		TempData.NounWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->NounWord);
		TempData.VerbWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->VerbWord);
		SubCommandDatas.Add(TempData);
	}

	if (SubCommandDatas.Num() == 0)
		return;

	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::VideoChatUserList, SubCommandDatas, FWRVoiceEventDelegate::CreateUObject(this, &UWRUIPanel_VideoChatUserList::OnResultVoiceRecognitionReceived));
}
