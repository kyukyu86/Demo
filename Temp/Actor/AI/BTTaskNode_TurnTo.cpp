// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "BTTaskNode_TurnTo.h"
#include "../Character/WRCharacter.h"
#include <AIController.h>
#include "WRAIControllerNPC.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <RotationMatrix.h>
#include <Engine/World.h>
#include "Core/WRGameInstance.h"

UBTTaskNode_TurnTo::UBTTaskNode_TurnTo()
{
	NodeName = TEXT("TurnTo");
}

EBTNodeResult::Type UBTTaskNode_TurnTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == OwnerCharacter)
	{
		return EBTNodeResult::Failed;
	}

	AWRCharacter* Target = Cast<AWRCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AWRAIControllerNPC::TargetKey));
	if (nullptr == Target)
	{
		return EBTNodeResult::Failed;
	}

	FVector LookVector = Target->GetActorLocation() - OwnerCharacter->GetActorLocation();
	LookVector.Z = 0.0f;

	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	OwnerCharacter->SetRotation(FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRot, UWRGameInstance::GetGameInstance()->GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}
