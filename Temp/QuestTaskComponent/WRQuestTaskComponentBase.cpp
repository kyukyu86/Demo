// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRQuestTaskComponentBase.h"
#include "Manager/WRQuestManager.h"

WRQuestTaskComponentBase::WRQuestTaskComponentBase(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
	: QuestTID(InQuestTID), TaskTID(InTaskTID), TaskTable(InQuestTaskTable)
{

}

void WRQuestTaskComponentBase::OnDestroy()
{

}

void WRQuestTaskComponentBase::OnLoadLevelStart()
{

}

void WRQuestTaskComponentBase::OnLoadLevelProcessEnded()
{

}

void WRQuestTaskComponentBase::OnTaskCompleted()
{

}

void WRQuestTaskComponentBase::OnTaskGaveUp()
{

}

void WRQuestTaskComponentBase::OnInformUpdateQuestTaskReceived(const QUEST_TASK_VALUE_t IN InValue)
{
	WRQuestManager::Get()->RequestCompletedQuestTask(QuestTID, TaskTID);
}
