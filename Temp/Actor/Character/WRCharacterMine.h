// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Component/Character/WRActorComponent360Movie.h"
#include "Engine/EngineTypes.h"
#include "Network/Share/Protocol/Protocol_Game_Vehicle.h"
#include "WRCharacterMine.generated.h"

#define DEF_AIDA_TABLIE_ID 20000007
#define DEF_AIDA_HEAL_SKILL_TABLE_ID 200201

DECLARE_DELEGATE_TwoParams(FWRBindingInputDelegate, EWRInput, EInputEvent);

UCLASS()
class WR_API AWRCharacterMine : public AWRCharacterPlayer
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;
	// [ 2019-7-19 : magedoga ] Will be called after spawn from world
	virtual void OnPostSpawn() override;
	virtual void OnCompleteSpawn() override;
	// [ 2019-7-19 : magedoga ] Will be called before actor destroy from world
	virtual void OnPreDestroy() override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//FSM for me
	virtual void BeginFSMAttack(const WRFSMInfo& InFSM) override;
	virtual void TickFSMAttack(float InDeltaTime) override;
	virtual void EndFSMAttack(const WRFSMInfo& InFSM) override;

	virtual void BeginFSMDie(const WRFSMInfo& InFSM) override;
	virtual void EndFSMDie(const WRFSMInfo& InFSM) override;

	virtual class UWRAnimInstance* GetAnimInstance();

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void DetermineTerminateSomethingWhenChangeFSM(const WRFSMInfo& InFSMInfo);

	//====================================================================================
	// Weapon
	virtual void ChangeWeaponWithoutAnimation(const EWRChangeEquipment IN InType); // 보유한 데이터 기반으로 셋팅. 데이터가 먼저 셋팅되어있어야 한다

	//====================================================================================
	// move
	virtual bool AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType = EWRActionType::en::RUN);
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);

protected:
	virtual void BuildCustomComponents() override;
	//====================================================================================
	// [ 2019-7-18 : magedoga ] Input
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	
	void MoveForward(float InScale);
	void MoveRight(float InScale);
	bool IsMovable();

	//L-stick forward
	void MoveForward4SpawnSimulation(float InScale);
	void MoveForward4CharacterMovement(float InScale);
	//L-stick right
	void MoveRightForSpawnSimulation(float InScale);
	void MoveRightForCharacterMovement(float InScale);

	void PitchKey(float InScale);
	void PitchPad(float InScale);
	void TurnRight(float InScale);

	//R-stick
	void TurnRight4SpawnSimulation(float InScale);
	void TurnRight4CharacterRotation(float InScale);

	void OnFixRotation();
	void OffFixRotation();

	
public:
	virtual WRTableID GetRightWeaponTID();		// Returns CharWeaponTID
	virtual WRTableID GetLeftWeaponTID();		// Returns CharWeaponTID
	virtual WRTableID GetTemporaryTID();	// Returns CharWeaponTID
	virtual WRTableID GetRightWeaponItemTID();	// Returns ItemTID
	virtual WRTableID GetLeftWeaponItemTID();	// Returns ItemTID
	virtual WRTableID GetTemporaryItemTID();	// Returns ItemTID

	// [ 2020-9-3 : magedoga ] 180도 로테이션 후 잠금됨. 잠금은 수동으로 상황에 맞게 풀어줘야 다시 180 돌릴 수 있음.
	void Rotation180();
	bool IsLockRotation180() const;
	void UnlockRotate180();
	//====================================================================================
	// Tick Process
	void TickRequestMovement(float DeltaTime);


	//====================================================================================
	// Packet Process
	void OnAckUseSkill(const pt_Game_User::Ack_UseSkill_G2U& InInform);
	void OnAckBeginSkill(const pt_Game_User::Ack_BeginSkill_G2U& InInform);
	void OnAckEndSkill(const pt_Game_User::Ack_EndSkill_G2U& InInform);

	void OnAckBeginFall(const pt_Game_User::Ack_BeginFall_G2U& InInform);
	void OnAckEndFall(const pt_Game_User::Ack_EndFallen_G2U& InInform);

	void OnAckGuardBreak(const pt_Game_User::Ack_GuardBreak_G2U& InInform);
	void OnAckSuccessToShield(const pt_Game_User::Ack_SuccessToShield_G2U& InInform);
	void OnAckSuccessAvoid(const pt_Game_User::Ack_SuccessToAvoid_G2U& InInform);
	void OnAckSyncDynamicMoveLocation(const pt_Game_User::Ack_SyncDynamicMoveLocation_G2U& InInform);
	void OnAckDynamicMove(const pt_Game_User::Ack_DynamicMove_G2U& InInform);
	void OnAckHitResult(const pt_Game_User::Ack_HitResult_G2U& InInform);

	void OnAckSpawnVehicle(const pt_Game_Vehicle::Ack_SpawnVehicle_G2U& InAck);
	void OnAckGetOnVehicle(const pt_Game_Vehicle::Ack_GetOnVehicle_G2U& InAck);
	void OnAckGetOffVehicle(const pt_Game_Vehicle::Ack_GetOffVehicle_G2U& InAck);
	void OnAckBeginBrake(const pt_Game_Vehicle::Ack_BeginBrake_G2U& InAck);
	void OnAckEndBrake(const pt_Game_Vehicle::Ack_EndBrake_G2U& InAck);
	void OnAckAcceleration(const pt_Game_Vehicle::Ack_Acceleration_G2U& InAck);
	void OnAckSteering(const pt_Game_Vehicle::Ack_Steering_G2U& InAck);
	void OnAckAgreeToBoard(const pt_Game_Vehicle::Ack_AgreeToBoard_G2U& IN InPacket);

	virtual void OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform) override;
	virtual void OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform) override;
	virtual void OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform) override;
	virtual void OnInformRevive(const pt_Game_Actor::Inf_Revive_G2U& InInform) override;
	virtual void OnInformChangeHP(const pt_Game_Actor::Inf_ChangeHP_G2U& InInform);

	//====================================================================================
	// [ 2019-9-6 : kyu ] Stereo Layer
