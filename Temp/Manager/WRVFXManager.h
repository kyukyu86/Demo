#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"

#include "Actor/Effect/WRVFX.h"

#include "Enum/EWRVFX.h"
#include "Enum/EWRCharacter.h"

DECLARE_DELEGATE_OneParam(WRDestoryHandle, AActor*)
DECLARE_DELEGATE_OneParam(WRAsyncHandle, AWRVFX*)

DECLARE_STATS_GROUP(TEXT("WRVFXManager"), STATGROUP_WRVFXManager, STATCAT_Advanced);

struct FWRVFXSpawnDistribution
{
	FString Path;
	TSubclassOf<AWRVFX> GeneratedClass;

	AActor* Spawner = nullptr;
	AActor* Target = nullptr;
	AWRVFX* Parent = nullptr;

	EWRVFX Bits = EWRVFX::None;

	FRotator Angle = FRotator::ZeroRotator;
};

struct FWRVFXSpawnAsync
{
	uint64 SpawnerAID = 0;
	uint64 TargetAID = 0;
	uint32 ParentUID = 0;

	EWRVFX Bits = EWRVFX::None;

	FRotator Angle = FRotator::ZeroRotator;

	WRAsyncHandle AsyncHandle;
};

/**
*
*/

class WR_API WRVFXManager : public WRSingleton<WRVFXManager>
{
	CDECLARE_MANAGER(WRVFXManager);
	
private:
	WRVFXManager();
	virtual ~WRVFXManager();
	
public:
	virtual void OnInitialize() override;

	virtual void OnShutdown() override;

	virtual void OnLoadLevelStart() override;
	virtual void OnLoadLevelComplete() override;

	virtual void OnRenderStart() override;

	virtual bool OnTick(float InDeltaTime) override;
	
public:
	AWRVFX* Spawn(const FString& InPath, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent = nullptr, const EWRVFX InBits = EWRVFX::None, const FRotator& InAngle = FRotator::ZeroRotator);
	AWRVFX* Spawn(TSubclassOf<AWRVFX> InGeneratedClass, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent = nullptr, const EWRVFX InBits = EWRVFX::None, const FRotator& InAngle = FRotator::ZeroRotator);
	AWRVFX* Spawn(const FWRVFXSpawnDistribution& InParameter, const bool InIsDistributed = false);
	FString AsyncSpawn(const FString& InPath, uint64 InSpawnerAID, uint64 InTargetAID, uint32 InParentUID = 0, const EWRVFX InBits = EWRVFX::None, const FRotator& InAngle = FRotator::ZeroRotator, const WRAsyncHandle& InHandle = nullptr);

	void Remove(const uint32 InUniqueID, const bool InShouldFadeParticle = false, const bool InIsDelayed = false);
	void Remove(AWRVFX* InVFX, const bool InShouldFadeParticle = false, const bool InIsDelayed = false);
	void Remove();
	void RemoveIncludeChild(AWRVFX* InVFX, const bool InShouldFadeParticle = false, const bool InIsDelayed = false);

	bool AsyncRemove(const FString& InHash);

	AWRVFX* Find(const uint32 InUniqueID);

private:
	void Delay();
	void Distribute();

	void Register(AActor* InActor, AWRVFX* InVFX);

	void Unregister(AActor* InActor);
	void Unregister(AWRVFX* InVFX);
	void Unregister();

	void OnComplete(AWRVFX* InVFX, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent, const EWRVFX InBits, const FRotator& InAngle);
	void OnComplete(AWRVFX* InVFX, uint64 InSpawnerAID, uint64 InTargetAID, uint32 InParentUID, const EWRVFX InBits, const FRotator& InAngle);

	bool IsBinding(AActor* InActor);

	template<class T>
	bool Bind(AActor* InActor);

	void AsyncLoadCompleted(UObject* InObject, FString InHash);

	bool IsPendingKillPending(const AActor* InActor);

private:
	TMap<uint32, AWRVFX*> VFXs;
	TMap<FString, FWRVFXSpawnAsync> AsyncVFXs;

	TMap<AActor*, TDoubleLinkedList<AWRVFX*>*> Actors;

	TDoubleLinkedList<AWRVFX*> Delays;
	TDoubleLinkedList<FWRVFXSpawnDistribution*> Distributions;

	bool IsLock = false;

private:
	friend class AWRVFX;
};

template<class T>
bool WRVFXManager::Bind(AActor* InActor)
{
	T* Actor = Cast<T>(InActor);
	if (Actor == nullptr)
	{
		return false;
	}

	Actor->OnDestroy().BindRaw(this, &WRVFXManager::Unregister);

	return Actor->OnDestroy().IsBound();
}