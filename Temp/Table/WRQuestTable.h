#pragma once
#include "Base/WRTableRow.h"
#include "Enum/EWRQuestEnum.h"
#include "Enum/EWRUIEnum.h"
#include "Network/Share/Define/Define_Quest.h"
#include "ObjectMacros.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRQuestTable.generated.h"

#define DEF_QUEST_TABLE_BANDWIDTH_STANDARD 100000
#define DEF_QUEST_TABLE_BANDWIDTH_MAIN_START 100000

#define DEF_QUEST_TASK_TABLE_BANDWIDTH_STANDARD 1000000
#define DEF_QUEST_TASK_TABLE_BANDWIDTH_MOVE_START 1000000

#define DEF_QUEST_TASK_TABLE_BANDWIDTH_IO_START 3000000

#define DEF_QUEST_TASK_TABLE_BANDWIDTH_TRIGGER_START 5000000
#define DEF_QUEST_TASK_TABLE_BANDWIDTH_TUTORIAL_START 6000000



USTRUCT(BlueprintType)
struct FWRQuestAction
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestAction)
	EWRQuestActionType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestAction)
	int32 Value;
};

USTRUCT(BlueprintType)
struct FWRQuestReadyCondition
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
	EWRQuestReadyCoditionType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
	int32 Value;
};

USTRUCT(BlueprintType)
struct FWRQuestTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:

	//-- 시작 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	FName QuestNameLocalkey;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	TArray<FWRQuestReadyCondition> ReadyConditions; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	EWRQuestAcceptType AcceptType; 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	int32 AcceptTypeValue01; 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	int32 AcceptTypeValue02;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	int32 ReadyDialogueTID;

	//-- 수락 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	TArray<FWRQuestAction> AcceptActions;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	int32 AcceptDialogueTID;


	//-- 수행 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	int32 SummaryDialogueTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	EWRQuestTaskStyle TaskStyle; 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	TArray<int32> TaskTIDs;

	//-- 완료 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	EWRQuestCompleteType CompleteType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	TArray<FWRQuestAction> CompleteActions;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	int32 CompleteDialogueTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestData)
	int32 CompleteReward;

	virtual EWRQuestType::en GetQuestType() const { return EWRQuestType::en::None; }
	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::None; }

	static EWRQuestType::en GetQuestType(const WRTableID IN InQuestTID)
	{
		if (DEF_QUEST_TABLE_BANDWIDTH_MAIN_START <= InQuestTID && InQuestTID < DEF_QUEST_TABLE_BANDWIDTH_MAIN_START + DEF_QUEST_TABLE_BANDWIDTH_STANDARD)
		{
			return EWRQuestType::Main;
		}

		return EWRQuestType::None;
	}
};

USTRUCT(BlueprintType)
struct FWRMainQuestTable : public FWRQuestTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("QuestData_MainQuest"); }


	virtual EWRQuestType::en GetQuestType() const { return EWRQuestType::en::Main; }
	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::MainQuest; }
};

USTRUCT(BlueprintType)
struct FWRQuestTaskTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestTask)
	FName DiscriptionLocalkey;
	
	virtual EWRQuestTaskType GetTaskType() const { return EWRQuestTaskType::None; }

	static EWRQuestTaskType GetQuestTaskType(const WRTableID IN InQuestTaskTID)
	{
		if (DEF_QUEST_TASK_TABLE_BANDWIDTH_MOVE_START <= InQuestTaskTID && InQuestTaskTID < DEF_QUEST_TASK_TABLE_BANDWIDTH_MOVE_START + DEF_QUEST_TASK_TABLE_BANDWIDTH_STANDARD)
		{
			return EWRQuestTaskType::MovePosition;
		}
		else if (DEF_QUEST_TASK_TABLE_BANDWIDTH_IO_START <= InQuestTaskTID && InQuestTaskTID < DEF_QUEST_TASK_TABLE_BANDWIDTH_IO_START + DEF_QUEST_TASK_TABLE_BANDWIDTH_STANDARD)
		{
			return EWRQuestTaskType::ActiveIO;
		}
		else if (DEF_QUEST_TASK_TABLE_BANDWIDTH_TRIGGER_START <= InQuestTaskTID && InQuestTaskTID < DEF_QUEST_TASK_TABLE_BANDWIDTH_TRIGGER_START + DEF_QUEST_TASK_TABLE_BANDWIDTH_STANDARD)
		{
			return EWRQuestTaskType::ActiveTrigger;
		}
		else if (DEF_QUEST_TASK_TABLE_BANDWIDTH_TUTORIAL_START <= InQuestTaskTID && InQuestTaskTID < DEF_QUEST_TASK_TABLE_BANDWIDTH_TUTORIAL_START + DEF_QUEST_TASK_TABLE_BANDWIDTH_STANDARD)
		{
			return EWRQuestTaskType::ClearTutorial;
		}

		return EWRQuestTaskType::None;
	}
};

USTRUCT(BlueprintType)
struct FWRQuestMoveTaskTable : public FWRQuestTaskTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TaskData_MoveTask"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestMoveTask)
	int32 CoordinateTID;

	virtual EWRQuestTaskType GetTaskType() const { return EWRQuestTaskType::MovePosition; }
};

USTRUCT(BlueprintType)
struct FWRQuestTriggerTaskTable : public FWRQuestTaskTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TaskData_TriggerTask"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestTriggerTask)
	int32 TriggerTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestTriggerTask)
	int32 MapTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestTriggerTask)
	FVector TriggerPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestTriggerTask)
	int32 IconPositionOffset;

	virtual EWRQuestTaskType GetTaskType() const { return EWRQuestTaskType::ActiveTrigger; }
};

USTRUCT(BlueprintType)
struct FWRQuestTutorialTaskTable : public FWRQuestTaskTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TaskData_TutorialTask"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestTutorialTask)
	int32 TutorialTID;

	virtual EWRQuestTaskType GetTaskType() const { return EWRQuestTaskType::ClearTutorial; }
};

USTRUCT(BlueprintType)
struct FWRQuestActiveIOTaskTable : public FWRQuestTaskTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TaskData_ObjectTask"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestActiveIOTask)
		int32 ObjectTID;

	virtual EWRQuestTaskType GetTaskType() const { return EWRQuestTaskType::ActiveIO; }
};

USTRUCT(BlueprintType)
struct FWRQuestActiveGroupIOTaskTable : public FWRQuestTaskTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TaskData_GroupObjectTask"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestTriggerTask)
		TArray<int32> SetObjectTIDs;


	virtual EWRQuestTaskType GetTaskType() const { return EWRQuestTaskType::ActiveIO_Group; }
};