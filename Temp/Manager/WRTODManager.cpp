#include "WRTODManager.h"
#include "WRConsoleManager.h"
#include "WRCharacterManager.h"
#include "WRSFXManager.h"
#include "WRMaterialParameterCollectionManager.h"
#include "WRMapManager.h"

#include "Core/WRGameInstance.h"

#include "Component/Character/WRActorComponentVFX.h"
#include "Component/Character/WRActorComponentSFX.h"
#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Components/LightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/WindDirectionalSourceComponent.h"

#include "Define/WRPathDefine.h"

#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/WindDirectionalSource.h"
#include "EngineUtils.h"

#include "Landscape.h"
#include "LandscapeComponent.h"

#include "Utility/WREnumUtility.h"
#include <Materials/MaterialInstanceDynamic.h>

#include "Actor/Character/WRCharacterMine.h"
#include "Actor/EnvActor/WRRealisticDynamicSky.h"
#include "Actor/Volume/WRTODVolume.h"

#include "Curves/CurveVector.h"
#include "Curves/CurveLinearColor.h"

#include "Table/Base/WRTableManager.h"

#include "Kismet/GameplayStatics.h"

#include "GameMode/WRGameModeBase.h"


WRTODManager::WRTODManager()
{
}

WRTODManager::~WRTODManager()
{
}

void WRTODManager::OnInitialize()
{
}

void WRTODManager::OnShutdown()
{
	this->Remove();
}

void WRTODManager::OnLoadLevelStart()
{
	this->Remove();
}

void WRTODManager::OnLoadLevelComplete()
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	for (TActorIterator<AWRRealisticDynamicSky> iter(World); iter; ++iter)
 	{
 		if (RealisticDynamicSky)
 		{
 #if UE_EDITOR
 			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("RealisticDynamicSky"), TEXT("WRTODManager::OnLoadLevelComplete()"));
 #else // UE_EDITOR		
 #endif // UE_EDITOR
 			continue;
 		}
 
		RealisticDynamicSky = *iter;
 	}

//	for (TActorIterator<ADirectionalLight> iter(World); iter; ++iter)
//	{
//		if (Light)
//		{
//#if UE_EDITOR
//			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("ADirectionalLight"), TEXT("WRTODManager::OnLoadLevelComplete()"));
//#else // UE_EDITOR		
//#endif // UE_EDITOR
//			continue;
//		}
//
//		Light = *iter;
//	}

	for (TActorIterator<AExponentialHeightFog> iter(World); iter; ++iter)
	{
		if (iter->GetName().Contains("ExponentialHeightFog") == false)
		{
			continue;
		}

		if (Fog)
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("AExponentialHeightFog"), TEXT("WRTODManager::OnLoadLevelComplete()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
			continue;
		}

		Fog = *iter;
	}

	for (TActorIterator<ALandscape> iter(World); iter; ++iter)
	{
		if (Landscape)
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("ALandscape"), TEXT("WRTODManager::OnLoadLevelComplete()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
			continue;
		}

		Landscape = *iter;
	}

	for (TActorIterator<AWindDirectionalSource> iter(World); iter; ++iter)
	{
		if (Wind)
		{
#if UE_EDITOR
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("AWindDirectionalSource"), TEXT("WRTODManager::OnLoadLevelComplete()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
			continue;
		}

		Wind = *iter;
	}

	this->Register(EWRTODETC::FogDensity, "/Game/Asset/BG/SKY/FogDensity.FogDensity");
	this->Register(EWRTODETC::HeightFalloff, "/Game/Asset/BG/SKY/FogHeightFalloff.FogHeightFalloff");
	this->Register(EWRTODETC::FogColor, "/Game/Asset/BG/SKY/FogInscatteringColor.FogInscatteringColor");
	this->Register(EWRTODETC::StartDistance, "/Game/Asset/BG/SKY/FogStartDistance.FogStartDistance");

	this->Register(EWRTODETC::SecondFogDensity, "/Game/Asset/BG/SKY/SecondFogDensity.SecondFogDensity");
	this->Register(EWRTODETC::SecondHeightFalloff, "/Game/Asset/BG/SKY/SecondFogHeightFalloff.SecondFogHeightFalloff");
	this->Register(EWRTODETC::SecondHeightOffset, "/Game/Asset/BG/SKY/SecondHeightOffset.SecondHeightOffset");

	this->Register(EWRTODETC::RainAmount, "/Game/Asset/Effect/FX_Rain/Curve/Rain_Amount.Rain_Amount");
	this->Register(EWRTODETC::RainVelocity, "/Game/Asset/Effect/FX_Rain/Curve/Rain_Speed.Rain_Speed");
	this->Register(EWRTODETC::MistAmount, "/Game/Asset/Effect/FX_Rain/Curve/Mist_Amount.Mist_Amount");

	this->Register(EWRTODETC::TODEmissive, "/Game/Asset/Effect/FX_Rain/Curve/TODEmissive.TODEmissive");

	this->InitializeTOD();
}

