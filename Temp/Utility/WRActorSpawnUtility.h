// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

/**
 *
 */
class WR_API WRActorSpawnUtility
{
public:

	template <class TActorType>
	static TActorType* Spawn(UObject* InWorldContextObject, const FString& InFullPath);

	template <class TActorType>
	static TActorType* Spawn(UObject* InWorldContextObject, const FString& InFullPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale = FVector::OneVector);

	template <class TActorType>
	static TActorType* Spawn(UObject* InWorldContextObject, TSubclassOf<TActorType> InGeneratedClass);

	template <class TActorType>
	static TActorType* Spawn(UObject* InWorldContextObject, TSubclassOf<TActorType> InGeneratedClass, const FVector& InLocation, const FRotator& InRotation, FVector InScale = FVector::OneVector);

	template <class TActorType>
	static TSubclassOf<UObject> GetGeneratedClass(const FString& InLongPackageName);

	static FString ResolvePath(const FString& InPath);

	template <class TActorType>
	static TActorType* FindOrLoadObject(const FString& InPath);
};

//====================================================================================

DECLARE_LOG_CATEGORY_EXTERN(ActorSpawnUtility, Warning, All);

//====================================================================================

template <class TActorType>
TSubclassOf<UObject> WRActorSpawnUtility::GetGeneratedClass(const FString& InLongPackageName)
{
	UObject* GeneratedClass = nullptr;
	FString BlueprintName = WRActorSpawnUtility::ResolvePath(InLongPackageName);
	// [ 2019-7-11 : magedoga ] Create GeneratedClass
	FStringAssetReference StringAssetRefofBP = FStringAssetReference(BlueprintName);
	GeneratedClass = StringAssetRefofBP.ResolveObject();
	if (GeneratedClass == nullptr)
	{
		GeneratedClass = (UClass*)StringAssetRefofBP.TryLoad();
	}

	if (GeneratedClass == nullptr)
	{
		UE_LOG(ActorSpawnUtility, Warning, TEXT("WRActorSpawnUtility::Spawn - Null GeneradtedClass. Please Check Path of Asset"));
		return nullptr;
	}

	return (UClass*)GeneratedClass;
}

template <class TActorType>
TActorType* WRActorSpawnUtility::FindOrLoadObject(const FString& InPath)
{
	TActorType* Object = FindObject<TActorType>(nullptr, *InPath);
	if (Object == nullptr)
	{
		Object = LoadObject<TActorType>(nullptr, *InPath);
	}
	return Object;
}

//====================================================================================


//====================================================================================


//====================================================================================
template <class TActorType>
TActorType* WRActorSpawnUtility::Spawn(UObject* InWorldContextObject, const FString& InFullPath)
{
	return WRActorSpawnUtility::Spawn<TActorType>(InWorldContextObject, InFullPath, FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector);
}

#define USE_GENERATEDCLASS 1

template <class TActorType>
TActorType* WRActorSpawnUtility::Spawn(UObject* InWorldContextObject, const FString& InFullPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale /*= FVector::OneVector*/)
{
	// [ 2019-7-11 : magedoga ] Check WorldContextObject
	if (InWorldContextObject == nullptr)
	{
		UE_LOG(ActorSpawnUtility, Warning, TEXT("WRActorSpawnUtility::Spawn - Null WorldContextObject. Please Check"));
		return nullptr;
	}

#if USE_GENERATEDCLASS
	// [ 2019-7-11 : magedoga ] Get Generated Class of BP
	TSubclassOf<UObject> GeneratedClassOfBP = WRActorSpawnUtility::GetGeneratedClass<TActorType>(InFullPath);
#endif // USE_GENERATEDCLASS

	// [ 2019-7-11 : magedoga ] Spawn Actor From World
	UWorld* World = InWorldContextObject->GetWorld();
	checkf(World != nullptr, TEXT("Null World. Please Check your WorldContextObject"));

	FTransform TR = FTransform(InRotation, InLocation, InScale);
#if USE_GENERATEDCLASS
	AActor* SpawnedActor = World->SpawnActor<TActorType>(GeneratedClassOfBP, TR);
#else
	AActor* SpawnedActor = World->SpawnActor<TActorType>(TActorType::StaticClass(), TR);
#endif // USE_GENERATEDCLASS
	if (SpawnedActor == nullptr)
	{
		UE_LOG(ActorSpawnUtility, Warning, TEXT("Null Spawned Actor."));
		return nullptr;
	}

	return Cast<TActorType>(SpawnedActor);
}

template <class TActorType>
TActorType* WRActorSpawnUtility::Spawn(UObject* InWorldContextObject, TSubclassOf<TActorType> InGeneratedClass)
{
	return WRActorSpawnUtility::Spawn<TActorType>(InWorldContextObject, InGeneratedClass, FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector);
}

template <class TActorType>
TActorType* WRActorSpawnUtility::Spawn(UObject* InWorldContextObject, TSubclassOf<TActorType> InGeneratedClass, const FVector& InLocation, const FRotator& InRotation, FVector InScale /*= FVector::OneVector*/)
{
	// [ 2019-7-11 : magedoga ] Check WorldContextObject
	if (InWorldContextObject == nullptr)
	{
		UE_LOG(ActorSpawnUtility, Warning, TEXT("WRActorSpawnUtility::Spawn - Null WorldContextObject. Please Check"));
		return nullptr;
	}

	// [ 2019-7-11 : magedoga ] Spawn Actor From World
	UWorld* World = InWorldContextObject->GetWorld();
	checkf(World != nullptr, TEXT("Null World. Please Check your WorldContextObject"));

	FTransform TR = FTransform(InRotation, InLocation, InScale);
#if USE_GENERATEDCLASS
	AActor* SpawnedActor = World->SpawnActor<TActorType>(InGeneratedClass, TR);
#else
	AActor* SpawnedActor = World->SpawnActor<TActorType>(TActorType::StaticClass(), TR);
#endif // USE_GENERATEDCLASS
	if (SpawnedActor == nullptr)
	{
		UE_LOG(ActorSpawnUtility, Warning, TEXT("Null Spawned Actor."));
		return nullptr;
	}

	return Cast<TActorType>(SpawnedActor);
}