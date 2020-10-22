// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "../../../Manager/DMUIManager.h"
#include "../../../Component/DMActorComponentFSM.h"



ADMCharacterPlayer::ADMCharacterPlayer() : ADMCharacterBase()
{

}

void ADMCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

}

void ADMCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	Super::EndPlay(EndPlayReason);
}

void ADMCharacterPlayer::BuildCustomComponents()
{
	Super::BuildCustomComponents();

}