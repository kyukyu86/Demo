// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRQuestManager.h"
#include "WRUIManager.h"
#include "WRTriggerManager.h"
#include "WRInventoryManager.h"
#include "WRNoticeManager.h"
#include "WRDialogueManager.h"
#include "WRTutorialManager.h"
#include "WRMapManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRQuestTable.h"
#include "Table/WRItemTable.h"

#include "UI/Panel/WRUIPanel_Main.h"
#include "UI/Panel/WRUIPanel_QuestBoard.h"

#include "Network/Share/Define/Define_BluePrint.h"
#include "Network/IMMessageSender.h"
#include "GameMode/WRGameModeBase.h"
#include "Core/WRGameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "WRCharacterManager.h"
#include "WRCompassManager.h"
#include "Component/Character/WRActorComponentAcceptQuest.h"
#include "Table/WRCustomTextTable.h"
#include "Utility/WRUIUtility.h"

#include "Localization/WRLocalizationProcessor.h"


//========================================================================================================================================================================
WRQuestManager::WRQuestManager()
{
}

WRQuestManager::~WRQuestManager()
{
}
	
void WRQuestManager::OnInitialize()
{
	InitializeQuestData();
}

void WRQuestManager::OnShutdown()
{
	ProgressQuestTaskController.OnShutdown();
}

void WRQuestManager::OnLoadLevelStart()
{
	ProgressQuestTaskController.OnLoadLevelStart();
}

void WRQuestManager::OnLoadLevelComplete()
{
		
}

void WRQuestManager::OnRenderStart()
{
	
}

bool WRQuestManager::OnTick(float InDeltaTime)
{
	ProgressQuestTaskController.Tick(InDeltaTime);

	//// 서버 작업후 삭제해야한다
	//TDoubleLinkedList<FWRQuestPacketData>::TDoubleLinkedListNode* Node = PacketDatas.GetHead();
	//for (; Node != nullptr; Node = Node->GetNextNode())
	//{
	//	FWRQuestPacketData& QuestPacketData = Node->GetValue();
	//	
	//	switch (QuestPacketData.QuestPacketType)
	//	{
	//	case EWRQuestPacketType::OnResultReadyQuestReceived: OnResultReadyQuestReceived(true, QuestPacketData.QuestTID);
	//		break;
	//	case EWRQuestPacketType::OnResultAcceptQuestReceived: OnAckAcceptQuestReceived(true, QuestPacketData.QuestTID);
	//		break;
	//	case EWRQuestPacketType::OnResultCompletableQuestReceived: OnAckCompletableQuestReceived(true, QuestPacketData.QuestTID);
	//		break;
	//	case EWRQuestPacketType::OnResultCompletedQuestReceived: OnAckCompletedQuestReceived(true, QuestPacketData.QuestTID);
	//		break;
	//	case EWRQuestPacketType::OnResultCompletedQuestTaskReceived: OnAckCompletedQuestTaskReceived(true, QuestPacketData.QuestTID, QuestPacketData.TaskTID);
	//		break;	
	//	default:
	//		break;
	//	}
	//}
	//PacketDatas.Empty();

	return true;
}

void WRQuestManager::EndLoadLevelProcess()
{
	// 임시 조건문
	if (WRMapManager::Get()->GetPreviousMapTD() == 10001 && WRMapManager::Get()->GetCurrentMapTID() == 3)
	{
		WRQuestManager::Get()->StartGame();
		return;
	}
	
	ProgressQuestTaskController.OnLoadLevelProcessEnded();
}

bool WRQuestManager::IsCompletedQuest(const WRTableID IN InQuestTID)
{
	return CompletedQuestList.Contains(InQuestTID);
}

bool WRQuestManager::IsProgressQuest(const WRTableID IN InQuestTID)
{
	return AcceptQuestList.Contains(InQuestTID);
}

bool WRQuestManager::IsWaitQuestPacket(const WRTableID IN InQuestTID)
{
	if (WaitQuestPacketList.Contains(InQuestTID))
		return true;

	return false;
}

