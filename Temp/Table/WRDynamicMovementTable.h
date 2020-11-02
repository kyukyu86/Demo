#pragma once
#include "Enum/EWRCharacter.h"
#include "Base/WRTableRow.h"
#include "WRDynamicMovementTable.generated.h"


USTRUCT(BlueprintType)
struct FWRDynamicMovementTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()

public:
	static FString GetFileName() { return TEXT("SkillPlayer_DynamicMove"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DMove)
		EWRDynamicMovementType DynamicMovementType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DMove)
		float Distance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DMove)
		float Height;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DMove)
		float Velocity;
};
