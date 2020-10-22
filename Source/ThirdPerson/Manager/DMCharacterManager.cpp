// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterManager.h"
#include "DMAsyncLoadManager.h"

#include "../Util/DMEnumUtil.h"
#include "../GameInstance/DMGameInstance.h"
#include "../Actor/Character/Player/DMCharacterMine.h"
#include "../Actor/FunctionComponent/DMInteractionComponent.h"

#include <GameFramework/PlayerStart.h>
#include <EngineUtils.h>


DMCharacterManager::DMCharacterManager()
{
	
}

DMCharacterManager::~DMCharacterManager()
{

}

void DMCharacterManager::OnInit()
{

}

void DMCharacterManager::OnShutdown()
{

}

void DMCharacterManager::ChangeFSM(EDMFSMType IN InNewFSMType)
{

}

void DMCharacterManager::SpawnMyCharacter()
{
	auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		ADMCharacterMine* CastedMine = Cast<ADMCharacterMine>(InObject);
		SetMyCharacter(CastedMine);

		FVector SpawnLocation;
		FRotator SpawnRotation;
		for (TActorIterator<APlayerStart> It(UDMGameInstance::GetGameInstance()->GetWorld()); It; ++It)
		{
			SpawnLocation = It->GetActorLocation();
			SpawnRotation = It->GetActorRotation();
		}
		CastedMine->SetActorLocation(SpawnLocation);
		CastedMine->SetActorRotation(SpawnRotation);

		APlayerController* PlayerController = CastedMine->GetWorld()->GetFirstPlayerController();
		PlayerController->Possess(CastedMine);
	});
	
	FString MyCharPath = "/Game/Blueprints/Actor/Character/Player/Player_01_BP.Player_01_BP_C";
	strAsyncKey = DMAsyncLoadManager::Get()->AsyncSpawnActor(MyCharPath, AsyncCreateComplete);
}

bool DMCharacterManager::IsInnerActor(AActor* IN InActor, const float IN InDistance)
{
	if (MyCharacter == nullptr)
		return false;

	FVector MyCharacterLocation = MyCharacter->GetActorLocation();
	FVector OtherActorLocation = InActor->GetActorLocation();
	float fActorDistance = FVector::Distance(MyCharacterLocation, OtherActorLocation);
	if (fActorDistance < InDistance)
		return true;
	
	return false;
}

AActor* DMCharacterManager::GetMyTargetActor()
{
	if (MyCharacter == nullptr)
		return nullptr;

	UDMInteractionComponent* TargetComponent = MyCharacter->GetTargetedComponent();
	if (TargetComponent == nullptr)
		return nullptr;

	return TargetComponent->GetParentActor();
}

bool DMCharacterManager::IsTargetedActor(AActor* IN InActor)
{
	if (MyCharacter == nullptr)
		return false;

	UDMInteractionComponent* TargetComponent = MyCharacter->GetTargetedComponent();
	if (TargetComponent == nullptr)
		return false;

	return TargetComponent->GetParentActor() == InActor;
}