bool WRQuestManager::IsWaitTaskPacket(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID)
{
	if (WaitTaskPacketList.Contains(TPair<WRTableID, WRTableID>(InQuestTID, InTaskTID)))
		return true;

	return false;
}

bool WRQuestManager::HasReadyQuest(const WRTableID IN InNPCTID, WRTableID IN InMapTID /*= INVALID_TABLE_TID*/)
{
	WRTableID QuestTID = GetAccpetableQuestTIDByNpcTalk(InNPCTID, InMapTID);
	if (QuestTID == INVALID_TABLE_TID)
		return false;

	return true;
}

WRTableID WRQuestManager::GetAccpetableQuestTIDByNpcTalk(const WRTableID IN InNPCTID, WRTableID IN InMapTID /*= INVALID_TABLE_TID*/)
{
	if (InNPCTID == INVALID_TABLE_TID)
		return INVALID_TABLE_TID;

	if (InMapTID == INVALID_TABLE_TID)
	{
		InMapTID = WRMapManager::Get()->GetCurrentMapTID();
		if (InMapTID == INVALID_TABLE_TID)
			return INVALID_TABLE_TID;
	}

	TDoubleLinkedList<WRTableID>::TDoubleLinkedListNode* Node = ReadyQuestList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		const FWRQuestTable* FoundQuestTable = FindQuestTable(Node->GetValue());
		if (FoundQuestTable == nullptr)
			continue;

		if (FoundQuestTable->AcceptType != EWRQuestAcceptType::NPCTalk)
			continue;

		if (FoundQuestTable->AcceptTypeValue01 != InNPCTID)
			continue;

		if (FoundQuestTable->AcceptTypeValue02 != InMapTID)
			continue;

		return Node->GetValue();
	}

	return INVALID_TABLE_TID;
}

EWRQuestState::en WRQuestManager::GetQuestState(const WRTableID IN InQuestTID)
{
	if (DisableQuestList.Contains(InQuestTID))
	{
		return EWRQuestState::en::Disable;
	}
	else if (ReadyQuestList.Contains(InQuestTID))
	{
		return EWRQuestState::en::Ready;
	}
	else if (AcceptQuestList.Contains(InQuestTID))
	{
		return EWRQuestState::en::Accept;
	}
	else if (CompletableQuestList.Contains(InQuestTID))
	{
		return EWRQuestState::en::Completable;
	}
	else if (CompletedQuestList.Contains(InQuestTID))
	{
		return EWRQuestState::en::Completed;
	}
	else
	{
		return EWRQuestState::en::None;
	}
}

const FWRQuestTable* WRQuestManager::FindQuestTable(const WRTableID IN InQuestTID, EWRQuestType::en IN InQuestType/*= EWRQuestType::en::None*/)
{
	if (InQuestType == EWRQuestType::None)
	{
		InQuestType = FWRQuestTable::GetQuestType(InQuestTID);
	}

	switch (InQuestType)
	{
	case EWRQuestType::en::Main:	return WRTableManager::Get()->FindRow<FWRMainQuestTable>(InQuestTID);
	default: {}
	}


	return nullptr;
}

const FWRQuestTaskTable* WRQuestManager::FindQuestTaskTable(const WRTableID IN InQuestTaskTID, EWRQuestTaskType IN InQuestTaskType /*= EWRQuestTaskType::en::None*/)
{
	if (InQuestTaskType == EWRQuestTaskType::None)
	{
		InQuestTaskType = FWRQuestTaskTable::GetQuestTaskType(InQuestTaskTID);
	}

	switch (InQuestTaskType)
	{
	case EWRQuestTaskType::MovePosition:	return WRTableManager::Get()->FindRow<FWRQuestMoveTaskTable>(InQuestTaskTID);
	case EWRQuestTaskType::ActiveTrigger:	return WRTableManager::Get()->FindRow<FWRQuestTriggerTaskTable>(InQuestTaskTID);
	case EWRQuestTaskType::ClearTutorial:	return WRTableManager::Get()->FindRow<FWRQuestTutorialTaskTable>(InQuestTaskTID);
	case EWRQuestTaskType::ActiveIO:		return WRTableManager::Get()->FindRow<FWRQuestActiveIOTaskTable>(InQuestTaskTID);
	default: {}
	}

	return nullptr;
}

