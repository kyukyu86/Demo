// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "BTDecorator_CanAttack.h"
#include <AIController.h>
#include "../Character/WRCharacterPlayer.h"
#include "WRAIControllerNPC.h"
#include <BrainComponent.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "Component/Character/WRActorComponentFSM.h"

UBTDecorator_CanAttack::UBTDecorator_CanAttack()
{
	NodeName = TEXT("CanAttack");
	AcceptableAttackRadius = 200.0f;
}

bool UBTDecorator_CanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AWRCharacter* ControllingPawn = Cast<AWRCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	UWRActorComponentFSM* FSMComponent = ControllingPawn->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return false;

	if (FSMComponent->IsPossibleEndCurrentFSM(EWRFSM::Attack) == false)
		return false;

	auto Target = Cast<AWRCharacterPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AWRAIControllerNPC::TargetKey));
	if (nullptr == Target)
	{
		return false;
	}

	float fDistance = Target->GetHorizontalDistanceTo(ControllingPawn);
	bResult = (fDistance <= AcceptableAttackRadius);

	return bResult;
}
