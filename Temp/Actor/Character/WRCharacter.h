// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enum/EWRActorComponent.h"
#include "Enum/EWRCharacter.h"
#include "Define/WRTypeDefine.h"
#include <AITypes.h>
#include <Navigation/PathFollowingComponent.h>
#include "Define/WRCharacterDefine.h"
#include "Component/Character/WRActorComponentAttack.h"
#include "Animation/AnimInstance.h"
#include "Enum/EWRUIEnum.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"
#include "Component/Character/WRActorComponentInteractionObject.h"
#include "Network/WRProtocolHeaders.h"
#include "Kismet/KismetMathLibrary.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Manager/WRVFXManager.h"
#include "Table/WRFactionTable.h"
#include "../../Network/Share/Define/Define_Actor.h"
#include "Interface/Actor/WRActorComponentInterface.h"
#include "Component/Base/WRBaseComponentManageMaterial.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"

#include "WRCharacter.generated.h"

DECLARE_DELEGATE(FWRRotationEndDelegate);

// [19/09/09 firefirefox] " 추후 삭제 필요

//DECLARE_DELEGATE_OneParam(WRDestoryHandle, AActor*)
class UWRCapsuleCollisionComponent;
class AWRVehicle;

struct FWRCharacterBaseInfo
{
	AID_t ActorID;
};

UCLASS()
class WR_API AWRCharacter : public ACharacter, public IWRActorComponentInterface
{
	GENERATED_UCLASS_BODY()

		UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		USceneComponent* MeshParent;

public:
	USceneComponent* GetMeshParentComponent() const { return MeshParent; }

public:
	// Sets default values for this character's properties
	AWRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BuildCustomComponents() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnPostSpawn();
	// [ 2020-9-4 : magedoga ] 캐릭터 매니저에서 완료 후 호출
	virtual void OnCompleteSpawn();

	virtual void PostInitializeComponents() override;

