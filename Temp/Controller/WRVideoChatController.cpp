#include "WRVideoChatController.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterManager.h"

#include "Actor/Character/WRCharacterMine.h"

#include "Enum/EWRCharacter.h"
#include "Network/Share/Define/Define_Actor.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Network/IMMessageSender.h"
#include "Utility/WRUIUtility.h"
#include "UI/Panel/WRUIPanel_VideoChatUserList.h"
#include "UI/Panel/WRUIPanel_VideoChatScreen.h"
#include "Manager/WRNoticeManager.h"
#include "Manager/WRDialogueManager.h"
#include "Table/WRVideoChatTable.h"
#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRInputSystemManager.h"


#define DEF_VIDEO_CHAT_RECEIVING_FORCE_FEEDBACK_TID 25
WRVideoChatController::WRVideoChatController()
{

}

WRVideoChatController::~WRVideoChatController()
{

}

void WRVideoChatController::OnShutdown()
{
	Clear();
}

void WRVideoChatController::OnLoadLevelStart()
{

}

void WRVideoChatController::OnLoadLevelProcessEnded()
{
	
}

void WRVideoChatController::OpenUI_VideoChatUserList()
{
	if (WRUIManager::IsValid() == false)
		return;

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatUserList))
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	if (MyCharacter->GetChatType() != EWRChatType::Normal)
		return;

	RequestVideoChatList();
}

const FWRVideoChatUserData* WRVideoChatController::FindVideoChatUserData(const AID_t IN InAID)
{
	if (InAID == INVALID_AID)
		return nullptr;

	for (int32 i = 0; i < VideoChatUserDataArray.Num(); ++i)
	{
		if (VideoChatUserDataArray[i].PlayerAID != InAID)
			continue;
		
		return &VideoChatUserDataArray[i];
	}

	return nullptr;
}

const FWRVideoChatUserData* WRVideoChatController::FindVideoChatUserData(const int32 IN InShortcutNumber)
{
	if (InShortcutNumber == INVALID_SHORTCUT_NUMBER)
		return nullptr;


	for (int32 i = 0; i < VideoChatUserDataArray.Num(); ++i)
	{
		if (VideoChatUserDataArray[i].ShortcutNumber != InShortcutNumber)
			continue;

		return &VideoChatUserDataArray[i];
	}

	return nullptr;
}

const FWRVideoChatUserData* WRVideoChatController::FindVideoChatUserData(const FString& IN InNickName)
{
	if (InNickName.IsEmpty())
		return nullptr;

	for (int32 i = 0; i < VideoChatUserDataArray.Num(); ++i)
	{
		if(InNickName.Equals(WRUIUtility::ConvertFNameToFString(VideoChatUserDataArray[i].NickName), ESearchCase::IgnoreCase) == false)
			continue;

		return &VideoChatUserDataArray[i];
	}

	return nullptr;
}

void WRVideoChatController::CallVideoChatByShortcutNumber(const int32 IN InShortcutNumber, const bool bUpdateUserList /*= false*/)
{
	const FWRVideoChatUserData* VideoChatUserData = FindVideoChatUserData(InShortcutNumber);
	if (VideoChatUserData == nullptr)
	{
		if (bUpdateUserList)
		{
			CallReservedNickName.Empty();
			CallReservedShortcutNumber = InShortcutNumber;

			RequestVideoChatList();
		}
		
		return;
	}

	RequestBeginVideoChat(VideoChatUserData->PlayerAID);
}

void WRVideoChatController::CallVideoChatByNickName(const FString& IN InNickName, const bool bUpdateUserList /*= false*/)
{
	const FWRVideoChatUserData* VideoChatUserData = FindVideoChatUserData(InNickName);
	if (VideoChatUserData == nullptr)
	{
		if (bUpdateUserList)
		{
			CallReservedNickName = InNickName;
			CallReservedShortcutNumber = INVALID_SHORTCUT_NUMBER;

			RequestVideoChatList();
		}

		return;
	}

	RequestBeginVideoChat(VideoChatUserData->PlayerAID);
}

void WRVideoChatController::StartVideoChat()
{

}

void WRVideoChatController::StopVideoChat()
{

}

void WRVideoChatController::CancelVideoChat()
{

}

