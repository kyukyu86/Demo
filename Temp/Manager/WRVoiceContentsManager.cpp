// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRVoiceContentsManager.h"
#include "WRConsoleManager.h"
#include "WRQuestManager.h"
#include "WRCompassManager.h"
#include "WRPopupManager.h"
#include "WRTriggerManager.h"
#include "WRCharacterManager.h"
#include "WREnvInteractionManager.h"
#include "WRInputSystemManager.h"
#include "WRDialogueManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRVoiceRecognitionTable.h"
#include "Table/WRCoordinateTable.h"

#include "UI/Panel/WRUIPanel_DefaultVoiceCommand.h"
#include "UI/Panel/WRUIPanel_Menu.h"
#include "UI/Panel/WRUIPanel_SelectDialogue.h"
#include "UI/Panel/WRUIPanel_SocialMenu.h"
#include "UI/Panel/WRUIPanel_Main.h"
#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_VoiceCommandText.h"

#include "Network/IMMessageSender.h"
#include "Component/Character/WRActorComponentAttack.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Utility/WRUIUtility.h"
#include "WRTargetLocationGuideManager.h"
#include "Utility/WRActorUtility.h"
#include "Table/WRPlayerTable.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "UI/Panel/WRUIPanel_TargetLocationGuide.h"
#include "UI/Panel/WRUIPanel_RepeatSecretDialog.h"
#include "Table/WREnvObjectTable.h"
#include "Component/Character/WRActorComponentChatRecording.h"
#include "WRUIManager.h"
#include "Camera/CameraComponent.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "WRInventoryManager.h"
#include "Table/WRCustomTextTable.h"
#include "WRNoticeManager.h"
#include "WRSFXManager.h"
#include "UI/Panel/WRUIPanel_VideoChatUserList.h"
#include "UI/Panel/WRUIPanel_VideoChatScreen.h"

#include "Localization/WRLocalizationProcessor.h"

#define ECC_HeadTrace ECC_GameTraceChannel7

void WRVoiceContentsManager::OnInitialize()
{

}

void WRVoiceContentsManager::OnShutdown()
{
	AvailableVoiceCommandTIDs.Empty();
	RecommendedVoiceCommandTIDs.Empty();

	MyTranslatingPlayerList.Empty();
	TranslatingMePlayerList.Empty();
	WaitTranslatingPacketList.Empty();
}

void WRVoiceContentsManager::OnLoadLevelStart()
{
	AvailableVoiceCommandTIDs.Empty();
	RecommendedVoiceCommandTIDs.Empty();

	MyTranslatingPlayerList.Empty();
	TranslatingMePlayerList.Empty();
	WaitTranslatingPacketList.Empty();
}

void WRVoiceContentsManager::OnLoadLevelComplete()
{

}

void WRVoiceContentsManager::OnRenderStart()
{

}

bool WRVoiceContentsManager::OnTick(float InDeltaTime)
{
	// Remove FocusTalkController
	Tick_ReservedRemoveFocusTalk();

	// Tick FocusTalkController
	for (auto& Iter : FocusTalkControllerMap)
	{
		WRFocusTalkController& FocusTalkController = Iter.Value;
		FocusTalkController.Tick(InDeltaTime);
	}

	return true;
}

void WRVoiceContentsManager::StartDefaultVoiceRecording()
{
	if (CheckAvailableVoiceRecording() == false)
		return;

	bRequestPacketToServer = true;
	bStartDialogueWhenVoiceRecording = true;
	RetryDefaultVoiceRecordingCount = 0;
	TArray<FWRSubCommandData> SubCommandList;
	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::Default, SubCommandList, nullptr);
}

void WRVoiceContentsManager::StartDefaultVoiceRecordingAtRetry()
{
	if (CheckAvailableVoiceRecording() == false)
		return;

	bRequestPacketToServer = false;
	bStartDialogueWhenVoiceRecording = false;
	++RetryDefaultVoiceRecordingCount;
	TArray<FWRSubCommandData> SubCommandList;
	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::Default, SubCommandList, nullptr);
}

void WRVoiceContentsManager::StartDefaultVoiceRecordingAtTrigger()
{
	if (CheckAvailableVoiceRecording() == false)
		return;

	bRequestPacketToServer = true;
	bStartDialogueWhenVoiceRecording = false;
	RetryDefaultVoiceRecordingCount = 0;
	TArray<FWRSubCommandData> SubCommandList;
	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::Default, SubCommandList, nullptr);
}

void WRVoiceContentsManager::StartChatRecording()
{
	if (IsEmptyTranslatingMePlayerList())
		return;

	if (WRInputSystemManager::Get()->IsVoiceRecording())
		return;

	if (WRInputSystemManager::Get()->IsChatRecording())
		return;

	WRInputSystemManager::Get()->StartChatRecording();
}

void WRVoiceContentsManager::StopVoiceRecording()
{
	WRInputSystemManager::Get()->StopRecording();
}

void WRVoiceContentsManager::CancelRecording()
{
	if (WRInputSystemManager::Get()->GetVoiceContentType() == EWRVoiceContentType::Default)
	{
		RetryDefaultVoiceRecordingCount = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_RetryCount);
	}

	WRInputSystemManager::Get()->CancelRecording();
}

