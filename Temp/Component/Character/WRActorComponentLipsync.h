// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRActorComponentLipsync.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentLipsync : public UWRActorComponent
{
	GENERATED_BODY()

public:

	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool Start(class USoundBase* InSound, class UCurveFloat* InLipsyncCurve, bool bIn3DSound = false);
	void Stop();

	bool IsTalking() const;

	float GetLipsyncAlpha();
	
private:

	class UAudioComponent* LipsyncAudioComponent = nullptr;
	class UCurveFloat* LipsyncCurve = nullptr;
	float ElapsedTime = 0.0f;
};
