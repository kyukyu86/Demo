// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanAttack.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UBTDecorator_CanAttack : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CanAttack();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AcceptableAttackRadius;
};