void WRQuestManager::RequestAcceptQuest(const WRTableID IN InQuestTID)
{
	if (IsWaitQuestPacket(InQuestTID))
		return;

	if (AcceptQuestList.Contains(InQuestTID))
	{
		FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(24);
		if (CustomTextTable)
		{
			FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
			WRNoticeManager::Get()->AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString());
		}
		return;
	}
	if (CompletedQuestList.Contains(InQuestTID))
	{
		FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(13);
		if (CustomTextTable)
		{
			FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
			WRNoticeManager::Get()->AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString());
		}
		return;
	}

 	IMMessageSender::RequestAcceptQuest(InQuestTID);
	//PacketDatas.AddTail(FWRQuestPacketData(EWRQuestPacketType::OnResultAcceptQuestReceived, InQuestTID, INVALID_TABLE_TID));
	WaitQuestPacketList.AddTail(InQuestTID);
}

void WRQuestManager::OnAckAcceptQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID)
{
	WaitQuestPacketList.RemoveNode(InQuestTID);
	if (bInSuccess)
		AcceptQuest_Internal(InQuestTID);
}

void WRQuestManager::RequestCompletableQuest(const WRTableID IN InQuestTID)
{
	if (IsWaitQuestPacket(InQuestTID))
		return;

	IMMessageSender::RequestCompletableQuest(InQuestTID);
	//PacketDatas.AddTail(FWRQuestPacketData(EWRQuestPacketType::OnResultCompletableQuestReceived, InQuestTID, INVALID_TABLE_TID));
	WaitQuestPacketList.AddTail(InQuestTID);
}

void WRQuestManager::OnAckCompletableQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID)
{
	WaitQuestPacketList.RemoveNode(InQuestTID);
	if (bInSuccess)
		CompletableQuest_Internal(InQuestTID);
}

void WRQuestManager::RequestCompletedQuest(const WRTableID IN InQuestTID)
{
	if (IsWaitQuestPacket(InQuestTID))
		return;

	IMMessageSender::RequestCompleteQuest(InQuestTID);
	//PacketDatas.AddTail(FWRQuestPacketData(EWRQuestPacketType::OnResultCompletedQuestReceived, InQuestTID, INVALID_TABLE_TID));
	WaitQuestPacketList.AddTail(InQuestTID);
}

void WRQuestManager::OnAckCompletedQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID)
{
	WaitQuestPacketList.RemoveNode(InQuestTID);
	if (bInSuccess)
		CompletedQuest_Internal(InQuestTID);
}

void WRQuestManager::RequestCompletedQuestTask(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	if (IsWaitTaskPacket(InQuestTID, InQuestTaskTID))
		return;

	IMMessageSender::RequestCompleteQuestTask(InQuestTID, InQuestTaskTID);
	//PacketDatas.AddTail(FWRQuestPacketData(EWRQuestPacketType::OnResultCompletedQuestTaskReceived, InQuestTID, InQuestTaskTID));
	WaitTaskPacketList.AddTail(TPair<WRTableID, WRTableID>(InQuestTID, InQuestTaskTID));
}

void WRQuestManager::OnAckCompletedQuestTaskReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	WaitTaskPacketList.RemoveNode(TPair<WRTableID, WRTableID>(InQuestTID, InQuestTaskTID));
	if (bInSuccess)
		CompletedQuestTask_Internal(InQuestTID, InQuestTaskTID);
}

void WRQuestManager::OnInformCompletedQuestTaskReceived(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	CompletedQuestTask_Internal(InQuestTID, InQuestTaskTID);
}

void WRQuestManager::OnInformUpdateQuestTaskReceived(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID, const QUEST_TASK_VALUE_t IN InValue)
{
	ProgressQuestTaskController.OnInformUpdateQuestTaskReceived(InQuestTID, InQuestTaskTID, InValue);
}

void WRQuestManager::OnInformForcedQuestCompleteReceived(const WRTableID IN InQuestTID)
{
	CompletedQuest_Internal(InQuestTID);
}