void WRVideoChatController::GetVideChatUserDataArray(TArray<FWRVideoChatUserData>& OUT OutVideoChatUserDataArray)
{
	OutVideoChatUserDataArray.Empty();
	OutVideoChatUserDataArray = VideoChatUserDataArray;
}

bool WRVideoChatController::IsPossibleVoiceRecognitionInVideoChatScreen()
{
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatScreen) == false)
		return false;

	if (CurrentVideoChatState == EWRVideoChatState::Connected)
		return false;

	return true;
}

void WRVideoChatController::OnPlayerCharacterSpawned(AWRCharacterPlayer* IN InCharacter)
{
	if (InCharacter == nullptr)
		return;

	if (InCharacter->GetCharacterType() != EWRCharacterType::Player)
		return;

	// 플레이어 타입으로 설치된 캐릭터 때문에 임시 처리(제 생각에는 플레이어 타입은 유저라고 생각이 듭니다)
	if (InCharacter->GetActorID() == INVALID_AID)
		return;

	AddVideoChatUserData(InCharacter->GetActorID(), InCharacter->GetCharacterName());
}

void WRVideoChatController::OnCharacterDestroyed(const AID_t IN InAID)
{
	//if (InAID == INVALID_AID)
	//	return;

	//for (int32 i = 0; i < VideoChatUserDataArray.Num(); ++i)
	//{
	//	if(VideoChatUserDataArray[i].PlayerAID != InAID)
	//		continue;

	//	VideoChatUserDataArray.RemoveAt(i);
	//	break;
	//}
}

bool WRVideoChatController::OnInputEvent(const EWRInput IN InEvent)
{
	if (CurrentVideoChatState == EWRVideoChatState::Calling)
	{
		if (InEvent == EWRInput::Cross)
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->PlayRejectAnimation();

				Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
				if (Panel_VideoChatScreen)
				{
					Panel_VideoChatScreen->PlayRejectAnimation();
				}

				return true;
			}
		}
	}
	else if (CurrentVideoChatState == EWRVideoChatState::Receiving)
	{
		if (InEvent == EWRInput::Circle)
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->PlayCallingAnimation();

				Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
				if (Panel_VideoChatScreen)
				{
					Panel_VideoChatScreen->PlayCallingAnimation();
				}

				return true;
			}
		}
		else if (InEvent == EWRInput::Cross)
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->PlayRejectAnimation();

				Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
				if (Panel_VideoChatScreen)
				{
					Panel_VideoChatScreen->PlayRejectAnimation();
				}

				return true;
			}
		}
	}
	else if (CurrentVideoChatState == EWRVideoChatState::Connected)
	{
		if (InEvent == EWRInput::Cross)
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->PlayRejectAnimation();

				Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
				if (Panel_VideoChatScreen)
				{
					Panel_VideoChatScreen->PlayRejectAnimation();
				}

				return true;
			}
		}
	}

	return false;
}

void WRVideoChatController::OnCallingEventReceivedFromUI(const AID_t IN InPlayerAID)
{
	if (CurrentVideoChatState == EWRVideoChatState::Receiving)
	{
		RequestAgree2BeginVideoChat(true, InPlayerAID);
	}
}

void WRVideoChatController::OnRejectEventReceivedFromUI(const AID_t IN InPlayerAID)
{
	if (CurrentVideoChatState == EWRVideoChatState::Calling)
	{
		RequestEndVideoChat(InPlayerAID);
	}
	else if (CurrentVideoChatState == EWRVideoChatState::Receiving)
	{
		RequestAgree2BeginVideoChat(false, InPlayerAID);
	}
	else if (CurrentVideoChatState == EWRVideoChatState::Connected)
	{
		RequestEndVideoChat(InPlayerAID);
	}
}

void WRVideoChatController::RequestVideoChatList()
{
	IMMessageSender::RequestVideoChatList();
}

void WRVideoChatController::RequestBeginVideoChat(const AID_t IN InReceiverAID)
{
	if (InReceiverAID == INVALID_AID)
		return;

	IMMessageSender::RequestBeginVideoChat(InReceiverAID);
}

