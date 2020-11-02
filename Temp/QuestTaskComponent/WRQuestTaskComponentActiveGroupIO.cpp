// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRQuestTaskComponentActiveGroupIO.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRCompassManager.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCoordinateTable.h"
#include "Manager/WRMapManager.h"
#include "Table/WRQuestTable.h"

WRQuestTaskComponentActiveGroupIO::WRQuestTaskComponentActiveGroupIO(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
: WRQuestTaskComponentBase(InQuestTID, InTaskTID, InQuestTaskTable), CompassMarkHandle(INVALID_COMPASS_MARK_HANDLE)
{
	AddMark();
}

void WRQuestTaskComponentActiveGroupIO::Tick(const float IN InDeltaTime)
{
	WRQuestTaskComponentBase::Tick(InDeltaTime);
}

void WRQuestTaskComponentActiveGroupIO::OnDestroy()
{
	RemoveMark();

	WRQuestTaskComponentBase::OnDestroy();
}

void WRQuestTaskComponentActiveGroupIO::OnLoadLevelStart()
{

}

void WRQuestTaskComponentActiveGroupIO::OnLoadLevelProcessEnded()
{

}

void WRQuestTaskComponentActiveGroupIO::OnTaskCompleted()
{
	WRQuestTaskComponentBase::OnTaskCompleted();

	RemoveMark();
}

void WRQuestTaskComponentActiveGroupIO::OnTaskGaveUp()
{
	RemoveMark();
}

void WRQuestTaskComponentActiveGroupIO::AddMark()
{
	if (TaskTable == nullptr)
		return;

	const FWRQuestActiveGroupIOTaskTable* QuestActiveGroupIOTaskTable = static_cast<const FWRQuestActiveGroupIOTaskTable*>(TaskTable);
	if (QuestActiveGroupIOTaskTable == nullptr)
		return;

	//CompassMarkHandle = WRCompassManager::Get()->AddMark(WRMapManager::Get()->GetCurrentMapTID(), QuestActiveGroupIOTaskTable->ObjectPosition, QuestActiveGroupIOTaskTable->IconPositionOffset);
}

void WRQuestTaskComponentActiveGroupIO::RemoveMark()
{
	if (CompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
		return;

	WRCompassManager::Get()->RemoveMark(CompassMarkHandle);
}
