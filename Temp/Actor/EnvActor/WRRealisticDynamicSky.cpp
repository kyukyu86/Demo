// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRRealisticDynamicSky.h"

#include "Components/ExponentialHeightFogComponent.h"

#include "Curves/CurveVector.h"
#include "Curves/CurveLinearColor.h"

#include "Engine/ExponentialHeightFog.h"

#include "Utility/WREnumUtility.h"


// Sets default values
AWRRealisticDynamicSky::AWRRealisticDynamicSky()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWRRealisticDynamicSky::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWRRealisticDynamicSky::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ProcessTOD(DeltaTime);
}

bool AWRRealisticDynamicSky::ShouldTickIfViewportsOnly() const
{
	return true;
}

#if WITH_EDITOR
void AWRRealisticDynamicSky::PreEditChange(FProperty* PropertyThatWillChange)
{
	StartTOD = DayCycle * 0.01f;

	SpawnFog(1.0F);

	Super::PreEditChange(PropertyThatWillChange);
}

void AWRRealisticDynamicSky::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FinishTOD = DayCycle * 0.01f;
	
	LastDeltaTime = AccDeltaTime;
}
#else // WITH_EDITOR
#endif // WITH_EDITOR

void AWRRealisticDynamicSky::ProcessTOD(const float DeltaTime)
{
	AccDeltaTime += DeltaTime;

	if (Duration - FLT_EPSILON < 0.0F)
	{
	}
	else
	{
		float Difference = AccDeltaTime - LastDeltaTime;
		if (Difference - DeltaTime > Duration - FLT_EPSILON)
		{
			return;
		}

		float Ratio = FMath::Clamp(Difference / Duration, 0.0F, 1.0F);

		this->ProcessFog(Ratio);
	}
}

void AWRRealisticDynamicSky::SpawnFog(const float InTime)
{
	if (Fog == nullptr)
	{
		return;
	}

	FinishTOD = InTime;

	LastDeltaTime = AccDeltaTime;

#if UE_EDITOR
	for (int32 i = 1; i < (int32)EWRTODETC::RainAmount; ++i)
	{
		EWRTODETC Key = (EWRTODETC)i;
		if (Curves.Contains(Key))
		{
			continue;
		}

		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(WREnumUtility::EnumToString("EWRTODETC", Key));

		FString Text = FString::Format(TEXT("Missing curve data {0}"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRRealisticDynamicSky::Spawn()"));
	}

	if (Fog == nullptr)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Missing fog data"), TEXT("AWRRealisticDynamicSky::Spawn()"));
	}
#else // UE_EDITOR
#endif // UE_EDITOR

	FogDensity = Fog->GetComponent()->FogDensity;
	FogHeightFalloff = Fog->GetComponent()->FogHeightFalloff;
	FogInscatteringColor = Fog->GetComponent()->FogInscatteringColor;
	FogOpacity = Fog->GetComponent()->FogMaxOpacity;
	FogStartDistance = Fog->GetComponent()->StartDistance;

	SecondFogDensity = Fog->GetComponent()->SecondFogData.FogDensity;
	SecondFogHeightFalloff = Fog->GetComponent()->SecondFogData.FogHeightFalloff;
	SecondFogHeightOffset = Fog->GetComponent()->SecondFogData.FogHeightOffset;
}

void AWRRealisticDynamicSky::ProcessFog(const float DeltaTime)
{
	UCurveFloat* Density = this->GetCastedCurve<UCurveFloat>(EWRTODETC::FogDensity);
	if (Density == nullptr)
	{
		return;
	}
	float ResultDensity = FMath::Lerp(FogDensity, Density->GetFloatValue(FinishTOD), DeltaTime);

	UCurveFloat* HeightFalloff = this->GetCastedCurve<UCurveFloat>(EWRTODETC::HeightFalloff);
	if (HeightFalloff == nullptr)
	{
		return;
	}
	float ResultHeightFalloff = FMath::Lerp(FogHeightFalloff, HeightFalloff->GetFloatValue(FinishTOD), DeltaTime);

	UCurveLinearColor* Color = this->GetCastedCurve<UCurveLinearColor>(EWRTODETC::FogColor);
	if (Color == nullptr)
	{
		return;
	}
	FLinearColor ResultInscatteringColor = FMath::Lerp(FogInscatteringColor, Color->GetLinearColorValue(FinishTOD), DeltaTime);

	UCurveFloat* Opacity = this->GetCastedCurve<UCurveFloat>(EWRTODETC::FogOpacity);
	if (Opacity == nullptr)
	{
		return;
	}
	float ResultOpacity = FMath::Lerp(FogOpacity, Opacity->GetFloatValue(FinishTOD), DeltaTime);

	UCurveFloat* StartDistance = this->GetCastedCurve<UCurveFloat>(EWRTODETC::StartDistance);
	if (StartDistance == nullptr)
	{
		return;
	}
	float ResultStartDistance = FMath::Lerp(FogStartDistance, StartDistance->GetFloatValue(FinishTOD), DeltaTime);

	UCurveFloat* SecondDensity = this->GetCastedCurve<UCurveFloat>(EWRTODETC::SecondFogDensity);
	if (SecondDensity == nullptr)
	{
		return;
	}
	float ResultSecondDensity = FMath::Lerp(SecondFogDensity, SecondDensity->GetFloatValue(FinishTOD), DeltaTime);

	UCurveFloat* SecondHeightFalloff = this->GetCastedCurve<UCurveFloat>(EWRTODETC::SecondHeightFalloff);
	if (SecondHeightFalloff == nullptr)
	{
		return;
	}
	float ResultSecondHeightFalloff = FMath::Lerp(SecondFogHeightFalloff, SecondHeightFalloff->GetFloatValue(FinishTOD), DeltaTime);

	UCurveFloat* SecondHeightOffset = this->GetCastedCurve<UCurveFloat>(EWRTODETC::SecondHeightOffset);
	if (SecondHeightOffset == nullptr)
	{
		return;
	}
	float ResultSecondHeightOffset = FMath::Lerp(SecondFogHeightOffset, SecondHeightOffset->GetFloatValue(FinishTOD), DeltaTime);

	if (Fog == nullptr)
	{
		return;
	}

	Fog->GetComponent()->SetFogDensity(ResultDensity);
	Fog->GetComponent()->SetFogHeightFalloff(ResultHeightFalloff);
	Fog->GetComponent()->SetFogInscatteringColor(ResultInscatteringColor);
	Fog->GetComponent()->SetFogMaxOpacity(ResultOpacity);
	Fog->GetComponent()->SetStartDistance(ResultStartDistance);

	Fog->GetComponent()->SecondFogData.FogDensity = ResultSecondDensity;
	Fog->GetComponent()->SecondFogData.FogHeightFalloff = ResultSecondHeightFalloff;
	Fog->GetComponent()->SecondFogData.FogHeightOffset = ResultSecondHeightOffset;
}