// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/DMComponentBase.h"
#include "DMComponentInteraction.generated.h"


class UDMInteractionComponent;

UCLASS()
class THIRDPERSON_API UDMComponentInteraction : public UDMComponentBase
{
	GENERATED_BODY()
	
private:
	TDoubleLinkedList<UDMInteractionComponent*> InteractionableComponentList;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnCreateComponent();
	virtual void OnDestroyComponent();

public:
	void AddInteractionableComponent(UDMInteractionComponent* IN InComponent);
	void RemoveInteractionableComponent(UDMInteractionComponent* IN InComponent);
	bool IsInteractionableComponent(UDMInteractionComponent* IN InComponent);

	bool OnInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);
};
