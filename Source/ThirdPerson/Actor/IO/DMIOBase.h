// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interface/DMInterfaceComponentManagement.h"
#include "DMIOBase.generated.h"


class UDMInteractionComponent;

UCLASS()
class THIRDPERSON_API ADMIOBase : public AActor, public IDMInterfaceComponentManagement
{
	GENERATED_UCLASS_BODY()
	
protected:
	TArray<UDMInteractionComponent*> InteractionComponentList;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BuildCustomComponents();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual class UMeshComponent* GetMeshComponent() { return nullptr; }
	USceneComponent* GetRootComponent() { return RootComponent; }
};
