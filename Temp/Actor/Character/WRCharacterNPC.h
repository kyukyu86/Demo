// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRCharacter.h"
#include <DelegateCombinations.h>
#include "IDelegateInstance.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "WRCharacterNPC.generated.h"

/**
 * 
 */
struct FWRNPCBaseTable;

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

enum class EWRNPCEmotion : uint8
{
	None,
	Cheers,
};


UCLASS()
class WR_API AWRCharacterNPC : public AWRCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWRCharacterNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnPostSpawn();

	virtual void PostInitializeComponents() override;

	virtual void OnPreDestroy();

	virtual void BuildCustomComponents() override;
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void BeginFSMAttack(const WRFSMInfo& InFSM) override;
	virtual void TickFSMAttack(float InDeltaTime) override;
	virtual void EndFSMAttack(const WRFSMInfo& InFSM) override;

	virtual void SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart EquipPart, ECollisionEnabled::Type InType) override;
	virtual void SetWeaponCollisionBasedOnSide(EWREquipPart InPart, ECollisionEnabled::Type InType) override;
	virtual bool AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType = EWRActionType::en::RUN) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION(BlueprintCallable)
	virtual void OnEndOverlapHittingCapsule(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	//====================================================================================
	// Packet Process	
	virtual void OnInformHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform) override;
	virtual void OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform) override;
	virtual void OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform);
	virtual void OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform);

	//====================================================================================
	// Common
	virtual void SetCharacterTableID(WRTableID InTID) override;
	FWRNPCBaseTable* GetNpcBaseTable() const;
	EWRNpcJobType GetNpcJobType();
	EWRNpcCreateType GetNpcCreateType();
public:
	bool bSpawnedByServer = false;

	FOnAttackEndDelegate OnAttackEnd;

	//====================================================================================
	// [ 2020-1-8 : kyu ] Target or Hook
public:
	virtual const EWRHookType GetHookType() override;
	virtual void SetHighLight(EWRObjectHighLight IN InHighLightType) override;

	//====================================================================================
	// [ 2020-1-14 : ljh ] LookIKForDialog
public:
	void SetLookTargetForDialog(AWRCharacter* InTarget, FWRRotationEndDelegate InRotationEndDelegate = nullptr, FName InSocketName = NAME_None);
	void ReleaseLookTargetForEndDialog(const FRotator& InRotator);

	//====================================================================================



	//====================================================================================
	// [ 2020-3-20 : kyu ] Emotion
public:
	void OnEmotion(const EWRNPCEmotion IN InEmotion, AWRCharacter* IN InTarget = nullptr);
};
