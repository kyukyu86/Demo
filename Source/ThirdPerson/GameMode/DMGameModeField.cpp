// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameModeField.h"
#include "../ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "../Manager/DMUIManager.h"
#include "../Manager/DMCharacterManager.h"

ADMGameModeField::ADMGameModeField()
{
	// set default pawn class to our Blueprinted character
// 	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Actor/Character/Player/Player_01_BP.Player_01_BP_C"));
// 	if (PlayerPawnBPClass.Class != NULL)
// 	{
// 		DefaultPawnClass = PlayerPawnBPClass.Class;
// 	}
}

void ADMGameModeField::StartPlay()
{
	Super::StartPlay();

	DMUIManager::Get()->OpenPanel(EDMPanelKind::Main);
	DMCharacterManager::Get()->SpawnMyCharacter();
}