void WRTODManager::OnRenderStart()
{
}

bool WRTODManager::OnTick(float InDeltaTime)
{
	this->Process(InDeltaTime);
	this->ProcessDead(InDeltaTime);

	return true;
}

void WRTODManager::SpawnTOD(const float InTime, const int32 InPriority, const float InDuration, const FWRTODETC& InETC, const FWRWind& InWind)
{
	//if (TODStacks.Num())
	//{
	//	int32 LastKey = TODStacks.Last();

	//	if (InPriority <= TODs[LastKey])
	//	{
	//		return;
	//	}
	//}

	Duration = InDuration;

	StartTOD = this->GetCurrent();
	FinishTOD = InTime;

	StartETC = FinishETC;
	FinishETC = InETC;

	UCurveLinearColor* TODEmissive = this->GetCastedCurve<UCurveLinearColor>(EWRTODETC::TODEmissive);
	if (TODEmissive == nullptr)
	{
		return;
	}
	StartEmissive = TODEmissive->GetLinearColorValue(StartTOD);

	LastDeltaTime = AccDeltaTime;

	// [ 2019-11-26 : animal14 ] 
	int32 Time = FinishTOD * 100.0F;

	TODStacks.Add(Time);
	TODs.Add(Time, InPriority);

	// [ 2019-11-26 : animal14 ] 
	this->Spawn(InTime, InETC, InWind);

	// [ 2019-11-27 : animal14 ] 
	if (Duration - FLT_EPSILON < 0.0F)
	{
		float Ratio = 1.0F;

		this->ProcessTOD(Ratio);
		this->ProcessLandscape(Ratio);
		this->ProcessWind(Ratio);
		//this->ProcessFog(Ratio);
		this->ProcessVFX(Ratio);
	}
	else
	{
	}
}

//void WRTODManager::RemoveTOD(const float InTime, const float InDuration)
//{
//	int32 Time = InTime * 100.0F;
//	if (TODStacks.Contains(Time) == false)
//	{
//		return;
//	}
//
//	// [ 2019-10-2 : animal14 ] 컨테이너 삭제
//	int32 LastKey = TODStacks.Last();
//	TODStacks.Remove(Time);
//
//	if (Time != LastKey)
//	{
//		return;
//	}
//
//	// [ 2019-10-2 : animal14 ] 최신화
//	if (TODStacks.Num() == 0)
//	{
//		return;
//	}
//
//	LastKey = TODStacks.Last();
//	this->SpawnTOD(TODs[LastKey], LastKey * 0.01F, InDuration);
//}
//
//void WRTODManager::RemoveFX(const float InTime, const EWRTODFX InETC)
//{
//	if (WREnumUtility::IsBit((uint64)InETC, (uint64)EWRTODFX::Rain))
//	{
//		this->RemoveVFX(InTime);
//		this->RemoveParameter();
//
//		WRMaterialParameterCollectionManager::Get()->SetScreenOnRain(false);
//
//		RemoveHandle.Broadcast();
//	}
//	else
//	{
//		if (VFXStacks.Num() == 0)
//		{
//		}
//		else
//		{
//			int32 LastKey = VFXStacks.Last();
//			this->SpawnVFX(VFXs[LastKey]);
//			this->SpawnParameter();
//
//			WRMaterialParameterCollectionManager::Get()->SetScreenOnRain(true);
//
//			SpawnHandle.Broadcast();
//		}
//	}
//
//	if (WREnumUtility::IsBit((uint64)InETC, (uint64)EWRTODFX::Thunder))
//	{
//		this->RemoveSFX(InTime);
//	}
//	else
//	{
//		if (SFXStacks.Num() == 0)
//		{
//		}
//		else
//		{
//			int32 LastKey = SFXStacks.Last();
//			this->SpawnSFX(SFXs[LastKey]);
//		}
//	}
//
//	if (WREnumUtility::IsBit((uint64)InETC, (uint64)EWRTODFX::Lightning))
//	{
//	}
//	else
//	{
//	}
//
//	this->RemoveWind(InTime);
//}

#define WR_DAYCYCLE 24.0F

