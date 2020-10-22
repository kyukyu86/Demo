// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/DMActorComponentBase.h"
#include "../FSM/DMFSMBase.h"
#include "../FSM/Idle/DMFSMIdleBase.h"
#include "DMActorComponentFSM.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMActorComponentFSM : public UDMActorComponentBase
{
	GENERATED_BODY()
	
private:
	EDMIdleType CurrentIdleType = EDMIdleType::Stand;
	EDMIdleType ReservedIdleType = EDMIdleType::None;

	EDMFSMType CurrentFSMType = EDMFSMType::Idle;
	EDMFSMType ReservedFSMType = EDMFSMType::None;

	DMFSMBase* CurrentFSM = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnCreateComponent();
	virtual void OnDestroyComponent();

public:
	bool DetermineFSM(EDMFSMType IN InNewFSM);

	FORCEINLINE EDMFSMType GetFSMType() { return CurrentFSMType; }
	FORCEINLINE EDMIdleType GetIdleType() {	return CurrentIdleType; }
};