void WRVoiceContentsManager::OnDefaultVoiceRecordResulted(EWRVoiceWordType IN InVoiceEventType, WRTableID InVoiceCommandDataTID)
{
	int32 CheatTID = WRConsoleManager::Get()->GetInt(EWRConsole::Cheat_VoiceTID);
	if (CheatTID != 0)
		InVoiceCommandDataTID = CheatTID;

	FWRVoiceCommandData* VoiceCommandData = nullptr;
	if (InVoiceCommandDataTID != INVALID_TABLE_TID)
	{
		VoiceCommandData = WRTableManager::Get()->FindRow<FWRVoiceCommandData>(InVoiceCommandDataTID);
		if (VoiceCommandData)
			InVoiceEventType = VoiceCommandData->WordType;
	}

	FString Text = FString::Printf(TEXT("[WRInputSystemManager::InputVoiceEvent] %d"), InVoiceEventType);
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	FWRVoiceEventDialogue* VoiceEventDialogueTable = FWRVoiceEventDialogue::GetVoiceEventDialogueData(InVoiceEventType);

	if (VoiceCommandData && VoiceCommandData->VolumeCheck && AvailableVoiceCommandTIDs.Contains(InVoiceCommandDataTID) == false)
	{
		FWRVoiceEventDialogue::OpenRandVolumeImposibbleDialogue(VoiceEventDialogueTable);

		IMMessageSender::RequestEndVoiceCommand();
		StartChatRecording();
		return;
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter && MyCharacter->GetBoardingVehicleAID() != INVALID_AID)
	{
		if (VoiceCommandData && VoiceCommandData->UseWhileDriving == false)
		{
			FWRVoiceEventDialogue::OpenRandVolumeImposibbleDialogue(VoiceEventDialogueTable);

			IMMessageSender::RequestEndVoiceCommand();
			StartChatRecording();
			return;
		}
	}	

	switch (InVoiceEventType)
	{
	case EWRVoiceWordType::EmptyVoice:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
	}
	break;
	case EWRVoiceWordType::Unknown:
	{
		if (RetryDefaultVoiceRecordingCount < WRConsoleManager::Get()->GetInt(EWRConsole::Voice_RetryCount))
		{
			WRTableID DialogueTID = WRConsoleManager::Get()->GetInt(EWRConsole::Voice_RetryDialogueTID);
			if (DialogueTID != INVALID_TABLE_TID)
				WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);

			StartDefaultVoiceRecordingAtRetry();
			return;	// IMMessageSender::RequestEndVoiceCommand(); 이게 호출되면 안되기에 return 한다.
		}
		else
		{
			FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
		}
	}
	break;
	case EWRVoiceWordType::WakeUp:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
	}
	break;
	case EWRVoiceWordType::Heal:
	{
		if (MyCharacter)
		{
			UWRActorComponentAttack* ActorComponentAttack = MyCharacter->GetComponent<UWRActorComponentAttack>();
			if (ActorComponentAttack)
			{
				if (ActorComponentAttack->IsActiveCoolTime(DEF_AIDA_HEAL_SKILL_TABLE_ID))
				{
					FWRVoiceEventDialogue::OpenRandImpossibleDialogue(VoiceEventDialogueTable);
				}
				else
				{
					FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);

					MyCharacter->UseAidaHeal();
				}
			}
		}
	}
	break;
	case EWRVoiceWordType::QuestInfo:
	{
		const FWRQuestTable* QuestTable = WRQuestManager::Get()->FindQuestTable(WRQuestManager::Get()->GetTrackedQuestTID());
		if (QuestTable && QuestTable->SummaryDialogueTID != INVALID_TABLE_TID)
		{
			FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, QuestTable->SummaryDialogueTID);

			float IgnoreDistCheckingTime = 5.f;
			WRCompassManager::Get()->StartIgnoreDistChecking(IgnoreDistCheckingTime);
		}
		else
		{
			FWRVoiceEventDialogue::OpenRandImpossibleDialogue(VoiceEventDialogueTable);
		}
	}
	break;
	case  EWRVoiceWordType::Tutorial:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
			{
				InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
				UWRUIPanel_Menu* Panel_Menu = Cast<UWRUIPanel_Menu>(InCombinedPanelData.UIPanel);
				if (Panel_Menu)
					Panel_Menu->SetCategory(EWRMenuCategory::Tutorial);
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Menu, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
	break;
	case  EWRVoiceWordType::Teleport:
	{
		if (VoiceCommandData && VoiceEventDialogueTable)
		{
			FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(VoiceCommandData->TargetTID);
			if (CoordinateTable)
			{
				FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);

				FTransform Transform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, 0), FVector::OneVector);
				FWRPopupInfo PopupInfo;
				PopupInfo.eButtonFlag = EPopupButtonFlag::OK_CANCEL;
				FString LocalKey = WRUIUtility::ConvertFNameToFString(CoordinateTable->NameLocalkey);
				PopupInfo.Contents = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString();
				PopupInfo.Delegate_First = FOnPopupButtonDelegate::CreateUObject(WRCharacterManager::Get()->GetMyCharacter(), &AWRCharacterMine::TeleportUsingVoiceRecognition, VoiceCommandData->TargetTID);
				PopupInfo.Delegate_Second = FOnPopupButtonDelegate::CreateStatic(&IMMessageSender::RequestEndVoiceCommand);
				WRPopupManager::Get()->OpenPopup(PopupInfo, Transform);
			}
		}
	}
	break;
	case  EWRVoiceWordType::Summon:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);

		if (MyCharacter)
			MyCharacter->CallAida();
	}
	break;
	case EWRVoiceWordType::Option:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
		
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::Menu) == false)
		{
			if (WRUIManager::Get()->IsWidgetCreating(EWRUIPanelKind::Menu))
				return;

			FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
			{
				if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
				{
					InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
				}
			});

			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Menu, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		}
	}
	break;
	case EWRVoiceWordType::Trigger:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
		WRTriggerManager::Get()->RunActiveTrigger(VoiceCommandData->TargetTID);
	}
	break;
	case EWRVoiceWordType::Dialogue:
	{
		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::NpcTalk, VoiceCommandData->TargetTID, FSimpleDelegate::CreateStatic(&IMMessageSender::RequestEndVoiceCommand));
	}
	break;
	case EWRVoiceWordType::ChatRecord:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);

		if (WRCharacterManager::IsValid())
		{
			const AWRCharacter* MyTargetCharacter = WRCharacterManager::Get()->GetMyTarget();
			if (MyTargetCharacter && WRVoiceContentsManager::Get()->ContainsInMyTranslatingPlayerList(MyTargetCharacter->GetActorID()) == false)
				RequestRegistSpeaker(MyTargetCharacter->GetActorID());
		}

		//StartChatRecording();
	}
	break;
	case EWRVoiceWordType::StopVoiceRecord:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
	}
	break;
	case EWRVoiceWordType::Inventory:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);

		if (WRInventoryManager::IsValid())
		{
			WRInventoryManager::Get()->OpenInventory(EWRInventoryCategoryType(VoiceCommandData->TargetTID));
		}
	}
	break;
	case EWRVoiceWordType::VideoChat:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
		WRVoiceContentsManager::Get()->GetVideoChatController().OpenUI_VideoChatUserList();
	}
	break;
	case EWRVoiceWordType::VideoChatShortcutNumber:
	case EWRVoiceWordType::VideoChatNickname:
	{
		if (VoiceCommandData->TargetTID == INVALID_TABLE_TID)
			break;

		const FWRVideoChatVoiceSubCommandTable* FoundVideoChatVoiceSubCommandTable = WRTableManager::Get()->FindRow<FWRVideoChatVoiceSubCommandTable>(VoiceCommandData->TargetTID);
		if (FoundVideoChatVoiceSubCommandTable == nullptr)
			break;

		if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::ShortcutNumber)
		{
			int32 ShortcutNumber = FCString::Atoi(*WRUIUtility::ConvertFNameToFString(FoundVideoChatVoiceSubCommandTable->ActionValue));
			WRVoiceContentsManager::Get()->GetVideoChatController().CallVideoChatByShortcutNumber(ShortcutNumber, true);
		}
		else if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::NickName)
		{
			WRVoiceContentsManager::Get()->GetVideoChatController().CallVideoChatByNickName(WRUIUtility::ConvertFNameToFString(FoundVideoChatVoiceSubCommandTable->ActionValue), true);
		}	
	}
	break;
	default:
	{
		FWRVoiceEventDialogue::OpenRandUnderstandDialogue(VoiceEventDialogueTable);
	}
	break;
	}

	// 팝업이 뜨는경우가 아니면 바로 end
	if (InVoiceEventType != EWRVoiceWordType::Teleport && InVoiceEventType != EWRVoiceWordType::Dialogue)
		IMMessageSender::RequestEndVoiceCommand();

	StartChatRecording();
}

