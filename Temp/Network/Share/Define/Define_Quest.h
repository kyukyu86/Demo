//******************************************************************************
//	FileName: 	Define_Quest.h
//------------------------------------------------------------------------------
//	Created	:	2019/09/24
//------------------------------------------------------------------------------
//	Author	:	gyutaelee & seungyeop
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	퀘스트 관련 정보
//******************************************************************************

#pragma once

#include "../Protocol/IMMessage.h"

#define MAX_QUEST_TASK_COUNT			5					// 하나의 퀘스트 당 최대 태스크 개수
#define MOVE_QUEST_POSTION_OFFSET		(float)200			// 특정 좌표 이동퀘스트 거리계산 여유값


struct stQuestTask : IMSerialize
{
	TID_t				taskTID;
	QUEST_TASK_VALUE_t	taskValue;

	stQuestTask()
		: taskTID(INVALID_TID)
		, taskValue(INVALID_QUEST_TASK_VALUE)
	{
	}
	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> taskTID >> taskValue;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << taskTID << taskValue;
	}
#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{

	}
#endif	// USE_PROTOCOL_TOSTRING
};

struct stQuest : IMSerialize
{
	TID_t						TID;
	std::vector<stQuestTask>	vecTasks;

	stQuest()
		: TID(INVALID_TID)
	{
		vecTasks.clear();
	}
	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> TID >> vecTasks;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << TID << vecTasks;
	}
#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{

	}
#endif	// USE_PROTOCOL_TOSTRING
};

/*----------------------------
 - 퀘스트 타입
------------------------------*/
struct EWRQuestType
{
	enum en : int8
	{
		None,
		Main,		// 메인 퀘스트
		MAX
	};

#ifdef USE_SERVER_ONLY
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,		L"None");
		InsertMap(Main,		L"Main");
	}
#endif // USE_SERVER_ONLY
};

/*----------------------------
 - 퀘스트 상태
------------------------------*/
struct EWRQuestState
{
	enum en : int8
	{
		None,
		Disable,			// 완료/수락 불가능
		Ready,				// 수락 가능
		Accept,				// 진행중
		Completable,		// 완료 가능
		Completed,			// 완료
		MAX
	};

#ifdef USE_SERVER_ONLY
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,			L"None");
		InsertMap(Disable,		L"Disable");
		InsertMap(Ready,		L"Ready");
		InsertMap(Accept,		L"Accept");
		InsertMap(Completable,	L"Completable");
		InsertMap(Completed,	L"Completed");
	}
#endif // USE_SERVER_ONLY
};