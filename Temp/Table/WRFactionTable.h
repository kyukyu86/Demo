#pragma once
#include "Base/WRTableRow.h"
#include "Enum/EWRCharacter.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Utility/WREnumUtility.h"
#include "WRFactionTable.generated.h"

USTRUCT(BlueprintType)
struct FWRFactionTable: public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("FactionData_Faction"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
		EWRCharacterFactionType FactionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
		EWRFactionRelation FactionA;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
		EWRFactionRelation FactionB;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
		EWRFactionRelation FactionC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
		EWRFactionRelation FactionD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
		EWRFactionRelation FactionE;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
// 		EWRFactionRelation FactionF;
// 
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = faction)
// 		EWRFactionRelation FactionG;

public:
	TMap<EWRCharacterFactionType, EWRFactionRelation> mapFactionRelation;


public:
	virtual void GenerateCustomData() override
	{
		mapFactionRelation.Add(EWRCharacterFactionType::FactionA, FactionA);
		mapFactionRelation.Add(EWRCharacterFactionType::FactionB, FactionB);
		mapFactionRelation.Add(EWRCharacterFactionType::FactionC, FactionC);
		mapFactionRelation.Add(EWRCharacterFactionType::FactionD, FactionD);
		mapFactionRelation.Add(EWRCharacterFactionType::FactionE, FactionE);
// 		mapFactionRelation.Add(EWRCharacterFactionType::FactionF, FactionF);
// 		mapFactionRelation.Add(EWRCharacterFactionType::FactionG, FactionG);
	}

	EWRFactionRelation GetFactionRelation(EWRCharacterFactionType InFactionType)
	{
		return mapFactionRelation.FindRef(InFactionType);
	}

	bool IsValidFactionTypeOfTable(EWRCharacterFactionType InFactionType)
	{
		return FactionType == InFactionType;
	}
};