void WRVoiceContentsManager::OnVoiceRecordingStateChanged(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	UpdateUI(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);
	if (bInChatRecording)
		UpdateChatRecording(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);

	if (bRequestPacketToServer)
	{
		if (!bInChatRecording && InVoiceContentType == EWRVoiceContentType::Default && InVoiceRecordingState == EWRVoiceRecordingStateType::DELAY)
		{

			IMMessageSender::RequestBeginVoiceCommand(bStartDialogueWhenVoiceRecording);
		}
	}
}

void WRVoiceContentsManager::OnImpossibleVoiceRecordingChanged(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{

}

void WRVoiceContentsManager::OnVoiceChatSended()
{
	UpdateChatRecording(EWRVoiceRecordingStateType::RECORDING, EWRVoiceContentType::NONE, true, false);
}

void WRVoiceContentsManager::OnChatRecordingForciblyStoped()
{
	if (IsEmptyTranslatingMePlayerList())
		return;

	IMMessageSender::RequestReleaseListener(INVALID_AID);
}

void WRVoiceContentsManager::OnCharacterDestroyed(const AID_t IN InAID)
{
	if (ContainsInMyTranslatingPlayerList(InAID))
	{
		RequestDeregistSpeaker(InAID);
	}
}

void WRVoiceContentsManager::OnVideoChatConnected(const AID_t IN InTargetAID)
{
	if (MyTranslatingPlayerList.Num() == 1 && ContainsInMyTranslatingPlayerList(InTargetAID))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::ChatRecordingState);
	}
}

void WRVoiceContentsManager::AddAvailableVoiceCommand(const TArray<WRTableID>& IN InVoiceCommandTIDs)
{
	int32 MaxNum = InVoiceCommandTIDs.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		WRTableID VoiceCommandTID = InVoiceCommandTIDs[i];
		int32* Count = AvailableVoiceCommandTIDs.Find(VoiceCommandTID);
		if (Count)
		{
			++(*Count);
		}
		else
		{
			AvailableVoiceCommandTIDs.Add(VoiceCommandTID, 1);
		}
	}
}

void WRVoiceContentsManager::RemoveAvailableVoiceCommand(const TArray<WRTableID>& IN InVoiceCommandTIDs)
{
	int32 MaxNum = InVoiceCommandTIDs.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		WRTableID VoiceCommandTID = InVoiceCommandTIDs[i];
		int32* Count = AvailableVoiceCommandTIDs.Find(VoiceCommandTID);
		if (Count == nullptr)
			return;

		--(*Count);
		if (*Count <= 0)
			AvailableVoiceCommandTIDs.Remove(VoiceCommandTID);
	}
}

void WRVoiceContentsManager::AddRecommendedVoiceCommand(const TArray<WRTableID>& IN InRecommendedVoiceCommandTIDs)
{
	int32 MaxNum = InRecommendedVoiceCommandTIDs.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		WRTableID VoiceCommandTID = InRecommendedVoiceCommandTIDs[i];
		int32* Count = RecommendedVoiceCommandTIDs.Find(VoiceCommandTID);
		if (Count)
		{
			++(*Count);
		}
		else
		{
			RecommendedVoiceCommandTIDs.Add(VoiceCommandTID, 1);
		}
	}
}

void WRVoiceContentsManager::RemoveRecommendedVoiceCommand(const TArray<WRTableID>& IN InRecommendedVoiceCommandTIDs)
{
	int32 MaxNum = InRecommendedVoiceCommandTIDs.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		WRTableID VoiceCommandTID = InRecommendedVoiceCommandTIDs[i];
		int32* Count = RecommendedVoiceCommandTIDs.Find(VoiceCommandTID);
		if (Count == nullptr)
			return;

		--(*Count);
		if (*Count <= 0)
			RecommendedVoiceCommandTIDs.Remove(VoiceCommandTID);
	}
}

void WRVoiceContentsManager::GetRemmendedVoiceCommand(TArray<WRTableID>& OUT OutRecommendedVoiceCommandTIDs)
{
	OutRecommendedVoiceCommandTIDs.Empty();

	for (auto& Iter : RecommendedVoiceCommandTIDs)
	{
		WRTableID RecommendedVoiceTID = Iter.Key;
		OutRecommendedVoiceCommandTIDs.Add(RecommendedVoiceTID);
	}
}

void WRVoiceContentsManager::ReserveToRemoveFocusTalkController(const AID_t IN InPlayerAID)
{
	ReservedRemoveFocusTalkList.AddTail(InPlayerAID);
}

bool WRVoiceContentsManager::RotateHMDInDirectionOfFocusTalkTarget()
{
	if (FocusTalkControllerMap.Num() == 0)
		return false;

	AID_t TargetAID = GetTargetAIDForHMDRotationInFocusTalk();

	AWRCharacter* TargetCharacter = WRCharacterManager::Get()->FindbyAID(TargetAID);
	if (TargetCharacter == nullptr)
		return false;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return false;

	FVector DirVec = TargetCharacter->GetActorLocation() - MyCharacter->GetActorLocation();

	FRotator MyCharacterRotator = MyCharacter->GetActorRotation();
	MyCharacterRotator.Yaw = DirVec.Rotation().Yaw;
	//MyCharacter->SetActorRotation(MyCharacterRotator);

	//if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	//{
	//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(DirVec.Rotation().Yaw);
	//}

	MyCharacter->GetRotator()->RotateCharacterWithHMD(MyCharacterRotator);
	IMMessageSender::RequestPlayerMoveStop(MyCharacter);

	return true;
}

AID_t WRVoiceContentsManager::GetTargetAIDForHMDRotationInFocusTalk()
{
	if (FocusTalkControllerMap.Num() == 0)
		return INVALID_AID;

	for (auto& Iter : FocusTalkControllerMap)
	{
		return Iter.Key;
	}

	return INVALID_AID;
}