void WRQuestManager::OnInformForcedQuestGiveUpReceived(const WRTableID IN InQuestTID)
{
	GiveUpQuest_Internal(InQuestTID);
}

void WRQuestManager::OpenQuestBoard()
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_QuestBoard* Panel_QuestBoard = Cast<UWRUIPanel_QuestBoard>(InCombinedPanelData.UIPanel);
			if (Panel_QuestBoard)
				Panel_QuestBoard->DisplayQuestBoard();
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::QuestBoard, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 	FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::QuestBoard);
// 	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::QuestBoard, FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
}

void WRQuestManager::ReadyQuest_Internal(const WRTableID IN InQuestTID)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	ChangeQuestState(InQuestTID, EWRQuestState::Disable, EWRQuestState::Ready);

	const FWRQuestTable* QuestTable = FindQuestTable(InQuestTID);
	if (QuestTable == nullptr)
		return;

	if (QuestTable->AcceptType == EWRQuestAcceptType::Auto)
	{
		if(bImpossibleAuto == false)
			RequestAcceptQuest(InQuestTID);
	}
	else if (QuestTable->AcceptType == EWRQuestAcceptType::NPCTalk)
	{
		if (QuestTable->AcceptTypeValue02 == WRMapManager::Get()->GetCurrentMapTID())
		{
			TDoubleLinkedList<AWRCharacter*> OutCharacterLIst;
			WRCharacterManager::Get()->FindAllByTID(QuestTable->AcceptTypeValue01, OutCharacterLIst);

			TDoubleLinkedList<AWRCharacter*>::TDoubleLinkedListNode* Node = OutCharacterLIst.GetHead();
			for (; Node != nullptr; Node = Node->GetNextNode())
			{
				AWRCharacter* Character = Cast<AWRCharacter>(Node->GetValue());
				if (Character == nullptr)
					continue;

				UWRActorComponentAcceptQuest* AcceptQuestComponent = Character->GetComponent<UWRActorComponentAcceptQuest>();
				if(AcceptQuestComponent == nullptr)
					continue;

				AcceptQuestComponent->OnQuestStateChanged();
			}
		}
	}

	if (QuestTable->ReadyDialogueTID)
	{
		QuestDirectionController.AddQuestDirectionData(FWRDialogueDirectionData(FWRQuestDirectionData::EWRQuestDirectionType::Dialogue, QuestTable->GetDialogueType(), QuestTable->ReadyDialogueTID));
		//WRDialogueManager::Get()->OpenDialogue(QuestTable->GetDialogueType(), QuestTable->ReadyDialogueTID);
	}
}

void WRQuestManager::AcceptQuest_Internal(const WRTableID IN InQuestTID)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	ChangeQuestState(InQuestTID, EWRQuestState::Ready, EWRQuestState::Accept);

	const FWRQuestTable* QuestTable = FindQuestTable(InQuestTID);
	if (QuestTable == nullptr)
		return;

	if (QuestTable->AcceptType == EWRQuestAcceptType::NPCTalk)
	{
		if (QuestTable->AcceptTypeValue02 == WRMapManager::Get()->GetCurrentMapTID())
		{
			TDoubleLinkedList<AWRCharacter*> OutCharacterLIst;
			WRCharacterManager::Get()->FindAllByTID(QuestTable->AcceptTypeValue01, OutCharacterLIst);

			TDoubleLinkedList<AWRCharacter*>::TDoubleLinkedListNode* Node = OutCharacterLIst.GetHead();
			for (; Node != nullptr; Node = Node->GetNextNode())
			{
				AWRCharacter* Character = Cast<AWRCharacter>(Node->GetValue());
				if (Character == nullptr)
					continue;

				UWRActorComponentAcceptQuest* AcceptQuestComponent = Character->GetComponent<UWRActorComponentAcceptQuest>();
				if (AcceptQuestComponent == nullptr)
					continue;

				AcceptQuestComponent->OnQuestStateChanged();
			}
		}

		bImpossibleAuto = true;
	}

	// 수락 액션 
	int32 MaxNum = QuestTable->AcceptActions.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		ProgressQuestAction(QuestTable->AcceptActions[i]);
	}

	// Task 수락
	if (QuestTable->TaskStyle == EWRQuestTaskStyle::Serial)
	{
		ProgressQuestTaskController.AddTask(InQuestTID, QuestTable->TaskTIDs[0]);
	}
	else if (QuestTable->TaskStyle == EWRQuestTaskStyle::Parallel)
	{
		MaxNum = QuestTable->TaskTIDs.Num();
		for (int32 i = 0; i < MaxNum; ++i)
		{
			if (QuestTable->TaskTIDs[i] == INVALID_TABLE_TID)
				break;

			ProgressQuestTaskController.AddTask(InQuestTID, QuestTable->TaskTIDs[i]);
		}
	}

	TrackedQuestTID = InQuestTID;

	if (QuestTable->AcceptDialogueTID)
	{
		QuestDirectionController.AddQuestDirectionData(FWRDialogueDirectionData(FWRQuestDirectionData::EWRQuestDirectionType::Dialogue, QuestTable->GetDialogueType(), QuestTable->AcceptDialogueTID));
		//WRDialogueManager::Get()->OpenDialogue(QuestTable->GetDialogueType(), QuestTable->AcceptDialogueTID);
	}

	QuestDirectionController.AddQuestDirectionData(FWRQuestDirectionData(FWRQuestDirectionData::EWRQuestDirectionType::QuestBoard, false));
 	//OpenQuestBoard();
}

