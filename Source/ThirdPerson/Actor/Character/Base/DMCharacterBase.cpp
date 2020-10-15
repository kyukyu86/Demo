// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterBase.h"

// Sets default values
ADMCharacterBase::ADMCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADMCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADMCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