bool WRVoiceContentsManager::IsVisibleFocusTalkUI(const AID_t IN InPlayerAID)
{
	if (FocusTalkControllerMap.Num() == 0)
		return false;

	const WRFocusTalkController* FocusTalkController = FocusTalkControllerMap.Find(InPlayerAID);
	if (FocusTalkController == nullptr)
		return false;

	return FocusTalkController->IsVisibleFocusTalkUI();
}

const TDoubleLinkedList<AID_t>& WRVoiceContentsManager::GetMyTranslatingPlayerList()
{
	return MyTranslatingPlayerList;
}

const TDoubleLinkedList<AID_t>& WRVoiceContentsManager::GetTranslatingMePlayerList()
{
	return TranslatingMePlayerList;
}

bool WRVoiceContentsManager::ContainsInMyTranslatingPlayerList(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return false;

	return MyTranslatingPlayerList.Contains(InPlayerAID);
}

bool WRVoiceContentsManager::ContainsInTranslatingMePlayerList(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return false;

	return TranslatingMePlayerList.Contains(InPlayerAID);
}

bool WRVoiceContentsManager::ContainsInWaitTranslatingPacketList(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return false;

	return WaitTranslatingPacketList.Contains(InPlayerAID);
}

bool WRVoiceContentsManager::IsEmptyMyTranslatingPlayerList()
{
	return (MyTranslatingPlayerList.Num() > 0 ? false : true);
}

bool WRVoiceContentsManager::IsEmptyTranslatingMePlayerList()
{
	return (TranslatingMePlayerList.Num() > 0 ? false : true);
}

void WRVoiceContentsManager::AddMyTranslatingPlayer(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return;

	if (MyTranslatingPlayerList.Contains(InPlayerAID))
		return;

	MyTranslatingPlayerList.AddTail(InPlayerAID);

	if (WRCharacterManager::IsValid())
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter)
		{
			UWRActorComponentChatRecording* ChatRecordingComponent = MyCharacter->GetComponent<UWRActorComponentChatRecording>();
			if (ChatRecordingComponent)
				ChatRecordingComponent->CheckTickNeeded();
		}
	}

	if (IsEmptyMyTranslatingPlayerList() == false && WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::ChatRecordingState) == false)
	{
		if (MyTranslatingPlayerList.Num() == 1 && VideoChatController.GetCurrentVideoChatState() == WRVideoChatController::EWRVideoChatState::Connected
			&& ContainsInMyTranslatingPlayerList(VideoChatController.GetVideoChatPlayerAID()))
		{
			// 유일한 목록이 화상채팅 대상이면 UI를 안띄운다.
		}
		else
		{
			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::ChatRecordingState, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr);
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		}
	}
}

void WRVoiceContentsManager::AddTranslatingMePlayer(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return;

	if (TranslatingMePlayerList.Contains(InPlayerAID))
		return;

	TranslatingMePlayerList.AddTail(InPlayerAID);

	StartChatRecording();
}

void WRVoiceContentsManager::RemoveMyTranslatingPlayer(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return;

	if (MyTranslatingPlayerList.Contains(InPlayerAID) == false)
		return;

	MyTranslatingPlayerList.RemoveNode(InPlayerAID);

	if (WRCharacterManager::IsValid())
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter)
		{
			UWRActorComponentChatRecording* ChatRecordingComponent = MyCharacter->GetComponent<UWRActorComponentChatRecording>();
			if (ChatRecordingComponent)
				ChatRecordingComponent->CheckTickNeeded();
		}
	}

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::ChatRecordingState))
	{
		if (IsEmptyMyTranslatingPlayerList())
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::ChatRecordingState);
		}
		else if(MyTranslatingPlayerList.Num() == 1 	&& VideoChatController.GetCurrentVideoChatState() == WRVideoChatController::EWRVideoChatState::Connected
				&& ContainsInMyTranslatingPlayerList(VideoChatController.GetVideoChatPlayerAID()))
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::ChatRecordingState);
		}
	}
}

void WRVoiceContentsManager::RemoveTranslatingMePlayer(const AID_t IN InPlayerAID)
{
	if (InPlayerAID == INVALID_AID)
		return;

	if (TranslatingMePlayerList.Contains(InPlayerAID) == false)
		return;

	TranslatingMePlayerList.RemoveNode(InPlayerAID);

	if (IsEmptyTranslatingMePlayerList() && WRInputSystemManager::Get()->IsChatRecording())
	{
		CancelRecording();
	}
}

bool WRVoiceContentsManager::CheckLookingTargetHeadForTranslating()
{
	if (WRCharacterManager::IsValid() == false)
		return false
		;
	AWRCharacter* MyCharacter = Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (MyCharacter == nullptr)
		return false;

	const AWRCharacter* TargetCharacter = MyCharacter->GetTargetCharacter();
	if (TargetCharacter == nullptr)
		return false;

	if (TargetCharacter->GetCharacterName().IsEqual(""))
		return false;

	if (ContainsInMyTranslatingPlayerList(TargetCharacter->GetActorID()))
		return false;

	// 여기서 이젠 레이쏴서 체크해라
	FHitResult Hit(1.0f);

	float RayLength = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingRayLength);

	UCameraComponent* CamComponet = Cast<UCameraComponent>(MyCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CamComponet == nullptr)
		return false;

	FVector HMDForward = CamComponet->GetForwardVector();
	FVector StartLocation = CamComponet->GetComponentLocation();
	FVector EndLocation = StartLocation + (HMDForward * RayLength);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(MyCharacter);

	TArray<AWRCharacter*> DeadCharacters = WRCharacterManager::Get()->GetDeadCharacters();
	for (int i = 0; i < DeadCharacters.Num(); ++i)
		CollisionParams.AddIgnoredActor(DeadCharacters[i]);

	if (MyCharacter->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_HeadTrace, CollisionParams) == true)
	{
		AActor* HittingActor = Hit.Actor.Get();
		if (HittingActor != TargetCharacter)
			return false;
	}
	else
	{
		return false;
	}

	return true;
}

