#include "WRProgressQuestController.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRQuestManager.h"
#include "UI/Panel/WRUIPanel_Main.h"
#include "QuestTaskComponent/WRQuestTaskComponentBase.h"
#include "QuestTaskComponent/WRQuestTaskComponentMove.h"
#include "QuestTaskComponent/WRQuestTaskComponentTrigger.h"
#include "QuestTaskComponent/WRQuestTaskComponentTutorial.h"
#include "QuestTaskComponent/WRQuestTaskComponentActiveIO.h"
#include "Actor/Character/WRCharacter.h"

WRProgressQuestTaskController::WRProgressQuestTaskController()
{

}

WRProgressQuestTaskController::~WRProgressQuestTaskController()
{

}

void WRProgressQuestTaskController::Tick(const float IN InDeltaTime)
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		Node->GetValue()->Tick(InDeltaTime);
	}
}

void WRProgressQuestTaskController::OnShutdown()
{
	Clear();
}

void WRProgressQuestTaskController::OnLoadLevelStart()
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRQuestTaskComponentBase* QuestTaskComponent = Node->GetValue();

		if (QuestTaskComponent == nullptr)
			continue;

		QuestTaskComponent->OnLoadLevelStart();
	}
}

void WRProgressQuestTaskController::OnLoadLevelProcessEnded()
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRQuestTaskComponentBase* QuestTaskComponent = Node->GetValue();

		if (QuestTaskComponent == nullptr)
			continue;

		QuestTaskComponent->OnLoadLevelProcessEnded();
	}
}

void WRProgressQuestTaskController::GetProgressTaskByQuest(const WRTableID IN InQuestTID, TDoubleLinkedList<WRTableID>& OUT OutTaskTIDs)
{
	OutTaskTIDs.Empty();

	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRQuestTaskComponentBase* QuestTaskComponent = Node->GetValue();

		if(QuestTaskComponent == nullptr)
			continue;

		if (QuestTaskComponent->QuestTID != InQuestTID)
			continue;

		OutTaskTIDs.AddTail(QuestTaskComponent->TaskTID);
	}
}

void WRProgressQuestTaskController::AddTask(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	if (InQuestTID == INVALID_TABLE_TID || InQuestTaskTID == INVALID_TABLE_TID)
		return;

	const FWRQuestTaskTable* QuestTaskTable = WRQuestManager::Get()->FindQuestTaskTable(InQuestTaskTID);
	if (QuestTaskTable == nullptr)
		return;

	AddTask_Internal(InQuestTID, InQuestTaskTID, QuestTaskTable);
}

void WRProgressQuestTaskController::OnTaskCompleted(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRQuestTaskComponentBase* QuestTaskComponent = Node->GetValue();

		if (QuestTaskComponent->QuestTID != InQuestTID)
			continue;

		if (QuestTaskComponent->TaskTID != InQuestTaskTID)
			continue;

		QuestTaskComponent->OnTaskCompleted();

		ProgressTaskComponentList.RemoveNode(Node);
		delete QuestTaskComponent;
		break;
	}
}

void WRProgressQuestTaskController::OnQuestCompleted(const WRTableID IN InQuestTID)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; )
	{
		WRQuestTaskComponentBase* QuestTaskComponent = Node->GetValue();

		if (QuestTaskComponent->QuestTID != InQuestTID)
		{
			Node = Node->GetNextNode();
			continue;
		}
			
		QuestTaskComponent->OnTaskCompleted();

		TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* NodeTemp = Node;
		Node = Node->GetNextNode();
		ProgressTaskComponentList.RemoveNode(NodeTemp);
		
		delete QuestTaskComponent;
	}
}

void WRProgressQuestTaskController::OnQuestGaveUp(const WRTableID IN InQuestTID)
{
	if (InQuestTID == INVALID_TABLE_TID)
		return;

	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; )
	{
		WRQuestTaskComponentBase* QuestTaskComponent = Node->GetValue();

		if (QuestTaskComponent->QuestTID != InQuestTID)
		{
			Node = Node->GetNextNode();
			continue;
		}

		QuestTaskComponent->OnTaskGaveUp();

		TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* NodeTemp = Node;
		Node = Node->GetNextNode();
		ProgressTaskComponentList.RemoveNode(NodeTemp);

		delete QuestTaskComponent;
	}
}

