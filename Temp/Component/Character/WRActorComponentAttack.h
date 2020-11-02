// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "Enum/EWRCharacter.h"
#include <TimerManager.h>
#include "WRActorComponentConnectbyCable.h"
#include "Define/WRTypeDefine.h"
#include "Table/WREquipmentLookTable.h"
#include <Tuple.h>
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRActorComponentAttack.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentAttack : public UWRActorComponent
{
	GENERATED_BODY()

	typedef TPair<EWRInput, EWRSkillType> Key;
	typedef TMap<EWRSkillType, TArray<WRTableID>> mapTypeContainer;
	
public:
	virtual void OnCreateComponent()	override;
	virtual void OnDestroyComponent()	override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//공통
public:
	void Initialize();

	void GenerateSkillInfo(WRTableID InWeaponTID);
	void RemoveSkillInfo(EWREquipPart InPart);

	void SetThrowingShieldFlag(bool bFlag);


	//평타 리셋
	void CallbackFuncFastAttack();
	void CallbackFuncStrongAttack();
	void OnProcessExecuteSkill(WRTableID InSkillTID);
	void OnProcessNormalAttackInitialization(WRTableID InSkillTID);
	void SetTimer(FTimerHandle& InTimerHandle, FTimerDelegate const& InDelegate, float InRate);
	void ClearTimer(FTimerHandle& InTimerHandle);
	

	WRTableID GetValidTID(EWRInput InType);
	TArray<WRTableID> GetTIDArrBySkillType(EWRSkillType InType, EWREquipPart InEquipType);
	WRTableID GetValidHookTID(EWRSkillType InSkillType, const TArray<WRTableID>& InArrSkill);
	WRTableID GetValidDefaultTID(EWRSkillType InSkillType, const TArray<WRTableID>& InArrSkill);
	


	//cooltime
	bool IsActiveCoolTime(WRTableID InTID);
	void SetCoolTime(AWRCharacter* InActor, WRTableID InTID);
	void CallBackCoolTimeEnd(WRTableID InTID);
	int32 GetCoolTime(WRTableID InTID);
	
	//입력
public:
	void PressBtn(EWRInput InInputType);
	void ReleaseBtn(EWRInput InInputType);

public:
	bool DetermineParrying();
	void TestShield();

	//====================================================================================
	// [ 2019-10-2 : kyu ] Sub Weapon Skill
public:	
	WRTableID GetCurrentHookTID(const AWRCharacter* InTarget);				// [ 2020-4-13 : kyu ] 타겟타입에 따른 HookTID 획득
private:	
	EWRHookEvent DetermineHookEvent();
	//====================================================================================

private:
	int32 FastAttackIndex = 0;
	int32 StrongAttackIndex = 0;
	int32 SubAttackIndex = 0;

	FTimerHandle FastAttackComboTimer;
	FTimerHandle StrongAttackComboTimer;

	bool bThrowingShield = false;
	TMap<WRTableID, FTimerHandle> CoolTimeContainer;

	TMap<EWRInput, mapTypeContainer> mapCurrentSkill;
};
