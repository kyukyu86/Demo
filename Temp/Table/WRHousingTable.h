#pragma once
#include "Base/WRTableRow.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "ObjectMacros.h"
#include "WRHousingTable.generated.h"


USTRUCT(BlueprintType)
struct FWRHousingSiteTable : public FWRTableRow		// 토지
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("HousingData_Site"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		int32 SizeX;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		int32 SizeY;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		FName MainBPPath;
};

//====================================================================================
//	Housing
//====================================================================================

USTRUCT(BlueprintType)
struct FWRHousingBuildingBaseTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		FName NameLocalkey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		FName IconPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		FName MainBPPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		int32 PurchasePrice;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		int32 SellingPrice;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		int32 BuildTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		TArray<int32> PaperListTIDs;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		TArray<int32> PaintListTIDs;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		TArray<int32> YardListTIDs;
};

USTRUCT(BlueprintType)
struct FWRHousingHouseTable : public FWRHousingBuildingBaseTable		// 주택
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("HousingData_House"); }

};

//====================================================================================
//	Housing Interior
//====================================================================================

USTRUCT(BlueprintType)
struct FWRHousingInteriorBaseTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		FName NameLocalkey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		FName IconPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		int32 PurchasePrice;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HousingData)
		FName MaterialPath;

};

USTRUCT(BlueprintType)
struct FWRHousingPaintTable : public FWRHousingInteriorBaseTable		// 외벽 텍스쳐
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("HousingData_Paint"); }

};

USTRUCT(BlueprintType)
struct FWRHousingPaperTable : public FWRHousingInteriorBaseTable		// 벽지 텍스쳐
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("HousingData_Paper"); }

};

USTRUCT(BlueprintType)
struct FWRHousingYardTable : public FWRHousingInteriorBaseTable			// 마당 텍스쳐
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("HousingData_Yard"); }

};
