// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "Define/WRTypeDefine.h"
#include "Table/WRQuestTable.h"
#include "Network/Share/Define/Define_Type.h"

struct FWRQuestTaskTable;
class AWRCharacter;
class WR_API WRQuestTaskComponentBase
{
	friend class WRProgressQuestTaskController;
public:
	WRQuestTaskComponentBase(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);
	virtual ~WRQuestTaskComponentBase() {}
	
public:
	virtual void Tick(const float IN InDeltaTime){}
	virtual void OnDestroy();
	virtual void OnLoadLevelStart();
	virtual void OnLoadLevelProcessEnded();

	virtual void OnTaskCompleted();
	virtual void OnTaskGaveUp();
	virtual void OnCharacterSpawned(AWRCharacter* IN InCharacter) {}
	virtual void OnCharacterUnSpawned(AWRCharacter* IN InCharacter) {}
	void OnInformUpdateQuestTaskReceived(const QUEST_TASK_VALUE_t IN InValue);
	

	EWRQuestTaskType GetTaskType() { return (TaskTable ? TaskTable->GetTaskType() : EWRQuestTaskType::None); }
protected:
	WRTableID QuestTID;
	WRTableID TaskTID;
	const FWRQuestTaskTable* TaskTable;
};
