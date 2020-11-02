// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_TurnTo.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UBTTaskNode_TurnTo : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_TurnTo();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
