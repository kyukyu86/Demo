// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMUIManager.h"
#include "Singleton/DMSingleton.h"
#include "../Enum/DMActorEnum.h"


class ADMCharacterMine;
struct FDMAttackTable;
struct FDMMontageTable;

class THIRDPERSON_API DMCharacterManager : public DMSingleton<DMCharacterManager>
{
private:
	ADMCharacterMine* MyCharacter = nullptr;
	FString strAsyncKey = "";

	TAssetPtr<UDataTable> AttackTable = nullptr;
	TAssetPtr<UDataTable> MontageTable = nullptr;

public:
	DMCharacterManager();
	virtual ~DMCharacterManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;

	// FSM
	void ChangeFSM(EDMFSMType IN InNewFSMType);

	// Character
	FORCEINLINE void SetMyCharacter(ADMCharacterMine* IN InCharacter) { MyCharacter = InCharacter; }
	FORCEINLINE ADMCharacterMine* GetMyCharacter() { return MyCharacter; }
	void SpawnMyCharacter();

	// Actor
	bool IsInnerActor(AActor* IN InActor, const float IN InDistance);

	// Target
	AActor* GetMyTargetActor();
	bool IsTargetedActor(AActor* IN InActor);

	// Battle
	FDMAttackTable* GetAttackTable(const FName IN InRowName);
	FDMMontageTable* GetMontageTable(const FName IN InRowName);
};