void WRQuestManager::CompletableQuest_Internal(const WRTableID IN InQuestTID)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	ChangeQuestState(InQuestTID, EWRQuestState::Accept, EWRQuestState::Completable);

	const FWRQuestTable* QuestTable = FindQuestTable(InQuestTID);
	if (QuestTable == nullptr)
		return;

	if (QuestTable->CompleteType == EWRQuestCompleteType::Auto)
	{
		RequestCompletedQuest(InQuestTID);
	}
}

void WRQuestManager::CompletedQuest_Internal(const WRTableID IN InQuestTID)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	ChangeQuestState(InQuestTID, EWRQuestState::Completable, EWRQuestState::Completed);

	const FWRQuestTable* QuestTable = FindQuestTable(InQuestTID);
	if (QuestTable == nullptr)
		return;

	ProgressQuestTaskController.OnQuestCompleted(InQuestTID);

	// 완료 액션 
	int32 MaxNum = QuestTable->CompleteActions.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		ProgressQuestAction(QuestTable->CompleteActions[i]);
	}

	if (QuestTable->CompleteDialogueTID)
		QuestDirectionController.AddQuestDirectionData(FWRDialogueDirectionData(FWRQuestDirectionData::EWRQuestDirectionType::Dialogue, QuestTable->GetDialogueType(), QuestTable->CompleteDialogueTID));
		//WRDialogueManager::Get()->OpenDialogue(QuestTable->GetDialogueType(), QuestTable->CompleteDialogueTID);
		
		

	if (TrackedQuestTID == InQuestTID)
		TrackedQuestTID = INVALID_TABLE_TID;

	CheckDisableStateToReadyState();
}

void WRQuestManager::GiveUpQuest_Internal(const WRTableID IN InQuestTID)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	ChangeQuestState(InQuestTID, EWRQuestState::Accept, EWRQuestState::Ready);

	const FWRQuestTable* QuestTable = FindQuestTable(InQuestTID);
	if (QuestTable == nullptr)
		return;

	if (QuestTable->AcceptType == EWRQuestAcceptType::NPCTalk)
	{
		if (QuestTable->AcceptTypeValue02 == WRMapManager::Get()->GetCurrentMapTID())
		{
			TDoubleLinkedList<AWRCharacter*> OutCharacterLIst;
			WRCharacterManager::Get()->FindAllByTID(QuestTable->AcceptTypeValue01, OutCharacterLIst);

			TDoubleLinkedList<AWRCharacter*>::TDoubleLinkedListNode* Node = OutCharacterLIst.GetHead();
			for (; Node != nullptr; Node = Node->GetNextNode())
			{
				AWRCharacter* Character = Cast<AWRCharacter>(Node->GetValue());
				if (Character == nullptr)
					continue;

				UWRActorComponentAcceptQuest* AcceptQuestComponent = Character->GetComponent<UWRActorComponentAcceptQuest>();
				if (AcceptQuestComponent == nullptr)
					continue;

				AcceptQuestComponent->OnQuestStateChanged();
			}
		}
	}

	ProgressQuestTaskController.OnQuestGaveUp(InQuestTID);
}

