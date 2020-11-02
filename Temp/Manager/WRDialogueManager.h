// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRUIEnum.h"
#include "Define/WRTypeDefine.h"
#include "Table/WRDialogueTable.h"

/**
*
*/

// 

class WRLoopDialogueElement
{
public:
	WRLoopDialogueElement(const TArray<WRTableID>& IN InDialogueTIDs, const float IN InLoopDelayTime);
	void Tick(const float IN InDeltaTime);
private:
	void RegisterDialogue();
private:
	TArray<WRTableID> RandomDialogueTIDs;
	float FlowTime;
	float LoopDelayTime;
};

class WR_API WRLoopDialogueRegister
{
public:
	void Tick(const float IN InDeltaTime);

	void StartLoopDialogue(const int32 IN InUID, const TArray<WRTableID>& IN InDialogueTIDs, const float IN InLoopDelayTime);
	void StopLoopDialogue(const int32 IN InUID);

private:
	TMap<int32, WRLoopDialogueElement> LoopDialogueElements;
};

struct FWRReserveDialogueData
{
	FWRReserveDialogueData(const EWRDialogueType IN InDialogueType, const WRTableID InDialogueTID, const FSimpleDelegate IN InDelegate)
		: DialogueType(InDialogueType), DialogueTID(InDialogueTID), Delegate(InDelegate) {}
	
	EWRDialogueType DialogueType;
	WRTableID DialogueTID;
	FSimpleDelegate Delegate;
};

struct FWRDialogueTable;

class WR_API WRDialogueManager : public WRSingleton<WRDialogueManager>

{
	CDECLARE_MANAGER(WRDialogueManager);
	
private:
	WRDialogueManager();
	virtual ~WRDialogueManager();
	
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
	//====================================================================================

public:
	void OpenDialogue(const EWRDialogueType IN InDialogueType, const WRTableID IN InDialogueTID, const FSimpleDelegate& IN InDelegate = nullptr);
	void StartLoopDialogue(const int32 IN InUID, const TArray<WRTableID>& IN InDialogueTIDs, const float IN InLoopDelayTime);
	void StopLoopDialogue(const int32 IN InUID);

	bool IsOpenDialogue();
	void OnDialogueFinished();

	const FWRDialogueTable* GetDialogueTable(const EWRDialogueType IN InDialogueType, const WRTableID IN InDialogueTID);
	EWRDialogueType GetOpenedDialogueType() { return OpenedDialogue.Key; }
	// �켱�������� �������� �켱������ �������̴�.
	int8 GetDialoguePriority(const EWRDialogueType IN InDialogueType);
	// �ش��Լ����� false��� A���� B�� �켱������ ���ų� �������̴�.
	bool IsBTypeLowerPriorityThanAType(const EWRDialogueType IN InAType, const EWRDialogueType IN InBType);

	void ProgressDialogueAction(const FWRDialogueAction& IN InDialogueAction, const WRTableID IN InDialogueTID);
private:
	void Clear();
	void OpenDialogue_Internal(const EWRDialogueType IN InDialogueType, const WRTableID IN InDialogueTID, const FSimpleDelegate& IN InDelegate);
private:
	TDoubleLinkedList<FWRReserveDialogueData> ReserveDialogueList;
	TPair<EWRDialogueType, WRTableID> OpenedDialogue;		// ���� �������� �ִ� Dialogue����
	FSimpleDelegate OpenedDelegate = nullptr;
	WRLoopDialogueRegister LoopDialogueRegister;
};

