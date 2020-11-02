// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "BTDecorator_PossibleAI.h"
#include <AIController.h>
#include "../Character/WRCharacterPlayer.h"
#include "WRAIControllerNPC.h"
#include <BrainComponent.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "../Character/WRCharacterNPC.h"

bool UBTDecorator_PossibleAI::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AWRCharacterNPC* ControllingPawn = Cast<AWRCharacterNPC>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	if (ControllingPawn->IsAlive() == false)
	{
		return false;
	}

	if (ControllingPawn->bSpawnedByServer)
	{
		return false;
	}

	return bResult;
}
