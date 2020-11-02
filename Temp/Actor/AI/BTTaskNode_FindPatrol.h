// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindPatrol.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UBTTaskNode_FindPatrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_FindPatrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