void WRProgressQuestTaskController::OnTutorialCompleted(const WRTableID IN InTutorialTID)
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		if(Node->GetValue()->GetTaskType() != EWRQuestTaskType::ClearTutorial)
			continue;

		WRQuestTaskComponentTutorial* TutorialComponent = static_cast<WRQuestTaskComponentTutorial*>(Node->GetValue());
		if (TutorialComponent == nullptr)
			continue;

		TutorialComponent->OnTutorialCompleted(InTutorialTID);
	}
}

void WRProgressQuestTaskController::OnQuestCompleteTriggered(const int32 IN InTriggerUID)
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		if (Node->GetValue()->GetTaskType() != EWRQuestTaskType::ActiveTrigger)
			continue;

		WRQuestTaskComponentTrigger* TriggerComponent = static_cast<WRQuestTaskComponentTrigger*>(Node->GetValue());
		if (TriggerComponent == nullptr)
			continue;

		TriggerComponent->OnQuestCompleteTriggered(InTriggerUID);
	}
}

void WRProgressQuestTaskController::OnCharacterSpawned(AWRCharacter* IN InCharacter)
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRQuestTaskComponentBase* QuestTaskComponent = static_cast<WRQuestTaskComponentBase*>(Node->GetValue());
		if (QuestTaskComponent == nullptr)
			continue;

		QuestTaskComponent->OnCharacterSpawned(InCharacter);
	}
}

void WRProgressQuestTaskController::OnCharacterUnSpawned(AWRCharacter* IN InCharacter)
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRQuestTaskComponentBase* QuestTaskComponent = static_cast<WRQuestTaskComponentBase*>(Node->GetValue());
		if (QuestTaskComponent == nullptr)
			continue;

		QuestTaskComponent->OnCharacterUnSpawned(InCharacter);
	}
}

void WRProgressQuestTaskController::OnInformUpdateQuestTaskReceived(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID, const QUEST_TASK_VALUE_t IN InValue)
{
	WRQuestTaskComponentBase* QuestTaskComponentBase = FindQuestTaskComponentBase(InQuestTID, InQuestTaskTID);
	if (QuestTaskComponentBase == nullptr)
		return;

	QuestTaskComponentBase->OnInformUpdateQuestTaskReceived(InValue);
}

void WRProgressQuestTaskController::Clear()
{
	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRQuestTaskComponentBase* QuestTaskComponent = Node->GetValue();

		if (QuestTaskComponent == nullptr)
			continue;

		QuestTaskComponent->OnDestroy();
		delete QuestTaskComponent;
	}
	ProgressTaskComponentList.Empty();
}

void WRProgressQuestTaskController::AddTask_Internal(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
{
	if (InQuestTID == INVALID_TABLE_TID || InQuestTaskTable == nullptr)
		return;

	WRQuestTaskComponentBase* CreatedComponent = nullptr;

	switch (InQuestTaskTable->GetTaskType())
	{
	case EWRQuestTaskType::MovePosition: 
	{
		CreatedComponent = static_cast<WRQuestTaskComponentBase*>(new WRQuestTaskComponentMove(InQuestTID, InQuestTaskTID, InQuestTaskTable));
	}
	break;
	case EWRQuestTaskType::ActiveTrigger:
	{
		CreatedComponent = static_cast<WRQuestTaskComponentBase*>(new WRQuestTaskComponentTrigger(InQuestTID, InQuestTaskTID, InQuestTaskTable));
	}
	break;
	case EWRQuestTaskType::ClearTutorial:
	{
		CreatedComponent = static_cast<WRQuestTaskComponentBase*>(new WRQuestTaskComponentTutorial(InQuestTID, InQuestTaskTID, InQuestTaskTable));
	}
	break;
	case EWRQuestTaskType::ActiveIO:
	{
		CreatedComponent = static_cast<WRQuestTaskComponentBase*>(new WRQuestTaskComponentActiveIO(InQuestTID, InQuestTaskTID, InQuestTaskTable));
	}
	break;
	default:
		break;
	}

	if (CreatedComponent == nullptr)
		return;
	
	ProgressTaskComponentList.AddTail(CreatedComponent);
}

WRQuestTaskComponentBase* WRProgressQuestTaskController::FindQuestTaskComponentBase(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	if (InQuestTID == INVALID_TABLE_TID || InQuestTaskTID == INVALID_TABLE_TID)
		return nullptr;

	TDoubleLinkedList<WRQuestTaskComponentBase*>::TDoubleLinkedListNode* Node = ProgressTaskComponentList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		if (Node->GetValue()->QuestTID != InQuestTID)
			continue;

		if (Node->GetValue()->TaskTID != InQuestTaskTID)
			continue;

		return Node->GetValue();
	}

	return nullptr;
}
