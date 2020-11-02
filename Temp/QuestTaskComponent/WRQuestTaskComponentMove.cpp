// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRQuestTaskComponentMove.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRCompassManager.h"
#include "Manager/WRVFXManager.h"

#include "Table/WRCoordinateTable.h"
#include "Table/Base/WRTableManager.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Component/Character/WRActorComponentVFX.h"
#include "Define/WRPathDefine.h"
#include "Manager/WRMapManager.h"
#include "Network/IMMessageSender.h"
#include "Manager/WRTargetLocationGuideManager.h"


#define DEF_MOVE_QUEST_MARK_OFFSET 220
WRQuestTaskComponentMove::WRQuestTaskComponentMove(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable)
	: WRQuestTaskComponentBase(InQuestTID, InTaskTID, InQuestTaskTable), FXUID(0), CompassMarkHandle(INVALID_COMPASS_MARK_HANDLE), TargetLocationGuideHandle(INVALID_TARGET_LOCATION_GUIDE_HANDLE)
{
	SpawnFX();
	AddMark();
	AddTargetLocationGuide();
}

void WRQuestTaskComponentMove::Tick(const float IN InDeltaTime)
{
	WRQuestTaskComponentBase::Tick(InDeltaTime);

	if (WRQuestManager::Get()->IsWaitTaskPacket(QuestTID, TaskTID))
		return;

	CheckArriveTargetPosition();
}

void WRQuestTaskComponentMove::OnDestroy()
{
	if (FXUID)
		RemoveFX();

	RemoveMark();
	RemoveTargetLocationGuide();

	WRQuestTaskComponentBase::OnDestroy();
}

void WRQuestTaskComponentMove::OnLoadLevelStart()
{
	if (FXUID)
		RemoveFX();

}

void WRQuestTaskComponentMove::OnLoadLevelProcessEnded()
{
	SpawnFX();
	RemoveTargetLocationGuide();
}

void WRQuestTaskComponentMove::OnTaskCompleted()
{
	WRQuestTaskComponentBase::OnTaskCompleted();

	if (FXUID)
		RemoveFX();

	RemoveMark();
	RemoveTargetLocationGuide();

	SpawnCompleteQuestFX();
}

void WRQuestTaskComponentMove::OnTaskGaveUp()
{
	if (FXUID)
		RemoveFX();

	RemoveMark();
	RemoveTargetLocationGuide();
}

void WRQuestTaskComponentMove::CheckArriveTargetPosition()
{
	if (TaskTable == nullptr)
		return;

	const FWRQuestMoveTaskTable* QuestMoveTaskTable = static_cast<const FWRQuestMoveTaskTable*>(TaskTable);
	if (QuestMoveTaskTable == nullptr)
		return;

	const FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(QuestMoveTaskTable->CoordinateTID);
	if (CoordinateTable == nullptr)
		return;

	if (WRMapManager::Get()->GetCurrentMapTID() != CoordinateTable->MapTID)
		return;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	float Dist = FVector::Dist(MyCharacter->GetActorLocation(), CoordinateTable->Position);
	if (Dist < CoordinateTable->Range)
	{
		MyCharacter->RequestStop();

		WRQuestManager::Get()->RequestCompletedQuestTask(QuestTID, TaskTID);
	}
}

void WRQuestTaskComponentMove::SpawnFX()
{
	if (TaskTable == nullptr)
		return;

	const FWRQuestMoveTaskTable* QuestMoveTaskTable = static_cast<const FWRQuestMoveTaskTable*>(TaskTable);
	if (QuestMoveTaskTable == nullptr)
		return;

	const FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(QuestMoveTaskTable->CoordinateTID);
	if (CoordinateTable == nullptr)
		return;

	if (WRMapManager::Get()->GetCurrentMapTID() != CoordinateTable->MapTID)
		return;

	AWRVFX* SpawnedFX = WRVFXManager::Get()->Spawn(EFFECT_PATH_4_QUEST_POINT, nullptr, nullptr);
	if (SpawnedFX)
	{
		FXUID = SpawnedFX->GetUniqueID();
		SpawnedFX->SetActorLocation(CoordinateTable->Position);
	}
	else
	{
		FXUID = 0;
	}
}

void WRQuestTaskComponentMove::RemoveFX()
{
	WRVFXManager::Get()->Remove(FXUID);
	FXUID = 0;
}

void WRQuestTaskComponentMove::SpawnCompleteQuestFX()
{
	if (TaskTable == nullptr)
		return;

	const FWRQuestMoveTaskTable* QuestMoveTaskTable = static_cast<const FWRQuestMoveTaskTable*>(TaskTable);
	if (QuestMoveTaskTable == nullptr)
		return;

	const FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(QuestMoveTaskTable->CoordinateTID);
	if (CoordinateTable == nullptr)
		return;

	AWRVFX* SpawnedFX = WRVFXManager::Get()->Spawn(EFFECT_PATH_4_COMPLETE_QUEST, nullptr, nullptr);
	if (SpawnedFX == nullptr)
		return;

	SpawnedFX->SetActorLocation(CoordinateTable->Position);
}

void WRQuestTaskComponentMove::AddMark()
{
	if (CompassMarkHandle != INVALID_COMPASS_MARK_HANDLE)
		return;

	if (TaskTable == nullptr)
		return;

	const FWRQuestMoveTaskTable* QuestMoveTaskTable = static_cast<const FWRQuestMoveTaskTable*>(TaskTable);
	if (QuestMoveTaskTable == nullptr)
		return;

	const FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(QuestMoveTaskTable->CoordinateTID);
	if (CoordinateTable == nullptr)
		return;

	CompassMarkHandle = WRCompassManager::Get()->AddMark(CoordinateTable->MapTID, CoordinateTable->Position, DEF_MOVE_QUEST_MARK_OFFSET);
}

void WRQuestTaskComponentMove::RemoveMark()
{
	if (CompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
		return;

	WRCompassManager::Get()->RemoveMark(CompassMarkHandle);
}

void WRQuestTaskComponentMove::AddTargetLocationGuide()
{
	if (TargetLocationGuideHandle != INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return;

	if (TaskTable == nullptr)
		return;

	const FWRQuestMoveTaskTable* QuestMoveTaskTable = static_cast<const FWRQuestMoveTaskTable*>(TaskTable);
	if (QuestMoveTaskTable == nullptr)
		return;

	const FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(QuestMoveTaskTable->CoordinateTID);
	if (CoordinateTable == nullptr)
		return;

	FVector TargetPosition = CoordinateTable->Position;
	TargetPosition.Z += DEF_MOVE_QUEST_MARK_OFFSET;

	TargetLocationGuideHandle = WRTargetLocationGuideManager::Get()->AddTargetLocationInfo(CoordinateTable->MapTID, TargetPosition, EWRUISlotKind::QuestMarkForTargetLocationGuide, nullptr, false, WRConsoleManager::Get()->GetInt(EWRConsole::UI_3DMarkDIst));
}

void WRQuestTaskComponentMove::RemoveTargetLocationGuide()
{
	if (TargetLocationGuideHandle == INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return;

	WRTargetLocationGuideManager::Get()->RemoveTargetLocationInfo(TargetLocationGuideHandle);
}
