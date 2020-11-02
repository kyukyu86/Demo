// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WRAnimNotifyState.generated.h"

/**
 * 
 */

UCLASS()
class WR_API UWRAnimNotifyState_Blocking : public UAnimNotifyState
{
	GENERATED_BODY()
		
public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};


UCLASS()
class WR_API UWRAnimNotifyState_LeftAttack: public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};

UCLASS()
class WR_API UWRAnimNotifyState_RightAttack: public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};

UCLASS()
class WR_API UWRAnimNotifyState_Parrying: public UAnimNotifyState
{
public:
	GENERATED_BODY()

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};

UCLASS()
class WR_API UWRAnimNotifyState_RightAttackingCollision: public UAnimNotifyState
{
public:
	GENERATED_BODY()

public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};

UCLASS()
class WR_API UWRAnimNotifyState_LeftAttackingCollision : public UAnimNotifyState
{
public:
	GENERATED_BODY()

public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;

};

UCLASS()
class WR_API UWRAnimNotifyState_ForceFeedback : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotifyState_ForceFeedback")
	int32 ForceFeedbackTID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotifyState_ForceFeedback")
	bool BroadCast = true;
};

UCLASS()
class WR_API UWRAnimNotifyState_InteractionFABRIK : public UAnimNotifyState
{
public:
	GENERATED_BODY()

public:
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime);
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotifyState_InteractionFABRIK")
		bool LHandIK = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotifyState_InteractionFABRIK")
		bool RHandIK = true;
private:
	void UpdateFABRIK(FBranchingPointNotifyPayload& BranchingPointPayload);
};