	virtual void OnPreDestroy();

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor);

	//FSM
	virtual void BeginFSMAttack(const WRFSMInfo& InFSM);
	virtual void TickFSMAttack(float InDeltaTime);
	virtual void EndFSMAttack(const WRFSMInfo& InFSM);

	virtual void BeginFSMDamage(const WRFSMInfo& InFSM);
	virtual void EndFSMDamage(const WRFSMInfo& InFSM);

	virtual void BeginFSMDie(const WRFSMInfo& InFSM);
	virtual void EndFSMDie(const WRFSMInfo& InFSM);

	//Collision Capsule
	virtual void OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnEndOverlapHittingCapsule(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OutsideWorldBounds();
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;

	void SetVisibility(bool bNewVisibility, bool bPropagateToChildren = false);
	bool IsVisibility();

	virtual void SpawnDefaultController() override;

private:

	void PopupForOutofBound();

	bool bProcessingOutsideWorldBounds = false;

	//====================================================================================

	// [ 2019-9-19 : magedoga ] MID

public:

	template <typename TParameter>
	void SetMaterialParameterValue(FName InParamName, TParameter InValue, UMeshComponent* InMeshComponent = nullptr);

private:
	void SetMaterialParameterValue_Internal(FName InParamName, float InValue, UMeshComponent* InMeshComponent = nullptr);
	void SetMaterialParameterValue_Internal(FName InParamName, FLinearColor InValue, UMeshComponent* InMeshComponent = nullptr);

	//====================================================================================

public:

	UFUNCTION()
	void ReceiveMoveCompleted(FAIRequestID InRequestID, EPathFollowingResult::Type InResult);

	UFUNCTION()
	void OnAnimInitializedDele();

//#ifdef REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
//#else // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_
	UFUNCTION()
	void OnAnimInitializedDele4Me();
//#endif // REMOVE_FIRSTCHARACTER_FROM_MYCHARACTER_BY_ANIMAL14_

public:
	//함수내에서 스킬아이디로 정보를 찾아 왼손인지 오른손인지 판단
	virtual void SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart EquipPart, ECollisionEnabled::Type InType) {};
	virtual void SetWeaponCollisionBasedOnSide(EWREquipPart InPart, ECollisionEnabled::Type InType) {};
	virtual void SetBattleState(bool bFlag);
	virtual bool IsInBattle();

	void InitializeCombatCollisionCapsule();
	void RemoveCapsuleCollisionComponentInfo(const EWRCollisionType& InType);
	UWRCapsuleCollisionComponent* FindCapsuleCollisionComponent(const EWRCollisionType& InType);

	virtual void DetermineTerminateSomethingWhenChangeFSM(const WRFSMInfo& InFSMInfo) {};
	bool DetermineFSMSignal(EWRFSM FSMType, bool bInvokeImmediately = false);
	bool DetermineFSMSignal(WRFSMInfo FSMInfo, bool bInvokeImmediately = false);
	void RegistSequenceFSMSignal(EWRFSM InFSMType);
	void RegistSequenceFSMSignal(WRFSMInfo InFSMType);
	void RegistSequenceFSMSignal(const TArray<EWRFSM>& InFSMArr, bool bImmediatelyInvokeFirstIndexFSM);
	void RegistSequenceFSMSignal(const TArray<WRFSMInfo>& InFSMArr, bool bImmediatelyInvokeFirstIndexFSM);

	virtual class UWRAnimInstance* GetAnimInstance();
	EWRFSM GetCurrentFSMType();
	class WRFSMBase* GetCurrentFSM();

	void SetActorID(AID_t InAID);
	AID_t GetActorID() const;
	FORCEINLINE void SetHP(const HP_t InCurrentHP) { CharacterStats.HP = InCurrentHP; }
	FORCEINLINE void SetMaxHP(const HP_t IN InMaxHP) { CharacterStats.MaxHP = InMaxHP; }
	FORCEINLINE void SetATK(const ATK_t IN InATK) { CharacterStats.Atk = InATK; }
	FORCEINLINE void SetDEF(const DEF_t IN InDEF) { CharacterStats.Def = InDEF; }
	void SetMoveAnimType(const EWRMoveAnimType IN InMoveAnimType);
	HP_t	GetHP() const { return CharacterStats.HP; }
	HP_t	GetMaxHP() const { return CharacterStats.MaxHP; }
	ATK_t	GetATK() const { return CharacterStats.Atk; }
	DEF_t	GetDEF() const { return CharacterStats.Def; }
	EWRMoveAnimType GetMoveAnimType() { return MoveAnimType; }
	bool	IsAlive();
	bool	IsMe();
	bool	IsPlayer();	
	EWRCharacterType GetCharacterType() const;
	void SetCharacterType(EWRCharacterType InType);

	void SetWalkSpeedByRateValue(float InRate);
	void SetWalkMaxSpeed(float InValue);

	AWRCharacter* GetParent();
	AWRCharacter* GetChild();

	bool SetLocation(const FVector& NewLocation, bool bModifyZ = false, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	bool SetRotation(FRotator NewRotation, ETeleportType Teleport = ETeleportType::None);
	bool SetLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bModifyZ = false, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);

	bool SetActorLocation(const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	bool SetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);

	//====================================================================================
	// AIMove
	class AAIController* GetAIController();
	virtual bool AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType = EWRActionType::en::RUN);
	UFUNCTION()
	virtual void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);
	bool IsAIMoveTo();
	bool ClearAIMoveTo();
	void SetDestPositionWhenCharacterSpawned(const FVector& InDestPosition);
	bool IsSamePostion2StopWithReceivedDestPos2AIMove(const FVector& InStopPosition);
	void ProcessInterpolateDestinationLocationWhenSpawned(FVector DestPosition, EWRActionType::en InMoveType = EWRActionType::en::RUN);

	//====================================================================================
	// Animation
	bool PlayAnimationMontage(int32 MontageID, FString StartSectionName = "", float InPlayRate = 1.0f);
	void StopCurrentAnimMontage();
	void PlayAnimationSequence(class UAnimationAsset* NewAnimToPlay, bool bLooping = false);
	void StopAnimationSequence();
	void PlaySlotAnimationAsDynamicMontage(class UAnimSequenceBase* AnimSeq, const FName IN InSlotName);
	virtual void OnAnimEnded(UAnimSequenceBase* AnimSeq) {} // [ 2019-10-18 : kyu ] 시퀀스 애니 플레이용. UWRAnimNotify_AnimEnded 로 받는다.
	WRTableID GetMontageTID(EWRFSM InFSMType, FString InSectionName);
	WRTableID GetMontageTIDBasedOnCharacterTable(const EWRFSM& InFSMType);
	void SetAnimMontageRate(float InRate);
	void PlayAnimJumpToSection(EWRFSM InFSMType, const FString& InSectionName);
	FString GetCurrentMontageSectionName();
	WRRecordTID GetMontageBandwidth();
	float GetCurrentMontageSectionLength();
	float GetSectionTimeLeftFromPos();

	WRTableID ConvertMontageTIDFromBaseTID(const WRTableID IN InBaseTID);

	void OnAnimUpdateRateParameterCreated(FAnimUpdateRateParameters* InParamter);
	void SetEnableURO(const bool bInEnable);
	void SetEnableDisplayURODebug(const bool bInEnable);

	void SetCollisionProfileNameAndEnabledTemporary(FName InProfileName, ECollisionEnabled::Type InType, EWRSetCollisionTo InSetTo);
	void RevertCollision();

