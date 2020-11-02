#pragma once
#include "Base/WRTableRow.h"
#include "Enum/EWRCharacter.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRSkillTable.generated.h"

USTRUCT(BlueprintType)
struct FWRSkillTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		int32 SkillGroupMontageTID;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		bool ComboSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		int32 Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float Impulse;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		EWRAttackType AttackType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		EWRSkillType SkillType;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		EWREquipPart EquipType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		EWRTargetingType TargetingType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float Range;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float Angle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		bool GuardBreak;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		int32 OwnerDynamicMovementTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		int32 TargetDynamicMovementTID;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
// 		EWRCCType CCType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float CCTime;
	   
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
	//	TSoftClassPtr<class AWRVFX> HitFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
		FString HitFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
		int32 ScreenEffectTM = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		EWRDisplayType ScreenEffectType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float ProjectileVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float CoolTm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float AttackPauseAnimTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float HitPauseAnimTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		EWRFSM FSMType;	//해당 스킬이 사용할 FSMType지정



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		EWRInvokeSkillTargetType TargetType;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
// 		EWRSOPosType SoPosType;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
	//	TSoftClassPtr<class AWRVFX> FxName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
		FString FxName;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
// 		EWRAttackHeightType AdjustHeightType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
		TArray<EWRAttackHeightType> AdjustHeightType;

//	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
//		int32 CollisionValue;
	
public:
	virtual void GenerateCustomData() override
	{
		for (int32 Index = 0; Index < AdjustHeightType.Num(); ++Index)
		{
			AttackHeightFlag |= AdjustHeightType[Index];
		}
	}

	bool IsInHeightFlag(EWRAttackHeightType InState)
	{
		if ((AttackHeightFlag & InState) != EWRAttackHeightType::None)
		{
			return true;
		}

		return false;
	}

	bool IsNoneHeightFlag()
	{
		if (AdjustHeightType.Num() != 0)
		{
			if (AdjustHeightType[0] == EWRAttackHeightType::None)
			{
				return true;
			}
		}

		return false;
	}

	EWRAttackHeightType GetAttackHeightFlag()
	{
		return AttackHeightFlag;
	}

	float GetCoolTime()
	{
		if (0 < CoolTm)
		{
			return CoolTm / 1000.0f;
		}

		return 0.0f;
	}

private:
	EWRAttackHeightType AttackHeightFlag = EWRAttackHeightType::None;
};

USTRUCT(BlueprintType)
struct FWRPlayerSkillTable : public FWRSkillTable
{
	GENERATED_USTRUCT_BODY()

public:
	static FString GetFileName() { return TEXT("SkillPlayer_Atk"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		EWRApplyReactionType BuffApplyType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		int32 BuffTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		int32 TargetBuffTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		bool CondUse;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		int32 CondTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		int32 CondParam;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		int32 CondValue;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		EWRInput InputKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		int32 SkillGroupSubMontageTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = playerskill)
		int32 PrecedingSkillTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		int32 MultiCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = skill)
		float MinRange;
};

USTRUCT(BlueprintType)
struct FWREnpcSkillTable : public FWRSkillTable
{
	GENERATED_USTRUCT_BODY()

public:
	static FString GetFileName() { return TEXT("SkillEnpc_Atk"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = skill)
		int32 HitMoveLimitTM = 0;
};