float WRTODManager::GetCurrent()
{
	if (RealisticDynamicSky == nullptr)
	{
		return 0.0F;
	}
	return RealisticDynamicSky->DayCycle * 0.01F;
}

bool WRTODManager::IsRaining()
{
	return FinishETC.Raining > 0.0F;
}

bool WRTODManager::IsThundering()
{
	return FinishETC.Thundering > 0.0F;
}

bool WRTODManager::IsLightning()
{
	return FinishETC.Lightning > 0.0F;
}

void WRTODManager::Spawn(const float InTime, const FWRTODETC& InETC, const FWRWind& InWind)
{
	if (InETC.Raining + InETC.Thundering + InETC.Lightning > 0.0F)
	{
		this->SpawnVFX(InTime, InETC);
		this->SpawnSFX(InTime, InETC);

		SpawnHandle.Broadcast();
	}
	else
	{
		this->RemoveVFX();
		this->RemoveSFX();

		RemoveHandle.Broadcast();
	}

	this->SpawnWind(InTime, InWind);
	//this->SpawnFog();
	RealisticDynamicSky->SpawnFog(InTime);
}

void WRTODManager::SpawnVFX(const float InTime, const FWRTODETC& InETC)
{
	this->SpawnVFX(InETC);

	int32 Time = InTime * 100.0F;

	VFXStacks.Add(Time);
	VFXs.Add(Time, InETC);
}

void WRTODManager::SpawnVFX(const FWRTODETC& InETC)
{
	FString Path = this->GetVFX(InETC);
	if (Path.IsEmpty())
	{
		return;
	}

	AWRCharacterMine* Character = WRCharacterManager::Get()->GetMyCharacter();
	if (Character == nullptr)
	{
		return;
	}

	UWRActorComponentVFX* ActorComponentVFX = Character->GetComponent<UWRActorComponentVFX>();
	if (ActorComponentVFX == nullptr)
	{
		return;
	}

	AWRVFX* VFX = ActorComponentVFX->ChangeVFX(EWRVFXActor::TOD, Path, Character, nullptr, false);
	if (VFX == nullptr)
	{
		return;
	}
}

void WRTODManager::SpawnSFX(const float InTime, const FWRTODETC& InETC)
{
	this->SpawnSFX(InETC);

	int32 Time = InTime * 100.0F;

	SFXStacks.Add(Time);
	SFXs.Add(Time, InETC);
}

void WRTODManager::SpawnSFX(const FWRTODETC& InETC)
{
	FString Path = this->GetSFX(InETC);
	if (Path.IsEmpty())
	{
		return;
	}

	AWRCharacterMine* Character = WRCharacterManager::Get()->GetMyCharacter();
	if (Character == nullptr)
	{
		return;
	}

	UWRActorComponentSFX* ActorComponentSFX = Character->GetComponent<UWRActorComponentSFX>();
	if (ActorComponentSFX == nullptr)
	{
		return;
	}

	UAudioComponent* AudioComponent = ActorComponentSFX->ChangeSFX(EWRSFXActor::TOD, Path, Character->GetRootComponent(), Duration);
	if (AudioComponent == nullptr)
	{
		return;
	}
}

void WRTODManager::SpawnWind(const float InTime, const FWRWind& InWind)
{
	this->SpawnWind(InWind);

	int32 Time = InTime * 100.0F;

	WindStacks.Add(Time);
	Winds.Add(Time, InWind);
}

void WRTODManager::SpawnWind(const FWRWind& InWind)
{
	if (Wind == nullptr)
	{
		return;
	}

	UWindDirectionalSourceComponent* WindDirectionalSourceComponent = Wind->GetComponent();
	if (WindDirectionalSourceComponent == nullptr)
	{
		return;
	}

	StartWind.Strength = WindDirectionalSourceComponent->Strength;
	StartWind.Speed = WindDirectionalSourceComponent->Speed;
	StartWind.MinGustAmount = WindDirectionalSourceComponent->MinGustAmount;
	StartWind.MaxGustAmount = WindDirectionalSourceComponent->MaxGustAmount;
	StartWind.Direction = Wind->GetActorRotation();

	FinishWind.Strength = InWind.Strength;
	FinishWind.Speed = InWind.Speed;
	FinishWind.MinGustAmount = InWind.MinGustAmount;
	FinishWind.MaxGustAmount = InWind.MaxGustAmount;
	FinishWind.Direction = InWind.Direction;
}

