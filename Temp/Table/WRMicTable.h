#pragma once
#include "Base/WRTableRow.h"
#include "WRMicTable.generated.h"

USTRUCT(BlueprintType)
struct FWRMicTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("SocialData_Mic"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		int32 RandomAniTimeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		int32 RandomAniTimeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		TArray<int32> RandomAniTID;
};