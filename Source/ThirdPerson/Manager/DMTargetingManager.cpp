// Fill out your copyright notice in the Description page of Project Settings.


#include "DMTargetingManager.h"
#include "../GameInstance/DMGameInstance.h"
#include "../Actor/Character/Player/DMCharacterMine.h"
#include "DMCharacterManager.h"
#include <DrawDebugHelpers.h>
#include "../Actor/FunctionComponent/DMInteractionComponent.h"


DMTargetingManager::DMTargetingManager()
{
	
}

DMTargetingManager::~DMTargetingManager()
{

}

void DMTargetingManager::OnInit()
{

}

void DMTargetingManager::OnShutdown()
{
	
}

bool DMTargetingManager::OnTick(float InDeltaTime)
{
	UpdateTarget();

	return true;
}

void DMTargetingManager::UpdateTarget()
{
	ADMCharacterMine* MyCharacter = DMCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	APlayerController* PlayerController = UDMGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	FVector TraceLocation = MyCharacter->GetActorLocation();
	TraceLocation.Z += 20.f;
	FVector TraceForward = MyCharacter->GetActorForwardVector();
	float RayLength = 100.f;

	FVector StartLocation = TraceLocation;
	FVector EndLocation = StartLocation + (TraceForward * RayLength);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(MyCharacter);

// 	TArray<AWRCharacter*> DeadCharacters = WRCharacterManager::Get()->GetDeadCharacters();
// 	for (int i = 0; i < DeadCharacters.Num(); ++i)
// 		CollisionParams.AddIgnoredActor(DeadCharacters[i]);

	UWorld* pWorld = UDMGameInstance::GetGameInstance()->GetWorld();
	//FlushPersistentDebugLines(pWorld);
	DrawDebugLine(pWorld, StartLocation, EndLocation, FColor::Green, false, 0.05f, 0, 5.f);
	
	FHitResult Hit(1.0f);
	if (pWorld->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_GameTraceChannel1, CollisionParams) == true)
	{
		UPrimitiveComponent* HittedComponent = Hit.Component.Get();
		UDMInteractionComponent* CastedComponent = Cast<UDMInteractionComponent>(HittedComponent);
		if (CastedComponent == nullptr)
			return;

		MyCharacter->SetTargetedComponent(CastedComponent);
	}
	else
	{
		MyCharacter->SetTargetedComponent(nullptr);
	}
}