// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"


#define USE_GENERATEDCLASS 1


class THIRDPERSON_API DMActorUtil
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

	static FTimerHandle SetTimer(FTimerDelegate const& InDelegate, float InRate, bool InbLoop);
	static void ClearTimer(FTimerHandle& IN InTimerHandle);

	void SetDOF(const float IN InValue);
};




template <class TActorType>
TSubclassOf<UObject> DMActorUtil::GetGeneratedClass(const FString& InLongPackageName)
{
	UObject* GeneratedClass = nullptr;
	FString BlueprintName = DMActorUtil::ResolvePath(InLongPackageName);
	FStringAssetReference StringAssetRefofBP = FStringAssetReference(BlueprintName);
	GeneratedClass = StringAssetRefofBP.ResolveObject();
	if (GeneratedClass == nullptr)
	{
		GeneratedClass = (UClass*)StringAssetRefofBP.TryLoad();
	}

	if (GeneratedClass == nullptr)
	{
		return nullptr;
	}

	return (UClass*)GeneratedClass;
}

template <class TActorType>
TActorType* DMActorUtil::Spawn(UObject* InWorldContextObject, const FString& InFullPath)
{
	return DMActorUtil::Spawn<TActorType>(InWorldContextObject, InFullPath, FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector);
}

template <class TActorType>
TActorType* DMActorUtil::Spawn(UObject* InWorldContextObject, const FString& InFullPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale /*= FVector::OneVector*/)
{
	if (InWorldContextObject == nullptr)
	{
		return nullptr;
	}

#if USE_GENERATEDCLASS
	TSubclassOf<UObject> GeneratedClassOfBP = DMActorUtil::GetGeneratedClass<TActorType>(InFullPath);
#endif // USE_GENERATEDCLASS

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
		return nullptr;
	}

	return Cast<TActorType>(SpawnedActor);
}

template <class TActorType>
TActorType* DMActorUtil::Spawn(UObject* InWorldContextObject, TSubclassOf<TActorType> InGeneratedClass)
{
	return DMActorUtil::Spawn<TActorType>(InWorldContextObject, InGeneratedClass, FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector);
}

template <class TActorType>
TActorType* DMActorUtil::Spawn(UObject* InWorldContextObject, TSubclassOf<TActorType> InGeneratedClass, const FVector& InLocation, const FRotator& InRotation, FVector InScale /*= FVector::OneVector*/)
{
	if (InWorldContextObject == nullptr)
	{
		return nullptr;
	}

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
		return nullptr;
	}

	return Cast<TActorType>(SpawnedActor);
}