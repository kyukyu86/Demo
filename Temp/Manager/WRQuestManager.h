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
	// [ 2019-8-14 : hansang88 ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;

	virtual void EndLoadLevelProcess() override;
	//====================================================================================
public:
	bool IsCompletedQuest(const WRTableID IN InQuestTID);
	bool IsProgressQuest(const WRTableID IN InQuestTID);
	bool IsWaitQuestPacket(const WRTableID IN InQuestTID);
	bool IsWaitTaskPacket(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID);

	bool HasReadyQuest(const WRTableID IN InNPCTID, WRTableID IN InMapTID = INVALID_TABLE_TID);	// �ش� NPC���� ���� ������ ����Ʈ�� �ִ°�?

	WRTableID GetTrackedQuestTID() { return TrackedQuestTID; }
	WRTableID GetAccpetableQuestTIDByNpcTalk(const WRTableID IN InNPCTID, WRTableID IN InMapTID = INVALID_TABLE_TID); // �ش� NPC���� ���� ������ ����Ʈ�� TID
	void GetProgressTaskByQuest(const WRTableID IN InQuestTID, TDoubleLinkedList<WRTableID>& OUT OutTaskTIDs) { ProgressQuestTaskController.GetProgressTaskByQuest(InQuestTID, OutTaskTIDs); }
	EWRQuestState::en GetQuestState(const WRTableID IN InQuestTID);

	const FWRQuestTable* FindQuestTable(const WRTableID IN InQuestTID, EWRQuestType::en IN InQuestType = EWRQuestType::en::None);
	const FWRQuestTaskTable* FindQuestTaskTable(const WRTableID IN InQuestTaskTID, EWRQuestTaskType IN InQuestTaskType = EWRQuestTaskType::None);
public:
	// ����Ʈ�� �������·� �Ԥ�
	void RequestAcceptQuest(const WRTableID IN InQuestTID);
	void OnAckAcceptQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID);
	// ����Ʈ�� �Ϸᰡ�ɻ��·� ��
	void RequestCompletableQuest(const WRTableID IN InQuestTID);
	void OnAckCompletableQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID);
	// ����Ʈ�� �Ϸ���·� ��
	void RequestCompletedQuest(const WRTableID IN InQuestTID);
	void OnAckCompletedQuestReceived(const bool IN bInSuccess, const WRTableID IN InQuestTID);
	// ����Ʈ �׽�ũ �Ϸ� ��û
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
	void CheckDisableStateToReadyState();  // �غ� ���°� �ɼ� �ִ� ����Ʈ�� �ִ��� �˻��ϴ� �Լ�
	bool CheckQuestReadyCondition(const FWRQuestReadyCondition& IN InQuestReadyCondition);
	bool CheckAllTaskCompleted(const WRTableID IN InQuestTID);

	void ChangeQuestState(const WRTableID IN InQuestTID, const EWRQuestState::en IN InBeforeQuestState, const EWRQuestState::en IN InAfterQuestState);
	// ����Ʈ ����, �Ϸ��  �����ų �׼ǵ�
	void ProgressQuestAction(const FWRQuestAction& IN InQuestAction);

	TDoubleLinkedList<WRTableID>*  GetQuestListByQuestState(const EWRQuestState::en IN InQuestState);
private:
	TDoubleLinkedList<WRTableID> DisableQuestList;
	TDoubleLinkedList<WRTableID> ReadyQuestList;
	TDoubleLinkedList<WRTableID> AcceptQuestList;
	TDoubleLinkedList<WRTableID> CompletableQuestList;
	TDoubleLinkedList<WRTableID> CompletedQuestList;

	TDoubleLinkedList<TPair<WRTableID, WRTableID>> CompletedTaskList; // �������� ����Ʈ�߿� �Ϸ�� Task ��� TPair<QuestTID< TaskTID>
	//TDoubleLinkedList<FWRQuestPacketData> PacketDatas;
	TDoubleLinkedList<WRTableID> WaitQuestPacketList;	// ��Ŷ ���� ��ٸ��� ����Ʈ ���
	TDoubleLinkedList<TPair<WRTableID, WRTableID>> WaitTaskPacketList;	// ��Ŷ ���� ��ٸ��� �׽�ũ ���
	WRProgressQuestTaskController ProgressQuestTaskController;
	WRQuestDirectionController QuestDirectionController;

	WRTableID TrackedQuestTID;
	bool bImpossibleAuto = false;	// �ӽ� �ڵ�, ����� ����Ʈ ������ �ȵǰ� �ϴ°�
};