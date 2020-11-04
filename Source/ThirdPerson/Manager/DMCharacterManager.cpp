// Fill out your copyright notice in the Description page of Project Settings.

#include "DMCharacterManager.h"
#include "DMAsyncLoadManager.h"

#include "../Util/DMEnumUtil.h"
#include "../GameInstance/DMGameInstance.h"
#include "../Actor/Character/Player/DMCharacterMine.h"
#include "../Actor/FunctionComponent/DMInteractionComponent.h"
#include "../Data/CustomData/DMAttackTable.h"
#include "../Data/CustomData/DMMontageTable.h"

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
	FStringAssetReference rAssetRef = FStringAssetReference(DEF_ATTACK_TABLE_PATH);
	rAssetRef.TryLoad();
	UObject* pObject = rAssetRef.ResolveObject();
	if (pObject)
	{
		AttackTable = Cast<UDataTable>(pObject);
		AttackTable.Get()->AddToRoot();
	}

	rAssetRef = FStringAssetReference(DEF_MONTAGE_TABLE_PATH);
	rAssetRef.TryLoad();
	pObject = rAssetRef.ResolveObject();
	if (pObject)
	{
		MontageTable = Cast<UDataTable>(pObject);
		MontageTable.Get()->AddToRoot();
	}
}

void DMCharacterManager::OnShutdown()
{
	if (AttackTable->IsValidLowLevel() && AttackTable->IsRooted())
	{
		AttackTable->RemoveFromRoot();
	}
	if (MontageTable->IsValidLowLevel() && MontageTable->IsRooted())
	{
		MontageTable->RemoveFromRoot();
	}
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

FDMAttackTable* DMCharacterManager::GetAttackTable(const FName IN InRowName)
{
	if (AttackTable == nullptr)
		return nullptr;

	FString strContext = "";
	FDMAttackTable* FoundTable = AttackTable->FindRow<FDMAttackTable>(InRowName, strContext);
	if (FoundTable == nullptr)
		return nullptr;

	return FoundTable;
}

FDMMontageTable* DMCharacterManager::GetMontageTable(const FName IN InRowName)
{
	if (MontageTable == nullptr)
		return nullptr;

	FString strContext = "";
	FDMMontageTable* FoundTable = MontageTable->FindRow<FDMMontageTable>(InRowName, strContext);
	if (FoundTable == nullptr)
		return nullptr;

	return FoundTable;
}
