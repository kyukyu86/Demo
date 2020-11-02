// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentInterface.h"

// Add default functionality here for any IWRActorComponentInterface functions that are not pure virtual.

void IWRActorComponentInterface::RemoveComponent(EWRCharacterComponent InComponentType, bool bInRemoveFromContiner /*= true*/)
{
	AActor* OwnerActor = Cast<AActor>(this);
	if (OwnerActor == nullptr)
		return;

	if (Components.Contains(InComponentType) == false)
		return;

	UWRActorComponent* ActorComponent = Components[InComponentType];
	if (ActorComponent->IsValidLowLevel() == false)
		return;

	if (ActorComponent->IsRegistered() == true)
		ActorComponent->UnregisterComponent();

	ActorComponent->RemoveFromRoot();

	OwnerActor->RemoveOwnedComponent(ActorComponent);

	ActorComponent->DestroyComponent();

	bInRemoveFromContiner == true ? Components.Remove(InComponentType) : 0;
}

void IWRActorComponentInterface::BuildCustomComponents()
{

}

void IWRActorComponentInterface::RemoveComponentAll()
{
	for (auto It = Components.CreateIterator(); It; ++It)
	{
		this->RemoveComponent(It.Key(), false);
	}
	Components.Empty();
}
