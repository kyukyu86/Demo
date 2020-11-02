// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRQuestTaskComponentTrigger.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRCompassManager.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCoordinateTable.h"
#include "Manager/WRMapManager.h"

WRQuestTaskComponentTrigger::WRQuestTaskComponentTrigger(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
: WRQuestTaskComponentBase(InQuestTID, InTaskTID, InQuestTaskTable), CompassMarkHandle(INVALID_COMPASS_MARK_HANDLE)
{
	AddMark();
}

void WRQuestTaskComponentTrigger::Tick(const float IN InDeltaTime)
{
	WRQuestTaskComponentBase::Tick(InDeltaTime);
}

void WRQuestTaskComponentTrigger::OnDestroy()
{
	RemoveMark();

	WRQuestTaskComponentBase::OnDestroy();
}

void WRQuestTaskComponentTrigger::OnLoadLevelStart()
{

}

void WRQuestTaskComponentTrigger::OnLoadLevelProcessEnded()
{

}

void WRQuestTaskComponentTrigger::OnTaskCompleted()
{
	WRQuestTaskComponentBase::OnTaskCompleted();

	RemoveMark();
}

void WRQuestTaskComponentTrigger::OnTaskGaveUp()
{
	RemoveMark();
}

void WRQuestTaskComponentTrigger::OnQuestCompleteTriggered(const int32 IN InTriggerUID)
{
	if (InTriggerUID == 0)
		return;

	const FWRQuestTriggerTaskTable* TriggerTaskTable = static_cast<const FWRQuestTriggerTaskTable*>(TaskTable);

	if (TriggerTaskTable == nullptr)
		return;

	if (WRMapManager::Get()->GetCurrentMapTID() != TriggerTaskTable->MapTID)
		return;

	if (TriggerTaskTable->TriggerTID != InTriggerUID)
		return;

	WRQuestManager::Get()->RequestCompletedQuestTask(QuestTID, TaskTID);
}

void WRQuestTaskComponentTrigger::AddMark()
{
	if (TaskTable == nullptr)
		return;

	const FWRQuestTriggerTaskTable* QuestTriggerTaskTable = static_cast<const FWRQuestTriggerTaskTable*>(TaskTable);
	if (QuestTriggerTaskTable == nullptr)
		return;

	CompassMarkHandle = WRCompassManager::Get()->AddMark(QuestTriggerTaskTable->MapTID, QuestTriggerTaskTable->TriggerPosition, QuestTriggerTaskTable->IconPositionOffset);
}

void WRQuestTaskComponentTrigger::RemoveMark()
{
	if (CompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
		return;

	WRCompassManager::Get()->RemoveMark(CompassMarkHandle);
}