void WRVoiceContentsManager::RequestRegistSpeaker(const AID_t IN InPlayerAID)
{
	if (ContainsInMyTranslatingPlayerList(InPlayerAID))
		return;

	if (WaitTranslatingPacketList.Contains(InPlayerAID))
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	if (VideoChatController.GetCurrentVideoChatState() == WRVideoChatController::EWRVideoChatState::Connected
		&& InPlayerAID == VideoChatController.GetVideoChatPlayerAID())
	{

	}
	else
	{
		AWRCharacterPlayer* FoundCharacter = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(InPlayerAID));
		if (FoundCharacter == nullptr)
			return;

		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter == nullptr)
			return;

		if (FoundCharacter->GetLanguageType() == MyCharacter->GetLanguageType())
		{
			/*FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(43);
			if (CustomTextTable)
			{
				WRNoticeManager::Get()->AddNotice(WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey));
			}*/
			return;
		}

		if (FoundCharacter->GetChatType() == EWRChatType::Secret)
		{
			if (MyCharacter->GetChatType() == EWRChatType::Secret && FoundCharacter->GetDialogGroupID() == MyCharacter->GetDialogGroupID())
			{
				// 우리둘이 비밀대화중

			}
			else
			{
				// 대상자는 다른 사람과 비밀대화중
				return;
			}
			
			/*FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(41);
			if (CustomTextTable)
			{
				WRNoticeManager::Get()->AddNotice(WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey));
			}*/
		}
	}
	
	

	IMMessageSender::RequestRegistSpeaker(InPlayerAID);
	WaitTranslatingPacketList.AddTail(InPlayerAID);
}

void WRVoiceContentsManager::RequestDeregistSpeaker(const AID_t IN InPlayerAID)
{
	if (ContainsInMyTranslatingPlayerList(InPlayerAID) == false)
		return;

	if (WaitTranslatingPacketList.Contains(InPlayerAID))
		return;

	IMMessageSender::RequestDeregistSpeaker(InPlayerAID);
	WaitTranslatingPacketList.AddTail(InPlayerAID);
}

void WRVoiceContentsManager::OnAckRegistSpeakerReceived(const bool IN bInSuccess, const AID_t IN InPlayerAID)
{
	WaitTranslatingPacketList.RemoveNode(InPlayerAID);

	if (bInSuccess)
	{
		AddMyTranslatingPlayer(InPlayerAID);
		VideoChatController.OnAckRegistSpeakerReceived(InPlayerAID);
	}		
}

void WRVoiceContentsManager::OnAckDeregistSpeakerReceived(const bool IN bInSuccess, const AID_t IN InPlayerAID)
{
	WaitTranslatingPacketList.RemoveNode(InPlayerAID);

	if (bInSuccess)
	{
		RemoveMyTranslatingPlayer(InPlayerAID);
		VideoChatController.OnAckDeregistSpeakerReceived(InPlayerAID);
	}
}

void WRVoiceContentsManager::OnAckReleaseListenerReceived(const AID_t IN InPlayerAID)
{

}

void WRVoiceContentsManager::OnInformRegistSpeakerReceived(const AID_t IN InPlayerAID)
{
	AddTranslatingMePlayer(InPlayerAID);
}

void WRVoiceContentsManager::OnInformDeregistSpeakerReceived(const AID_t IN InPlayerAID)
{
	RemoveTranslatingMePlayer(InPlayerAID);
}

void WRVoiceContentsManager::OnInformReleaseListenerReceived(const AID_t IN InPlayerAID)
{
	RemoveMyTranslatingPlayer(InPlayerAID);
}

void WRVoiceContentsManager::OnInformVoiceChat(const AID_t IN InSpeakerAID)
{
	/*if (WRCharacterManager::Get()->GetMyAID() == InFocusedAID)
	{
		AddFocusTalkController(InSpeakerAID);
	}*/
}

void WRVoiceContentsManager::OnInformSetFocusedAID(const AID_t IN InSpeakerAID)
{
	AddFocusTalkController(InSpeakerAID);
}

void WRVoiceContentsManager::OnInformVoiceMessage(const AID_t IN InSpeakerAID)
{
	/*if (WRCharacterManager::Get()->GetMyAID() == InFocusedAID)
	{
		AddFocusTalkController(InSpeakerAID);
	}*/
	VideoChatController.OnInformVoiceMessage(InSpeakerAID);
}

void WRVoiceContentsManager::UpdateUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	UpdateDefaultUI(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);
	UpdateMenuUI(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);
	UpdateSelectDialogueUI(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);
	UpdateSocialMenuUI(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);
	UpdateVideoChatUserListUI(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);
	UpdateVideoChatScreen(InVoiceRecordingState, InVoiceContentType, bInChatRecording, bInImpossibleVoiceRecording);
}

void WRVoiceContentsManager::UpdateDefaultUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	if (bInImpossibleVoiceRecording || bInChatRecording)
	{
		UWRUIPanel_DefaultVoiceCommand* Panel_DefaultVoiceCommand = WRUIManager::Get()->FindUI<UWRUIPanel_DefaultVoiceCommand>(EWRUIPanelKind::DefaultVoiceCommand);
		if (Panel_DefaultVoiceCommand == nullptr)
			return;

		UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_DefaultVoiceCommand->GetSlotVoiceCommand();
		if (Slot_VoiceCommand)
		{
			Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Impossible);
		}

		UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_DefaultVoiceCommand->GetSlotVoiceCommandText();
		if (Slot_VoiceCommandText)
		{
			Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Impossible);
		}
		return;
	}

	if (InVoiceContentType != EWRVoiceContentType::Default)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::DefaultVoiceCommand);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				UWRUIPanel_DefaultVoiceCommand* Panel_DefaultVoiceCommand = Cast<UWRUIPanel_DefaultVoiceCommand>(InCombinedPanelData.UIPanel);
				if (Panel_DefaultVoiceCommand == nullptr)
					return;

				UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_DefaultVoiceCommand->GetSlotVoiceCommand();
				if (Slot_VoiceCommand)
				{
					Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Recording);
					Slot_VoiceCommand->SetRecordingTime(WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_DefaultTime));
				}

				UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_DefaultVoiceCommand->GetSlotVoiceCommandText();
				if (Slot_VoiceCommandText)
				{
					Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Recording);
				}

				Panel_DefaultVoiceCommand->UpdateRecommendedVoiceCommand();
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DefaultVoiceCommand, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				UWRUIPanel_DefaultVoiceCommand* Panel_DefaultVoiceCommand = Cast<UWRUIPanel_DefaultVoiceCommand>(InCombinedPanelData.UIPanel);
				if (Panel_DefaultVoiceCommand == nullptr)
					return;

				UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_DefaultVoiceCommand->GetSlotVoiceCommand();
				if (Slot_VoiceCommand)
				{
					Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::VoiceSending);
				}

				UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_DefaultVoiceCommand->GetSlotVoiceCommandText();
				if (Slot_VoiceCommandText)
				{
					Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::VoiceSending);
				}
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DefaultVoiceCommand, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
	else
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::DefaultVoiceCommand);
	}
}

