// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRCharacter.h"
#include "WRPlayerRotator.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Manager/WRObjectInteractionManager.h"
#include "WRCharacterPlayer.generated.h"

/**
 *
 */
UCLASS()
class WR_API AWRCharacterPlayer : public AWRCharacter
{
	GENERATED_UCLASS_BODY()

public:

	// [ 2019-7-19 : magedoga ] Will be called after spawn from world
	virtual void OnPostSpawn() override;
	// [ 2019-7-19 : magedoga ] Will be called before actor destroy from world
	virtual void OnPreDestroy() override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual bool AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType = EWRActionType::en::RUN);
	virtual void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);


protected:
	virtual void BuildCustomComponents() override;

public:
	//FSM for me
	virtual void BeginFSMAttack(const WRFSMInfo& InFSM) override;
	virtual void TickFSMAttack(float InDeltaTime) override;
	virtual void EndFSMAttack(const WRFSMInfo& InFSM) override;

	virtual void SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart EquipPart, ECollisionEnabled::Type InType) override;
	virtual void SetWeaponCollisionBasedOnSide(EWREquipPart InPart, ECollisionEnabled::Type InType) override;

	UFUNCTION(BlueprintCallable)
		virtual void OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintCallable)
		virtual void OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION(BlueprintCallable)
		virtual void OnEndOverlapHittingCapsule(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	//====================================================================================
	// Tick Process	
	void TickAddMovementInput(float DeltaTime);

	//====================================================================================
	// Packet Process	
	virtual void OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform) override;
	virtual void OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform) override;
	virtual void OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform) override;
	virtual void OnInformHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform) override;
	void OnInformPlayerMove(const pt_Game_User::Inf_PlayerMove_G2U& InInform);
	void OnInformPlayerMoveStop(const pt_Game_User::Inf_PlayerStop_G2U& InInform);
	void OnInformPlayerMoveInVehicle(const pt_Game_Vehicle::Inf_PlayerMoveInTransport_G2U& InInform);
	void OnInformPlayerStopInVehicle(const pt_Game_Vehicle::Inf_PlayerStopInTransport_G2U& InInform);
	void OnInformPlayerMoveInMW(const pt_Game_Vehicle::Inf_PlayerMoveInMW_G2U& InInform);
	void OnInformPlayerStopInMW(const pt_Game_Vehicle::Inf_PlayerStopInMW_G2U& InInform);
	void OnInformSyncHMD(const pt_Game_User::Inf_SyncHMD_G2U& InInform);




	//====================================================================================
	// [ 2019-10-8 : kyu ] Hook
public:
	virtual const EWRHookType GetHookType() override;
	virtual bool IsHookableState() override;
	virtual void OnHookThrow() override;	// 걸기
	virtual void OnHookReturn() override;	// 회수인데, 회수 동작은 없을것.
	virtual void OnHookEventToCaster() override;
	virtual void OnConnectCableEvent(EWRConnectCableEvent InEventType) override;
	void CutOffHookChain(const bool bInImmediately = false, AWRCharacter* IN InTarget = nullptr, const bool IN InByMe = true, const bool IN InIdle = false);
	FORCEINLINE void SetHookPart(const EWRCollisionType IN InPart) { eHookPart = InPart; }
	FORCEINLINE EWRCollisionType GetHookPart() { return eHookPart; }

	void HitHook();

protected:
	void UpdateHookMoving();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HookMovingLimitDist = 100.0f;
	FVector vStartHookLocation;
	EWRCollisionType eHookPart = EWRCollisionType::None;

	//====================================================================================


	// [ 2019-11-12 : magedoga ] 임시코드 : Interaction 관련 FABRIK
public:
	// [ 2019-11-14 : magedoga ] May be disable FABRIK if you set identity
	void SetTransformForFABRIK(const FTransform& InRHand = FTransform::Identity, const FTransform& InLHand = FTransform::Identity)
	{
		RHandTransform_FABRIK = InRHand;
		LHandTransform_FABRIK = InLHand;
	}
	void GetTransformForFABRIK(FTransform& OutRHand, FTransform& OutLHand)
	{
		OutRHand = RHandTransform_FABRIK;
		OutLHand = LHandTransform_FABRIK;
	}

