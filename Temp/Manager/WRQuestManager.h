// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_Quest.h"
#include "Controller/WRProgressQuestController.h"
#include "Controller/WRQuestDirectionController.h"
#include "Table/WRQuestTable.h"
#include "Define/WRTypeDefine.h"



/**
*
*/

// 

enum EWRQuestPacketType
{
	OnResultReadyQuestReceived,
	OnResultAcceptQuestReceived,
	OnResultCompletableQuestReceived,
	OnResultCompletedQuestReceived,
	OnResultCompletedQuestTaskReceived,
};

struct FWRQuestPacketData
{
	FWRQuestPacketData(EWRQuestPacketType IN InQuestPacketType, WRTableID IN InQuestTID, WRTableID IN InTaskTID) : QuestPacketType(InQuestPacketType), QuestTID(InQuestTID), TaskTID(InTaskTID){}
	EWRQuestPacketType QuestPacketType;
	WRTableID QuestTID;
	WRTableID TaskTID;
};


struct FWRQuestTable;

struct FWRProgressQuestDataBase
{

};

class WR_API WRQuestManager : public WRSingleton<WRQuestManager>
{
	CDECLARE_MANAGER(WRQuestManager);
	
private:
	WRQuestManager();
	virtual ~WRQuestManager();
	
public:
	//====================================================================================
	// [ 2019-8-14 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;

	virtual void EndLoadLevelProcess() override;
	//====================================================================================
public:
	bool IsCompletedQuest(const WRTableID IN InQuestTID);
	bool IsProgressQuest(const WRTableID IN InQuestTID);
	bool IsWaitQuestPacket(const WRTableID IN InQuestTID);
	bool IsWaitTaskPacket(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID);

	bool HasReadyQuest(const WRTableID IN InNPCTID, WRTableID IN InMapTID = INVALID_TABLE_TID);	// 해당 NPC에게 수락 가능한 퀘스트가 있는가?

	WRTableID GetTrackedQuestTID() { return TrackedQuestTID; }
	WRTableID GetAccpetableQuestTIDByNpcTalk(const WRTableID IN InNPCTID, WRTableID IN InMapTID = INVALID_TABLE_TID); // 해당 NPC에게 수락 가능한 퀘스트의 TID
	void GetProgressTaskByQuest(const WRTableID IN InQuestTID, TDoubleLinkedList<WRTableID>& OUT OutTaskTIDs) { ProgressQuestTaskController.GetProgressTaskByQuest(InQuestTID, OutTaskTIDs); }
	EWRQuestState::en GetQuestState(const WRTableID IN InQuestTID);

	const FWRQuestTable* FindQuestTable(const WRTableID IN InQuestTID, EWRQuestType::en IN InQuestType = EWRQuestType::en::None);
	const FWRQuestTaskTable* FindQuestTaskTable(const WRTableID IN InQuestTaskTID, EWRQuestTaskType IN InQuestTaskType = EWRQuestTaskType::None);
public:
	// 퀘스트를 수락상태로 함ㅐ
	void RequestAcceptQuest(const WRTableID IN InQuestTID);
	void OnAckAcceptQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID);
	// 퀘스트를 완료가능상태로 함
	void RequestCompletableQuest(const WRTableID IN InQuestTID);
	void OnAckCompletableQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID);
	// 퀘스트를 완료상태로 함
	void RequestCompletedQuest(const WRTableID IN InQuestTID);
	void OnAckCompletedQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID);
	// 퀘스트 테스크 완료 요청
	void RequestCompletedQuestTask(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);
	void OnAckCompletedQuestTaskReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);


	void OnInformCompletedQuestTaskReceived(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);
	void OnInformUpdateQuestTaskReceived(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID, const QUEST_TASK_VALUE_t IN InValue);
	void OnInformForcedQuestCompleteReceived(const WRTableID IN InQuestTID);
	void OnInformForcedQuestGiveUpReceived(const WRTableID IN InQuestTID);

	void OpenQuestBoard();
public:
	void OnTutorialCompleted(const WRTableID IN InTutorialTID);
	void OnQuestCompleteTriggered(const int32 IN TriggerUID);
	void OnCharacterSpawned(AWRCharacter* IN InCharacter);
	void OnCharacterUnSpawned(AWRCharacter* IN InCharacter);
	void OnQuestDirectionFinished(const FWRQuestDirectionData::EWRQuestDirectionType IN InQuestDirectionTYpe);

	void StartGame();
private:
	void ReadyQuest_Internal(const WRTableID IN InQuestTID);
	void AcceptQuest_Internal(const WRTableID IN InQuestTID);
	void CompletableQuest_Internal(const WRTableID IN InQuestTID);
	void CompletedQuest_Internal(const WRTableID IN InQuestTID);
	void GiveUpQuest_Internal(const WRTableID IN InQuestTID);
	void CompletedQuestTask_Internal(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);
	

private:
	void InitializeQuestData();
	void CheckDisableStateToReadyState();  // 준비 상태가 될수 있는 퀘스트가 있는지 검사하는 함수
	bool CheckQuestReadyCondition(const FWRQuestReadyCondition& IN InQuestReadyCondition);
	bool CheckAllTaskCompleted(const WRTableID IN InQuestTID);

	void ChangeQuestState(const WRTableID IN InQuestTID, const EWRQuestState::en IN InBeforeQuestState, const EWRQuestState::en IN InAfterQuestState);
	// 퀘스트 수락, 완료시  진행시킬 액션들
	void ProgressQuestAction(const FWRQuestAction& IN InQuestAction);

	TDoubleLinkedList<WRTableID>*  GetQuestListByQuestState(const EWRQuestState::en IN InQuestState);
private:
	TDoubleLinkedList<WRTableID> DisableQuestList;
	TDoubleLinkedList<WRTableID> ReadyQuestList;
	TDoubleLinkedList<WRTableID> AcceptQuestList;
	TDoubleLinkedList<WRTableID> CompletableQuestList;
	TDoubleLinkedList<WRTableID> CompletedQuestList;

	TDoubleLinkedList<TPair<WRTableID, WRTableID>> CompletedTaskList; // 진행중인 퀘스트중에 완료된 Task 목록 TPair<QuestTID< TaskTID>
	//TDoubleLinkedList<FWRQuestPacketData> PacketDatas;
	TDoubleLinkedList<WRTableID> WaitQuestPacketList;	// 패킷 응답 기다리는 퀘스트 목록
	TDoubleLinkedList<TPair<WRTableID, WRTableID>> WaitTaskPacketList;	// 패킷 응답 기다리는 테스크 목록
	WRProgressQuestTaskController ProgressQuestTaskController;
	WRQuestDirectionController QuestDirectionController;

	WRTableID TrackedQuestTID;
	bool bImpossibleAuto = false;	// 임시 코드, 오토로 퀘스트 수락이 안되게 하는것
};