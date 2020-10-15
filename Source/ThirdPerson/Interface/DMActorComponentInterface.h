// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Enum/DMActorEnum.h"
#include "../Component/Base/DMActorComponentBase.h"
#include "DMActorComponentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDMActorComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THIRDPERSON_API IDMActorComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	template <class TComponent>
	TComponent* GetComponent();
	template <class TComponent>
	TComponent* CreateComponent(EDMActorComponentType InComponentType, bool bInEnableTick = false);
	void RemoveComponent(EDMActorComponentType InComponentType, bool bInRemoveFromContiner = true);

protected:
	virtual void BuildCustomComponents();
	void RemoveComponentAll();

private:
	TMap<EDMActorComponentType, class UDMActorComponentBase*> Components;
};



template <class TComponent>
TComponent* IDMActorComponentInterface::GetComponent()
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

 	UDMActorComponentBase* CastedActorComponent = Cast<UDMActorComponentBase>(FoundActorComponent);
 	if (CastedActorComponent == nullptr)
 	{
 //		UE_LOG(WRCharacter, Warning, TEXT("AWRCharacter::GetComponent - Not custom ActorComponent Type."));
 		return nullptr;
 	}
 
 	EDMActorComponentType ComponentType = CastedActorComponent->GetActorComponentType();
 
 	if (Components.Contains(ComponentType) == false)
 	{
 //		UE_LOG(WRCharacter, Warning, TEXT("AWRCharacter::GetComponent - Have no component"));
 		return nullptr;
 	}
 
 	return Cast<TComponent>(Components[ComponentType]);
}

template <class TComponent>
TComponent* IDMActorComponentInterface::CreateComponent(EDMActorComponentType InComponentType, bool bInEnableTick /*= false*/)
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
	UDMActorComponentBase* ActorComponent = NewObject<TComponent>(OwnerActor);
 	if (bInEnableTick == true)
 	{
 		ActorComponent->SetComponentTickEnabled(bInEnableTick);
 	}
 
 	// Setting Component Type
 	ActorComponent->SetActorComponentType(InComponentType);
 	ActorComponent->AddToRoot();
 	ActorComponent->RegisterComponent();
 
 	OwnerActor->AddInstanceComponent(ActorComponent);
 	OwnerActor->AddOwnedComponent(ActorComponent);
 
 	Components.Add(InComponentType, ActorComponent);
 
 	return Cast<TComponent>(ActorComponent);
}