public:
	FORCEINLINE class UStereoLayerComponent* GetStereoLayerComponent() { return StereoLayerComponent; }
	FORCEINLINE FVector2D GetQuadSize() { return QuadSize; }
	FORCEINLINE FVector2D GetRenderSize() { return RenderSize; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool StereoLayerTestOn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D QuadSize = FVector2D(800, 600);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D RenderSize = FVector2D(800, 600);
private:
	class UStereoLayerComponent* StereoLayerComponent;


	//====================================================================================
	// [ 2019-9-16 : ysy ] 타겟팅 관련된 내용
public:
	virtual void SetTargetAID(const AID_t IN InTargetAID) override;
	void OnInputSystemTargetChanged(ACharacter* IN InTarget);
	void OnTargetLockChanged(const bool IN bInLock, const ACharacter* IN InTarget);	


	//====================================================================================
	// [ 2019-10-4 : kyu ] Interaction Object & Hook
public:
	virtual void OnHookEventToCaster() override;
	FORCEINLINE float GetRecognitionDistance() { return RecognitionDistance; }
	FORCEINLINE bool IsHookTargeted() { return pTargetedHookCollision == nullptr ? false : true; }
	bool IsRecognizableRange(const ACharacter* IN InTarget);
	bool IsHookableRange(const AWRCharacter* IN InTarget);	
	void SaveCurrentHookPart();
	void OnChainCutTime();
	EWRCollisionType GetCurrentTargetedHookPart();
	AWRCharacter* GetHookTargetCharacter();
	UFUNCTION(BlueprintCallable)
	FVector GetGyroVector() { return vGyro; }
private:
	void UpdateHookTraceCollision();
public:
	FVector vGyro;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecognitionDistance = 600.0f;
private:
	FDelegateHandle InputSystemTargetChangedHandle;
	UWRCapsuleCollisionComponent* pTargetedHookCollision = nullptr;


	//====================================================================================
	// [ 2020-2-19 : kyu ] Transportation
public:
	void OnBeginOverlap_EnvRecognitionArea(class AWREnvVehicleBase* IN InEnvVehicleBase);
	void OnEndOverlap_EnvRecognitionArea(class AWREnvVehicleBase* IN InEnvVehicleBase);
private:
	void UpdateCheckEnvGetOnOff();
private:
	bool bCheckEnvGetOnOff = false;
	bool bGetOnEnvVehicle = false;


	//====================================================================================
	// [ 2020-4-17 : kyu ] Cheers
public:
	bool IsEquippedBottle();


	//====================================================================================
	// [ 2019-8-26 : magedoga ] Character Rotation
public:
	void SetRotationMode(EWRCharacterRotationType InType);
	void SetSyncRotationByHMD(const bool bInEnable);
	WRPlayerCharacterRotator* GetRotator() const { return CharacterRotator; }
private:
	EWRCharacterRotationType RotationType;
	WRPlayerCharacterRotator* CharacterRotator = nullptr;


	//====================================================================================
	// [ 2020-1-7 : magedoga ] 3rd SkeletalMeshComponent
public:
	virtual bool IsShowing3rdCharacter() const override { return bShow3rdCharacter; }

	void Show3rdCharacter(const bool bInShow, FVector InCameraOffset = FVector::ZeroVector);

	void TransplantFSMWhenChangeViewpoint(bool bInShow);

	bool Set3rdSkeletalMesh(USkeletalMesh* InSkeletalMesh);

	FVector Get3rdCharacterCameraOffset() const { return CameraOffset; }

	USkeletalMeshComponent* Get3rdSkeletalMeshComponent() const { return ThirdPersonSkeletalMeshComponent; }

	virtual USkeletalMeshComponent* GetCurrentSkeletalMeshComponent() const override;

private:
	FVector CameraOffset = FVector::ZeroVector;

	bool bShow3rdCharacter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent * ThirdPersonSkeletalMeshComponent = nullptr;

public:
	class UAudioCaptureComponent* GetAudioCaptureComponent() { 
		return AudioCaptureComponent;
	}
	class UStaticMeshComponent* GetSphereFlippedNormalComponent() {
		return SphereFlippedNormalComponent;
	}
#if USE_AUDIOMIXER_BP_LIBRARY
	class USoundSubmix* GetSoundSubmix() const { return SoundSubmix; }
#endif // USE_AUDIOMIXER_BP_LIBRARY

private:
	UPROPERTY()
	class UAudioCaptureComponent* AudioCaptureComponent = nullptr;
	UPROPERTY()
	class UStaticMeshComponent* SphereFlippedNormalComponent = nullptr;
#if USE_AUDIOMIXER_BP_LIBRARY
	class USoundSubmix* SoundSubmix = nullptr;
#endif // USE_AUDIOMIXER_BP_LIBRARY

	//====================================================================================
	// Common
public:
	bool IsInputPad() {	return bInputPad;}
	bool IsReqBeginFallPacket() { return bReqBeginFallPacket; }
	void DetermineFallPacket();
public:
	virtual void SetWhiteOut(const FVector InLocation, const FRotator InRotation) override;
protected:
	virtual void SetWhiteIn() override;
private:
	float ElapsedTime2RequestMovement = 0.0f;
	bool bInputPad= false;
	EWRAttackHeightType LastHeightType = EWRAttackHeightType::None;
	bool bReqBeginFallPacket = false;

	// [ 2020-3-26 : magedoga ] SyncHMD
public:
	void SetEnableSyncHMDTick(const bool bInEnable);
	void CallbackSyncHMD();

	virtual bool TickSyncHMD(float InDeltaTime) override;

private:
	float AccTimeForRequestSyncHMD = 0.0f;
	FDelegateHandle TickSyncHMDHandle;

	FRotator LastSyncHMDRotation = FRotator::ZeroRotator;
	FTimerDelegate SyncTimerDelegate;
	FTimerHandle SyncHMDTimer;

	// [ 2020-1-13 : hansang88 ] 영상관련
public:
	UFUNCTION(BlueprintCallable)
	void OnMediaOpened(FString OpenedURL);

	//====================================================================================
public:
	void TeleportUsingVoiceRecognition(const WRTableID IN InCoordinateTID);
	void UseAidaHeal();
	void CallAida(); // 에이다를 내 근처로 소환

public:
	void SetUsingWarp(const bool IN bInUsingWarp);
private:
	bool bUsingWarp = false;

	//====================================================================================

public:
	void DashOut();

private:
	void DashIn();

public:
	bool IsDanceModeOn();



	// [ 2020-3-24 : magedoga ] 360 Movie
public:
	bool Play360Movie(class UWRPlay360VideoAsset* InAsset, bool b3DUse = false);
	void Stop360Movie();

	void OnChange360MovieState(EWR360MovieEvent InState);

	//====================================================================================
	// [ 2020-9-4 : magedoga ] HMD Camera
public:
	void InitCameraOffsetByPlayerTable();


//====================================================================================
// [ 2020-07-09 : EZ ] MoveState Refactoring
private:
	EWRMoveStateType CurrentMoveState = EWRMoveStateType::OnFoot;

public:
	FORCEINLINE void SetMoveState(const EWRMoveStateType IN InMoveStateType) { CurrentMoveState = InMoveStateType; }
	FORCEINLINE EWRMoveStateType& GetMoveState() { return CurrentMoveState; }

	void RequestMove();
	void RequestStop();
//====================================================================================
	// Common
};


// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRCharacterMine, Display, All);