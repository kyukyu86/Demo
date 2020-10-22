// Fill out your copyright notice in the Description page of Project Settings.


#include "DMTargetComponent.h"

void UDMTargetComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UDMTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
}

void UDMTargetComponent::Initialize(AActor* IN InParentActor)
{
	ParentActor = InParentActor;

	// Setup
	SetupAttachment(InParentActor->GetRootComponent());
	this->SetCollisionProfileName(FName("Targeting"));
}

void UDMTargetComponent::OnTargeted()
{

}

void UDMTargetComponent::OnTargetReleased()
{

}
