// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVFXComponentSound.h"

#include "Actor/Effect/WRVFX.h"

#include "Components/AudioComponent.h"

#include "Manager/WRSFXManager.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WREnumUtility.h"

#include "Kismet/GameplayStatics.h"

void UWRVFXComponentSound::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& VFXSound : VFXSounds)
	{
		if (VFXSound.IsSpawnWhenCreate)
		{
			continue;
		}

#if UE_EDITOR
		if (VFXSound.Delay < 0.0F)
		{
		}
		else
		{
			if (this->GetOwnerVFX()->GetVFX().Lifespan < VFXSound.Delay)
			{
				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Delay of Sound Component is larger than Lifespan of BP"), TEXT("UWRVFXComponentSound::SetActive()"));
			}
		}
#else // UE_EDITOR
#endif // UE_EDITOR

		bool IsSpawned = VFXSound.Tick(DeltaTime);
		if (IsSpawned != VFXSound.IsSpawned)
		{
			VFXSound.IsSpawned = IsSpawned;

#if UE_EDITOR
			if (WRActorUtility::IsEditorPreview(this->GetOwnerVFX()->GetSpawner()))
			{
				if (VFXSound.GeneratedClass->IsLooping())
				{
					TArray<FStringFormatArg> StringArgs;
					StringArgs.Add(GetNameSafe(this));
					StringArgs.Add(GetNameSafe(VFXSound.GeneratedClass));

					FString Text = FString::Format(TEXT("BP {0} tried to spawn infinitely looping sound asset {1}."), StringArgs);

					FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRVFXComponentSound::TickComponent()"));
					return;
				}

				UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAttached(VFXSound.GeneratedClass, this->GetOwnerVFX()->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, VFXSound.VolumeMultiplier, VFXSound.PitchMultiplier);
			}
			else
#else // UE_EDITOR
#endif // UE_EDITOR
			{
				UAudioComponent* AudioComponent = WRSFXManager::Get()->SpawnAttached(VFXSound.GeneratedClass, this->GetOwnerVFX()->GetRootComponent(), VFXSound.VolumeMultiplier, VFXSound.PitchMultiplier);
				if (AudioComponent == nullptr)
				{
					return;
				}
				VFXSound.UID = AudioComponent->GetUniqueID();

				if (VFXSound.GeneratedClass->IsLooping())
				{
					AudioComponent->FadeIn(VFXSound.Duration);
				}
			}
		}
		else
		{
		}
	}
}

void UWRVFXComponentSound::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	WRActorUtility::SetComponentTickEnable(this, bNewActive);

	if (bNewActive)
	{
		for (auto& VFXSound : VFXSounds)
		{
			VFXSound.Reset();

			if (VFXSound.GeneratedClass == nullptr)
			{
#ifdef UE_EDITOR
				TArray<FStringFormatArg> StringArgs;
				StringArgs.Add(GetNameSafe(this->GetOwner()));

				FString Text = FString::Format(TEXT("GeneratedClass == nullptr : {0}"), StringArgs);

				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRVFXComponentSound::SetActive()"));
#else // UE_EDITOR
#endif // UE_EDITOR
				continue;
			}

			if (VFXSound.IsSpawnWhenCreate == false)
			{
				continue;
			}
#if UE_EDITOR
			if (WRActorUtility::IsEditorPreview(this->GetOwnerVFX()->GetSpawner()))
			{
				if (VFXSound.GeneratedClass->IsLooping())
				{
					TArray<FStringFormatArg> StringArgs;
					StringArgs.Add(GetNameSafe(this->GetOwner()));
					StringArgs.Add(GetNameSafe(VFXSound.GeneratedClass));

					FString Text = FString::Format(TEXT("BP {0} tried to spawn infinitely looping sound asset {1}."), StringArgs);

					FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRVFXComponentSound::SetActive()"));
					return;
				}

				UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAttached(VFXSound.GeneratedClass, this->GetOwnerVFX()->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, VFXSound.VolumeMultiplier, VFXSound.PitchMultiplier);
			}
			else
#else // UE_EDITOR
#endif // UE_EDITOR
			{
				UAudioComponent* AudioComponent = WRSFXManager::Get()->SpawnAttached(VFXSound.GeneratedClass, this->GetOwnerVFX()->GetRootComponent(), VFXSound.VolumeMultiplier, VFXSound.PitchMultiplier);
				if (AudioComponent == nullptr)
				{
					return;
				}
				VFXSound.UID = AudioComponent->GetUniqueID();

				if (VFXSound.GeneratedClass->IsLooping())
				{
					AudioComponent->FadeIn(VFXSound.Duration);
				}
			}
		}
	}
	else
	{
		for (auto& VFXSound : VFXSounds)
		{
			if (VFXSound.GeneratedClass == nullptr)
			{
#ifdef UE_EDITOR
				TArray<FStringFormatArg> StringArgs;
				StringArgs.Add(GetNameSafe(this->GetOwner()));

				FString Text = FString::Format(TEXT("GeneratedClass == nullptr : {0}"), StringArgs);

				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRVFXComponentSound::SetActive()"));
#else // UE_EDITOR
#endif // UE_EDITOR
				continue;
			}

			if (VFXSound.GeneratedClass->IsLooping())
			{
				WRSFXManager::Get()->Remove(VFXSound.UID, VFXSound.Duration);
			}
			else
			{
				WRSFXManager::Get()->Remove(VFXSound.UID);
			}
		}
	}
}

bool FWRVFXSounds::Tick(float DeltaTime)
{
	if (Delay < 0.0F)
	{
		return false;
	}

	AccDelay += DeltaTime;
	if (AccDelay < Delay)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void FWRVFXSounds::Reset()
{
	AccDelay = 0.0F;

	IsSpawned = false;
}