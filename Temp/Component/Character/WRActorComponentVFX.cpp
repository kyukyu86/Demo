// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentVFX.h"

#include "Manager/WRVFXManager.h"

#include "Utility/WREnumUtility.h"

void UWRActorComponentVFX::OnCreateComponent()
{
	for (int32 i = 0; i < (int32)EWRVFXActor::Max; ++i)
	{
		VFXs.Add((EWRVFXActor)i, new TDoubleLinkedList<AWRVFX*>());
	}

	Super::OnCreateComponent();
}

void UWRActorComponentVFX::OnDestroyComponent()
{
	for (auto& VFX : VFXs)
	{
		TDoubleLinkedList<AWRVFX*>* Actors = VFX.Value;
		for (auto& Actor : *Actors)
		{
			WRVFXManager::Get()->Remove(Actor);
		}
		Actors->Empty();

		delete Actors;
		Actors = nullptr;
	}
	VFXs.Empty();

	Super::OnDestroyComponent();
}

AWRVFX* UWRActorComponentVFX::AddVFX(EWRVFXActor InType, const FString& InFullPath, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle /*= false*/)
{
	if (VFXs.Contains(InType) == false)
	{
		return nullptr;
	}

	AWRVFX* VFX = WRVFXManager::Get()->Spawn(InFullPath, InSpawner, InTarget, nullptr, EWRVFX::Component);
	if (VFX == nullptr)
	{
		return nullptr;
	}
	VFX->PostInitialize(InShouldFadeParticle);

	VFXs[InType]->AddTail(VFX);

	return VFX;
}

AWRVFX* UWRActorComponentVFX::AddVFX(EWRVFXActor InType, TSubclassOf<AWRVFX> InGeneratedClass, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle /*= false*/)
{
	if (VFXs.Contains(InType) == false)
	{
		return nullptr;
	}

	AWRVFX* VFX = WRVFXManager::Get()->Spawn(InGeneratedClass, InSpawner, InTarget, nullptr, EWRVFX::Component);
	if (VFX == nullptr)
	{
		return nullptr;
	}
	VFX->PostInitialize(InShouldFadeParticle);

	VFXs[InType]->AddTail(VFX);

	return VFX;
}

AWRVFX* UWRActorComponentVFX::ChangeVFX(EWRVFXActor InType, const FString& InFullPath, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle /*= false*/)
{
	this->RemoveVFX(InType, InShouldFadeParticle);

	AWRVFX* VFX = WRVFXManager::Get()->Spawn(InFullPath, InSpawner, InTarget, nullptr, EWRVFX::Component);
	if (VFX == nullptr)
	{
		return nullptr;
	}
	VFX->PostInitialize(InShouldFadeParticle);

	VFXs[InType]->AddTail(VFX);

	return VFX;
}

AWRVFX* UWRActorComponentVFX::ChangeVFX(EWRVFXActor InType, TSubclassOf<AWRVFX> InGeneratedClass, AActor* InSpawner, AActor* InTarget, const bool InShouldFadeParticle /*= false*/)
{
	this->RemoveVFX(InType, InShouldFadeParticle);

	AWRVFX* VFX = WRVFXManager::Get()->Spawn(InGeneratedClass, InSpawner, InTarget, nullptr, EWRVFX::Component);
	if (VFX == nullptr)
	{
		return nullptr;
	}
	VFX->PostInitialize(InShouldFadeParticle);

	VFXs[InType]->AddTail(VFX);

	return VFX;
}

void UWRActorComponentVFX::RemoveVFX(const uint32 InIndex, EWRVFXActor InType, const bool InShouldFadeParticle /*= false*/)
{
	if (VFXs.Contains(InType) == false)
	{
		return;
	}

	AWRVFX* VFX = WRVFXManager::Get()->Find(InIndex);
	if (VFX == nullptr)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(WREnumUtility::EnumToString("EWRVFXActor", InType));
		
		FString Text = FString::Format(TEXT("VFX {0} already was removed."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRActorComponentVFX::RemoveVFX()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
		return;
	}

	WRVFXManager::Get()->Remove(VFX);

	VFXs[InType]->RemoveNode(VFX);
}

void UWRActorComponentVFX::RemoveVFX(AWRVFX* InVFX, EWRVFXActor InType, const bool InShouldFadeParticle /*= false*/)
{
	if (VFXs.Contains(InType) == false)
	{
		return;
	}

	WRVFXManager::Get()->Remove(InVFX);

	VFXs[InType]->RemoveNode(InVFX);
}

void UWRActorComponentVFX::RemoveVFX(EWRVFXActor InType, const bool InShouldFadeParticle /*= false*/)
{
	if (VFXs.Contains(InType) == false)
	{
		return;
	}

	TDoubleLinkedList<AWRVFX*>* Actors = VFXs[InType];
	for (auto& Actor : *Actors)
	{
		WRVFXManager::Get()->Remove(Actor, InShouldFadeParticle);
	}
	Actors->Empty();
}

void UWRActorComponentVFX::RemoveVFX()
{
	for (auto& VFX : VFXs)
	{
		TDoubleLinkedList<AWRVFX*>* Actors = VFX.Value;
		for (auto& Actor : *Actors)
		{
			WRVFXManager::Get()->Remove(Actor);
		}
		Actors->Empty();
	}
}

TDoubleLinkedList<AWRVFX*>* UWRActorComponentVFX::GetVFXs(EWRVFXActor InType)
{
	return VFXs.Contains(InType) == false ? nullptr : VFXs[InType];
}

bool UWRActorComponentVFX::Contains(EWRVFXActor InType)
{
	if (VFXs.Contains(InType) == false)
	{
		return false;
	}

	TDoubleLinkedList<AWRVFX*>* Actors = VFXs[InType];
	if (Actors == nullptr)
	{
		return false;
	}

#if UE_4_25
	return Actors->Num() > 0;
#else // UE_4_25
	return Actors->Num();
#endif // UE_4_25
}