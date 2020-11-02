// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentSFX.h"

#include "Manager/WRSFXManager.h"

void UWRActorComponentSFX::OnCreateComponent()
{
	Super::OnCreateComponent();
}

void UWRActorComponentSFX::OnDestroyComponent()
{
	this->RemoveSFX();

	Super::OnDestroyComponent();
}

UAudioComponent* UWRActorComponentSFX::AddSFX(EWRSFXActor InType, const FString& InFilePath, USceneComponent* InAttachToComponent, const float InDuration /*= 0.0F*/, const float InVolumeMultiplier /*= 1.0F*/, const float InPitchMultiplier /*= 1.0F*/)
{
	if (SFXs.Contains(InType))
	{
		return nullptr;
	}

	UAudioComponent* AudioComponent = WRSFXManager::Get()->SpawnAttached(InFilePath, InAttachToComponent, InVolumeMultiplier, InPitchMultiplier);
	if (AudioComponent == nullptr)
	{
		return nullptr;
	}

#if UE_EDITOR
	if (AudioComponent->Sound)
	{
		if (AudioComponent->Sound->IsLooping() == false)
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(AudioComponent->Sound));

			FString Text = FString::Format(TEXT("Spawn infinitely looping sound asset {0}."), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRAnimNotify_SFX::Notify()"));
		}
	}
#else // UE_EDITOR		
#endif // UE_EDITOR
	AudioComponent->FadeIn(InDuration);

	SFXs.Add(InType, AudioComponent);

	return AudioComponent;
}

UAudioComponent* UWRActorComponentSFX::ChangeSFX(EWRSFXActor InType, const FString& InFilePath, USceneComponent* InAttachToComponent, const float InDuration /*= 0.0F*/, const float InVolumeMultiplier /*= 1.0F*/, const float InPitchMultiplier /*= 1.0F*/)
{
	this->RemoveSFX(InType, InDuration);

	UAudioComponent* AudioComponent = WRSFXManager::Get()->SpawnAttached(InFilePath, InAttachToComponent, InVolumeMultiplier, InPitchMultiplier);
	if (AudioComponent == nullptr)
	{
		return nullptr;
	}

#if UE_EDITOR
	if (AudioComponent->Sound)
	{
		if (AudioComponent->Sound->IsLooping() == false)
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(AudioComponent->Sound));

			FString Text = FString::Format(TEXT("Spawn infinitely looping sound asset {0}."), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRAnimNotify_SFX::Notify()"));
		}
	}
#else // UE_EDITOR		
#endif // UE_EDITOR
	AudioComponent->FadeIn(InDuration);

	SFXs.Add(InType, AudioComponent);

	return AudioComponent;
}

void UWRActorComponentSFX::RemoveSFX(EWRSFXActor InType, const float InShouldFadeAudio /*= -1.0F*/)
{
	if (SFXs.Contains(InType) == false)
	{
		return;
	}

	WRSFXManager::Get()->Remove(SFXs[InType], InShouldFadeAudio);
	SFXs.Remove(InType);
}

void UWRActorComponentSFX::RemoveSFX(const float InShouldFadeAudio /*= -1.0F*/)
{
	for (auto& SFX : SFXs)
	{
		WRSFXManager::Get()->Remove(SFX.Value, InShouldFadeAudio);
	}
	SFXs.Empty();
}

UAudioComponent* UWRActorComponentSFX::GetSFX(EWRSFXActor InType)
{
	if (SFXs.Contains(InType) == false)
	{
		return nullptr;
	}
	return SFXs[InType];
}