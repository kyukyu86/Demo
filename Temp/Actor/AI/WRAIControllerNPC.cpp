// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAIControllerNPC.h"
#include <ConstructorHelpers.h>
#include <BehaviorTree/BlackboardData.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>

const FName AWRAIControllerNPC::HomePosKey(TEXT("HomePos"));
const FName AWRAIControllerNPC::PatrolPosKey(TEXT("PatrolPos"));
const FName AWRAIControllerNPC::TargetKey(TEXT("Target"));

AWRAIControllerNPC::AWRAIControllerNPC()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Asset/BluePrint/AI/SimpleNPCAI_Blackboard.SimpleNPCAI_Blackboard"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

 	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Asset/BluePrint/AI/SimpleNPCAI_BehaviorTree.SimpleNPCAI_BehaviorTree"));
	if (BTObject.Succeeded())
	{
		BTreeAsset = BTObject.Object;
	}

}

void AWRAIControllerNPC::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());

		if (RunBehaviorTree(BTreeAsset))
		{
			CreatePersistentKey();
		}
	}
}

void AWRAIControllerNPC::OnUnPossess()
{
	Super::OnUnPossess();

// 	FVector tt(1, 2, 3);
// 	Blackboard->SetValueAsVector(SpawnedPosition, tt);
// 
// 
// 	FVector aa = Blackboard->GetValue<UBlackboardKeyType_Vector>(SpawnedPosition);
// 
// 	FVector bb = GetKeyValue<UBlackboardKeyType_Vector>(SpawnedPosition);
// 
// 	BBAsset->Keys.Reset();
}

void AWRAIControllerNPC::CreatePersistentKey()
{
	//UBlackboardKeyType_Vector* vecte = BBAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("SpawnedPositon"));

	//SpawnedPosition

}
