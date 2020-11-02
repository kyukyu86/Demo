#pragma once
#include "Base/WRTableRow.h"
#include "Enum/EWRCharacter.h"
#include "WRWeaponTable.generated.h"

USTRUCT(BlueprintType)
struct FWRWeaponTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("CharData_Weapons"); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 BaseMontageTID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 BaseSubMontageTID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		TArray<int32> SkillTID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		EWRWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 WeaponLookTid;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
// 		TSoftClassPtr<class AWRVFX> PartialFxPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
		FString PartialFxPath;
};