void WRVideoChatController::RequestEndVideoChat(const AID_t IN InPlayerAID)
{
	if (CurrentVideoChatState != EWRVideoChatState::Calling && CurrentVideoChatState != EWRVideoChatState::Connected)
		return;

	IMMessageSender::RequestEndVideoChat(InPlayerAID);
}

void WRVideoChatController::RequestVideoChatMessage(const FString& IN InvoiceData, const bool IN bInfirst, const bool IN bInLast)
{
	// 아직 신경 쓰지말자
}

void WRVideoChatController::RequestAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InRequesterAID)
{
	IMMessageSender::RequestAgree2BeginVideoChat(bInAgree, InRequesterAID);
}

void WRVideoChatController::OnAckVideoChatListReceived(const bool IN bInSuccess, const std::list<stVideoChatMember>& IN InVideoChatMember)
{
	if (bInSuccess == false)
	{
		CallReservedShortcutNumber = INVALID_SHORTCUT_NUMBER;
		CallReservedNickName.Empty();
		return;
	}

	VideoChatUserDataArray.Empty();

	std::list<stVideoChatMember>::const_iterator Iter = InVideoChatMember.begin();

	for (; Iter != InVideoChatMember.end(); ++Iter)
	{
		AddVideoChatUserData(FWRVideoChatUserData(*Iter));
	}
	   
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatUserList))
	{
		UWRUIPanel_VideoChatUserList* Panel_VideoChatUserList = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatUserList>(EWRUIPanelKind::VideoChatUserList);
		if (Panel_VideoChatUserList)
		{
			Panel_VideoChatUserList->UpdateVideoChatUserList();
		}
	}
	else if (CallReservedNickName.IsEmpty() == false)
	{
		CallVideoChatByNickName(CallReservedNickName);
		CallReservedNickName.Empty();
	}
	else if (CallReservedShortcutNumber != INVALID_SHORTCUT_NUMBER)
	{
		CallVideoChatByShortcutNumber(CallReservedShortcutNumber);
		CallReservedShortcutNumber = INVALID_SHORTCUT_NUMBER;
	}
	else
	{
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::VideoChatUserList, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
}

void WRVideoChatController::OnAckBeginVideoChatReceived(const bool IN bInSuccess, const AID_t IN InReceiverAID)
{
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatUserList))
	{
		UWRUIPanel_VideoChatUserList* Panel_VideoChatUserList = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatUserList>(EWRUIPanelKind::VideoChatUserList);
		if (Panel_VideoChatUserList)
		{
			Panel_VideoChatUserList->CloseUIAfterAnimation();
		}
	}

	if (bInSuccess == false)
	{
		//WRNoticeManager::Get()->AddNotice(TEXT("VideoChat is not possible!"));
		WRTableID DialogueTID = FWRVideoChatDialogTable::GetUnableCallDialogueTID();
		if (DialogueTID != INVALID_TABLE_TID)
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

		return;
	}

	const FWRVideoChatUserData* VideoChatUserData = FindVideoChatUserData(InReceiverAID);
	if (VideoChatUserData == nullptr)
	{
		ensureMsgf(false, TEXT("WRVideoChatController::OnAckBeginVideoChatReceived - VideoChatuserData nullptr."));
		RequestEndVideoChat(InReceiverAID);
		return;
	}

	SetVideoChatState(EWRVideoChatState::Calling);

	// 통화 거는중 UI 출력
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = Cast<UWRUIPanel_VideoChatScreen>(InCombinedPanelData.UIPanel);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowCallingUI(*VideoChatUserData);
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::VideoChatScreen, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

	FWRUILoadCompletedDelegate Delegate2 = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = Cast<UWRUIPanel_VideoChatScreen>(InCombinedPanelData.UIPanel);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowCallingUI(*VideoChatUserData);
			}
		}
	});

	FWRWidgetInfo WidgetInfo2(EWRUIPanelKind::VideoChatScreen_NoCyber, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate2, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo2);
}

void WRVideoChatController::OnAckEndVideoChatReceived()
{
	SetVideoChatState(EWRVideoChatState::None);

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatScreen))
	{
		UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowDisConnectedUI();
		}

		Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowDisConnectedUI();
		}
	}
}

void WRVideoChatController::OnAckVideoChatMessageReceived()
{
	// 아직 신경 쓰지말자
}

