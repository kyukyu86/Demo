// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRDialogueManager.h"
#include "Table/WRDialogueTable.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCustomTextTable.h"

#include "WRUIManager.h"
#include "WRInputSystemManager.h"
#include "WRTriggerManager.h"
#include "WRTutorialManager.h"
#include "WRQuestManager.h"
#include "WRCharacterManager.h"
#include "WRNoticeManager.h"

#include "UI/Panel/WRUIPanel_Dialogue.h"
#include "Utility/WRUIUtility.h"
#include "Network/IMMessageSender.h"
#include "Component/Character/WRActorComponentCommonNpcTalk.h"
#include "WRConsoleManager.h"


WRDialogueManager::WRDialogueManager()
{
}

WRDialogueManager::~WRDialogueManager()
{
}
	
void WRDialogueManager::OnInitialize()
{

}

void WRDialogueManager::OnShutdown()
{
	
}

void WRDialogueManager::OnLoadLevelStart()
{
	Clear();
}

void WRDialogueManager::OnLoadLevelComplete()
{

}

void WRDialogueManager::OnRenderStart()
{
	
}

bool WRDialogueManager::OnTick(float InDeltaTime)
{
	LoopDialogueRegister.Tick(InDeltaTime);
	return true;
}

void WRDialogueManager::OpenDialogue(const EWRDialogueType IN InDialogueType, const WRTableID IN InDialogueTID, const FSimpleDelegate& IN InDelegate /*= nullptr*/)
{
	if (InDialogueType == EWRDialogueType::None || InDialogueTID == INVALID_TABLE_TID)
		return;

	if (InDialogueType == EWRDialogueType::NpcTalk)
	{
		Clear();
		OpenDialogue_Internal(InDialogueType, InDialogueTID, InDelegate);
		return;
	}


	TDoubleLinkedList<FWRReserveDialogueData>::TDoubleLinkedListNode* LastMember = ReserveDialogueList.GetTail();
	if (LastMember != nullptr)
	{
		if (IsBTypeLowerPriorityThanAType(LastMember->GetValue().DialogueType, InDialogueType))
		{
			return;
		}

		ReserveDialogueList.AddTail(FWRReserveDialogueData(InDialogueType, InDialogueTID, InDelegate));
	}
	else if (IsOpenDialogue())
	{
		if (IsBTypeLowerPriorityThanAType(OpenedDialogue.Key, InDialogueType))
		{
			return;
		}

		ReserveDialogueList.AddTail(FWRReserveDialogueData(InDialogueType, InDialogueTID, InDelegate));
	}
	else
	{
		// 현재 보여지는게 없으니 바로 보여준다.
		OpenDialogue_Internal(InDialogueType, InDialogueTID, InDelegate);
	}
}

void WRDialogueManager::StartLoopDialogue(const int32 IN InUID, const TArray<WRTableID>& IN InDialogueTIDs, const float IN InLoopDelayTime)
{
	LoopDialogueRegister.StartLoopDialogue(InUID, InDialogueTIDs, InLoopDelayTime);
}

void WRDialogueManager::StopLoopDialogue(const int32 IN InUID)
{
	LoopDialogueRegister.StopLoopDialogue(InUID);
}

bool WRDialogueManager::IsOpenDialogue()
{
	if (OpenedDialogue.Key == EWRDialogueType::None || OpenedDialogue.Value == INVALID_TABLE_TID)
		return false;

	return true;
}

void WRDialogueManager::OnDialogueFinished()
{
	if(OpenedDialogue.Key == EWRDialogueType::MainQuest)
		WRInputSystemManager::Get()->SetImpossibleVoiceRecording(false);

	OpenedDialogue.Key = EWRDialogueType::None;
	OpenedDialogue.Value = INVALID_TABLE_TID;
	OpenedDelegate.ExecuteIfBound();

	if (IsOpenDialogue())
		return;

	TDoubleLinkedList<FWRReserveDialogueData>::TDoubleLinkedListNode* Node = ReserveDialogueList.GetHead();
	if (Node != nullptr)
	{
		FWRReserveDialogueData& NextDialogue = Node->GetValue();
		OpenDialogue_Internal(NextDialogue.DialogueType, NextDialogue.DialogueTID, NextDialogue.Delegate);
		ReserveDialogueList.RemoveNode(Node);

		return;
	}

	TArray<AWRCharacter*> OutCharacter;
	WRCharacterManager::Get()->GetCharactersByType(EWRCharacterType::NPC, OutCharacter);
	for (auto& Element : OutCharacter)
	{
		AWRCharacter* Character = Element;
		if (Character == nullptr)
			continue;

		UWRActorComponentCommonNpcTalk* CommonNpcTalkComponent = Character->GetComponent<UWRActorComponentCommonNpcTalk>();
		if (CommonNpcTalkComponent == nullptr)
			continue;

		CommonNpcTalkComponent->OnDialogueFinished();
	}

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::Dialogue);
}

