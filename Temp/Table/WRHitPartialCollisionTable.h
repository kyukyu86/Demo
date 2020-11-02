#pragma once
#include "Base/WRTableRow.h"
#include "../Network/Share/Define/Define_BluePrint.h"
#include "WRHitPartialCollisionTable.generated.h"

USTRUCT(BlueprintType)
struct FWRHitPartialCollisionTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CharData_Collision"); }

	TArray<EWRCollisionType>	CollisionType;
	TArray<int32>				CollisionMaxValue;
	TArray<int32>				ApplyBuffTID;
	//TArray<EWRAttackHeightType>	CollisionHeightType; //현재는 서버만 사용함
};