#pragma once
#include "Base/WRTableRow.h"
#include "WRVehicleTable.generated.h"

USTRUCT(BlueprintType)
struct FWRVehicleTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VehicleData_Vehicle"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		FName NameLocalkey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		FString BlueprinTid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		int32 MaxRideNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HookInformation)
		int32 MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
		FString SpawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
		FString SpawnSoundPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
		FString UnspawnEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicle)
		FString UnspawnSoundPath;
};