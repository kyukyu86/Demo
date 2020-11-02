// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "BTTask_Move.h"
#include "../Character/WRCharacter.h"
#include <AIController.h>
#include "Component/Character/WRActorComponentFSM.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "WRAIControllerNPC.h"
#include "Component/Character/WRActorComponentDynamicMovement.h"
#include "Utility/WRActorUtility.h"

UBTTask_Move::UBTTask_Move(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	NodeName = TEXT("WRMoveTo");
	AcceptableRadius = 250.0f;
}

EBTNodeResult::Type UBTTask_Move::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AWRCharacter* ControllingPawn = Cast<AWRCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
		return EBTNodeResult::Failed;

	UWRActorComponentFSM* FSMComponent = ControllingPawn->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return EBTNodeResult::Failed;

	if (FSMComponent->IsPossibleEndCurrentFSM(EWRFSM::Move) == false)
		return EBTNodeResult::Failed;

	if (ControllingPawn->GetCurrentFSMType() != EWRFSM::Move)
	{
		ControllingPawn->DetermineFSMSignal(EWRFSM::Move);
	}

	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	switch (Result)
	{
	case EBTNodeResult::Failed:
	case EBTNodeResult::Aborted:
	{
		ControllingPawn->DetermineFSMSignal(EWRFSM::Idle);
	}
	break;
	case EBTNodeResult::Succeeded:
	case EBTNodeResult::InProgress:
	default:
	{
		int32 aa = 0;
	}
	break;
	}

	return EBTNodeResult::Succeeded;
}

UBTTask_SplineMove::UBTTask_SplineMove(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("WRSplineMove");
}

EBTNodeResult::Type UBTTask_SplineMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AWRCharacter* ControllingPawn = Cast<AWRCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
		return EBTNodeResult::Failed;
	
	UWRActorComponentDynamicMovement* DM = ControllingPawn->GetComponent<UWRActorComponentDynamicMovement>();
	if (DM)
	{
		DM->DetermineDynamicMovement(1, ControllingPawn);
	}

	return EBTNodeResult::Succeeded;
}
