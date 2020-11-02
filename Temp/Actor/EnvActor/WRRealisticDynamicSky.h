// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

//#include "EWRTOD.h"
#include "../../Enum/EWRTOD.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WRRealisticDynamicSky.generated.h"

UCLASS()
class WR_API AWRRealisticDynamicSky : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWRRealisticDynamicSky();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** If true, actor is ticked even if TickType==LEVELTICK_ViewportsOnly	 */
	virtual bool ShouldTickIfViewportsOnly() const;

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyThatWillChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#else // WITH_EDITOR
#endif // WITH_EDITOR

public:
	void SpawnFog(const float InTime);

private:
	void ProcessTOD(const float DeltaTime);

	void ProcessFog(const float DeltaTime);

	template<class T>
	T* GetCastedCurve(const EWRTODETC InKey);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DayCycle = 1200.0F;

	// [ 2020-6-10 : animal14 ] 시간
	float AccDeltaTime = 0.0F;
	float LastDeltaTime = -1.0F;

	float Duration = 1.0F;

	float StartTOD = 1.0F;
	float FinishTOD = 1.0F;

	// [ 2020-6-10 : animal14 ] 안개
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AExponentialHeightFog* Fog = nullptr;

	float FogDensity = 0.0F;
	float FogHeightFalloff = 0.0F;
	FLinearColor FogInscatteringColor = FLinearColor::Black;
	float FogOpacity = 0.0F;
	float FogStartDistance = 0.0F;

	float SecondFogDensity = 0.0F;
	float SecondFogHeightFalloff = 0.0F;
	float SecondFogHeightOffset = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EWRTODETC, class UCurveBase*> Curves;
};

template<class T>
T* AWRRealisticDynamicSky::GetCastedCurve(const EWRTODETC InKey)
{
	if (Curves.Contains(InKey) == false)
	{
		return nullptr;
	}
	return Cast<T>(Curves[InKey]);
}