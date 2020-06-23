// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameModeField.h"
#include "../ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADMGameModeField::ADMGameModeField()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ADMGameModeField::StartPlay()
{

}