protected:
	FTransform RHandTransform_FABRIK;
	FTransform LHandTransform_FABRIK;

	// [ 2020-3-9 : magedoga ] Head & HMD Sync
public:
	void SetUsingSyncHMDHead(const bool bInEnable) { bSyncHMDHead = bInEnable; }
	bool IsUsingSyncHMDHead() const { return bSyncHMDHead; }

	void SetHMDRotationToUpdate(const FRotator& InRotation) { HMDRotationToUpdate = InRotation; }
	FRotator GetHMDRotationToUpdate() const { return HMDRotationToUpdate; }

	virtual bool TickSyncHMD(float InDeltaTime);
	virtual bool IsShowing3rdCharacter() const { return true; }

protected:
	bool bSyncHMDHead = false;
	FRotator HMDRotationToUpdate = FRotator::ZeroRotator;

private:
	FRotator LastHMDRotation = FRotator::ZeroRotator;
	FRotator GoalHMDRotation = FRotator::ZeroRotator;


	//====================================================================================
	// Ladder
public:
	float GetLadderMoveTime() { return LadderMoveTime; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LadderMoveTime = 1.0f; // 사실 애니시간에 맞추는게 나음

	//====================================================================================
	// Move
public:
	void SetForceStop();
	bool IsMoving();
	void ResetMovementValue2Sync();

	//====================================================================================
	// Transportation
public:
	FORCEINLINE void SetEnvVehicleAID(const AID_t IN InAID) { EnvVehicleAID = InAID; }
	FORCEINLINE AID_t GetEnvVehicleAID() { return EnvVehicleAID; }
	FORCEINLINE FVector GetEnvVehicleOffset() { return EnvVehicleOffset; }
	FORCEINLINE float GetEnvVehicleOffsetAngle() { return EnvVehicleOffsetAngle; }
	void SetEnvVehicleTransformOffset(const bool IN InSet, const FVector IN InOffset = FVector::ZeroVector, const float IN InAngle = 0.f);
	void UpdateEnvVehicleLocationOffset();
	virtual float GetEnvVehicleMoveSpeed() override;
	virtual FVector GetEnvVehicleMoveVector() override;
private:
	AID_t EnvVehicleAID = INVALID_AID;
	FVector EnvVehicleOffset = FVector::ZeroVector;
	float EnvVehicleOffsetAngle = 0.f;

	//====================================================================================
	// Preset
public:
	void SetPresetTID(const WRTableID IN InTID) { PresetTID = InTID; }
	WRTableID GetPresetTID() { return PresetTID; }
protected:
	WRTableID PresetTID = INVALID_TABLE_TID;

private:
	void DashIn();

	//====================================================================================
	// Voice Chatting
private:
	void OnSpeakingStartedForVoiceChatting();	// 음성인식을 통해 말을 하기 시작
	void OnSpeakingStopedForVoiceChatting();	// 음성인식을 통한 말이 끝남

	//====================================================================================
	// Language
public:
	FORCEINLINE EWRLanguageType::en GetLanguageType() const { return LanguageType; }
	FORCEINLINE EWRLanguageType::en SetLanguageType(const EWRLanguageType::en IN InLanguageType)  {return LanguageType = InLanguageType;}
private:
	EWRLanguageType::en LanguageType = EWRLanguageType::Korean;

	//====================================================================================
	// Common
public:
	virtual void SetBattleState(bool bFlag) override;
	virtual bool IsInBattle() override;
	
protected:
	virtual void MakeDebugString(TArray<FString>& OUT OutString);
	//====================================================================================
	// [ 2020-06-08 : LJH ] Movingwalk

public:
	FORCEINLINE bool GetIsBoardingEnv() { return IsBoardingEnv; }
	FORCEINLINE void SetIsBoardingEnv(bool InIsBoardingEnv) { IsBoardingEnv = InIsBoardingEnv; }
private:
	bool IsBoardingEnv = false;
	//====================================================================================

	//====================================================================================
	// [ 2020-07-20 : LJH ] SecretDialog

public:
	//FVector CalcLocationForSecretDialogPortal(const FVector& InLastPosition);
	//FVector CalcLocationForSecretDialogArea(const FVector& InLastPosition);
	//====================================================================================
};

// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRCharacterPlayer, Display, All);