void WRDialogueManager::Clear()
{
	UWRUIPanel_Dialogue* Panel_Dialogue = WRUIManager::Get()->FindUI<UWRUIPanel_Dialogue>(EWRUIPanelKind::Dialogue);
	if (Panel_Dialogue)
	{
		Panel_Dialogue->StopDialogue();
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::Dialogue);
	}


	ReserveDialogueList.Empty();
	OpenedDialogue.Key = EWRDialogueType::None;
	OpenedDialogue.Value = INVALID_TABLE_TID;
	
	if(WRInputSystemManager::Get()->IsImpossibleVoiceRecording())
		WRInputSystemManager::Get()->SetImpossibleVoiceRecording(false);
}

void WRDialogueManager::OpenDialogue_Internal(const EWRDialogueType IN InDialogueType, const WRTableID IN InDialogueTID, const FSimpleDelegate& IN InDelegate)
{
	const FWRDialogueTable* DialogueTable = GetDialogueTable(InDialogueType, InDialogueTID);
	if (DialogueTable == nullptr)
		return;

	if (InDialogueType == EWRDialogueType::MainQuest)
	{
		WRInputSystemManager::Get()->SetImpossibleVoiceRecording(true);
	}

	OpenedDialogue.Key = InDialogueType;
	OpenedDialogue.Value = InDialogueTID;
	OpenedDelegate = InDelegate;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_Dialogue* Panel_Dialogue = Cast<UWRUIPanel_Dialogue>(InCombinedPanelData.UIPanel);
			if (Panel_Dialogue)
				Panel_Dialogue->OpenDialgoue(DialogueTable);

			InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
		}
	});


	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Dialogue, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::Dialogue);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Dialogue, FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);

	// 
	TArray<AWRCharacter*> OutCharacter;
	WRCharacterManager::Get()->GetCharactersByType(EWRCharacterType::NPC, OutCharacter);
	for (auto& Element : OutCharacter)
	{
		AWRCharacter* Character = Element;
		if (Character == nullptr)
			continue;

		UWRActorComponentCommonNpcTalk* CommonNpcTalkComponent = Character->GetComponent<UWRActorComponentCommonNpcTalk>();
		if (CommonNpcTalkComponent == nullptr)
			continue;

		CommonNpcTalkComponent->OnDialogueOpend();
	}
}

const FWRDialogueTable* WRDialogueManager::GetDialogueTable(const EWRDialogueType IN InDialogueType, const WRTableID IN InDialogueTID)
{
	switch (InDialogueType)
	{
	case EWRDialogueType::MainQuest:		return WRTableManager::Get()->FindRow<FWRMainQuestDialogueTable>(InDialogueTID);
	case EWRDialogueType::NpcEventTalk:		return WRTableManager::Get()->FindRow<FWRNpcEventTalkDialogueTable>(InDialogueTID);
	case EWRDialogueType::FeedbackTalk:		return WRTableManager::Get()->FindRow<FWRFeedbackTalkDialogueTable>(InDialogueTID);
	case EWRDialogueType::TriggerNpcTalk:	return WRTableManager::Get()->FindRow<FWRTriggerNpcTalkDialogueTable>(InDialogueTID);
	case EWRDialogueType::NpcIdleSpeech:	return WRTableManager::Get()->FindRow<FWRNpcIdleSpeechDialogueTable>(InDialogueTID);
	case EWRDialogueType::NpcTalk:			return WRTableManager::Get()->FindRow<FWRNpcTalkDialogueTable>(InDialogueTID);
	default: return nullptr;
	}

	return nullptr;
}