void WRVideoChatController::OnAckAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InRequesterAID)
{
	WRInputSystemManager::Get()->ClientStopForceFeedback(DEF_VIDEO_CHAT_RECEIVING_FORCE_FEEDBACK_TID);
	if (bInAgree)
	{
		SetVideoChatState(EWRVideoChatState::Connected, InRequesterAID);

		WRTableID DialogueTID = FWRVideoChatDialogTable::GetConnectingDialogueTID();
		if (DialogueTID != INVALID_TABLE_TID)
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

		UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowConnectedUI();
		}

		Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowConnectedUI();
		}
	}
	else
	{
		SetVideoChatState(EWRVideoChatState::None);

		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatScreen))
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowDisConnectedUI();
			}

			Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowDisConnectedUI();
			}
		}
	}
}

void WRVideoChatController::OnAckRegistSpeakerReceived(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return;

	if (CurrentVideoChatState != EWRVideoChatState::Connected)
		return;

	if (InPlayerAID != VideoChatPlayerAID)
		return;

	UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->DetermineTranslationUIState();


	Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->DetermineTranslationUIState();
}

void WRVideoChatController::OnAckDeregistSpeakerReceived(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return;

	if (CurrentVideoChatState != EWRVideoChatState::Connected)
		return;

	if (InPlayerAID != VideoChatPlayerAID)
		return;

	UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->DetermineTranslationUIState();


	Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->DetermineTranslationUIState();
}

void WRVideoChatController::OnInfEndVideoChatReceived()
{
	if (CurrentVideoChatState == EWRVideoChatState::None)
		return;

	if (CurrentVideoChatState == EWRVideoChatState::Connected)
	{
		WRTableID DialogueTID = FWRVideoChatDialogTable::GetEndingCallDialogueTID();
		if (DialogueTID != INVALID_TABLE_TID)
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

		if (WRVoiceContentsManager::Get()->ContainsInMyTranslatingPlayerList(VideoChatPlayerAID))
			WRVoiceContentsManager::Get()->RequestDeregistSpeaker(VideoChatPlayerAID);
	}
	else if (CurrentVideoChatState == EWRVideoChatState::Receiving)
	{
		WRTableID DialogueTID = FWRVideoChatDialogTable::GetCancelCallDialogueTID();
		if (DialogueTID != INVALID_TABLE_TID)
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

		WRInputSystemManager::Get()->ClientStopForceFeedback(DEF_VIDEO_CHAT_RECEIVING_FORCE_FEEDBACK_TID);
	}

	SetVideoChatState(EWRVideoChatState::None);

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatScreen))
	{
		UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowDisConnectedUI();
		}

		Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowDisConnectedUI();
		}
	}
}

void WRVideoChatController::OnInfVideoChatMessageReceived()
{
	// 아직 신경 쓰지 말자
}

void WRVideoChatController::OnInfAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InReceiverAID)
{
	if (bInAgree)
	{
		SetVideoChatState(EWRVideoChatState::Connected, InReceiverAID);

		WRTableID DialogueTID = FWRVideoChatDialogTable::GetConnectingDialogueTID();
		if (DialogueTID != INVALID_TABLE_TID)
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

		UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowConnectedUI();
		}

		Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
		if (Panel_VideoChatScreen)
		{
			Panel_VideoChatScreen->ShowConnectedUI();
		}
	}
	else
	{
		SetVideoChatState(EWRVideoChatState::None);;

		WRTableID DialogueTID = FWRVideoChatDialogTable::GetRejectCallDialogueTID();
		if (DialogueTID != INVALID_TABLE_TID)
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatScreen))
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowDisConnectedUI();
			}

			Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowDisConnectedUI();
			}
		}
	}
}

void WRVideoChatController::OnInformVoiceMessage(const AID_t IN InSpeakerAID)
{
	if (CurrentVideoChatState != EWRVideoChatState::Connected)
		return;

	if (VideoChatPlayerAID != InSpeakerAID)
		return;

	UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->PlaySpeakGesture();


	Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->PlaySpeakGesture();
}

