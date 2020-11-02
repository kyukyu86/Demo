// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponent.h"
#include "Actor/Character/WRCharacter.h"
#include "Actor/Effect/WRVFX.h"
#include "Actor/Vehicle/WRVehicle.h"

// Sets default values for this component's properties
UWRActorComponent::UWRActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWRActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OnCreateComponent();

}


void UWRActorComponent::SetEnableTick(const bool bInEnable)
{

	if (bInEnable == true)
	{
		if (this->PrimaryComponentTick.IsTickFunctionRegistered() == false)
		{
			this->PrimaryComponentTick.RegisterTickFunction(this->GetWorld()->GetCurrentLevel());
			this->PrimaryComponentTick.Target = this;
		}
	}

	this->PrimaryComponentTick.bCanEverTick = bInEnable;
	this->PrimaryComponentTick.SetTickFunctionEnable(bInEnable);

	if (bInEnable == false)
	{
		if (this->PrimaryComponentTick.IsTickFunctionRegistered() == true)
		{
			this->PrimaryComponentTick.UnRegisterTickFunction();
		}
	}

}

// Called every frame
void UWRActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWRActorComponent::DestroyComponent(bool bPromoteChildren /*= false*/)
{
	// ..
	OnDestroyComponent();

	Super::DestroyComponent(bPromoteChildren);
}

class AWRCharacter* UWRActorComponent::GetOwnerCharacter()
{
	return Cast<AWRCharacter>(GetOwner());
}

class AWRVFX* UWRActorComponent::GetOwnerVFX()
{
	return Cast<AWRVFX>(GetOwner());
}

class AWRVehicle* UWRActorComponent::GetOwnerVehicle()
{
	return Cast<AWRVehicle>(GetOwner());
}