//void WRTODManager::SpawnFog()
//{
//	if (Fog == nullptr)
//	{
//		return;
//	}
//
//	FogDensity = Fog->GetComponent()->FogDensity;
//	FogHeightFalloff = Fog->GetComponent()->FogHeightFalloff;
//	FogInscatteringColor = Fog->GetComponent()->FogInscatteringColor;
//	FogStartDistance = Fog->GetComponent()->StartDistance;
//
//	SecondFogDensity = Fog->GetComponent()->SecondFogData.FogDensity;
//	SecondFogHeightFalloff = Fog->GetComponent()->SecondFogData.FogHeightFalloff;
//	SecondFogHeightOffset = Fog->GetComponent()->SecondFogData.FogHeightOffset;
//}

void WRTODManager::RemoveVFX(const float InTime)
{
	int32 Time = InTime * 100.0F;
	if (VFXStacks.Contains(Time) == false)
	{
		return;
	}

	// [ 2019-10-2 : animal14 ] 컨테이너 삭제
	int32 LastKey = VFXStacks.Last();
	VFXStacks.Remove(Time);

	if (Time != LastKey)
	{
		VFXs.Remove(Time);
		return;
	}

	// [ 2019-10-2 : animal14 ] 최신화
	if (VFXStacks.Num() == 0)
	{
		this->RemoveVFX();
	}
	else
	{
		LastKey = VFXStacks.Last();
		this->SpawnVFX(VFXs[LastKey]);
	}
}

void WRTODManager::RemoveVFX()
{
	IsDead = true;
}

void WRTODManager::RemoveSFX(const float InTime)
{
	int32 Time = InTime * 100.0F;
	if (SFXStacks.Contains(Time) == false)
	{
		return;
	}

	// [ 2019-10-2 : animal14 ] 컨테이너 삭제
	int32 LastKey = SFXStacks.Last();
	SFXStacks.Remove(Time);

	if (Time != LastKey)
	{
		SFXs.Remove(Time);
		return;
	}

	// [ 2019-10-2 : animal14 ] 최신화
	if (SFXStacks.Num() == 0)
	{
		this->RemoveSFX();
	}
	else
	{
		LastKey = SFXStacks.Last();
		this->SpawnSFX(SFXs[LastKey]);
	}
}

void WRTODManager::RemoveSFX()
{
	AWRCharacterMine* Character = WRCharacterManager::Get()->GetMyCharacter();
	if (Character == nullptr)
	{
		return;
	}

	UWRActorComponentSFX* ActorComponentSFX = Character->GetComponent<UWRActorComponentSFX>();
	if (ActorComponentSFX == nullptr)
	{
		return;
	}

	ActorComponentSFX->RemoveSFX(EWRSFXActor::TOD, Duration);
}

void WRTODManager::RemoveWind(const float InTime)
{
	int32 Time = InTime * 100.0F;
	if (WindStacks.Contains(Time) == false)
	{
		return;
	}

	// [ 2019-10-2 : animal14 ] 컨테이너 삭제
	int32 LastKey = WindStacks.Last();
	WindStacks.Remove(Time);

	if (Time != LastKey)
	{
		Winds.Remove(Time);
		return;
	}

	// [ 2019-10-2 : animal14 ] 최신화
	if (WindStacks.Num() == 0)
	{
		
	}
	else
	{
		LastKey = WindStacks.Last();
		this->SpawnWind(Winds[LastKey]);
	}
}

void WRTODManager::Remove()
{
	RealisticDynamicSky = nullptr;
	Light = nullptr;
	Fog = nullptr;
	Landscape = nullptr;
	Wind = nullptr;

	TODStacks.Empty();

	VFXs.Empty();
	VFXStacks.Empty();

	SFXs.Empty();
	SFXStacks.Empty();

	Winds.Empty();
	WindStacks.Empty();

	Duration = 1.0F;

	AccDeltaTime = 0.0F;
	LastDeltaTime = -1.0F;

	IsDead = false;

	AccDeadTime = 0.0F;

	LandscapeScalars.Empty();
	LandscapeVectors.Empty();
	LandscapeTextures.Empty();

	this->Unregister();
}

void WRTODManager::ProcessTOD(const float InValue)
{
	if (RealisticDynamicSky == nullptr)
	{
		return;
	}

	float Value = (StartTOD > FinishTOD) ? FMath::Lerp(StartTOD, FinishTOD + WR_DAYCYCLE, InValue) : FMath::Lerp(StartTOD, FinishTOD, InValue);
	Value = (Value < WR_DAYCYCLE) ? Value : Value - WR_DAYCYCLE;
	
	RealisticDynamicSky->DayCycle = Value * 100.0F;
}

