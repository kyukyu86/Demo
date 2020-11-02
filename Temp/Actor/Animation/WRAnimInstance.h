// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enum/EWRCharacter.h"
#include <Animation/AnimMontage.h>
#include <Animation/AnimInstance.h>
#include "WRAnimInstance.generated.h"

#define SYNC_HMD_HEAD_INCLUDE_PLAYER 1

/**
 *
 */

enum class EWRMontageType : uint8
{
	None,
	Attack,
	Skill,
	HitReact,
	Common,
};

enum class EWRCombatGimmickState : uint8
{
	None = 0,
	Blocking = 1 << 1,
	AvoidingLeft = 1 << 2,
	AvoidingRight = 1 << 3,
	Parrying = 1 << 4,
};
ENUM_CLASS_FLAGS(EWRCombatGimmickState);

UCLASS()
class WR_API UWRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UWRAnimInstance();


public:
	float PlayMontage(int32 InMontageID, FString InSectionName, float InRate = 1.0f);
	class AWRCharacter* GetOwnerCharacter();

	// [ 2019-11-13 : magedoga ] Update values for anim graph
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void AnimNotify_HitTiming();
	void AnimNotify_SkipAnimation();

	bool IsIncludedCombatGimmickState(EWRCombatGimmickState InState);
	void BeginCombatGimmickAnimNotifyState(EWRCombatGimmickState InState);
	void EndCombatGimmickAnimNotifyState(EWRCombatGimmickState InState);

	FString GetMontageSectionName();

public:
	UFUNCTION()
	void OnAnimMontageEnded(UAnimMontage* rtnMontage, bool bInterrupted);
	UFUNCTION()
	void OnAnimMontageBlendingOut(UAnimMontage* rtnMontage, bool bInterrupted);

private:

	bool IsIdentityTR(const FTransform& InTransform);

	void ProcessFABRIK();
	void ProcessFABRIK(AWRCharacter* InOwnerCharacter);
	void ProcessLookIK(AWRCharacter* InOwnerCharacter);
	void ProcessLipsync(AWRCharacter* InOwnerCharacter);
#if SYNC_HMD_HEAD_INCLUDE_PLAYER
	void ProcessHMDHeadSync(class AWRCharacterPlayer* InOwnerPlayerCharacter);
#else // SYNC_HMD_HEAD_INCLUDE_PLAYER
	void ProcessHMDHeadSync(class AWRCharacter* InOwnerCharacter);
#endif // SYNC_HMD_HEAD_INCLUDE_PLAYER

	void UpdateMoveValue();

public:
	void SetLadderIdle(const bool IN InSet) { bIsLadder = InSet; }
	void SetLadderHand(EWRLadderHand IN InHand) { LadderHand = InHand; }
	void SetLayeredBlendPerBone(bool bFlag);
	void ResetHMDRotation();
	void SetIdleType(const EWRIdleType IN InType) { IdleType = InType; }
	EWRIdleType GetIdleType() { return IdleType; }

	void EnableAIMoveFlag();
	void DisableAIMoveFlag();

	void SetSkelMeshToFABRIK(USkeletalMeshComponent* InMesh);

private:
	EWRCombatGimmickState CombatGimmickState = EWRCombatGimmickState::None;

	USkeletalMeshComponent* SkeletalMeshComponentToFABRIK = nullptr;
	TArray<FName> arraySocketNameToFABRIK;	//현재는 사용하지 않음

	//====================================================================================
	// [ 2019-11-13 : magedoga ] FABRIK values for interaction
	UPROPERTY(BlueprintReadOnly, Category = "FABRIK", meta = (AllowPrivateAccess = "true"))
		FTransform RHandTransform_FABRIK = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly, Category = "FABRIK", meta = (AllowPrivateAccess = "true"))
		float RHandAlpha_FABRIK = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "FABRIK", meta = (AllowPrivateAccess = "true"))
		FRotator RHandAngle = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "FABRIK", meta = (AllowPrivateAccess = "true"))
		FTransform LHandTransform_FABRIK = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly, Category = "FABRIK", meta = (AllowPrivateAccess = "true"))
		float LHandAlpha_FABRIK = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "FABRIK", meta = (AllowPrivateAccess = "true"))
		FRotator LHandAngle = FRotator::ZeroRotator;

	//====================================================================================

	// [ 2019-11-13 : magedoga ] Look IK Value
	UPROPERTY(BlueprintReadOnly, Category = "Look IK", meta = (AllowPrivateAccess = "true"))
		FRotator LookAngle = FRotator::ZeroRotator;

	FRotator DestAngle = FRotator::ZeroRotator;

	//====================================================================================

	// [ 2019-11-20 : magedoga ] Lipsync
	UPROPERTY(BlueprintReadOnly, Category = "Lipsync", meta = (AllowPrivateAccess = "true"))
		float LipsyncAlpha = 0.0f;

	//====================================================================================
	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bLayeredBlend = false;

	// 	UPROPERTY(BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	// 		bool bIsAttack = false;

		//====================================================================================
	UPROPERTY(BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true"))
		bool bIsLadder = false;
	UPROPERTY(BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true"))
		EWRLadderHand LadderHand = EWRLadderHand::None;

	//====================================================================================

	// [ 2019-11-20 : dlwlgus9125 ] BlendSpace
	UPROPERTY(BlueprintReadOnly, Category = "BlendSpace", meta = (AllowPrivateAccess = "true"))
		float ForwardDelta = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlendSpace", meta = (AllowPrivateAccess = "true"))
		float RightDelta = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "BlendSpace", meta = (AllowPrivateAccess = "true"))
		bool bIsAIMoveTo = false;

	// [ 2020-1-14 : magedoga ] HMD & Head Sync
	// [ 2020-3-9 : magedoga ] SYNC_HMD_HEAD_INCLUDE_PLAYER (Location은 삭제)
	//UPROPERTY(BlueprintReadOnly, Category = "HMD Sync", meta = (AllowPrivateAccess = "true"))
	//FVector HMDLocation;
	//====================================================================================

	UPROPERTY(BlueprintReadOnly, Category = "HMD Sync", meta = (AllowPrivateAccess = "true"))
		FRotator HMDRotation;


	UPROPERTY(BlueprintReadOnly, Category = "HMD Sync", meta = (AllowPrivateAccess = "true"))
		float HMDSyncAlpha = 0.0f;

	//====================================================================================
	// [ 2020-7-23 : kyu ] Idle Type
	UPROPERTY(BlueprintReadOnly, Category = "Idle Type", meta = (AllowPrivateAccess = "true"))
		EWRIdleType IdleType = EWRIdleType::None;

	//====================================================================================

// [ 2019-11-20 : dlwlgus9125 ] BlendSpace
private:
	float MaxDelta = 1.f;
	float FinalMaxDelta = 1.f;
	//====================================================================================
public:
	FORCEINLINE void SetMaxDelta(const float InMaxDelta) { FinalMaxDelta = InMaxDelta; }
	void SetForwardDeltaAndRightDelta(const float InForwardDelta, const float InRightDelta);

};


// For Header
DECLARE_STATS_GROUP(TEXT("WRAnimInstance"), STATGROUP_WRAnimInstance, STATCAT_Advanced);

// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRAnimInstance, Display, All);