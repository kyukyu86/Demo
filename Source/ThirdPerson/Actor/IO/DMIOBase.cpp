// Fill out your copyright notice in the Description page of Project Settings.


#include "DMIOBase.h"
#include "../FunctionComponent/DMInteractionComponent.h"


ADMIOBase::ADMIOBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Create Root Component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	if (RootComponent == nullptr)
		return;

	// Create Interaction Component
	InteractionComponent = CreateDefaultSubobject<UDMInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->Initialize(this);
}

void ADMIOBase::BeginPlay()
{
	Super::BeginPlay();
	
	BuildCustomComponents();
}

void ADMIOBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveComponentAll();

	Super::EndPlay(EndPlayReason);
}

void ADMIOBase::BuildCustomComponents()
{

}

void ADMIOBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

