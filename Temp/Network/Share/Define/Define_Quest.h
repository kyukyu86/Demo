//******************************************************************************
//	FileName: 	Define_Quest.h
//------------------------------------------------------------------------------
//	Created	:	2019/09/24
//------------------------------------------------------------------------------
//	Author	:	gyutaelee & seungyeop
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	����Ʈ ���� ����
//******************************************************************************

#pragma once

#include "../Protocol/IMMessage.h"

#define MAX_QUEST_TASK_COUNT			5					// �ϳ��� ����Ʈ �� �ִ� �½�ũ ����
#define MOVE_QUEST_POSTION_OFFSET		(float)200			// Ư�� ��ǥ �̵�����Ʈ �Ÿ���� ������


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
 - ����Ʈ Ÿ��
------------------------------*/
struct EWRQuestType
{
	enum en : int8
	{
		None,
		Main,		// ���� ����Ʈ
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
 - ����Ʈ ����
------------------------------*/
struct EWRQuestState
{
	enum en : int8
	{
		None,
		Disable,			// �Ϸ�/���� �Ұ���
		Ready,				// ���� ����
		Accept,				// ������
		Completable,		// �Ϸ� ����
		Completed,			// �Ϸ�
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