void WRQuestManager::CompletedQuestTask_Internal(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	ProgressQuestTaskController.OnTaskCompleted(InQuestTID, InQuestTaskTID);
	CompletedTaskList.AddTail(TPair<WRTableID, WRTableID>(InQuestTID, InQuestTaskTID));

	if (CheckAllTaskCompleted(InQuestTID))
	{
		RequestCompletableQuest(InQuestTID);
	}
}

void WRQuestManager::InitializeQuestData()
{
	DisableQuestList.Empty();
	ReadyQuestList.Empty();
	AcceptQuestList.Empty();
	CompletableQuestList.Empty();
	CompletedQuestList.Empty();
	WaitQuestPacketList.Empty();
	WaitTaskPacketList.Empty();
	ProgressQuestTaskController.Clear();

	TrackedQuestTID = INVALID_TABLE_TID;

	UDataTable* MainQuestDataTable = WRTableManager::Get()->GetTable<FWRMainQuestTable>();
	if (MainQuestDataTable)
	{
		const TMap<FName, uint8*>& RowMap = MainQuestDataTable->GetRowMap();
		for (auto Iter = RowMap.CreateConstIterator(); Iter; ++Iter)
		{
			WRTableID QuestTID = FCString::Atoi(*(Iter.Key().ToString()));
			if (QuestTID == INVALID_TABLE_TID)
				continue;

			DisableQuestList.AddTail(QuestTID);
		}
	}
}

void WRQuestManager::CheckDisableStateToReadyState()
{
	TDoubleLinkedList<WRTableID> QuestTemp;	// 준비상태 패킷 보낼 퀘스트 TID 저장용
	TDoubleLinkedList<WRTableID>::TDoubleLinkedListNode* Node = DisableQuestList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRTableID QuestTID = Node->GetValue();
		if(QuestTID == INVALID_TABLE_TID)
			continue;

		const FWRQuestTable* QuestTable = FindQuestTable(QuestTID);
		if(QuestTable == nullptr)
			continue;

		bool bReady = true;
		int32 MaxNum = QuestTable->ReadyConditions.Num();
		for (int32 i = 0; i < MaxNum; ++i) 
		{
			const FWRQuestReadyCondition& QuestReadyCondition = QuestTable->ReadyConditions[i];
			if (QuestReadyCondition.Type == EWRQuestReadyCoditionType::None)
				break;

			if (CheckQuestReadyCondition(QuestReadyCondition) == false)
			{
				bReady = false;
				break;
			}
		}

		if(bReady == false)
			continue;

		QuestTemp.AddTail(QuestTID);
	}
	
	Node = QuestTemp.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRTableID QuestTID = Node->GetValue();
		if (QuestTID == INVALID_TABLE_TID)
			continue;

		ReadyQuest_Internal(QuestTID);
	}
}

bool WRQuestManager::CheckQuestReadyCondition(const FWRQuestReadyCondition& IN InQuestReadyCondition)
{
	if (InQuestReadyCondition.Type == EWRQuestReadyCoditionType::CompletedQuest)
	{
		return IsCompletedQuest(InQuestReadyCondition.Value);
	}

	return false;
}

bool WRQuestManager::CheckAllTaskCompleted(const WRTableID IN InQuestTID)
{
	const FWRQuestTable* QuestTable = FindQuestTable(InQuestTID);
	if (QuestTable == nullptr)
		return false;

	bool bAllCompleted = true;
	int32 MaxNum = QuestTable->TaskTIDs.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		if (QuestTable->TaskTIDs[i] == INVALID_TABLE_TID)
			break;;

		bool bCompletedTask = false;
		TDoubleLinkedList<TPair<WRTableID, WRTableID>>::TDoubleLinkedListNode* Node = CompletedTaskList.GetHead();
		for (; Node != nullptr; Node = Node->GetNextNode())
		{
			if(Node->GetValue().Key != InQuestTID)
				continue;

			if(Node->GetValue().Value != QuestTable->TaskTIDs[i])
				continue;

			bCompletedTask = true;
			break;
		}

		if(bCompletedTask)
			continue;

		return false;
	}

	return true;
}

