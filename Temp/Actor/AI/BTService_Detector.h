// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detector.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UBTService_Detector : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detector();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
