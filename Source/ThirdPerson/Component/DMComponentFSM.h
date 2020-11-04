// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/DMComponentBase.h"
#include "../FSM/DMFSMBase.h"
#include "../FSM/Idle/DMFSMIdleBase.h"
#include "../Enum/DMActorEnum.h"
#include "../Struct/DMActorStruct.h"
#include "DMComponentFSM.generated.h"


class UAnimMontage;

UCLASS()
class THIRDPERSON_API UDMComponentFSM : public UDMComponentBase
{
	GENERATED_BODY()
	
private:
	EDMIdleType CurrentIdleType = EDMIdleType::Stand;
	EDMIdleType ReservedIdleType = EDMIdleType::None;

	FDMFSMData CurrentFSMData;
	FDMFSMData ReservedFSMData;

	DMFSMBase* CurrentFSM = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnCreateComponent();
	virtual void OnDestroyComponent();

public:
	bool DetermineFSM(FDMFSMData IN InNewFSM);
	void OnMontageEnded(UAnimMontage* InMontage, bool InbInterrupted);

	FORCEINLINE EDMFSMType GetFSMType() { return CurrentFSMData.FSMType; }
	FORCEINLINE EDMIdleType GetIdleType() {	return CurrentIdleType; }
	DMFSMBase* GetCurrentFSM() { return CurrentFSM; }
	void GetCurrentFSMData(FDMFSMData& OUT OutFSMData);
};