void WRQuestManager::ChangeQuestState(const WRTableID IN InQuestTID, const EWRQuestState::en IN InBeforeQuestState, const EWRQuestState::en IN InAfterQuestState)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	TDoubleLinkedList<WRTableID>* BeforeQuestList = GetQuestListByQuestState(InBeforeQuestState);
	if (BeforeQuestList == nullptr)
		return;

	TDoubleLinkedList<WRTableID>* AfterQuestList = GetQuestListByQuestState(InAfterQuestState);
	if (AfterQuestList == nullptr)
		return;

	if (BeforeQuestList->Contains(InQuestTID) == false)
	{
		// 원래 상태가 틀렸다면 강제로 찾아서 바꿔준다.
		EWRQuestState::en FindQuestState = GetQuestState(InQuestTID);
		ChangeQuestState(InQuestTID, FindQuestState, InAfterQuestState);
		return;
	}

	BeforeQuestList->RemoveNode(InQuestTID);
	AfterQuestList->AddTail(InQuestTID);
}

void WRQuestManager::ProgressQuestAction(const FWRQuestAction& IN InQuestAction)
{
	if (InQuestAction.Type == EWRQuestActionType::None)
	{
		return;
	}
	else if (InQuestAction.Type == EWRQuestActionType::ExcuteTrigger)
	{
		WRTriggerManager::Get()->RunActiveTrigger(InQuestAction.Value);
	}
	else if (InQuestAction.Type == EWRQuestActionType::Tutorial)
	{
		WRTutorialManager::Get()->OnNewTutorial(InQuestAction.Value);
	}
	else
	{

	}
}

void WRQuestManager::OnTutorialCompleted(const WRTableID IN InTutorialTID)
{
	ProgressQuestTaskController.OnTutorialCompleted(InTutorialTID);
}

void WRQuestManager::OnQuestCompleteTriggered(const int32 IN TriggerUID)
{
	ProgressQuestTaskController.OnQuestCompleteTriggered(TriggerUID);
}

void WRQuestManager::OnCharacterSpawned(AWRCharacter* IN InCharacter)
{
	ProgressQuestTaskController.OnCharacterSpawned(InCharacter);
}

void WRQuestManager::OnCharacterUnSpawned(AWRCharacter* IN InCharacter)
{
	ProgressQuestTaskController.OnCharacterUnSpawned(InCharacter);
}

void WRQuestManager::OnQuestDirectionFinished(const FWRQuestDirectionData::EWRQuestDirectionType IN InQuestDirectionTYpe)
{
	QuestDirectionController.OnQuestDirectionFinished(InQuestDirectionTYpe);
}

void WRQuestManager::StartGame()
{
	InitializeQuestData();
	CheckDisableStateToReadyState();

	/*AWRGameModeBase* GameModeBase = Cast<AWRGameModeBase>(UGameplayStatics::GetGameMode(UWRGameInstance::GetGameInstance()));
	if (GameModeBase == nullptr)
		return;

	if (GameModeBase->GetGameModeType() == EWRGameMode::Field)
	{
		InitializeQuestData();
		CheckDisableStateToReadyState();
	}*/
}

TDoubleLinkedList<WRTableID>* WRQuestManager::GetQuestListByQuestState(const EWRQuestState::en IN InQuestState)
{
	switch (InQuestState)
	{
	case EWRQuestState::en::Disable: return &DisableQuestList;
	case EWRQuestState::en::Ready: return &ReadyQuestList;
	case EWRQuestState::en::Accept: return &AcceptQuestList;
	case EWRQuestState::en::Completable: return &CompletableQuestList;
	case EWRQuestState::en::Completed: return &CompletedQuestList;
	default: return nullptr;
	}

	return nullptr;
}