private:
	int32 BaseNonRenderedUpdateRate = 5;
	TArray<float> BaseVisibleDistanceFactorThesholds;
	TMap<int32, int32> LODToFrameSkipMap;

	//백업용
	EWRSetCollisionTo CollisionBackupType = EWRSetCollisionTo::None;
	FName CapsuleOriginalCollisionProfileName;
	ECollisionEnabled::Type CapsuleOriginalCollisionEnabled;

	FName MeshOriginalCollisionProfileName;
	ECollisionEnabled::Type MeshOriginalCollisionEnabled;

public:

	//====================================================================================
	// Weapon
	void SetEquippedWeaponItemTID(WRTableID InRightWeaponItemTID, WRTableID InLeftWeaponItemTID, WRTableID InTemporaryItemTID);
	void SetEquippedWeaponItemTID(WRTableID InWeaponItemTID);
	virtual WRTableID GetRightWeaponTID();		// Return to CharWeaponTID
	virtual WRTableID GetLeftWeaponTID();		// Return to CharWeaponTID
	virtual WRTableID GetTemporaryTID();		// Return to CharWeaponTID
	virtual WRTableID GetRightWeaponItemTID();	// Return to ItemTID
	virtual WRTableID GetLeftWeaponItemTID();	// Return to ItemTID
	virtual WRTableID GetTemporaryItemTID();	// Return to ItemTID
	void ChangeWeaponWithAnimation(EWRChangeEquipment IN InType);
	virtual void ChangeWeaponWithoutAnimation(const EWRChangeEquipment IN InType) {}
	void SetEquipPartVisibility(const EWREquipPart IN InPart, const bool IN InShow);	// Equip Part Visibility
	bool IsEquipedVisible(const EWREquipPart IN InPart);
	void ClearAllEquippedHands(); // [ 2020-3-12 : kyu ] for Temporary Item
	void RevertToSrcEquippedWeapons(const bool IN InWithAnimaiton); // [ 2020-3-12 : kyu ] for Temporary Item

	// Costume
	void SetEquippedCostume(const EWRItemEquippableSlot IN InEquipSlot, const WRTableID IN InCostumeItemTID, FWREquipmentComplete InDelegate = nullptr);
	void SetEquippedCostume(const TArray<WRTableID> CostumeList, FWREquipmentComplete InDelegate = nullptr);
	
	//====================================================================================
	// Faction
	WRTableID GetFactionTID();
	FWRFactionTable* GetFactionTable();
	EWRCharacterFactionType GetFactionType();
	EWRFactionRelation GetRelationToTarget(AWRCharacter* InTarget);
	EWRFactionRelation GetRelationToTarget(WRTableID InFactionTID);
	EWRFactionRelation GetRelationToTarget(EWRCharacterFactionType InFactionType);



	//====================================================================================
	// Packet Process
	void OnInformAIMoveTo(const pt_Game_User::Inf_ActorMoveToPos_G2U& InInform);
	void OnInformAIMoveStop(const pt_Game_User::Inf_ActorMoveStop_G2U& InInform);
	void OnInformAIWarp(const pt_Game_User::Inf_ActorWarp_G2U& InInform);
	void OnInformPlayerStop(const pt_Game_User::Inf_PlayerStop_G2U& InInform);
	void OnInformActorCCState(const pt_Game_User::Inf_ActorCCState_G2U& InInform);

	void OnInformGuardBreak(const pt_Game_User::Inf_GuardBreak_G2U& InInform);
	void OnInformSuccessToShield(const pt_Game_User::Inf_SuccessToShield_G2U& InInform);
	void OnInformSyncDynamicMoveLocation(const pt_Game_User::Inf_SyncDynamicMoveLocation_G2U& InInform);
	void OnInformDynamicMove(const pt_Game_User::Inf_DynamicMove_G2U& InInform);
	void OnInformSuccessAvoid(const pt_Game_User::Inf_SuccessToAvoid_G2U& InInform);
	virtual void OnInformHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform);

	virtual void OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform);
	virtual void OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform);
	virtual void OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform);
	virtual void OnInformRevive(const pt_Game_Actor::Inf_Revive_G2U& InInform);
	virtual void OnInformChangeHP(const pt_Game_Actor::Inf_ChangeHP_G2U& InInform);
	virtual void OnInformBeginFall(const pt_Game_User::Inf_BeginFall_G2U& InInform);
	virtual void OnInformEndFall(const pt_Game_User::Inf_EndFallen_G2U& InInform);

	void OnInformChangeCostume(const pt_Game_User_Inventory::Inf_ChangeCostume_G2U& InInform);


	//====================================================================================
	// Common
	virtual void SetCharacterTableID(WRTableID InTID);
	WRTableID GetCharacterTableID();

	void SetCharacterName(const FName& IN InCharacterName) { CharacterName = InCharacterName; }
	const FName& GetCharacterName() const { return CharacterName; }


	WRDestoryHandle& OnDestroy() { return DestoryHandle; }

	void SetSimulatePhysics(const bool InIsEnable);

	void AddImpulseToAllBodiesBelow(const float InImpulse = 1.0F, const FName& InBoneName = NAME_None);

	void SetMurdererID(const AID_t InAID) { MurdererAID = InAID;}

	bool IsSimulatingPhysics();

	void SetPhysicsDirection(const FVector& InDirection) { PhysicsDirection = InDirection; }
	FVector GetPhysicsDirection() {	return PhysicsDirection; }

	void SetAttackDirection(const FVector& InDirection) { AttackDirection = InDirection; }
	FVector GetAttackDirection() { return AttackDirection; }

	void SpawnTOD();
	void RemoveTOD();

	void SelectDeadDisplay();

	bool GetSpawnDisplay() { return IsSpawnDisplay; }

	bool GetRemoveDisplay() { return IsRemoveDisplay; }

	virtual void SetWhiteOut(const FVector InLocation, const FRotator InRotation);

	void StartRestraint(int32 InDuration);
	void StartDrunk(float InDuration = 1.0F);

	FString GetSkillTableFileName();

	void SetSpawnDisplay(bool InIsSpawnDisplay) { IsSpawnDisplay = InIsSpawnDisplay; };
	void SetRemoveDisplay(bool InIsRemoveDisplay) { IsRemoveDisplay = InIsRemoveDisplay; };

	void MakeHitDisplay(const FVector& InImpactPoint);
	void MakeUnderWaterDisplay();
	void MakeWarpDisplay();
	void PlayClothChangeDirection(const float IN InDuration = 3.f, const bool IN bInInverse = false);

	TArray<FString> DebugActorInfomation();
