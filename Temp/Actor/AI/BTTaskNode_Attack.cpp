// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "BTTaskNode_Attack.h"
#include <AIController.h>
#include "WRAIControllerNPC.h"
#include "Enum/EWRCharacter.h"
#include "../Character/WRCharacterNPC.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WREnpcTable.h"
#include <Kismet/KismetMathLibrary.h>
#include "Table/WRSkillTable.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AWRCharacterNPC* Npc = Cast<AWRCharacterNPC>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Npc)
	{
		return EBTNodeResult::Failed;
	}

	FWRNPCBaseTable* NpcBaseTable = Npc->GetNpcBaseTable();
	if (NpcBaseTable && NpcBaseTable->SkillTID.Num() != 0)
	{
		int32 RandomIdx = UKismetMathLibrary::RandomInteger(NpcBaseTable->SkillTID.Num());
		FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(NpcBaseTable->SkillTID[RandomIdx], "SkillEnpc_Atk");
		if (nullptr == SkillTable)
			return EBTNodeResult::Failed;

		WRFSMInfo fsmInfo(EWRFSM::Attack, NpcBaseTable->SkillTID[RandomIdx], 0, 0);
		if (Npc->DetermineFSMSignal(fsmInfo))
		{
			Npc->OnAttackEnd.AddLambda([this]()->void {bAttacking = false; });

			bAttacking = true;
			return EBTNodeResult::InProgress;
		}
	}
	else
	{
		//테스트용
		WRFSMInfo fsmInfo(EWRFSM::Attack, 1004, 0, 0);
		if (Npc->DetermineFSMSignal(fsmInfo))
		{
			Npc->OnAttackEnd.AddLambda([this]()->void {bAttacking = false; });

			bAttacking = true;
			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTaskNode_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
