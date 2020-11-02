#pragma once
#include "Base/WRTableRow.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "ObjectMacros.h"
#include "WRCurrencyTable.generated.h"

USTRUCT(BlueprintType)
struct FWRCurrencyTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CurrencyData_currency"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CurrencyData)
		EWRCurrencyType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CurrencyData)
		EWRCurrencyCategory Category;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CurrencyData)
		int32 LimitValue;		// CURRENCY_t 자료형이다.

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CurrencyData)
		FName NameLocalkey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CurrencyData)
		FName DescriptionLocalkey;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CurrencyData)
		FName IconPath;
};
