#pragma once

#include "Base/WRTableRow.h"
#include "../Enum/EWRCharacter.h"
#include <Sound/SoundCue.h>
#include "Network/Share/Define/Define_Actor.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WREnpcTable.generated.h"


UENUM(BlueprintType)
enum class EWREventAction : uint8
{
	None,
	PlayAni,					// 애니 재생
	ExcuteTrigger,				// 트리거 동작
	MAX
};

USTRUCT(BlueprintType)
struct FWREventAction
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FWREventAction)
		EWREventAction Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FWREventAction)
		int32 Value01;
};

UENUM(BlueprintType)
enum class EWREventActionConditionType : uint8
{
	None,
	HaveItem,
	MAX
};

USTRUCT(BlueprintType)
struct FWREventActionCondition
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
		EWREventActionConditionType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
		int32 Value01;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = QuestReadyCondition)
		int32 Value02;
};


// NPCBaseTable의 하위 Table끼리의 TID는 서로 Unique하며 이를 위해 대역폭을 나눔(아래는 그것을 위한 define)
#define DEF_STANDARD_VALUE_NPC_TID_BANDWIDTH	10000000				// 대역을 나누기 위한 기준 값
#define DEF_START_VALUE_FOR_ENPC_TID_BANDWIDTH	10000000				// ENPC TID Bandwidth의 시작 값
#define DEF_START_VALUE_FOR_NPC_TID_BANDWIDTH	20000000				// NPC TID Bandwidth의 시작 값
#define DEF_START_VALUE_FOR_SO_TID_BANDWIDTH	80000000				// NPC TID Bandwidth의 시작 값

USTRUCT(BlueprintType)
struct FWRNPCBaseTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
	// Enum BaseType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float SightRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FName FirstAttackType;		// 추후 Enum 으로 변경

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 BattleRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FName NameLocalkey;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
// 		TSoftClassPtr<class AWRCharacter> BlueprintID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString BlueprintID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float Range;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float Scale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float RunSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 AwayDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 MaxHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 Atk;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 Def;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 ReactType;	// 추후 Enum 으로 변경

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float MinDelayTimeRoaming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float MaxDelayTimeRoaming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float RoamingRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 BaseMontageTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		TArray<int32> SkillTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		EWRMoveAnimType MoveAnimType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		float TargetSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		EWRCharacterFactionType FactionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		int32 CollisionTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPCBaseTable)
		EWRNpcCreateType createType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString SpawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString SpawnSoundPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString UnspawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString UnspawnSoundPath;

	// Hook

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hook)
		EWRHookType HookType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hook)
		FString HookKeyIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hook)
		FString HookKeyDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hook)
		FString HookKeyActionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hook)
		FString HookKeyActionDescription;


	static EWRCharacterType GetNPCType(const IN WRTableID InWRTableID) 
	{
		if (InWRTableID < DEF_START_VALUE_FOR_ENPC_TID_BANDWIDTH || DEF_START_VALUE_FOR_NPC_TID_BANDWIDTH + DEF_STANDARD_VALUE_NPC_TID_BANDWIDTH < InWRTableID)
		{
			ensureMsgf(false, TEXT("Entered TID is Out of range"));
		}

		if (DEF_START_VALUE_FOR_ENPC_TID_BANDWIDTH <= InWRTableID && InWRTableID < DEF_START_VALUE_FOR_ENPC_TID_BANDWIDTH + DEF_STANDARD_VALUE_NPC_TID_BANDWIDTH)
		{
			return EWRCharacterType::ENPC;
		}

		return EWRCharacterType::NPC;
	}

	// DeadEffect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DeadEffect)
	EWRDeadEffectType DeadEffectType = EWRDeadEffectType::ragdoll;

	// DeadEffect
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DeadEffect)
		//TArray<TSoftObjectPtr<class UAnimationAsset>> DeadAnimations;

	// DeadEffect
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DeadEffect)
// 		TSoftObjectPtr<class UMaterialInterface> SliceCapMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DeadEffect)
		FString SliceCapMaterial;

};


USTRUCT(BlueprintType)
struct FWRNpcTable : public FWRNPCBaseTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("NpcData_Npc"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCTable)
		float LookIKDist;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCTable)
		bool Rotatable;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCTable)
		int32 IdleSpeechRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCTable)
		TArray<int32> IdleSpeechDialogue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCTable)
		float NpcTalkViewRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCTable)
		float NpcTalkRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCTable)
		int32 NpcTalkDialogueTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FName HeadBoneName;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
	//	TAssetPtr<USoundCue> DeadSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString DeadSound;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
	//	TSoftClassPtr<class AWRVFX> DMGFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString DMGFX;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NPCBaseTable)
	//	TSoftObjectPtr<class UMaterialInterface> bloodstainPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NPCBaseTable)
		FString bloodstainPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPCTable)
		EWRNpcJobType JobType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPCTable)
		bool ShowSocialMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPCTable)
		TArray<FWREventActionCondition> EventActionConditions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPCTable)
		TArray<FWREventAction> EventActions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPCTable)
		float EventActionRange;

	static EWRCharacterType GetNPCType() { return EWRCharacterType::NPC; }
};

USTRUCT(BlueprintType)
struct FWREnpcTable : public FWRNPCBaseTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("EnpcData_Field"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		int32 FightingPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		int32 Exp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		int32 Grade;		// 추후 Enum 으로 변경

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		int32 ReactCumulativeCount;	

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
	//	TAssetPtr<USoundCue> DeadSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString DeadSound;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
	//	TSoftClassPtr<class AWRVFX> DMGFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPCBaseTable)
		FString DMGFX;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NPCBaseTable)
	//	TSoftObjectPtr<class UMaterialInterface> bloodstainPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NPCBaseTable)
		FString bloodstainPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ENPCTable)
		EWRNpcJobType JobType;

	static EWRCharacterType GetNPCType() { return EWRCharacterType::ENPC; }
};

USTRUCT(BlueprintType)
struct FWRSOTable : public FWRNPCBaseTable
{
	GENERATED_USTRUCT_BODY()

public:
	static FString GetFileName() { return TEXT("SkillObject_SO"); }
};