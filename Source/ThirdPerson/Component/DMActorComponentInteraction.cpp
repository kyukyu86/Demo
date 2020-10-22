// Fill out your copyright notice in the Description page of Project Settings.


#include "DMActorComponentInteraction.h"
#include "../Manager/DMFSMManager.h"
#include "../Actor/Character/Player/DMCharacterMine.h"
#include "../Actor/FunctionComponent/DMInteractionComponent.h"

void UDMActorComponentInteraction::BeginPlay()
{
	Super::BeginPlay();

}

void UDMActorComponentInteraction::OnCreateComponent()
{
	
}

void UDMActorComponentInteraction::OnDestroyComponent()
{
	
}

void UDMActorComponentInteraction::AddInteractionableComponent(UDMInteractionComponent* IN InComponent)
{
	if (InteractionableComponentList.Contains(InComponent))
		return;

	InteractionableComponentList.AddTail(InComponent);
}

void UDMActorComponentInteraction::RemoveInteractionableComponent(UDMInteractionComponent* IN InComponent)
{
	if (InteractionableComponentList.Contains(InComponent) == false)
		return;

	InteractionableComponentList.RemoveNode(InComponent);
}

bool UDMActorComponentInteraction::IsInteractionableComponent(UDMInteractionComponent* IN InComponent)
{
	if (InteractionableComponentList.Contains(InComponent))
		return true;

	return false;
}

bool UDMActorComponentInteraction::OnInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	for (auto EachInteractionComponent : InteractionableComponentList)
	{
		UDMInteractionComponent* InteractionComponent = EachInteractionComponent;
		if(InteractionComponent == nullptr)
			continue;

		if (InteractionComponent->OnInputEvent(InInputType, InEventType))
			return true;		
	}

	return false;
}
