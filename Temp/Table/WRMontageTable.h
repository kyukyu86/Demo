#pragma once
#include "Base/WRTableRow.h"
#include "WRMontageTable.generated.h"

static FString MontageFileName("CharData_Montage");

USTRUCT(BlueprintType)
struct FWRMontageTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return MontageFileName; }

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
// 		FString Path;

//  	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Montage")
//  		TSoftObjectPtr<class UAnimMontage> Path;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharData_Montage")
		FString Path;
};