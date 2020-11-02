// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enum/EWRActorComponent.h"
#include "Component/Base/WRActorComponent.h"
#include "GameFramework/Actor.h"
#include "WRActorComponentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWRActorComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WR_API IWRActorComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	template <class TComponent>
	TComponent* GetComponent();
	template <class TComponent>
	TComponent* CreateComponent(EWRCharacterComponent InComponentType, bool bInEnableTick = false);
	void RemoveComponent(EWRCharacterComponent InComponentType, bool bInRemoveFromContiner = true);

protected:
	virtual void BuildCustomComponents();
	void RemoveComponentAll();

private:
	TMap<EWRCharacterComponent, class UWRActorComponent*> Components;
};


template <class TComponent>
TComponent* IWRActorComponentInterface::GetComponent()
{
	AActor* OwnerActor = Cast<AActor>(this);
	if (OwnerActor == nullptr)
		return nullptr;

	UActorComponent* FoundActorComponent = OwnerActor->GetComponentByClass(TComponent::StaticClass());
	if (FoundActorComponent == nullptr)
	{
//		UE_LOG(WRCharacter, Warning, TEXT("AWRCharacter::GetComponent - Not found component."));
		return nullptr;
	}

	UWRActorComponent* CastedActorComponent = Cast<UWRActorComponent>(FoundActorComponent);
	if (CastedActorComponent == nullptr)
	{
//		UE_LOG(WRCharacter, Warning, TEXT("AWRCharacter::GetComponent - Not custom ActorComponent Type."));
		return nullptr;
	}

	EWRCharacterComponent ComponentType = CastedActorComponent->GetCharacterComponentType();

	if (Components.Contains(ComponentType) == false)
	{
//		UE_LOG(WRCharacter, Warning, TEXT("AWRCharacter::GetComponent - Have no component"));
		return nullptr;
	}

	return Cast<TComponent>(Components[ComponentType]);
}

template <class TComponent>
TComponent* IWRActorComponentInterface::CreateComponent(EWRCharacterComponent InComponentType, bool bInEnableTick /*= false*/)
{
	AActor* OwnerActor = Cast<AActor>(this);
	if (OwnerActor == nullptr)
		return nullptr;

	if (OwnerActor->IsValidLowLevel() == false)
		return nullptr;

	if (Components.Contains(InComponentType) == true)
	{
//		UE_LOG(WRCharacter, Display, TEXT("AWRCharacter::CreateComponent - Already Created Component [%d]"), (int32)InComponentType);
		return nullptr;
	}

	// Setting Tick
	UWRActorComponent* ActorComponent = NewObject<TComponent>(OwnerActor);
	if (bInEnableTick == true)
	{
		ActorComponent->SetEnableTick(bInEnableTick);
	}

	// Setting Component Type
	ActorComponent->SetCharacterComponentType(InComponentType);
	ActorComponent->AddToRoot();
	ActorComponent->RegisterComponent();

	OwnerActor->AddInstanceComponent(ActorComponent);
	OwnerActor->AddOwnedComponent(ActorComponent);

	Components.Add(InComponentType, ActorComponent);

	return Cast<TComponent>(ActorComponent);
}

