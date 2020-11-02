// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Enum/EWRSFX.h"

#include "Components/AudioComponent.h"

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRActorComponentSFX.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentSFX : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

public:
	UAudioComponent* AddSFX(EWRSFXActor InType, const FString& InFilePath, USceneComponent* InAttachToComponent, const float InDuration = 0.0F, const float InVolumeMultiplier = 1.0F, const float InPitchMultiplier = 1.0F);
	UAudioComponent* ChangeSFX(EWRSFXActor InType, const FString& InFilePath, USceneComponent* InAttachToComponent, const float InDuration = 0.0F, const float InVolumeMultiplier = 1.0F, const float InPitchMultiplier = 1.0F);

	void RemoveSFX(EWRSFXActor InType, const float InShouldFadeAudio = -1.0F);
	void RemoveSFX(const float InShouldFadeAudio = -1.0F);

	bool IsContains(EWRSFXActor InType) {
		return SFXs.Contains(InType);
	}

	UAudioComponent* GetSFX(EWRSFXActor InType);

private:
	TMap<EWRSFXActor, UAudioComponent*> SFXs;
};
