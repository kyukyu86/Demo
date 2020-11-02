// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRQuestTaskComponentTutorial.h"
#include "Manager/WRQuestManager.h"
#include "Table/WRQuestTable.h"

WRQuestTaskComponentTutorial::WRQuestTaskComponentTutorial(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
: WRQuestTaskComponentBase(InQuestTID, InTaskTID, InQuestTaskTable)
{

}

void WRQuestTaskComponentTutorial::Tick(const float IN InDeltaTime)
{
	WRQuestTaskComponentBase::Tick(InDeltaTime);
}

void WRQuestTaskComponentTutorial::OnTutorialCompleted(const WRTableID IN InTutorialTID)
{
	if (InTutorialTID == INVALID_TABLE_TID)
		return;
	
	const FWRQuestTutorialTaskTable* TutorialTaskTable = static_cast<const FWRQuestTutorialTaskTable*>(TaskTable);

	if (TutorialTaskTable == nullptr)
		return;

	if (TutorialTaskTable->TutorialTID != InTutorialTID)
		return;

	WRQuestManager::Get()->RequestCompletedQuestTask(QuestTID, TaskTID);
}
