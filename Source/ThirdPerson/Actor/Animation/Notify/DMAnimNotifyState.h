// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DMAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMAnimNotifyState_IgnoreInput : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};

UCLASS()
class THIRDPERSON_API UDMAnimNotifyState_IgnoreAction : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};
