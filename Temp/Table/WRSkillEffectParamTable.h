#pragma once
#include "Base/WRTableRow.h"
#include "Enum/EWRCharacter.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRSkillEffectParamTable.generated.h"


USTRUCT(BlueprintType)
struct FWRBuffParam
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = buffparam)
		EWRParamType ParamType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = buffparam)
		int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = buffparam)
		int32 OverTime;
};

USTRUCT(BlueprintType)
struct FWRBuffTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("SkillEffectParam_Buff"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		EWRBuffType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		int32 PriorityTid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		int32 PriorityLv;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		bool Instance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		FName GroupName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		TArray<FWRBuffParam> Effect;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		int32 Stack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		int32 StackChangeTid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		float ApplyRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = buff)
		int32 RemainTm;
};