void WRVideoChatController::OnInfBeginVideoChatReceived(const stVideoChatMember& IN InRequesterData)
{
	SetVideoChatState(EWRVideoChatState::Receiving);

	WRTableID DialogueTID = FWRVideoChatDialogTable::GetReceivingDialogueTID();
	if (DialogueTID != INVALID_TABLE_TID)
		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

	// 통화 거는중 UI 출력
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = Cast<UWRUIPanel_VideoChatScreen>(InCombinedPanelData.UIPanel);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowReceivingUI(FWRVideoChatUserData(InRequesterData));
				WRInputSystemManager::Get()->ClientPlayForceFeedback(DEF_VIDEO_CHAT_RECEIVING_FORCE_FEEDBACK_TID);
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::VideoChatScreen, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);



	FWRUILoadCompletedDelegate Delegate2 = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = Cast<UWRUIPanel_VideoChatScreen>(InCombinedPanelData.UIPanel);
			if (Panel_VideoChatScreen)
			{
				Panel_VideoChatScreen->ShowReceivingUI(FWRVideoChatUserData(InRequesterData));
				WRInputSystemManager::Get()->ClientPlayForceFeedback(DEF_VIDEO_CHAT_RECEIVING_FORCE_FEEDBACK_TID);
			}
		}
	});

	FWRWidgetInfo WidgetInfo2(EWRUIPanelKind::VideoChatScreen_NoCyber, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate2, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo2);
}

void WRVideoChatController::OnInformSpeechBubbleReceived(const AID_t IN InAID, const EWRLanguageType::en IN InLanguageType, const FString& InStr)
{
	if (CurrentVideoChatState != EWRVideoChatState::Connected)
		return;

	if (VideoChatPlayerAID != InAID)
		return;

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatScreen) == false)
		return;

 	UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->AddChatBubble(FText::FromString(InStr));


	Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen_NoCyber);
	if (Panel_VideoChatScreen == nullptr)
		return;

	Panel_VideoChatScreen->AddChatBubble(FText::FromString(InStr));
}

void WRVideoChatController::Clear()
{
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatUserList))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::VideoChatUserList);
	}

	VideoChatUserDataArray.Empty();
}

void WRVideoChatController::AddVideoChatUserData(const AID_t IN InPlayerAID, const FName& IN InCharacterName, const FName& IN InNickName /*= ""*/, const int16 IN InShortcutNumber /*= 0*/, const bool IN bInOnline /*= true*/)
{
	if (InPlayerAID == INVALID_AID)
		return;

	VideoChatUserDataArray.Add(FWRVideoChatUserData(InPlayerAID, InCharacterName, InNickName, InShortcutNumber, bInOnline));
}

void WRVideoChatController::AddVideoChatUserData(const FWRVideoChatUserData& IN InVideoChatUserData)
{
	if (InVideoChatUserData.PlayerAID == INVALID_AID)
		return;

	VideoChatUserDataArray.Add(InVideoChatUserData);
}

void WRVideoChatController::SetVideoChatState(const EWRVideoChatState IN InVideoChatState, const AID_t IN InPlayerAID /*= INVALID_AID*/)
{
	CurrentVideoChatState = InVideoChatState;
	VideoChatPlayerAID = InPlayerAID;

	if (CurrentVideoChatState == EWRVideoChatState::Connected)
	{
		WRVoiceContentsManager::Get()->OnVideoChatConnected(VideoChatPlayerAID);
	}
}

FWRVideoChatUserData::FWRVideoChatUserData(const stVideoChatMember& IN InVideoChatMember)
{
	PlayerAID = InVideoChatMember.aid;
	CharacterName = WRUIUtility::ConvertWStringToFName(InVideoChatMember.name);
	NickName = WRUIUtility::ConvertWStringToFName(InVideoChatMember.nickName);
	ShortcutNumber = InVideoChatMember.shortcutNum;
	bOnline = InVideoChatMember.bOnline;
	CharacterPresetTID = InVideoChatMember.m_PlayerCostumeTID;
	CustomizeInfo = InVideoChatMember.m_PlayerCustomizeInfo;
	CostumeList.Empty();
	
	std::list<TID_t>::const_iterator iter = InVideoChatMember.listCostumeTID.begin();
	std::list<TID_t>::const_iterator iter_end = InVideoChatMember.listCostumeTID.end();
	for (; iter != iter_end; ++iter)
	{
		CostumeList.Add(*iter);
	}
}
