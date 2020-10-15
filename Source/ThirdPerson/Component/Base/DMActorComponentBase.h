// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Enum/DMActorEnum.h"
#include "DMActorComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSON_API UDMActorComponentBase : public UActorComponent
{
	GENERATED_BODY()

private:
	EDMActorComponentType ActorComponentType = EDMActorComponentType::None;

public:	
	// Sets default values for this component's properties
	UDMActorComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnCreateComponent() {};		// Custom
	virtual void OnDestroyComponent() {};		// Custom

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void DestroyComponent(bool bPromoteChildren = false);

	FORCEINLINE void SetActorComponentType(EDMActorComponentType IN InType) { ActorComponentType = InType; }
	FORCEINLINE EDMActorComponentType GetActorComponentType() { return ActorComponentType; }
		
};
