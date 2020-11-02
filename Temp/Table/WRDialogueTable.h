#pragma once
#include "Base/WRTableRow.h"
#include "ObjectMacros.h"
#include "Enum/EWRUIEnum.h"
#include "Curves/CurveFloat.h"
#include "SoftObjectPtr.h"
#include "Sound/SoundCue.h"
#include "Engine/Texture2D.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRDialogueTable.generated.h"


USTRUCT(BlueprintType)
struct FWRDialogueAction
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueAction)
		EWRDialogueActionType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueAction)
		int32 Value01;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueAction)
		int32 Value02;
};


USTRUCT(BlueprintType)
struct FWRDialogueTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		EWRDialogueUIType DialogueUIType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		int32 DialoguePlayTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		int32 SpeakNpcTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		FName DialogueLocalkey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		FName DialogueLipsyncCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		FName VoiceSound;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		bool PlayerLookIK;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		int32 NextDialogueTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		TArray<FName> SpeechAniID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		bool RoamingPossible;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		bool Use3DSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		int32 NextDialogueSetGroupTID;

	/*DialogueActions_Type_01
		DialogueActions_Value01_01
		DialogueActions_Value02_01*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		TArray<FWRDialogueAction> DialogueActions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueData)
		bool DBPosiable;	// DB에 저장할 것인가.

	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::None; }
};


USTRUCT(BlueprintType)
struct FWRMainQuestDialogueTable : public FWRDialogueTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_MainQuest"); }


	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::MainQuest; }
};

USTRUCT(BlueprintType)
struct FWRNpcEventTalkDialogueTable : public FWRDialogueTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_NpcEventTalk"); }


	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::NpcEventTalk; }
};

USTRUCT(BlueprintType)
struct FWRFeedbackTalkDialogueTable : public FWRDialogueTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_FeedbackTalk"); }


	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::FeedbackTalk; }
};

USTRUCT(BlueprintType)
struct FWRTriggerNpcTalkDialogueTable : public FWRDialogueTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_TriggerNpcTalk"); }


	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::TriggerNpcTalk; }
};


USTRUCT(BlueprintType)
struct FWRNpcIdleSpeechDialogueTable : public FWRDialogueTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_NPCIdleSpeech"); }


	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::NpcIdleSpeech; }
};

USTRUCT(BlueprintType)
struct FWRNpcTalkDialogueTable : public FWRDialogueTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_NpcTalk"); }


	virtual EWRDialogueType GetDialogueType() const { return EWRDialogueType::NpcTalk; }
};



USTRUCT(BlueprintType)
struct FWRDialogueSetCondition
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
		EWRDialogueSetCoditionType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
		int32 Value01;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
		int32 Value02;
};


USTRUCT(BlueprintType)
struct FWRDialogueSetTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_DialogueSet"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSet)
		EWRDialogueType NextDialogueType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSet)
		int32 NextDialogueTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSet)
		FName DialogueSetIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSet)
		TArray<int32> VoiceSubCommandTIDs;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSet)
		TArray<FWRDialogueSetCondition> Conditions;;
};

USTRUCT(BlueprintType)
struct FWRDialogueSetGroupTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("DialogueData_DialogueSetGroup"); }
	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		FName DialogueSet01Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		int32 DialogueSet01Tid;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		FName DialogueSet02Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		int32 DialogueSet02Tid;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		FName DialogueSet03Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		int32 DialogueSet03Tid;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		FName DialogueSet04Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DialogueSetGroup)
		int32 DialogueSet04Tid;
};