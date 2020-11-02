#pragma once
#include "Base/WRTableRow.h"
#include "Enum/EWRCharacter.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WREffectPathTable.generated.h"

USTRUCT(BlueprintType)
struct FWRFXPathTable: public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("FXPath_FXPath"); }

// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
// 		TSoftClassPtr<class AWRVFX> FXPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
		FString FXPath;
};