private:
	void ProcessRemoveDisplay(float InDeltaTime);

	void MakeSpawnDisplay();
	void MakeRemoveDisplay();

	void CompleteSpawn();
	void CompleteDissolve();
	void CompleteHit();

	void FinishRestraint();
	void FinishDrunk();
protected:
	virtual void SetWhiteIn();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimMove")
		float MoveForwardValue = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimMove")
		float MoveRightValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRHit")
		bool IsDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRHit")
		UMaterialInterface* InCapMaterial = nullptr;

protected:
	EWRCharacterType CharacterType = EWRCharacterType::None;
	AID_t ActorID = 0;

	TMap<EWRCollisionType, UWRCapsuleCollisionComponent*> mapAttackCapsuleCollisionComponent;
	TMap<EWRCollisionType, UWRCapsuleCollisionComponent*> mapHitCapsuleCollisionComponent;
	TMap<EWRItemEquippableSlot, WRTableID> EquippedItemList;	// [ 2020-7-28 : kyu ] New Equipped Item List ( ** Weapon Item Table ID ** )
	WRTableID CharacterTableID = 0;
	FName CharacterName;

	//====================================================================================
	//자동이동 위치 보간	
	FVector ReceivedDestPos2AIMove;		// 서버에서 ai이동을 하기 위해서 받은 좌표. 네비 위에 있는 좌표다	
	FRotator ReceivedRotateWhenStop;	// ReceivedRotateWhenStop가 세팅된 상태면 멈추었을때 적용하기 위해. 
										//OnInformAIMoveToPos의 위치에 도달 못했는데 OnInformActorMoveStop을 받게 될경우 회전값을 저장
	FVector OriginalMoveRequestGoalLocation = FVector::ZeroVector;	//aimoveto가 들어올때 같은 값이면 path요청을 하지 않게 하기 위함

	//====================================================================================
	//수동이동 위치 보간	
	FVector ReceivedCharLocation = FVector::ZeroVector;


	bool bBattleState = false;
	class AAIController* AIController = nullptr;

