// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_Move.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UBTTask_Move : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_Move(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};


UCLASS()
class WR_API UBTTask_SplineMove : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_SplineMove(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
