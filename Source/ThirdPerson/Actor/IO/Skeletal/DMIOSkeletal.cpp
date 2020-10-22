// Fill out your copyright notice in the Description page of Project Settings.


#include "DMIOSkeletal.h"
#include "../../FunctionComponent/DMInteractionComponent.h"


ADMIOSkeletal::ADMIOSkeletal(const FObjectInitializer& ObjectInitializer)
	: ADMIOBase(ObjectInitializer)
{
	// Create Skeletal Mesh
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SkeletalMeshComponent->SetRelativeTransform(FTransform::Identity);

	// Create Interaction Component
// 	DynamicInteractionComponent = CreateDefaultSubobject<UDMInteractionComponentDynamic>(TEXT("DynamicInteractionComponent"));
// 	DynamicInteractionComponent->Initialize(this);
}

void ADMIOSkeletal::BeginPlay()
{
	Super::BeginPlay();

}

void ADMIOSkeletal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
}

void ADMIOSkeletal::BuildCustomComponents()
{
	
}

// Called every frame
void ADMIOSkeletal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADMIOSkeletal::PlayAnimationSequence(UAnimSequenceBase* IN InAnimSequence)
{
	SkeletalMeshComponent->PlayAnimation(InAnimSequence, false);
}