private:
	stCharacterStat CharacterStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRDisplay", meta = (AllowPrivateAccess = "true"))
		bool IsSpawnDisplay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRDisplay", meta = (AllowPrivateAccess = "true"))
		bool IsRemoveDisplay = false;

	WRDestoryHandle DestoryHandle;

	FTimerHandle RestraintHandle;
	FTimerHandle DrunkHandle;

	FTransform SkeletalMeshLocalTransform = FTransform::Identity;

	FVector PreviousLocation = FVector::ZeroVector;
protected:
	FVector WarpLocation = FVector::ZeroVector;
	FRotator WarpRatation = FRotator::ZeroRotator;
private:
	FVector AttackDirection = FVector::ZeroVector;
	FVector PhysicsDirection = FVector::ZeroVector;

	float AccDeltaTime = 0.0F;

	uint64 MurdererAID = 0;

	bool IsDeadPlay = false;
	bool IsRemoveImmediately = true;

	EWRMoveAnimType MoveAnimType = EWRMoveAnimType::None;
	FDelegateHandle MyCharacterSpawnedDelegatehandle;

public:
	UPROPERTY(BlueprintReadOnly)
		bool IsAwayStep = false;

private:

	FORCEINLINE FRotator GetLookAtRotator(const FVector& InTargetLocation)
	{
		return UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), InTargetLocation);
	}

	FORCEINLINE void SetRotationForAwayStep()
	{
		if (IsAwayStep == true && this->IsInBattle() == true)
		{
			const AWRCharacter* InTargetCharacter = this->GetTargetCharacter();
			if (InTargetCharacter != nullptr)
			{
				FVector TargetLocation = InTargetCharacter->GetActorLocation();
				FRotator LookAtRotation = FRotator(0.f, GetLookAtRotator(TargetLocation).Yaw, 0.f);
				this->SetRotation(LookAtRotation);
			}
		}
	}

	void SetRotationForWhenArrived();

	//====================================================================================
	// [ 2019-8-26 : kyu ] IO
public:
	virtual bool IsIO() { return false; }
	virtual bool IsInteracting() { return false; }
private:
	void RemoveCombinedUI();
	//====================================================================================
	// [ 2019-9-17 : kyu ] Input
public:
	virtual bool OnPressedEvent(const EWRInput InInputType) { return false; }	// [ 2019-9-18 : kyu ] IO 에 대한 입력은 virtual AWRCharacterIO::OnPressedEvent 에서 받아간다.	
	virtual bool OnReleasedEvent(const EWRInput InInputType) { return false; }
	virtual void OnPadEvent(const EWRInput IN InEvent) {}	
	virtual FTransform GetInteractionTransform();
	//====================================================================================
	// [ 2019-10-2 : kyu ] Hook
