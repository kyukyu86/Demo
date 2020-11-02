// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Enum/EWRVFX.h"

#include "Actor/Effect/WRVFX.h"

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRActorComponentVFX.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentVFX : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

public:
	AWRVFX* AddVFX(EWRVFXActor InType, const FString& InFullPath, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle = false);
	AWRVFX* AddVFX(EWRVFXActor InType, TSubclassOf<AWRVFX> InGeneratedClass, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle = false);
	AWRVFX* ChangeVFX(EWRVFXActor InType, const FString& InFullPath, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle = false);
	AWRVFX* ChangeVFX(EWRVFXActor InType, TSubclassOf<AWRVFX> InGeneratedClass, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle = false);

	void RemoveVFX(const uint32 InIndex, EWRVFXActor InType, const bool InShouldFadeParticle = false);
	void RemoveVFX(AWRVFX* InVFX, EWRVFXActor InType, const bool InShouldFadeParticle = false);
	void RemoveVFX(EWRVFXActor InType, const bool InShouldFadeParticle = false);
	void RemoveVFX();

	TDoubleLinkedList<AWRVFX*>* GetVFXs(EWRVFXActor InType);

	bool Contains(EWRVFXActor InType);

private:
	TMap<EWRVFXActor, TDoubleLinkedList<AWRVFX*>*> VFXs;
};
