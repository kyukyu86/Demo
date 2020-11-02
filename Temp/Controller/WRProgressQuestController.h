// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "Define/WRTypeDefine.h"
#include "Table/WRQuestTable.h"


/**
 * 
 */
class WRQuestTaskComponentBase;
class AWRCharacter;

class WRProgressQuestTaskController
{
public:
	WRProgressQuestTaskController();
	~WRProgressQuestTaskController();
public:
	void Tick(const float IN InDeltaTime);
	void OnShutdown();
	void OnLoadLevelStart();
	void OnLoadLevelProcessEnded();
public:
	void Clear();

	void GetProgressTaskByQuest(const WRTableID IN InQuestTID, TDoubleLinkedList<WRTableID>& OUT OutTaskTIDs);

	void AddTask(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);
	void OnTaskCompleted(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);
	void OnQuestCompleted(const WRTableID IN InQuestTID);
	void OnQuestGaveUp(const WRTableID IN InQuestTID);	// 포기했따

	void OnTutorialCompleted(const WRTableID IN InTutorialTID);
	void OnQuestCompleteTriggered(const int32 IN InTriggerUID);
	void OnCharacterSpawned(AWRCharacter* IN InCharacter);
	void OnCharacterUnSpawned(AWRCharacter* IN InCharacter);
	
	void OnInformUpdateQuestTaskReceived(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID, const QUEST_TASK_VALUE_t IN InValue);
private:
	void AddTask_Internal(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);
	WRQuestTaskComponentBase* FindQuestTaskComponentBase(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);
private:
	TDoubleLinkedList<WRQuestTaskComponentBase*> ProgressTaskComponentList;
};
