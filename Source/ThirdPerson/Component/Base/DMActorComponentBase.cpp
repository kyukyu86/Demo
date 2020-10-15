// Fill out your copyright notice in the Description page of Project Settings.


#include "DMActorComponentBase.h"

// Sets default values for this component's properties
UDMActorComponentBase::UDMActorComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDMActorComponentBase::BeginPlay()
{
	Super::BeginPlay();

	OnCreateComponent();	
}


// Called every frame
void UDMActorComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDMActorComponentBase::DestroyComponent(bool bPromoteChildren /*= false*/)
{
	OnDestroyComponent();

	Super::DestroyComponent(bPromoteChildren);
}