void WRTODManager::ProcessLandscape(const float InValue)
{
	for (auto& LandscapeScalar : LandscapeScalars)
	{
		this->SetScalarParameterValue(LandscapeScalar);
	}

	for (auto& LandscapeVector : LandscapeVectors)
	{
		this->SetVectorParameterValue(LandscapeVector);
	}

	for (auto& LandscapeTexture : LandscapeTextures)
	{
		this->SetTextureParameterValue(LandscapeTexture);
	}
}

void WRTODManager::ProcessWind(const float InValue)
{
	if (Wind == nullptr)
	{
		return;
	}

	UWindDirectionalSourceComponent* WindDirectionalSourceComponent = Wind->GetComponent();
	if (WindDirectionalSourceComponent == nullptr)
	{
		return;
	}

	float Strength = FMath::Lerp(StartWind.Strength, FinishWind.Strength, InValue);
	WindDirectionalSourceComponent->SetStrength(Strength);

	float Speed = FMath::Lerp(StartWind.Speed, FinishWind.Speed, InValue);
	WindDirectionalSourceComponent->SetSpeed(FMath::Lerp(StartWind.Speed, FinishWind.Speed, InValue));
	WindDirectionalSourceComponent->SetMinimumGustAmount(FMath::Lerp(StartWind.MinGustAmount, FinishWind.MinGustAmount, InValue));
	WindDirectionalSourceComponent->SetMaximumGustAmount(FMath::Lerp(StartWind.MaxGustAmount, FinishWind.MaxGustAmount, InValue));

	FRotator Rotation = FMath::Lerp(StartWind.Direction, FinishWind.Direction, InValue);
	Wind->SetActorRotation(Rotation);

	// [ 2019-10-2 : animal14 ] 폴리지 머티리얼 셋팅
	float WindRotation = Rotation.Yaw / 360.0F;
	int32 Integer = WindRotation;
	float Decimal = WindRotation - Integer;

	WRMaterialParameterCollectionManager::Get()->SetWindRotation(Decimal);
	WRMaterialParameterCollectionManager::Get()->SetWindStrength(Strength);
	WRMaterialParameterCollectionManager::Get()->SetWindSpeed(Speed);
	WRMaterialParameterCollectionManager::Get()->SetWindMultiplier(1.0F);
}

//void WRTODManager::ProcessFog(const float InValue)
//{
//	UCurveFloat* Density = this->GetCastedCurve<UCurveFloat>(EWRTODETC::FogDensity);
//	if (Density == nullptr)
//	{
//		return;
//	}
//	float ResultDensity = FMath::Lerp(FogDensity, Density->GetFloatValue(FinishTOD), InValue);
//
//	UCurveFloat* HeightFalloff = this->GetCastedCurve<UCurveFloat>(EWRTODETC::HeightFalloff);
//	if (HeightFalloff == nullptr)
//	{
//		return;
//	}
//	float ResultHeightFalloff = FMath::Lerp(FogHeightFalloff, HeightFalloff->GetFloatValue(FinishTOD), InValue);
//
//	UCurveLinearColor* Color = this->GetCastedCurve<UCurveLinearColor>(EWRTODETC::FogColor);
//	if (Color == nullptr)
//	{
//		return;
//	}
//	FLinearColor ResultInscatteringColor = FMath::Lerp(FogInscatteringColor, Color->GetLinearColorValue(FinishTOD), InValue);
//
//	UCurveFloat* StartDistance = this->GetCastedCurve<UCurveFloat>(EWRTODETC::StartDistance);
//	if (StartDistance == nullptr)
//	{
//		return;
//	}
//	float ResultStartDistance = FMath::Lerp(FogStartDistance, StartDistance->GetFloatValue(FinishTOD), InValue);
//
//	UCurveFloat* SecondDensity = this->GetCastedCurve<UCurveFloat>(EWRTODETC::SecondFogDensity);
//	if (SecondDensity == nullptr)
//	{
//		return;
//	}
//	float ResultSecondDensity = FMath::Lerp(SecondFogDensity, SecondDensity->GetFloatValue(FinishTOD), InValue);
//
//	UCurveFloat* SecondHeightFalloff = this->GetCastedCurve<UCurveFloat>(EWRTODETC::SecondHeightFalloff);
//	if (SecondHeightFalloff  == nullptr)
//	{
//		return;
//	}
//	float ResultSecondHeightFalloff = FMath::Lerp(SecondFogHeightFalloff, SecondHeightFalloff ->GetFloatValue(FinishTOD), InValue);
//
//	UCurveFloat* SecondHeightOffset = this->GetCastedCurve<UCurveFloat>(EWRTODETC::SecondHeightOffset);
//	if (SecondHeightOffset == nullptr)
//	{
//		return;
//	}
//	float ResultSecondHeightOffset = FMath::Lerp(SecondFogHeightOffset, SecondHeightOffset->GetFloatValue(FinishTOD), InValue);
//
//	if (Fog == nullptr)
//	{
//		return;
//	}
//
//	Fog->GetComponent()->SetFogDensity(ResultDensity);
//	Fog->GetComponent()->SetFogHeightFalloff(ResultHeightFalloff);
//	Fog->GetComponent()->SetFogInscatteringColor(ResultInscatteringColor);
//	Fog->GetComponent()->SetStartDistance(ResultStartDistance);
//
//	Fog->GetComponent()->SecondFogData.FogDensity = ResultSecondDensity;
//	Fog->GetComponent()->SecondFogData.FogHeightFalloff = ResultSecondHeightFalloff;
//	Fog->GetComponent()->SecondFogData.FogHeightOffset = ResultSecondHeightOffset;
//}

