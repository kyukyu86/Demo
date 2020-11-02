#pragma once

#include "SingletonClass/WRSingleton.h"

#include "Enum/EWRTOD.h"

#include "Table/WRMapTable.h"

struct FWRScalar
{
	FName ParameterName;
	float Value;
};

struct FWRTexture
{
	FName ParameterName;
	class UTexture* Value;
};

struct FWRVector
{
	FName ParameterName;
	FLinearColor Value;
};

struct FWRTODETC
{
	float Raining = 0.0F;
	float Thundering = 0.0F;
	float Lightning = 0.0F;
};

struct FWRWind
{
	float Strength = 0.1F;
	float Speed = 0.2F;

	float MinGustAmount = 0.1F;
	float MaxGustAmount = 0.2F;

	FRotator Direction = FRotator::ZeroRotator;
};

DECLARE_MULTICAST_DELEGATE(WRTODSpawnHandle)
DECLARE_MULTICAST_DELEGATE(WRTODRemoveHandle)

class AWRTODVolume;
/**
*
*/
class WR_API WRTODManager : public WRSingleton<WRTODManager>
{
	CDECLARE_MANAGER(WRTODManager);
	
private:
	WRTODManager();
	virtual ~WRTODManager();
	
public:
	virtual void OnInitialize() override;

	virtual void OnShutdown() override;

	virtual void OnLoadLevelStart() override;
	virtual void OnLoadLevelComplete() override;

	virtual void OnRenderStart() override;

	virtual bool OnTick(float InDeltaTime) override;

public:
	void SpawnTOD(const float InTime, const int32 InPriority, const float InDuration, const FWRTODETC& InETC, const FWRWind& InWind);

	//void RemoveTOD(const float InTime, const float InDuration);
	//void RemoveFX(const float InTime, const EWRTODFX InType);

	float GetCurrent();

	WRTODSpawnHandle& OnSpawn() {
		return SpawnHandle;
	}

	WRTODRemoveHandle& OnRemove() {
		return RemoveHandle;
	}

	bool IsRaining();
	bool IsThundering();
	bool IsLightning();

private:
	void Spawn(const float InTime, const FWRTODETC& InETC, const FWRWind& InWind);

	// [ 2019-11-26 : animal14 ] 
	void SpawnVFX(const float InTime, const FWRTODETC& InETC);
	void SpawnVFX(const FWRTODETC& InETC);

	void SpawnSFX(const float InTime, const FWRTODETC& InETC);
	void SpawnSFX(const FWRTODETC& InETC);

	void SpawnWind(const float InTime, const FWRWind& InWind);
	void SpawnWind(const FWRWind& InWind);

	//void SpawnFog();

	// [ 2019-11-26 : animal14 ] 
	void RemoveVFX(const float InTime);
	void RemoveVFX();

	void RemoveSFX(const float InTime);
	void RemoveSFX();

	void RemoveWind(const float InTime);

	void Remove();

	// [ 2019-11-26 : animal14 ] 
	void ProcessTOD(const float InValue);
	void ProcessLandscape(const float InValue);
	void ProcessWind(const float InValue);
	//void ProcessFog(const float InValue);
	void ProcessVFX(const float InValue);

	void Process(float InDeltaTime);
	void ProcessDead(float InDeltaTime);

	// [ 2019-11-26 : animal14 ] 
	void SetScalarParameterValue(const FWRScalar& InParameter);
	void SetScalarParameterValue(const TArray<FWRScalar>& InParameters);

	void SetTextureParameterValue(const FWRTexture& InParameter);
	void SetTextureParameterValue(const TArray<FWRTexture>& InParameters);

	void SetVectorParameterValue(const FWRVector& InParameter);
	void SetVectorParameterValue(const TArray<FWRVector>& InParameters);

	// [ 2019-11-26 : animal14 ] 
	FString GetVFX(const FWRTODETC& InETC);
	FString GetSFX(const FWRTODETC& InETC);

private:
	void Register(const EWRTODETC InKey, const FString& InFilename);
	void Unregister();

	void InitializeTOD();

	template<class T>
	T* GetCastedCurve(const EWRTODETC InKey);

private:

	class AWRRealisticDynamicSky* RealisticDynamicSky = nullptr;
	class ADirectionalLight* Light = nullptr;
	class AExponentialHeightFog* Fog = nullptr;
	class ALandscape* Landscape = nullptr;
	class AWindDirectionalSource* Wind = nullptr;

	TMap<int32, int32> TODs;
	TArray<int32> TODStacks;

	TMap<int32, FWRTODETC> VFXs;
	TArray<int32> VFXStacks;

	TMap<int32, FWRTODETC> SFXs;
	TArray<int32> SFXStacks;

	TMap<int32, FWRWind> Winds;
	TArray<int32> WindStacks;

	float Duration = 1.0F;

	float StartTOD = 1.0F;
	float FinishTOD = 1.0F;

	FWRWind StartWind;
	FWRWind FinishWind;

	FWRTODETC StartETC;
	FWRTODETC FinishETC;

	float AccDeltaTime = 0.0F;
	float LastDeltaTime = -1.0F;

	bool IsDead = false;

	float AccDeadTime = 0.0F;

	WRTODSpawnHandle SpawnHandle;
	WRTODRemoveHandle RemoveHandle;

	TArray<FWRScalar> LandscapeScalars;
	TArray<FWRVector> LandscapeVectors;
	TArray<FWRTexture> LandscapeTextures;

	float FogDensity = 0.0F;
	float FogHeightFalloff = 0.0F;
	FLinearColor FogInscatteringColor = FLinearColor::White;
	float FogStartDistance = 0.0F;

	float SecondFogDensity = 0.0F;
	float SecondFogHeightFalloff = 0.0F;
	float SecondFogHeightOffset = 0.0F;

	TMap<EWRTODETC, class UCurveBase*> Curves;

	FLinearColor StartEmissive;
};

template<class T>
T* WRTODManager::GetCastedCurve(const EWRTODETC InKey)
{
	if (Curves.Contains(InKey) == false)
	{
		return nullptr;
	}
	return Cast<T>(Curves[InKey]);
}