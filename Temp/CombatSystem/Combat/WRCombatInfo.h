#pragma once
#include "Enum/EWRCharacter.h"
#include "Table/WRSkillTable.h"
#include "Network/Share/Define/Define_Type.h"
#include "Engine/EngineTypes.h"

class AWRCharacter;
class UWRCapsuleCollisionComponent;
struct FWRCombatParam
{
public:
	SKILL_SEQ_t	CombatSeqID;		// CombatSeqID
	AID_t		AttackerUID;		// 공격자 UID
	AID_t		TargetUID;		// 대상 UID //스킬 사용시에 주시하고 있던 대상. 이 대상이 공격에 맞는다는 보장은 없다
	WRRecordTID SkillTID;	// 스킬 ID
	
	EWRFSM		PreviousFSMType;	// 이전에 실행되던 FSM타입
	bool bReceivedServerSeqID = true;

	void Reset()
	{
		CombatSeqID = INVALID_SKILL_SEQ;
		AttackerUID = 0;
		TargetUID = 0;
		SkillTID = 0;
		PreviousFSMType = EWRFSM::None;
		bReceivedServerSeqID = true;
	}
};

enum class EWRDelayedPacketType : uint8
{
	GuardBreak,
	Block,
	Hit,
	Parrying,
	Avoid,
};

struct FWRDelayedPacketInfo
{
	EWRDelayedPacketType Type;

	AID_t CasterAID;
	AID_t TargetAID;

	uint8 AvoidDir;

	int32 CommonValue;
};

class WRCombatInfo
{
	friend class WRCombatManager;
public:
	WRCombatInfo();
	~WRCombatInfo();

public:
	void Init();
	void Reset();
	void Tick(float DeltaTime);	

	//throw
public:
	bool IsValidNextTarget(AID_t InAID);
	bool IsGettingBackProjectile();	//방패가 주인에게 돌아가는 중인지 체크
	void OnProcessProjectileBeginOverlap();
	void SetChainTargetInfo(const TArray<AID_t>& InChainTarget);
	void SetNextChainTaretIndex(AID_t InActorID);
	void OnNotifyThrow(WRTableID WeaponLookTID);
	FVector GetProjectileHitLocation(AWRCharacter* InActor);
	void ProcessCombatEnded();
	bool IsValidProjectileCombat();


private:
	void TickThrow(float DeltaTime);
	void TickRotateShield(float DeltaTime);
	void TickUpdateTargetLocation();

	// process packet
public:
	void TickProcessDelayedPacket();
	bool IsValidServerSeqID();
	void SetReceivedServerSeqIDFlag(bool bFlag);
	void ModifyCombatSeqID(SKILL_SEQ_t InSeqID);


	void InitializeCombatInfo(const FWRCombatParam& InParam);
	void SetCombatSeqParam(const FWRCombatParam& InParam);
	FWRCombatParam GetCombatSeqParam();

	SKILL_SEQ_t GetCombatSeqID();
	AWRCharacter* GetAttacker();
	AWRCharacter* GetTarget();
	AID_t GetAttackerAUID() { return CombatSeqParam.AttackerUID; }
	AID_t GetTargetAUID() { return CombatSeqParam.TargetUID; }

	FWRSkillTable* GetSkillTable();

	//hit and gimmick
public:
	bool DetermineHit(AID_t InAID, UWRCapsuleCollisionComponent* InCapsule);
	bool IsReservedDestroy();
	bool IsDelayedPacketTimerActive();
	void CallbackOverDelayTimer();
	bool DetermineBlocking(AWRCharacter* InAttacker, AWRCharacter* InHitChar);

public:
	void RequestToServer_GuardBreak(AWRCharacter* InCaster, AWRCharacter* InTarget);
	void RequestToServer_BlockSuccessed(AWRCharacter* InAttacker, AWRCharacter* InBlocker);
	void RequestToServer_Hit(AWRCharacter* InCaster, AWRCharacter* InTarget, EWRCollisionType InCollisionType);
	void RequestToServer_Avoid(AWRCharacter* InCaster, uint8 AvoidDir);

	void ExecuteGuardBreak(AID_t InAID);
	void ExecuteBlockSuccessed(AID_t InAttackerAID, AID_t InBlockAID);
	void ExecuteHit(AID_t InHitChar, int32 InDamage = 0, bool bExecuteDynamicMovement = true);
	void ExecuteParrying(AID_t InAID);

private:
	void Destroy(bool bForce = false);
	void DeleteTimer();
	void Tick4Hit();

	void MakeHit(AWRCharacter* InCharacter);
	void MakeHit(AWRCharacter* InCharacter, const FVector& InLocation, const FRotator& InRotator);
	void MakeHit4Me(AWRCharacter* InCharacter);
	void MakeHit4Other(AWRCharacter* InCharacter);

	void SetCharacter(AWRCharacter* InCharacter);
	FVector GetComponentLocation();
	FVector GetPreviousLocation(const FVector& InCurrentLocation);
	FVector GetAttackDirection(AWRCharacter* InCharacter, const FVector& InPreviousLocation, const FVector& InCurrentLocation);
	FVector GetForwardDirection();
	FVector GetPhysicsDirection(AWRCharacter* InCharacter, const FVector& InAttackDirection, const FVector& InForwardDirection);

	float GetHPRatio(AWRCharacter* InCharacter);

	FString TryLoad(AWRCharacter* InCharacter);

	void MakeDisplay4Me(AWRCharacter* InCharacter, const FVector& InLocation, const FVector& InDirection);
	void MakeDisplay4Other(AWRCharacter* InCharacter, const FVector& InLocation, const FVector& InDirection);
	void MakeDecal(AWRCharacter* InCharacter, const FVector& InDirection);
	void MakeHPDisplay(AWRCharacter* InCharacter);
	void MakeStrongDisplay();
	void MakeScreenDisplay();
	void MakeRestraint(AWRCharacter* InCharacter);

private:
	FWRCombatParam	CombatSeqParam;
	TArray<FWRDelayedPacketInfo> DelayedPacket;

	TSet<AID_t> HitCharacters;	// 실제 피격당하는 케릭터
	TArray<FVector> PreviousLocations;

	bool bReserveDestroy = false;

	class AWRStaticMeshProjectile* ThrowingkMeshActor = nullptr;
	class USplineComponent* SplineComponent = nullptr;

	int32 ChainTargetIndex = 0;
	TArray<AID_t> arrChainTarget;
	float ElapsedTime4ThrowingActorMove = 0.0f;
	float ElapsedTime4UpdateLocation = 0.0f;

	FTimerHandle InProcessDelayedPacketTiemr;

};

