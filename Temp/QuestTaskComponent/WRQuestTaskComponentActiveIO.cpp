// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRQuestTaskComponentActiveIO.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRCompassManager.h"
#include "Manager/WRMapManager.h"
#include "Manager/WRCharacterManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRCoordinateTable.h"
#include "Table/WRQuestTable.h"
#include "Define/WRTypeDefine.h"
#include "Actor/Character/WRCharacterIO.h"


WRQuestTaskComponentActiveIO::WRQuestTaskComponentActiveIO(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
: WRQuestTaskComponentBase(InQuestTID, InTaskTID, InQuestTaskTable)
{
	CompassMarkHandles.Empty();
	AddMark();
}

void WRQuestTaskComponentActiveIO::Tick(const float IN InDeltaTime)
{
	WRQuestTaskComponentBase::Tick(InDeltaTime);
}

void WRQuestTaskComponentActiveIO::OnDestroy()
{
	RemoveMark();

	WRQuestTaskComponentBase::OnDestroy();
}

void WRQuestTaskComponentActiveIO::OnLoadLevelStart()
{

}

void WRQuestTaskComponentActiveIO::OnLoadLevelProcessEnded()
{

}

void WRQuestTaskComponentActiveIO::OnTaskCompleted()
{
	WRQuestTaskComponentBase::OnTaskCompleted();

	RemoveMark();
}

void WRQuestTaskComponentActiveIO::OnTaskGaveUp()
{
	RemoveMark();
}

void WRQuestTaskComponentActiveIO::OnCharacterSpawned(AWRCharacter* IN InCharacter)
{
	if (InCharacter == nullptr)
		return;

	const FWRQuestActiveIOTaskTable* QuestActiveIOTaskTable = static_cast<const FWRQuestActiveIOTaskTable*>(TaskTable);
	if (QuestActiveIOTaskTable == nullptr)
		return;

	if (QuestActiveIOTaskTable->ObjectTID != InCharacter->GetCharacterTableID())
		return;

	WRCompassMarkHandle CompassMarkHandle = WRCompassManager::Get()->AddMark(WRMapManager::Get()->GetCurrentMapTID(), InCharacter, FVector::ZeroVector);
	if (CompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
		return;

	CompassMarkHandles.AddTail(TPair<WRCompassMarkHandle, AActor*>(CompassMarkHandle, InCharacter));
}

void WRQuestTaskComponentActiveIO::OnCharacterUnSpawned(AWRCharacter* IN InCharacter)
{
	if (InCharacter == nullptr)
		return;

	TDoubleLinkedList<TPair<WRCompassMarkHandle, AActor*>>::TDoubleLinkedListNode* Node = CompassMarkHandles.GetHead();

	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		if (Node->GetValue().Value == nullptr)
			continue;

		if( Node->GetValue().Value != InCharacter)
			continue;

		WRCompassManager::Get()->RemoveMark(Node->GetValue().Key);
		CompassMarkHandles.RemoveNode(Node);
		break;
	}
}

void WRQuestTaskComponentActiveIO::AddMark()
{
	if (TaskTable == nullptr)
		return;

	const FWRQuestActiveIOTaskTable* QuestActiveIOTaskTable = static_cast<const FWRQuestActiveIOTaskTable*>(TaskTable);
	if (QuestActiveIOTaskTable == nullptr)
		return;

	TDoubleLinkedList<AWRCharacter*> OutCharacterLIst;
	WRCharacterManager::Get()->FindAllByTID(QuestActiveIOTaskTable->ObjectTID, OutCharacterLIst);

	TDoubleLinkedList<AWRCharacter*>::TDoubleLinkedListNode* Node = OutCharacterLIst.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		AWRCharacterIO* CharacterIO = Cast<AWRCharacterIO>(Node->GetValue());
		if(CharacterIO == nullptr)
			continue;

		WRCompassMarkHandle CompassMarkHandle = WRCompassManager::Get()->AddMark(WRMapManager::Get()->GetCurrentMapTID(), CharacterIO, CharacterIO->GetMarkOffset());
		if (CompassMarkHandle != INVALID_COMPASS_MARK_HANDLE)
			CompassMarkHandles.AddTail(TPair<WRCompassMarkHandle, AActor*>(CompassMarkHandle, Node->GetValue()));		
	}
}

void WRQuestTaskComponentActiveIO::RemoveMark()
{
	if (CompassMarkHandles.Num() == 0)
		return;

	TDoubleLinkedList<TPair<WRCompassMarkHandle, AActor*>>::TDoubleLinkedListNode* Node = CompassMarkHandles.GetHead();

	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		if(Node->GetValue().Key == INVALID_COMPASS_MARK_HANDLE)
			continue;

		WRCompassManager::Get()->RemoveMark(Node->GetValue().Key);
	}

	CompassMarkHandles.Empty();
}