void WRTODManager::ProcessVFX(const float InValue)
{
	// [ 2019-11-27 : animal14 ] 머티리얼
	float Rain = FMath::Lerp(StartETC.Raining, FinishETC.Raining, InValue);

	WRMaterialParameterCollectionManager::Get()->SetRain(Rain);

	UCurveLinearColor* TODEmissive = this->GetCastedCurve<UCurveLinearColor>(EWRTODETC::TODEmissive);
	if (TODEmissive == nullptr)
	{
		return;
	}
	FLinearColor ResultTODEmissive = FMath::Lerp(StartEmissive, TODEmissive->GetLinearColorValue(FinishTOD), InValue);
	
	WRMaterialParameterCollectionManager::Get()->SetEmissive(ResultTODEmissive);

	// [ 2019-11-27 : animal14 ] 이팩트
	UCurveFloat* RainAmount = this->GetCastedCurve<UCurveFloat>(EWRTODETC::RainAmount);
	if (RainAmount == nullptr)
	{
		return;
	}
	float ResultRainAmount = FMath::Lerp(RainAmount->GetFloatValue(StartETC.Raining), RainAmount->GetFloatValue(FinishETC.Raining), InValue);
	
	UCurveVector* RainVelocity = this->GetCastedCurve<UCurveVector>(EWRTODETC::RainVelocity);
	if (RainVelocity == nullptr)
	{
		return;
	}
	FVector ResultRainVelocity = FMath::Lerp(RainVelocity->GetVectorValue(StartETC.Raining), RainVelocity->GetVectorValue(FinishETC.Raining), InValue);
	
	UCurveFloat* MistAmount = this->GetCastedCurve<UCurveFloat>(EWRTODETC::MistAmount);
	if (MistAmount == nullptr)
	{
		return;
	}
	float ResultMistAmount = FMath::Lerp(MistAmount->GetFloatValue(StartETC.Raining), MistAmount->GetFloatValue(FinishETC.Raining), InValue);

	AWRCharacterMine* Character = WRCharacterManager::Get()->GetMyCharacter();
	if (Character == nullptr)
	{
		return;
	}

	UWRActorComponentVFX* ActorComponentVFX = Character->GetComponent<UWRActorComponentVFX>();
	if (ActorComponentVFX == nullptr)
	{
		return;
	}

	TDoubleLinkedList<AWRVFX*>* TODVFXs = ActorComponentVFX->GetVFXs(EWRVFXActor::TOD);
	if (TODVFXs == nullptr)
	{
		return;
	}

	for (auto& TODVFX : *TODVFXs)
	{
		TODVFX->SetFloatParameter("Rain_Amount", ResultRainAmount);
		TODVFX->SetVectorParameter("Rain_Speed", ResultRainVelocity);
		TODVFX->SetFloatParameter("Mist_Amount", ResultMistAmount);
	}
}

void WRTODManager::Process(float InDeltaTime)
{
	AccDeltaTime += InDeltaTime;

	if (Duration - FLT_EPSILON < 0.0F)
	{
	}
	else
	{
		float Difference = AccDeltaTime - LastDeltaTime;
		if (Difference - InDeltaTime > Duration - FLT_EPSILON)
		{
			return;
		}

		float Ratio = FMath::Clamp(Difference / Duration, 0.0F, 1.0F);

		this->ProcessTOD(Ratio);
		this->ProcessLandscape(Ratio);
		//this->ProcessWind(Ratio);
		//this->ProcessFog(Ratio);
		this->ProcessVFX(Ratio);
	}
}

