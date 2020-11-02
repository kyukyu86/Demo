// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enum/EWRActorComponent.h"
#include "WRActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WR_API UWRActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWRActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnCreateComponent() {};
	virtual void OnDestroyComponent() {};

public:
	void SetEnableTick(const bool bInEnable);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void DestroyComponent(bool bPromoteChildren = false);

	FORCEINLINE void SetCharacterComponentType(EWRCharacterComponent InComponentType) {
		CharacterComponentType = InComponentType;
	}
	FORCEINLINE EWRCharacterComponent GetCharacterComponentType() const {
		return CharacterComponentType;
	}

	FORCEINLINE void SetFXComponentType(EWRFXComponent InComponentType) {
		FXComponentType = InComponentType;
	}
	FORCEINLINE EWRFXComponent GetFXComponentType() const {
		return FXComponentType;
	}

public:
	UFUNCTION(BlueprintCallable, Category = "AWRCharacter")
	class AWRCharacter* GetOwnerCharacter();

	UFUNCTION(BlueprintCallable, Category = "AWRVFX")
	class AWRVFX* GetOwnerVFX();

	UFUNCTION(BlueprintCallable, Category = "AWRVehicle")
	class AWRVehicle* GetOwnerVehicle();

protected:
	EWRCharacterComponent CharacterComponentType = EWRCharacterComponent::None;
	EWRFXComponent FXComponentType = EWRFXComponent::None;
		
};
