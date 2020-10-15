// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interface/DMActorComponentInterface.h"
#include "../FunctionComponent/DMInteractionComponent.h"
#include "DMIOBase.generated.h"

UCLASS()
class THIRDPERSON_API ADMIOBase : public AActor, public IDMActorComponentInterface
{
	GENERATED_UCLASS_BODY()
	
private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UDMInteractionComponent* InteractionComponent;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BuildCustomComponents();

public:	
	virtual void Tick(float DeltaTime) override;
	USceneComponent* GetRootComponent() { return RootComponent; }
};