public:
	virtual const EWRHookType GetHookType() { return EWRHookType::None; }
	virtual bool IsHookableState();
	virtual void OnHookEventToCaster() {}	// 갈고리 이벤트 발생 (pull, dash) -> 시전자에게 발생
	virtual void OnHookEventToTarget(AWRCharacter* IN InHookCaster, EWRCollisionType IN InConnectedCollisionType);
	virtual void OnHookThrow() {}	// 걸기 (시전자)
	virtual void OnHookArrival(const EWRConnectCableEvent IN InEvent, const FVector IN InHitLocation, AWRCharacter* IN InHookCaster);
	virtual void OnHookReturn() {}	// 회수인데, 회수 동작은 없을것.
	virtual void OnHookCutted();
	UFUNCTION()
	virtual void OnConnectCableEvent(EWRConnectCableEvent InEventType) {}
	FORCEINLINE void SetTargetedCollision(UWRCapsuleCollisionComponent* IN InCollision) { TargetedCollision = InCollision; }
	FORCEINLINE UWRCapsuleCollisionComponent* GetTargetedCollision() { return TargetedCollision; }
	FORCEINLINE void SetConnectedHookCableComponent(UWRActorComponentConnectbyCable* IN InCableComp) { ConnectedHookCable = InCableComp; }
	FORCEINLINE const UWRActorComponentConnectbyCable* GetConnectedHookCableComponent() { return ConnectedHookCable; }
	void UpdateRecognitionArea();

protected:
	EWRObjectHighLight HighLightType = EWRObjectHighLight::None;
	UWRCapsuleCollisionComponent* TargetedCollision = nullptr;
	UWRActorComponentConnectbyCable* ConnectedHookCable = nullptr;
	//====================================================================================
	// [ 2020-3-17 : kyu ] Cheers
public:
	void SetCheersCondition(const EWRCheersConditionType IN InCondition) { eCheersCondition = InCondition; }
	EWRCheersConditionType GetCheersCondition() { return eCheersCondition; }
private:
	EWRCheersConditionType eCheersCondition = EWRCheersConditionType::None;
	//====================================================================================


	//====================================================================================
	// [ 2019-10-4 : hansang88 ] Trigger 관련
public:
	const void SetTriggerSpawnTid(const int32 InTriggerSpawnTid) { TriggerSpawnTid = InTriggerSpawnTid; }
	const int32 GetTriggerSpawnTid() const { return TriggerSpawnTid; }

protected:
	int32						TriggerSpawnTid;
	//====================================================================================


	//====================================================================================
	// Target
public:
	virtual void SetTargetAID(const AID_t IN InTargetAID);
	virtual void OnTargeted(const bool IN InMulti = false, const bool IN InByMe = true);
	virtual void ReleasedTarget(const bool IN InMulti = false, const bool IN InByMe = true);

	virtual FTransform GetWidgetTransform();
	virtual EWRUIWidgetCreationBitFlag GetWidgetComponentShowType();

	virtual void SetHighLight(EWRObjectHighLight IN InHighLightType) {}

	FORCEINLINE AID_t GetTargetAID() { return TargetAID; }
	const AWRCharacter* GetTargetCharacter();

	void OnTargetLockChanged(const bool IN bInLcok) const;

protected:
	bool IsPossibleToTarget(const AWRCharacter* IN InTarget);

protected:
	AID_t TargetAID = INVALID_AID;
	
	//====================================================================================
	// [ 2019-11-13 : magedoga ] Look IK
public:

	bool IsLookingTarget() const { return LookTarget != nullptr; }
	void SetLookIKTarget(AWRCharacter* InTarget, FName InSocketName = NAME_None);
	void GetLookIKTarget(AWRCharacter*& OutTarget, FName& OutSocketName);
	void OnMyCharacterSpawned(class AWRCharacterMine* IN InMyNewCharacter);
	AWRCharacter* GetLookIKTarget() { return LookTarget; }

	void SetSkeletalMeshComponentToFABRIK(USkeletalMeshComponent* InMeshComp = nullptr);


private:

	AWRCharacter* LookTarget = nullptr;
	FName SocketName = NAME_None;

public:
	// [ 2020-2-10 : magedoga ] Get Activated MeshComponent
	virtual USkeletalMeshComponent* GetCurrentSkeletalMeshComponent() const { return GetMesh(); }
	virtual bool IsShowing3rdCharacter() const { return true; }

