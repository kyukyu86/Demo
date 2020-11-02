#pragma once

#include "Base/WRTableRow.h"
#include "Enum/EWRCharacter.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRTransportTable.generated.h"



USTRUCT(BlueprintType)
struct FWRElevatorTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TransportData_Elevator"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FString NameLocalKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 DoorOpenTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 WaitingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
		FString MainBPPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 DoorCloseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 FloorCount;

	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FVector Position1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 Angle1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 DoorIndex1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FVector Position2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 Angle2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 DoorIndex2;
			
};

USTRUCT(BlueprintType)
struct FWRFloatingBoardTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TransportData_FloatingBoard"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FString NameLocalKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		EWRVehicleMoveType MoveType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 WaitingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
		FString MainBPPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 FloorCount;

	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FVector Position1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 Angle1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FVector Position2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 Angle2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FVector Position3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 Angle3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FVector Position4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 Angle4;

};

USTRUCT(BlueprintType)
struct FWRTardisTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("TransportData_Tardis"); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		FString NameLocalKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 DoorOpenTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 WaitingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 DoorCloseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
		FString MainBPPath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Elevator)
		int32 ArrivalMapTID;

};