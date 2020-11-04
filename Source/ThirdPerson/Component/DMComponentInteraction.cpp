// Fill out your copyright notice in the Description page of Project Settings.


#include "DMComponentInteraction.h"
#include "../Manager/DMFSMManager.h"
#include "../Actor/Character/Player/DMCharacterMine.h"
#include "../Actor/FunctionComponent/DMInteractionComponent.h"

void UDMComponentInteraction::BeginPlay()
{
	Super::BeginPlay();

}

void UDMComponentInteraction::OnCreateComponent()
{
	
}

void UDMComponentInteraction::OnDestroyComponent()
{
	
}

void UDMComponentInteraction::AddInteractionableComponent(UDMInteractionComponent* IN InComponent)
{
	if (InteractionableComponentList.Contains(InComponent))
		return;

	InteractionableComponentList.AddTail(InComponent);
}

void UDMComponentInteraction::RemoveInteractionableComponent(UDMInteractionComponent* IN InComponent)
{
	if (InteractionableComponentList.Contains(InComponent) == false)
		return;

	InteractionableComponentList.RemoveNode(InComponent);
}

bool UDMComponentInteraction::IsInteractionableComponent(UDMInteractionComponent* IN InComponent)
{
	if (InteractionableComponentList.Contains(InComponent))
		return true;

	return false;
}

bool UDMComponentInteraction::OnInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
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
