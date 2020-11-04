// Fill out your copyright notice in the Description page of Project Settings.


#include "DMComponentInterface.h"

// Add default functionality here for any IDMInterfaceActorComponent functions that are not pure virtual.

void IDMComponentInterface::RemoveComponent(EDMComponentType InComponentType, bool bInRemoveFromContiner /*= true*/)
{
	AActor* OwnerActor = Cast<AActor>(this);
	if (OwnerActor == nullptr)
		return;

	if (Components.Contains(InComponentType) == false)
		return;

	UDMComponentBase* ActorComponent = Components[InComponentType];
	if (ActorComponent->IsValidLowLevel() == false)
		return;

	if (ActorComponent->IsRegistered() == true)
		ActorComponent->UnregisterComponent();

	ActorComponent->RemoveFromRoot();

	OwnerActor->RemoveOwnedComponent(ActorComponent);

	ActorComponent->DestroyComponent();

	bInRemoveFromContiner == true ? Components.Remove(InComponentType) : 0;
}

void IDMComponentInterface::BuildCustomComponents()
{

}

void IDMComponentInterface::RemoveComponentAll()
{
	for (auto It = Components.CreateIterator(); It; ++It)
	{
		this->RemoveComponent(It.Key(), false);
	}
	Components.Empty();
}
