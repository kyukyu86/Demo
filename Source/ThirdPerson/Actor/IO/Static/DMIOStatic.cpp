// Fill out your copyright notice in the Description page of Project Settings.


#include "DMIOStatic.h"


ADMIOStatic::ADMIOStatic(const FObjectInitializer& ObjectInitializer) 
	: ADMIOBase(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Create Static Mesh
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	StaticMeshComponent->SetRelativeTransform(FTransform::Identity);
}

void ADMIOStatic::BeginPlay()
{
	Super::BeginPlay();
	
	BuildCustomComponents();
}

void ADMIOStatic::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveComponentAll();
}

void ADMIOStatic::BuildCustomComponents()
{
	
}

// Called every frame
void ADMIOStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

