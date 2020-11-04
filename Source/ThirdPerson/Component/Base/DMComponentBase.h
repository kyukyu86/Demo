// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Enum/DMActorEnum.h"
#include "DMComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSON_API UDMComponentBase : public UActorComponent
{
	GENERATED_BODY()

private:
	EDMComponentType ActorComponentType = EDMComponentType::None;

public:	
	// Sets default values for this component's properties
	UDMComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnCreateComponent() {};		// Custom
	virtual void OnDestroyComponent() {};		// Custom

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void DestroyComponent(bool bPromoteChildren = false);

	FORCEINLINE void SetActorComponentType(EDMComponentType IN InType) { ActorComponentType = InType; }
	FORCEINLINE EDMComponentType GetActorComponentType() { return ActorComponentType; }
		
};
