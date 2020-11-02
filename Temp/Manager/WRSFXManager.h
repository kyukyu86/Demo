// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "SingletonClass/WRSingleton.h"

#include "../Enum/EWRSFX.h"

#include "Components/AudioComponent.h"
/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(WRLjh, Display, All);

DECLARE_STATS_GROUP(TEXT("WRSFXManager"), STATGROUP_WRSFXManager, STATCAT_Advanced);


struct FWRSFXSpawnAsync
{
	EWRSFXSpawnType SpawnType;
	FString Path;
	USceneComponent* AttachedComponent;
	FVector Location;
	float Volume = -1.f;
	float Pitch = -1.f;
	float FadeDuration = -1.f;
	int32 Index = -1;
};

class USoundBase;
//class UAudioComponent;

class WR_API WRSFXManager : public WRSingleton<WRSFXManager>
{
	CDECLARE_MANAGER(WRSFXManager);

private:
	WRSFXManager();
	virtual ~WRSFXManager();

public:
	virtual void OnInitialize() override;

	virtual void OnShutdown() override;

	virtual void OnLoadLevelStart() override;
	virtual void OnLoadLevelComplete() override;

	virtual void OnRenderStart() override;

public:
	// [2020 - 08 - 11 - EZ : 기존 스폰은 그대로 유지,  및에 비동기 및 PreLoad 함수추가]
	UAudioComponent* SpawnAtLocation(const FString& InFilePath, const FVector& InLocation, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F, const int32 InIndex = -1);
	//UAudioComponent* SpawnAtLocation(const FString& InFilePath, const FVector& InLocation);
	UAudioComponent* SpawnAtLocation(USoundBase* InGeneratedClass, const FVector& InLocation, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F, const int32 InIndex  = -1);

	UAudioComponent* SpawnAttached(const FString& InFilePath, USceneComponent* InAttachToComponent, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F, const int32 InIndex = -1);
	UAudioComponent* SpawnAttached(USoundBase* InGeneratedClass, USceneComponent* InAttachToComponent, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F, const int32 InIndex = -1);
	
	UAudioComponent* Spawn2D(const FString& InFilePath, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F, const int32 InIndex = -1);
	UAudioComponent* Spawn2D(USoundBase* InGeneratedClass, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F, const int32 InIndex = -1);

	UAudioComponent* SpawnBGM(const FString& InFilePath, const float InFadeDuration = 1.f, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F);
	UAudioComponent* SpawnBGM(USoundBase* InGeneratedClass, const float InFadeDuration = 1.f, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F);

	void AsyncSpawnSFX(FWRSFXSpawnAsync& IN AsyncInfo);
	void AsyncSpawn2D(const FString& InFilePath, const float InVolumeMultiplier = -1.0F, const float InPitchMultiplier = -1.0F, const int32 InIndex = -1);
	void OnCompleteSpawn(UObject* InObject, FString InHashKey);

	
	TArray<FString> GetPreLoadList();

	// [ 2020-2-14 : animal14 ] 인덱스는 UObjectBase 클래스 GetUniqueID 함수의 반환값
	void Remove(const uint32 InIndex, const float InFadeDuration = -1.0f);
	void Remove(UAudioComponent* InComponent, const float InFadeDuration = -1.0f, int32 InUniqueID = -1);
	void RemoveAll();

	void RemoveBGM(const bool InShouldFadeAudio);

	UAudioComponent* Find(const uint32 InIndex);

	void PlayBGM();
	void SFX_TestFunction();

	void TestLevelLoad();

	float GetSoundVolume(IN const EWRSoundClassKind& InKind);
	void SetSoundVolume(IN const EWRSoundClassKind& InKind, IN const float& InVolume);
	
private:
	void ReleaseSFX(IN UAudioComponent& InComponent, const float InFadeDuration = -1.0f);
	void Register(AActor* InActor, UAudioComponent* InAudioComp);

	void Unregister(AActor* InActor);
	//void Unregister(UAudioComponent* InAudioComponent);
	void Unregister();

	void OnComplete(UAudioComponent* InComponent, const int32 InIndex);

	void OnAudioFinishedNative(UAudioComponent* InComponent);

	USoundBase* TryLoad(const FString& InFilePath);


	FORCEINLINE uint32 ConvertIndex(const UAudioComponent& InGeneratedClass, const int32 InIndex)
	{
		uint32 ResultIndex = 0;

		if (InIndex <= -1)
		{
			ResultIndex = InGeneratedClass.GetUniqueID();
		}
		else
		{
			ResultIndex = InIndex;
		}
		return ResultIndex;
	};

	bool IsBinding(AActor* InActor);

	template<class T>
	bool Bind(AActor* InActor);
private:
	TMap<uint32, UAudioComponent*> SFXs;
	TMap<uint8, FString> WRSoundClasses;

	TMap<FString, FWRSFXSpawnAsync> AsyncSpawns;

	TMap<AActor*, TDoubleLinkedList<UAudioComponent*>*> Actors;

	UAudioComponent* CurrentBGM = nullptr;
	UAudioComponent* PreviousBGM = nullptr;

	bool IsLock = false;

	// [2020 - 07 -30 : EZ : 사운드 클래스 정립되기전에 비밀대화용 사운드 크기 임의 조절용 0.f~1.f , 사운드 클래스 분류 나오고나면 삭제 ]
	float TempVolume = 1.f;

public:
	void SetTempVolume(const float& InVolume);
};

template<class T>
bool WRSFXManager::Bind(AActor* InActor)
{
	T* Actor = Cast<T>(InActor);
	if (Actor == nullptr)
	{
		return false;
	}

	Actor->OnDestroy().BindRaw(this, &WRSFXManager::Unregister);

	return Actor->OnDestroy().IsBound();
}