private:
	bool IsSliced = false;

public:
	bool GetIsSliced() { return IsSliced; }
	void SetIsSliced(bool InIsSliced) { IsSliced = InIsSliced; }
	void CheckDeadEffect();

	// [ 2020-4-2 : dlwlgus9125 ] Disappear 패킷 예외처리를 위한 기능 추가
	FORCEINLINE void SetRemoveImmediately(bool InRemoveImmediately) { IsRemoveImmediately = InRemoveImmediately; }
	FORCEINLINE bool GetRemoveImmediately() { return IsRemoveImmediately; }

	//====================================================================================
    // [ 2020-04-09 : dlwlgus9125 ] Rotation Lerp

	void SetRotationLerpToTarget(AWRCharacter* InTarget, FWRRotationEndDelegate InRotationEndDelegate = nullptr, FName InSocketName = NAME_None);
	bool CheckShouldTurnAround(FRotator& InLookRotation);
	void ProcessRotationLerp();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookIK")
		bool LookTargetAlways = false;

	FWRRotationEndDelegate RotationEndDelegate;

	FRotator ForTurnAroundRotator = FRotator::ZeroRotator;

	bool IsOnRotationLerp = false;
	bool IsShouldTurnAround = false;
	   	 
	//====================================================================================

protected:
	virtual void MakeDebugString(TArray<FString>& OUT OutString);

	//====================================================================================
	// Culling
#ifdef REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
#else // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
public:
	FORCEINLINE void SetCullingState(const bool IN InSet) { bCullingState = InSet; }
	FORCEINLINE bool IsCullingState() { return bCullingState; }
private:
	bool bCullingState = false;
#endif // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_

public:
	virtual float GetEnvVehicleMoveSpeed() { return 0.f; }
	virtual FVector GetEnvVehicleMoveVector() { return FVector::ZeroVector; }

	
	//====================================================================================
	// Vehicle
public:
	void SetOwnedVehicleAID(AID_t InAID);
	void SetBoardingVehicleAID(AID_t InAID);

	AID_t GetOwnedVehicleAID();
	AID_t GetBoardingVehicleAID();

	AWRVehicle* GetOwnedVehicle();
	AWRVehicle* GetBoardingVehicle();

	bool IsVehicleDriver();

private:
	AID_t OwnedVehicleAID;		//소유(내가 소환한) 차량 aid	
	AID_t BoardingVehicleAID;	//탑승한 차량 aid

//====================================================================================
	// [ 2020-07-16 : EZ ] Secret Dialog
	
	// OnOff를 위해 Init시에 담아둠
	UCapsuleComponent* WRTargettingCapsule = nullptr;
	EWRChatType::en CurrentChatType = EWRChatType::Normal;
	CHAT_GROUP_ID_t CurrentGroupID = INVALID_CHAT_GROUP_ID; // Invalid 상태가 Default 전체상태

	bool IsCharacterActive = true;
public:
	FORCEINLINE EWRChatType::en& GetChatType() { return CurrentChatType; }
	FORCEINLINE void SetChatType(const EWRChatType::en InChatType) { CurrentChatType = InChatType; }

	FORCEINLINE CHAT_GROUP_ID_t& GetDialogGroupID() { return CurrentGroupID; }
	FORCEINLINE void SetDialogGroupID(const CHAT_GROUP_ID_t InGroupID) { CurrentGroupID = InGroupID; }

	FORCEINLINE bool GetIsCharacterActive() { return IsCharacterActive; }

	void SetCollisionTargetting(const ECollisionEnabled::Type InCollisionState);
	
	void SetCharacterInvisible(bool IsVisible, bool IsUseFade = true);
//====================================================================================

public:
	void AddVisibilityBit(const EWRVisibility InBit);
	void RemoveVisibilityBit(const EWRVisibility InBit);
	bool IsVisibilityBits(const EWRVisibility InBit);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EWRVisibility VisibilityBits = EWRVisibility::None;
};
//


template <typename TParameter>
void AWRCharacter::SetMaterialParameterValue(FName InParamName, TParameter InValue, UMeshComponent* InMeshComponent /*= nullptr*/)
{
	this->SetMaterialParameterValue_Internal(InParamName, InValue, InMeshComponent);
}