void WRTODManager::ProcessDead(float InDeltaTime)
{
	if (IsDead == false)
	{
		return;
	}

	AccDeadTime += InDeltaTime;
	if (AccDeadTime > Duration)
	{
		AWRCharacterMine* Character = WRCharacterManager::Get()->GetMyCharacter();
		if (Character == nullptr)
		{
			return;
		}

		UWRActorComponentVFX* ActorComponentVFX = Character->GetComponent<UWRActorComponentVFX>();
		if (ActorComponentVFX == nullptr)
		{
			return;
		}

		ActorComponentVFX->RemoveVFX(EWRVFXActor::TOD, false);

		// [ 2019-11-27 : animal14 ] 
		IsDead = false;

		AccDeadTime = 0.0F;
	}
	else
	{
		float Ratio = FMath::Clamp(AccDeadTime / Duration, 0.0F, 1.0F);

		UCurveFloat* RainAmount = this->GetCastedCurve<UCurveFloat>(EWRTODETC::RainAmount);
		if (RainAmount == nullptr)
		{
			return;
		}
		float ResultRainAmount = FMath::Lerp(RainAmount->GetFloatValue(FinishETC.Raining), 0.0F, Ratio);

		UCurveVector* RainVelocity = this->GetCastedCurve<UCurveVector>(EWRTODETC::RainVelocity);
		if (RainVelocity == nullptr)
		{
			return;
		}
		FVector ResultRainVelocity = FMath::Lerp(RainVelocity->GetVectorValue(FinishETC.Raining), FVector::ZeroVector, Ratio);

		UCurveFloat* MistAmount = this->GetCastedCurve<UCurveFloat>(EWRTODETC::MistAmount);
		if (MistAmount == nullptr)
		{
			return;
		}
		float ResultMistAmount = FMath::Lerp(MistAmount->GetFloatValue(FinishETC.Raining), 0.0F, Ratio);

		AWRCharacterMine* Character = WRCharacterManager::Get()->GetMyCharacter();
		if (Character == nullptr)
		{
			return;
		}

		UWRActorComponentVFX* ActorComponentVFX = Character->GetComponent<UWRActorComponentVFX>();
		if (ActorComponentVFX == nullptr)
		{
			return;
		}

		TDoubleLinkedList<AWRVFX*>* TODVFXs = ActorComponentVFX->GetVFXs(EWRVFXActor::TOD);
		if (TODVFXs == nullptr)
		{
			return;
		}

		for (auto& TODVFX : *TODVFXs)
		{
			TODVFX->SetFloatParameter("Rain_Amount", ResultRainAmount);
			TODVFX->SetVectorParameter("Rain_Speed", ResultRainVelocity);
			TODVFX->SetFloatParameter("Mist_Amount", ResultMistAmount);
		}
	}
}

void WRTODManager::SetScalarParameterValue(const FWRScalar& InParameter)
{
	TArray<ULandscapeComponent*> LandscapeComponents = Landscape->LandscapeComponents;
	for (int32 j = 0; j < LandscapeComponents.Num(); ++j)
	{
		for (int32 k = 0; k < LandscapeComponents[j]->GetMaterialInstanceCount(); ++k)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(LandscapeComponents[j]->GetMaterialInstance(k));
			if (MID == nullptr)
			{
				continue;
			}

			MID->SetScalarParameterValue(InParameter.ParameterName, InParameter.Value);
		}
	}
}

void WRTODManager::SetScalarParameterValue(const TArray<FWRScalar>& InParameters)
{
	TArray<ULandscapeComponent*> LandscapeComponents = Landscape->LandscapeComponents;
	for (int32 j = 0; j < LandscapeComponents.Num(); ++j)
	{
		for (int32 k = 0; k < LandscapeComponents[j]->GetMaterialInstanceCount(); ++k)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(LandscapeComponents[j]->GetMaterialInstance(k));
			if (MID == nullptr)
			{
				continue;
			}

			for (auto& Parameter : InParameters)
			{
				MID->SetScalarParameterValue(Parameter.ParameterName, Parameter.Value);
			}
		}
	}
}

