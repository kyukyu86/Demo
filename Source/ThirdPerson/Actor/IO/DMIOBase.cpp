// Fill out your copyright notice in the Description page of Project Settings.


#include "DMIOBase.h"


ADMIOBase::ADMIOBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create Root Component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	if (RootComponent == nullptr)
		return;

	// Create Interaction Component
// 	InteractionComponent = CreateDefaultSubobject<UDMInteractionComponent>(TEXT("InteractionComponent"));
// 	InteractionComponent->Initialize(this);

	// Create Target Component
// 	TargetComponent = CreateDefaultSubobject<UDMTargetComponent>(TEXT("TargetComponent"));
// 	TargetComponent->Initialize(this);
}

void ADMIOBase::BeginPlay()
{
	Super::BeginPlay();
	
	BuildCustomComponents();

	TArray<UObject*> InteractionComponents;
	GetDefaultSubobjects(InteractionComponents);
	for (auto Object : InteractionComponents)
	{
		UDMInteractionComponent* CastedObject = Cast<UDMInteractionComponent>(Object);
		if (CastedObject)
			CastedObject->Initialize(this);

		InteractionComponentList.Add(CastedObject);
	}
}

void ADMIOBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveComponentAll();

// 	if (InteractionComponent)
// 	{
// 		InteractionComponent->RemoveFromRoot();
// 		this->RemoveOwnedComponent(InteractionComponent);
// 		InteractionComponent->DestroyComponent();
// 	}

	Super::EndPlay(EndPlayReason);
}

void ADMIOBase::BuildCustomComponents()
{

}

void ADMIOBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

