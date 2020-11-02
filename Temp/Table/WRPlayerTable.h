#pragma once
#include "Base/WRTableRow.h"
#include "../Network/Share/Define/Define_BluePrint.h"
#include "WRPlayerTable.generated.h"

#define DEF_BANDWIDTH_PLAYER_TID				10000

// [ 2020-8-19 : kyu ] 변경된 대역대
#define DEF_BANDWIDTH_NORMAL_PLAYER_TID			1000
#define DEF_BANDWIDTH_3RD_PLAYER_TID			10000

USTRUCT(BlueprintType)
struct FWRPlayerTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CharData_Player"); }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		float Sight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		EWRCharacterFactionType FactionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		int32 MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		int32 Atk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		int32 Def;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		float DeadBodyDisappearTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		float VoiceChatRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		TArray<EWRFSM> FSMType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		TArray<int32> BaseMontageTID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		TArray<int32> BaseSubMontageTID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		int32 DanceRange;	// 100 당 1m

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		TArray<FName> VoiceChatAnis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		FString SpawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		FString SpawnSoundPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		FString UnspawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		FString UnspawnSoundPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Player")
		FVector HMDCameraOffset;

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
};