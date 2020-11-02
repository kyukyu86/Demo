#pragma once
#include "Base/WRTableRow.h"
#include "WRCoordinateTable.generated.h"

USTRUCT(BlueprintType)
struct FWRCoordinateTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CoordinateData_Coordinate"); }
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoordinateTable)
		FName NameLocalkey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoordinateTable)
		int32 MapTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoordinateTable)
		FVector Position;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoordinateTable)
		int32 Range;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CoordinateTable)
		int32 Angle;
};
