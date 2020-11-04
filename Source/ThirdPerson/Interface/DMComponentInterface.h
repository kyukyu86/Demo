// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Enum/DMActorEnum.h"
#include "../Component/Base/DMComponentBase.h"
#include "DMComponentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDMComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THIRDPERSON_API IDMComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	template <class TComponent>
	TComponent* GetComponent();
	template <class TComponent>
	TComponent* CreateComponent(EDMComponentType InComponentType, bool bInEnableTick = false);
	void RemoveComponent(EDMComponentType InComponentType, bool bInRemoveFromContiner = true);

protected:
	virtual void BuildCustomComponents();
	void RemoveComponentAll();

private:
	TMap<EDMComponentType, class UDMComponentBase*> Components;
};



template <class TComponent>
TComponent* IDMComponentInterface::GetComponent()
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

 	UDMComponentBase* CastedActorComponent = Cast<UDMComponentBase>(FoundActorComponent);
 	if (CastedActorComponent == nullptr)
 	{
 //		UE_LOG(WRCharacter, Warning, TEXT("AWRCharacter::GetComponent - Not custom ActorComponent Type."));
 		return nullptr;
 	}
 
 	EDMComponentType ComponentType = CastedActorComponent->GetActorComponentType();
 
 	if (Components.Contains(ComponentType) == false)
 	{
 //		UE_LOG(WRCharacter, Warning, TEXT("AWRCharacter::GetComponent - Have no component"));
 		return nullptr;
 	}
 
 	return Cast<TComponent>(Components[ComponentType]);
}

template <class TComponent>
TComponent* IDMComponentInterface::CreateComponent(EDMComponentType InComponentType, bool bInEnableTick /*= false*/)
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
	UDMComponentBase* ActorComponent = NewObject<TComponent>(OwnerActor);
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

