// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Sound/SoundCue.h"

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRVFXComponentSound.generated.h"

USTRUCT(BlueprintType)
struct FWRVFXSounds
{
	GENERATED_BODY()

public:
	bool Tick(float DeltaTime);

public:
	void Reset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSoundComponent")
	USoundCue*	GeneratedClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSoundComponent")
	float VolumeMultiplier = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSoundComponent")
	float PitchMultiplier = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSoundComponent")
	bool IsSpawnWhenCreate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSoundComponent")
	float Duration = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSoundComponent", meta = (EditCondition = "!IsSpawnWhenCreate"))
	float Delay = 1.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WRSoundComponent")
	float AccDelay = 0.0F;

	bool IsSpawned = false;

	uint32 UID = 0;
};

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRVFXComponentSound : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetActive(bool bNewActive, bool bReset = false) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSoundComponent")
	TArray<FWRVFXSounds> VFXSounds;
};