void WRVoiceContentsManager::UpdateSelectDialogueUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	UWRUIPanel_SelectDialogue* Panel_SelectDialogue = WRUIManager::Get()->FindUI<UWRUIPanel_SelectDialogue>(EWRUIPanelKind::SelectDialogue);
	if (Panel_SelectDialogue == nullptr)
		return;

	UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_SelectDialogue->GetSlotVoiceCommand();
	if (Slot_VoiceCommand == nullptr)
		return;

	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_SelectDialogue->GetSlotVoiceCommandText();
	if (Slot_VoiceCommandText == nullptr)
		return;

	if (bInImpossibleVoiceRecording || bInChatRecording)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Impossible);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Impossible);
		return;
	}

	if (InVoiceContentType != EWRVoiceContentType::SelectDialogue)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Recording);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Recording);
		Slot_VoiceCommand->SetRecordingTime(WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ContentTime));
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::VoiceSending);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::VoiceSending);
	}
	else
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
}

void WRVoiceContentsManager::UpdateMenuUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	UWRUIPanel_Menu* Panel_Menu = WRUIManager::Get()->FindUI<UWRUIPanel_Menu>(EWRUIPanelKind::Menu);
	if (Panel_Menu == nullptr)
		return;

	UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_Menu->GetSlotVoiceCommand();
	if (Slot_VoiceCommand == nullptr)
		return;

	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_Menu->GetSlotVoiceCommandText();
	if (Slot_VoiceCommandText == nullptr)
		return;

	if (bInImpossibleVoiceRecording || bInChatRecording)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Impossible);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Impossible);
		return;
	}

	if (InVoiceContentType != EWRVoiceContentType::Menu)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Recording);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Recording);
		Slot_VoiceCommand->SetRecordingTime(WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ContentTime));
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::VoiceSending);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::VoiceSending);
	}
	else
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
}

void WRVoiceContentsManager::UpdateSocialMenuUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	UWRUIPanel_SocialMenu* Panel_SocialMenu = WRUIManager::Get()->FindUI<UWRUIPanel_SocialMenu>(EWRUIPanelKind::SocialMenu);
	if (Panel_SocialMenu == nullptr)
		return;

	UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_SocialMenu->GetSlotVoiceCommand();
	if (Slot_VoiceCommand == nullptr)
		return;

	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_SocialMenu->GetSlotVoiceCommandText();
	if (Slot_VoiceCommandText == nullptr)
		return;

	if (bInImpossibleVoiceRecording || bInChatRecording)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Impossible);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Impossible);
		return;
	}

	if (InVoiceContentType != EWRVoiceContentType::SocialMenu)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Recording);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Recording);
		Slot_VoiceCommand->SetRecordingTime(WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ContentTime));
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::VoiceSending);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::VoiceSending);
	}
	else
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
}

void WRVoiceContentsManager::UpdateVideoChatUserListUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	UWRUIPanel_VideoChatUserList* Panel_VideoChatUserList = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatUserList>(EWRUIPanelKind::VideoChatUserList);
	if (Panel_VideoChatUserList == nullptr)
		return;

	UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_VideoChatUserList->GetSlotVoiceCommand();
	if (Slot_VoiceCommand == nullptr)
		return;

	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_VideoChatUserList->GetSlotVoiceCommandText();
	if (Slot_VoiceCommandText == nullptr)
		return;

	if (bInImpossibleVoiceRecording || bInChatRecording)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Impossible);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Impossible);
		return;
	}

	if (InVoiceContentType != EWRVoiceContentType::VideoChatUserList)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Recording);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Recording);
		Slot_VoiceCommand->SetRecordingTime(WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ContentTime));
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::VoiceSending);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::VoiceSending);
	}
	else
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
}

void WRVoiceContentsManager::UpdateVideoChatScreen(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
	if (Panel_VideoChatScreen == nullptr)
		return;

	UWRUISlot_VoiceCommand* Slot_VoiceCommand = Panel_VideoChatScreen->GetSlotVoiceCommand();
	if (Slot_VoiceCommand == nullptr)
		return;

	/*UWRUISlot_VoiceCommandText* Slot_VoiceCommandText = Panel_VideoChatScreen->GetSlotVoiceCommandText();
	if (Slot_VoiceCommandText == nullptr)
		return;*/

	if (bInImpossibleVoiceRecording || bInChatRecording)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Impossible);
		//Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Impossible);
		return;
	}

	if (InVoiceContentType != EWRVoiceContentType::VideoChatScreen)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		//Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Recording);
		//Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Recording);
		Slot_VoiceCommand->SetRecordingTime(WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ContentTime));
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::VoiceSending);
		//Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::VoiceSending);
	}
	else
	{
		Slot_VoiceCommand->SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
		//Slot_VoiceCommandText->SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
}

void WRVoiceContentsManager::UpdateChatRecording(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording)
{
	if (WRConsoleManager::Get()->GetBool(EWRConsole::Debug_ShowChatRecordUI_Speaker) == false)
		return;

	if (InVoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(InCombinedPanelData.UIPanel);
				if (Panel_Main)
				{
					Panel_Main->StartChatRecording(WRConsoleManager::Get()->GetFloat(EWRConsole::ChatRecording_Time));
				}
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
	else if (InVoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING || InVoiceRecordingState == EWRVoiceRecordingStateType::NONE)
	{
		UWRUIPanel_Main* Panel_Main = WRUIManager::Get()->FindUI<UWRUIPanel_Main>(EWRUIPanelKind::Main);
		if (Panel_Main)
			Panel_Main->StopChatRecording();
	}
}

bool WRVoiceContentsManager::CheckAvailableVoiceRecording()
{
	if (WRInputSystemManager::Get()->IsImpossibleVoiceRecording())
		return false;

	if (WRInputSystemManager::Get()->IsVoiceRecording())
		return false;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		if (WREnvInteractionManager::Get()->IsEnvVehiclePassenger(MyCharacter->GetActorID(), EWREnvType::Tardis))
			return false;
	}

	return true;
}

void WRVoiceContentsManager::Tick_ReservedRemoveFocusTalk()
{
	if (ReservedRemoveFocusTalkList.Num() == 0)
		return;

	TDoubleLinkedList<AID_t>::TDoubleLinkedListNode* Node = ReservedRemoveFocusTalkList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		AID_t PlayerAID = Node->GetValue();

		if (FocusTalkControllerMap.Contains(PlayerAID) == false)
			continue;

		FocusTalkControllerMap[PlayerAID].RemoveTargetLocationGuide();
		FocusTalkControllerMap.Remove(PlayerAID);
	}
	ReservedRemoveFocusTalkList.Empty();

	UpdateUIForHMDRotationInFocusTalk();
}