int8 WRDialogueManager::GetDialoguePriority(const EWRDialogueType IN InDialogueType)
{
	switch (InDialogueType)
	{
	case EWRDialogueType::NpcTalk:			return 1;
	case EWRDialogueType::MainQuest:		return 2;
	case EWRDialogueType::FeedbackTalk:		return 3;
	case EWRDialogueType::NpcEventTalk:		return 4;
	case EWRDialogueType::TriggerNpcTalk:	return 5;
	case EWRDialogueType::NpcIdleSpeech:	return 6;
	default:								return 127;
	}
}

bool WRDialogueManager::IsBTypeLowerPriorityThanAType(const EWRDialogueType IN InAType, const EWRDialogueType IN InBType)
{
	if (GetDialoguePriority(InAType) < GetDialoguePriority(InBType))
	{
		return true;
	}

	return false;
}

void WRDialogueManager::ProgressDialogueAction(const FWRDialogueAction& IN InDialogueAction, const WRTableID IN InDialogueTID)
{
	if (InDialogueAction.Type == EWRDialogueActionType::None)
	{
		return;
	}
	else if (InDialogueAction.Type == EWRDialogueActionType::ExcuteTrigger)
	{
		WRTriggerManager::Get()->RunActiveTrigger(InDialogueAction.Value01);
	}
	else if (InDialogueAction.Type == EWRDialogueActionType::Tutorial)
	{
		WRTutorialManager::Get()->OnNewTutorial(InDialogueAction.Value01);
	}
	else if(InDialogueAction.Type == EWRDialogueActionType::AcceptQuest)
	{
		WRQuestManager::Get()->RequestAcceptQuest(InDialogueAction.Value01);
	}
	else if (InDialogueAction.Type == EWRDialogueActionType::PlayAni)
	{
		AWRCharacter* Character = WRCharacterManager::Get()->FindByTID(InDialogueAction.Value01);

		if (Character == nullptr)
			return;

		WRFSMInfo FSMInfo;
		FSMInfo.FSMType = EWRFSM::Talk;
		FSMInfo.JumpToSectionName = FString::FromInt(InDialogueAction.Value02);
		Character->DetermineFSMSignal(FSMInfo);
	}
	else if (InDialogueAction.Type == EWRDialogueActionType::ActiveLifeEvent)
	{
		// 패킷
		IMMessageSender::RequestLifeEventReward(InDialogueTID);
	}
}

WRLoopDialogueElement::WRLoopDialogueElement(const TArray<WRTableID>& IN InDialogueTIDs, const float IN InLoopDelayTime)
	: FlowTime(0)
	, LoopDelayTime(InLoopDelayTime)
{
	RandomDialogueTIDs = InDialogueTIDs;
}

void WRLoopDialogueElement::Tick(const float IN InDeltaTime)
{
	FlowTime -= InDeltaTime;
	if (FlowTime < 0)
	{
		RegisterDialogue();
	}
}

void WRLoopDialogueElement::RegisterDialogue()
{
	if (RandomDialogueTIDs.Num() == 0)
		return;

	int32 RandIndex = FMath::Rand() % RandomDialogueTIDs.Num();
	WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::TriggerNpcTalk, RandomDialogueTIDs[RandIndex]);

	FlowTime = LoopDelayTime;
}

void WRLoopDialogueRegister::Tick(const float IN InDeltaTime)
{
	for (auto& Iter : LoopDialogueElements)
	{
		WRLoopDialogueElement& LoopDialogueElement = Iter.Value;
		LoopDialogueElement.Tick(InDeltaTime);
	}
}

void WRLoopDialogueRegister::StartLoopDialogue(const int32 IN InUID, const TArray<WRTableID>& IN InDialogueTIDs, const float IN InLoopDelayTime)
{
	if (InUID <= 0)
		return;

	if (InLoopDelayTime == 0)
		return;

	if (InDialogueTIDs.Num() == 0)
		return;

	if (LoopDialogueElements.Contains(InUID))
		return;

	LoopDialogueElements.Add(InUID, WRLoopDialogueElement(InDialogueTIDs, InLoopDelayTime));
}

void WRLoopDialogueRegister::StopLoopDialogue(const int32 IN InUID)
{
	if (LoopDialogueElements.Contains(InUID) == false)
		return;

	LoopDialogueElements.Remove(InUID);
}
