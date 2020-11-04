// Fill out your copyright notice in the Description page of Project Settings.


#include "DMComponentBase.h"

// Sets default values for this component's properties
UDMComponentBase::UDMComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDMComponentBase::BeginPlay()
{
	Super::BeginPlay();

	OnCreateComponent();	
}


// Called every frame
void UDMComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDMComponentBase::DestroyComponent(bool bPromoteChildren /*= false*/)
{
	OnDestroyComponent();

	Super::DestroyComponent(bPromoteChildren);
}