void WRVoiceContentsManager::AddFocusTalkController(const AID_t IN InSpeakerAID)
{
	if (InSpeakerAID == INVALID_AID)
		return;

	if (FocusTalkControllerMap.Contains(InSpeakerAID))
	{
		// [SeungYeop 20/08/24] : 기획변경으로 주석 처리(Feat : 영주님)
		//FocusTalkControllerMap[InSpeakerAID].ResetCoolTime();		
		return;
	}

	AWRCharacter* SpeakerCharacter = WRCharacterManager::Get()->FindbyAID(InSpeakerAID);
	if (SpeakerCharacter == nullptr)
		return;

	WRTargetLocationGuideHandle Handle = WRTargetLocationGuideManager::Get()->AddTargetLocationInfo(WRMapManager::Get()->GetCurrentMapTID(), SpeakerCharacter, EWRUISlotKind::FocusTalk, FSimpleDelegate::CreateRaw(this, &WRVoiceContentsManager::OnAutomaticallyRemoveTargetLocationGuide, InSpeakerAID), true, 0.f, FVector(0.f, 0.f, 50.f));
	if (Handle == INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return;

	FocusTalkControllerMap.Add(InSpeakerAID, WRFocusTalkController(Handle, InSpeakerAID));
	UpdateUIForHMDRotationInFocusTalk();
}

void WRVoiceContentsManager::OnAutomaticallyRemoveTargetLocationGuide(const AID_t IN InSpeakerAID)
{
	if (FocusTalkControllerMap.Contains(InSpeakerAID) == false)
		return;

	FocusTalkControllerMap[InSpeakerAID].ForcedResetTargetLocationGuideHandle();
	UpdateUIForHMDRotationInFocusTalk();

	// [SeungYeop 20/08/24] : 기획 내용 변경으로 TLG가 삭제될때 무조건 삭제하게 변경
	ReserveToRemoveFocusTalkController(InSpeakerAID);
}

void WRVoiceContentsManager::UpdateUIForHMDRotationInFocusTalk()
{
	UWRUIPanel_TargetLocationGuide* Panel_TargetLocationGuide = WRUIManager::Get()->FindUI<UWRUIPanel_TargetLocationGuide>(EWRUIPanelKind::TargetLocationGuide);
	if (Panel_TargetLocationGuide == nullptr)
		return;

	Panel_TargetLocationGuide->UpdateUIForHMDRotationInFocusTalk(GetTargetAIDForHMDRotationInFocusTalk());
}

// 현재는 종류가 한개 뿐이라 임시용으로 대응 추후 랜덤으로 공간이 늘어나게되면 패킷에서 제공하는 TID로 대응
#define SecretDialogAreaTID 90009

void WRVoiceContentsManager::OnCmdAgreeToSecretDialog(const AID_t IN InPartnerAID, const CHAT_GROUP_ID_t IN InGroupID)
{
	if (this->IsReservedOpenSecretDialogUI == true)
		return;

	this->GetSecretDialogController().SetCmdData(InGroupID, InPartnerAID);

	FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(44);
	if (CustomTextTable != nullptr)
	{
		FTransform Transform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, 0), FVector::OneVector);
		FWRPopupInfo PopupInfo;
		PopupInfo.eButtonFlag = EPopupButtonFlag::OK_CANCEL;
		FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
		PopupInfo.Contents = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString();
		PopupInfo.Delegate_First.BindRaw(this, &WRVoiceContentsManager::OnPopUpOKForSecretDialog);
		PopupInfo.Delegate_Second.BindRaw(this, &WRVoiceContentsManager::OnPopUpCancelForSecretDialog);
		WRPopupManager::Get()->OpenPopup(PopupInfo, Transform);
	}

	/*UWRUIPanel_RepeatSecretDialog* InPanel = WRUIManager::Get()->FindUI<UWRUIPanel_RepeatSecretDialog>(EWRUIPanelKind::RepeatSecretDialog);
	if (InPanel != nullptr)
		return;

	AWRCharacter* InCharacter = WRCharacterManager::Get()->FindbyAID(InPartnerAID);
	if (InCharacter == nullptr)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InPartnerAID == INVALID_AID)
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::RepeatSecretDialog);
			return;
		}
		FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
		if (pTable == nullptr)
			return;

		UWRUIPanel_RepeatSecretDialog* InPanel = Cast<UWRUIPanel_RepeatSecretDialog>(InCombinedPanelData.UIPanel);
		if (InPanel != nullptr)
		{
			InPanel->SetRptInfo(InPartnerAID, InGroupID, pTable->ClosePanelTime);
			this->IsReservedOpenSecretDialogUI = false;
		}
	});
	IsReservedOpenSecretDialogUI = true;

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::RepeatSecretDialog, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);*/
}

void WRVoiceContentsManager::OnAgreeToSecretDialog(const TArray<AID_t>& IN InAIDs, const CHAT_GROUP_ID_t IN InGroupID, bool InAgree)
{
	if (InAgree == false)
		return;

	if (GroupChatList.Contains(InGroupID) == true)
		return;

	AID_t MyAID = WRCharacterManager::Get()->GetMyAID();

	/*if(InAIDs.Contains(MyAID) == true)
		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, 3154 + InAgree);*/

	if (InAIDs.Contains(MyAID) == true)
	{
		FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
		if (pTable != nullptr)
			WRSFXManager::Get()->SetTempVolume(pTable->MinVolume);

		WREnvInteractionManager::Get()->SpawnSecretDialogDome(InGroupID, InAIDs);
	}
	else
	{
		WRCharacterManager::Get()->AddVisibilityBitByAIDList(InAIDs, EWRVisibility::SpecialConversation);
		WRCharacterManager::Get()->SetVisibleByList(InAIDs, false);
		// 대화 주체 캐릭터들 렌더링 오프
	}

	for (AID_t InAID : InAIDs)
	{
		AWRCharacter* InCharacter = WRCharacterManager::Get()->FindbyAID(InAID);
		if (InCharacter == nullptr)
			continue;

		InCharacter->SetDialogGroupID(InGroupID);
		InCharacter->SetChatType(EWRChatType::Secret);
	}

	GroupChatList.Add(InGroupID, InAIDs);
}

void WRVoiceContentsManager::OnEndSecretDialog()
{
	AWRCharacter* InMine = WRCharacterManager::Get()->GetMyCharacter();
	if (InMine == nullptr)
		return;

	this->OnEndSecretDialog(InMine->GetDialogGroupID());
}