void WRTODManager::SetTextureParameterValue(const FWRTexture& InParameter)
{
	TArray<ULandscapeComponent*> LandscapeComponents = Landscape->LandscapeComponents;
	for (int32 j = 0; j < LandscapeComponents.Num(); ++j)
	{
		for (int32 k = 0; k < LandscapeComponents[j]->GetMaterialInstanceCount(); ++k)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(LandscapeComponents[j]->GetMaterialInstance(k));
			if (MID == nullptr)
			{
				continue;
			}

			MID->SetTextureParameterValue(InParameter.ParameterName, InParameter.Value);
		}
	}
}

void WRTODManager::SetTextureParameterValue(const TArray<FWRTexture>& InParameters)
{
	TArray<ULandscapeComponent*> LandscapeComponents = Landscape->LandscapeComponents;
	for (int32 j = 0; j < LandscapeComponents.Num(); ++j)
	{
		for (int32 k = 0; k < LandscapeComponents[j]->GetMaterialInstanceCount(); ++k)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(LandscapeComponents[j]->GetMaterialInstance(k));
			if (MID == nullptr)
			{
				continue;
			}

			for (auto& Parameter : InParameters)
			{
				MID->SetTextureParameterValue(Parameter.ParameterName, Parameter.Value);
			}
		}
	}
}

void WRTODManager::SetVectorParameterValue(const FWRVector& InParameter)
{
	TArray<ULandscapeComponent*> LandscapeComponents = Landscape->LandscapeComponents;
	for (int32 j = 0; j < LandscapeComponents.Num(); ++j)
	{
		for (int32 k = 0; k < LandscapeComponents[j]->GetMaterialInstanceCount(); ++k)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(LandscapeComponents[j]->GetMaterialInstance(k));
			if (MID == nullptr)
			{
				continue;
			}

			MID->SetVectorParameterValue(InParameter.ParameterName, InParameter.Value);
		}
	}
}

void WRTODManager::SetVectorParameterValue(const TArray<FWRVector>& InParameters)
{
	TArray<ULandscapeComponent*> LandscapeComponents = Landscape->LandscapeComponents;
	for (int32 j = 0; j < LandscapeComponents.Num(); ++j)
	{
		for (int32 k = 0; k < LandscapeComponents[j]->GetMaterialInstanceCount(); ++k)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(LandscapeComponents[j]->GetMaterialInstance(k));
			if (MID == nullptr)
			{
				continue;
			}

			for (auto& Parameter : InParameters)
			{
				MID->SetVectorParameterValue(Parameter.ParameterName, Parameter.Value);
			}
		}
	}
}

FString WRTODManager::GetVFX(const FWRTODETC& InETC)
{
	if (InETC.Raining > 0.0F)
	{
		return EFFECT_PATH_4_TOD_RAIN;
	}
	if (InETC.Thundering > 0.0F)
	{
		return "";
	}
	if (InETC.Lightning > 0.0F)
	{
		return "";
	}
	return "";
}

FString WRTODManager::GetSFX(const FWRTODETC& InETC)
{
	if (InETC.Raining > 0.0F)
	{
		return SOUND_PATH_4_TOD_RAIN;
	}
	if (InETC.Thundering > 0.0F)
	{
		return SOUND_PATH_4_TOD_THUNDER;
	}
	if (InETC.Lightning > 0.0F)
	{
		return "";
	}
	return "";
}

void WRTODManager::Register(const EWRTODETC InKey, const FString& InFilename)
{
	FString Filename = InFilename;

	UCurveBase* CurveBase = LoadObject<UCurveBase>(nullptr, *Filename);
	if (CurveBase == nullptr)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(Filename);

		FString Text = FString::Format(TEXT("CurveBase {0} is nullptr"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRTODManager::Register()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}
	CurveBase->AddToRoot();

	Curves.Add(InKey, CurveBase);
}

void WRTODManager::Unregister()
{
	for (auto& Curve : Curves)
	{
		Curve.Value->RemoveFromRoot();
		Curve.Value = nullptr;
	}
	Curves.Empty();
}

void WRTODManager::InitializeTOD()
{
	int32 TID = WRMapManager::Get()->GetReservedMapTID();
	FWRMapTableBase* MapRecord = WRTableManager::Get()->FindRowInMapTable(TID, false);

	if (MapRecord == nullptr)
	{
		return;
	}
	if (MapRecord->UseTODControl == false)
	{
		return;
	}

	FWRWind LocalWind;

	FWRTODETC LocalETC;
	LocalETC.Raining = MapRecord->Raining;
	LocalETC.Thundering = MapRecord->Thundering;
	LocalETC.Lightning = MapRecord->Lightning;

	this->SpawnTOD(MapRecord->DayCycle, 0, 0.0F, LocalETC, LocalWind);
}