// Fill out your copyright notice in the Description page of Project Settings.


#include "DMIOStatic.h"
#include <Components/StaticMeshComponent.h>
#include "../../FunctionComponent/DMInteractionComponent.h"


ADMIOStatic::ADMIOStatic(const FObjectInitializer& ObjectInitializer) 
	: ADMIOBase(ObjectInitializer)
{
	// Create Static Mesh
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	StaticMeshComponent->SetRelativeTransform(FTransform::Identity);

	// Create Interaction Component
// 	InteractionComponent = CreateDefaultSubobject<UDMInteractionComponentStatic>(TEXT("InteractionComponent"));
// 	InteractionComponent->Initialize(this);
}

void ADMIOStatic::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADMIOStatic::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
}

void ADMIOStatic::BuildCustomComponents()
{
	
}

// Called every frame
void ADMIOStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

