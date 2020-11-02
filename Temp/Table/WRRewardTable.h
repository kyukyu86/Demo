#pragma once

#include "Base/WRTableRow.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRRewardTable.generated.h"

USTRUCT(BlueprintType)
struct FWRReward
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FWRReward)
	EWRRewardStyle Sytle;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FWRReward)
	EWRRewardType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FWRReward)
	int32 Value01;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FWRReward)
	int32 Value02;
};

USTRUCT(BlueprintType)
struct FWRRewardTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<FWRReward> Rewards;
};

USTRUCT(BlueprintType)
struct FWRDialogueRewardTable : public FWRRewardTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("Reward_Dialogue"); }

	
};


