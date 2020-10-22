// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterMonster.h"
#include "../../../Component/DMActorComponentFSM.h"



ADMCharacterMonster::ADMCharacterMonster() : ADMCharacterBase()
{
	
}

void ADMCharacterMonster::BeginPlay()
{
	Super::BeginPlay();

}

void ADMCharacterMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
}

void ADMCharacterMonster::BuildCustomComponents()
{
	Super::BuildCustomComponents();

}