void WRVoiceContentsManager::OnEndSecretDialog(const CHAT_GROUP_ID_t IN InGroupID)
{
	AWRCharacter* InMine = WRCharacterManager::Get()->GetMyCharacter();
	if (InMine == nullptr)
		return;

	if (GroupChatList.Contains(InGroupID) == false)
		return;

	TArray<AID_t> InAIDs = GroupChatList.FindRef(InGroupID);

	if (InMine->GetDialogGroupID() == InGroupID)
	{
		//WREnvInteractionManager::Get()->UnSpawnSecretDialogDome();
		FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
		if (pTable != nullptr)
			WRSFXManager::Get()->SetTempVolume(pTable->MaxVolume);

		WREnvInteractionManager::Get()->ReserveDestroyDome();
	}
	else
	{
		WRCharacterManager::Get()->RemoveVisibilityBitByAIDList(InAIDs, EWRVisibility::SpecialConversation);
		WRCharacterManager::Get()->SetVisibleByList(InAIDs, true);
	}

	for (AID_t InAID : InAIDs)
	{
		AWRCharacter* InCharacter = WRCharacterManager::Get()->FindbyAID(InAID);
		if (InCharacter == nullptr)
			continue;

		InCharacter->SetDialogGroupID(INVALID_CHAT_GROUP_ID);
		InCharacter->SetChatType(EWRChatType::Normal);
	}

	GroupChatList.Remove(InGroupID);
}

void WRVoiceContentsManager::OnPopUpOKForSecretDialog()
{
	WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, 3154);
	IMMessageSender::ReportAgreeToSecretDialog(GetSecretDialogController().GetCmdPartnerID(), GetSecretDialogController().GetCmdGroupID(), true);
}

void WRVoiceContentsManager::OnPopUpCancelForSecretDialog()
{
	//WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, 3155);
	IMMessageSender::ReportAgreeToSecretDialog(GetSecretDialogController().GetCmdPartnerID(), GetSecretDialogController().GetCmdGroupID(), false);
}

TArray<AID_t> WRVoiceContentsManager::GetActorListInGroupChatList(const CHAT_GROUP_ID_t InGroupID)
{
	TArray<AID_t> tempList;
	tempList.Empty();
	if (GroupChatList.Contains(InGroupID) == true)
		tempList = GroupChatList.FindRef(InGroupID);

	return tempList;
}

bool WRVoiceContentsManager::OnInputKeyForAgreeSecretDialog(const EWRInput IN InInput)
{
	//	UWRUIPanel_RepeatSecretDialog* InPanel = WRUIManager::Get()->FindUI<UWRUIPanel_RepeatSecretDialog>(WRCharacterManager::Get()->GetMyCharacter(), EWRUIPanelKind::RepeatSecretDialog);
	//	if (InPanel == nullptr)
	//		return false;
	//	if (InPanel->GetPartnerID() == INVALID_AID)
	//		return false;
	//
	//	switch (InInput)
	//	{
	//#if !PLATFORM_PS4
	//	case EWRInput::Circle:
	//#endif // !PLATFORM_PS4 // 터치패드 없는 경우 대비
	//	case EWRInput::TouchPadDragLeftX:
	//	{
	//		IMMessageSender::ReportAgreeToSecretDialog(InPanel->GetPartnerID(), InPanel->GetGroupID(), false);
	//		WRUIManager::Get()->CloseUI(WRCharacterManager::Get()->GetMyCharacter(), EWRUIPanelKind::RepeatSecretDialog);
	//		return true;
	//	}
	//	break;
	//#if !PLATFORM_PS4
	//	case EWRInput::Cross:
	//#endif // !PLATFORM_PS4 // 터치패드 없는 경우 대비
	//	case EWRInput::TouchPadDragRightX:
	//	{
	//		IMMessageSender::ReportAgreeToSecretDialog(InPanel->GetPartnerID(), InPanel->GetGroupID(), true);
	//		WRUIManager::Get()->CloseUI(WRCharacterManager::Get()->GetMyCharacter(), EWRUIPanelKind::RepeatSecretDialog);
	//		return true;
	//	}
	//	break;
	//	}

	return false;
}

void WRFocusTalkController::Tick(const float IN InDeltaTime)
{
	if (TargetLocationGuideHanlde != INVALID_TARGET_LOCATION_GUIDE_HANDLE)
	{
		FlowCheckTime += InDeltaTime;
		FlowTargetLocationGuideHoldingTime += InDeltaTime;
		if (FlowCheckTime > 0.5f)
		{
			FlowCheckTime = 0.f;
			CheckDist();
		}
		else if (FlowTargetLocationGuideHoldingTime > WRConsoleManager::Get()->GetFloat(EWRConsole::VoiceChat_TLG_HoldingTime))
		{
			WRTargetLocationGuideManager::Get()->RemoveTargetLocationInfo(TargetLocationGuideHanlde);
			TargetLocationGuideHanlde = INVALID_TARGET_LOCATION_GUIDE_HANDLE;
		}
	}

	FlowCoolTime += InDeltaTime;
	if (FlowCoolTime > WRConsoleManager::Get()->GetFloat(EWRConsole::UI_FocusTalkCoolTime))
	{
		WRVoiceContentsManager::Get()->ReserveToRemoveFocusTalkController(PlayerAID);
	}
}

void WRFocusTalkController::RemoveTargetLocationGuide()
{
	if (TargetLocationGuideHanlde != INVALID_TARGET_LOCATION_GUIDE_HANDLE)
	{
		WRTargetLocationGuideManager::Get()->RemoveTargetLocationInfo(TargetLocationGuideHanlde);
		TargetLocationGuideHanlde = INVALID_TARGET_LOCATION_GUIDE_HANDLE;
	}
}

bool WRFocusTalkController::IsVisibleFocusTalkUI() const
{
	if (TargetLocationGuideHanlde != INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return true;

	return false;
}

void WRFocusTalkController::CheckDist()
{
	if (TargetLocationGuideHanlde == INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return;

	AActor* Player = Cast<AActor>(WRCharacterManager::Get()->FindbyAID(PlayerAID));
	if (Player == nullptr)
		return;

	AWRCharacter* MyCharacter = Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (MyCharacter == nullptr)
		return;

	FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(MyCharacter->GetCharacterTableID());
	if (PlayerTable == nullptr)
		return;

	float Dist = WRActorUtility::GetDistance(Player, MyCharacter);
	if (Dist < PlayerTable->VoiceChatRange)
		return;

	WRTargetLocationGuideManager::Get()->RemoveTargetLocationInfo(TargetLocationGuideHanlde);
	TargetLocationGuideHanlde = INVALID_TARGET_LOCATION_GUIDE_HANDLE;
}
