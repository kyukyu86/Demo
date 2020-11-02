#pragma once
#include "Base/WRTableRow.h"
#include "WRCustomTextTable.generated.h"


USTRUCT(BlueprintType)
struct FWRCustomTextTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CustomTextData_Local"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CustomText)
		FName Localkey;
};
