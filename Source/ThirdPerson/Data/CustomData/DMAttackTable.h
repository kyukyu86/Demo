// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../../Enum/DMActorEnum.h"
#include "DMAttackTable.generated.h"

#define DEF_ATTACK_TABLE_PATH TEXT("/Game/Data/Battle/AttackTable.AttackTable")
#define DEF_INVALID_TABLE_ID -1


USTRUCT(BlueprintType)
struct FDMAttackSequence
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDMInput NeedInputKey = EDMInput::MouseLButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EInputEvent> NeedInputEvent = EInputEvent::IE_Pressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Montage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SectionName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDMCombatInputType InputType = EDMCombatInputType::Immediately;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InputHoldMaxTime = 0.f;
};


USTRUCT(BlueprintType)
struct FDMAttackTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDMAttackSequence Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> LinkedAttackRowName;


	EDMFSMType FSMType = EDMFSMType::AttackNormal;

public:
	static bool IsValidInput(FDMAttackTable* IN InTable, const EDMInput IN InInputType, const EInputEvent IN InEventType);
	static FDMAttackTable* GetValidLinkedAttackTable(FDMAttackTable* IN InTable, const EDMInput IN InInputType, const EInputEvent IN InEventType);
};
