// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "Define/WRTypeDefine.h"
#include "Enum/EWRUIEnum.h"
#include "Network/Share/Define/Define_BluePrint.h"

struct FWRQuestDirectionData
{
	enum EWRQuestDirectionType { None, Dialogue, QuestBoard };
	FWRQuestDirectionData() {}
	FWRQuestDirectionData(const EWRQuestDirectionType IN InQuestDirectionType, const bool IN bInWaitDirectionToFinish)
		: Type(InQuestDirectionType), bWaitDirectionToFinish(bInWaitDirectionToFinish) {}	
	EWRQuestDirectionType Type;
	bool bWaitDirectionToFinish;	// 해당 연출이 완료되길 기다려야 하는가?
};

struct FWRDialogueDirectionData : public FWRQuestDirectionData
{
	FWRDialogueDirectionData() {}
	FWRDialogueDirectionData(const FWRQuestDirectionData::EWRQuestDirectionType IN InQuestDirectionType, const EWRDialogueType IN InDialogueType, const WRTableID IN InDialogueTID)
		: FWRQuestDirectionData(InQuestDirectionType, true), DialogueType(InDialogueType), DialogueTID(InDialogueTID) {}
	EWRDialogueType DialogueType;
	WRTableID DialogueTID;
};

class WRQuestDirectionController
{
public:
	WRQuestDirectionController();
	~WRQuestDirectionController();
public:
	void OnShutdown();
	void OnLoadLevelStart();
	void OnLoadLevelProcessEnded();
public:
	void AddQuestDirectionData(const FWRQuestDirectionData& IN InQuestDirectionData);
	void AddQuestDirectionData(const FWRDialogueDirectionData& IN InDialogueDirectionData);

	void OnQuestDirectionFinished(const FWRQuestDirectionData::EWRQuestDirectionType IN InQuestDirectionType);
	void ProgressQuestDirection(const FWRQuestDirectionData* IN InQuestDirectionData);
private:
	void Clear();

	bool ProgressDialogueDirection(const FWRQuestDirectionData* IN InQuestDirectionData);		// 다이얼로그 연출
	bool ProgressQuestBoardDirection(const FWRQuestDirectionData* IN InQuestDirectionData);		// 퀘스트 보드 연출

	void DequeueQuestDirectionData();
private:
	TQueue<FWRQuestDirectionData*> QuestDirectionDataQueue;
	FWRQuestDirectionData::EWRQuestDirectionType  WaitQuestDirectionType = FWRQuestDirectionData::EWRQuestDirectionType::None;
};
