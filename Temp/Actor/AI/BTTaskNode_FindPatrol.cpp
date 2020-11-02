// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "BTTaskNode_FindPatrol.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <GameFramework/Controller.h>
#include <BrainComponent.h>
#include <AIController.h>
#include <NavigationSystem.h>
#include "WRAIControllerNPC.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <DrawDebugHelpers.h>

UBTTaskNode_FindPatrol::UBTTaskNode_FindPatrol()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTaskNode_FindPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AWRAIControllerNPC::HomePosKey);
	FNavLocation NextPatrol;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, 1000.0f, NextPatrol))
	{
		UWorld* World = ControllingPawn->GetWorld();
//		DrawDebugSphere(World, NextPatrol, 20.0f, 16, FColor::Green, false, 4.0f);


		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AWRAIControllerNPC::PatrolPosKey, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
