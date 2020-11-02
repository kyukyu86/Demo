// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Actor/FSM/WRFSMBase.h"

#include "Enum/EWRCharacter.h"

#include "CoreMinimal.h"
#include "../Base/WRActorComponent.h"
#include "WRActorComponentFSM.generated.h"

/**
 * 
 */

class UAnimMontage;
class WRFSMBase;
class AWRCharacter;
UCLASS()
class WR_API UWRActorComponentFSM : public UWRActorComponent
{
	GENERATED_BODY()

private:
	friend class AWRCharacter;

public:
	virtual void OnCreateComponent()	override;
	virtual void OnDestroyComponent()	override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
// 	UFUNCTION()
// 	void RegistFuncOnMontageDelegate();
// 	UFUNCTION()
// 	void RegistFuncOnMontageDelegate4Me();

	void PauseAnimation(float InPauseTime, bool bSkipAnimPauseTime = false);
	void ResumeAnimation(UAnimMontage* InMontage, float InPauseTime, bool bSkipAnim);

	void SetSkipAnimation(bool bFlag);
	bool IsSkipAnimation();
	bool IsPossibleEndCurrentFSM(EWRFSM InType, bool bImmediateUpdate = false, void* InVoidPtr = nullptr);

	//inline
public:
	FORCEINLINE WRFSMBase* GetCurrentFSM() { return CurrentFSM; }
	FORCEINLINE WRFSMInfo* GetCurrentFSMInfo() { return &CurrentFSMInfo; }
	EWRFSM GetFSMType();

public:
	UFUNCTION()
	void OnAnimMontageEnded(UAnimMontage* rtnMontage, bool bInterrupted);

	UFUNCTION()
	void OnAnimMontageBlendingOut(UAnimMontage* rtnMontage, bool bInterrupted);


protected:
	bool InvokeFSM(EWRFSM eType, bool bImmediateUpdate = false, bool bAnimationEnded = false);
	bool InvokeFSM(WRFSMInfo FSMInfo, bool bAnimationEnded = false);
	void RegistSequenceFSM(EWRFSM InType);
	void RegistSequenceFSM(WRFSMInfo FSMInfo);
	void RegistSequenceFSM(const TArray<WRFSMInfo>& InFSMInfo, bool bImmediatelyInvokeFirstIndexFSM);

	void EmptySequenceFSMInfo();

	void InvokeLinkedFSMWhenAnimationEnd();
	void DetermineAIMoveTo(WRFSMInfo InFSMInfo);

private:
	WRFSMBase* CurrentFSM;
	WRFSMInfo CurrentFSMInfo;
	FTimerHandle PauseAnimationTimerHandle;
	bool bSkipAnimation = false;

	TArray<WRFSMInfo> SequenceFSMInfo;
	
};
