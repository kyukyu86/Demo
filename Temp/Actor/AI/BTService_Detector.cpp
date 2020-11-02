// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "BTService_Detector.h"
#include <AIController.h>
#include <GameFramework/Controller.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <Engine/World.h>
#include <DrawDebugHelpers.h>
#include "../Character/WRCharacter.h"
#include "../Character/WRCharacterPlayer.h"
#include "WRAIControllerNPC.h"
#include <BrainComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "Manager/WRCharacterManager.h"

UBTService_Detector::UBTService_Detector()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detector::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 1500.0f;

	if (nullptr == World)
	{
		return;
	}
	
	TArray<AWRCharacter*> TypeResult;
	WRCharacterManager::Get()->GetCharactersByType(EWRCharacterType::Player, TypeResult);
	if (TypeResult.Num() != 0)
	{
		AWRCharacter* Player = TypeResult[0];
		if (Player)
		{
			if (Player->GetDistanceTo(ControllingPawn) <= DetectRadius)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AWRAIControllerNPC::TargetKey, Player);

				AWRCharacter* ControllingActor = Cast< AWRCharacter>(ControllingPawn);
				if (ControllingActor)
				{
					ControllingActor->SetTargetAID(Player->GetActorID());
				}

				//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.5f);

				//DrawDebugPoint(World, Player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.5f);

				//DrawDebugLine(World, ControllingPawn->GetActorLocation(), Player->GetActorLocation(), FColor::Blue, false, 0.5f);

				return;
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AWRAIControllerNPC::TargetKey, nullptr);
	
	//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.5f);
}
