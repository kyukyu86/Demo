// Fill out your copyright notice in the Description page of Project Settings.


#include "DMActorUtil.h"
#include "../GameInstance/DMGameInstance.h"
#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"


FString DMActorUtil::ResolvePath(const FString& InPath)
{
	FString ResolvedPath = InPath;
	int32 DotIndex = -1;
	ResolvedPath.FindLastChar('.', DotIndex);
	if (DotIndex != -1)
	{
		FString Check_C = InPath.Right(2);
		if (Check_C.ToLower() != TEXT("_c"))
		{
			ResolvedPath += TEXT("_C");
		}
	}
	else
	{
		int32 SlashIndex;
		InPath.FindLastChar('/', SlashIndex);
		FString FileName = InPath.Right(InPath.Len() - SlashIndex - 1);
		ResolvedPath += TEXT(".") + FileName + TEXT("_C");
	}

	return ResolvedPath;
}

FTimerHandle DMActorUtil::SetTimer(FTimerDelegate const& InDelegate, float InRate, bool InbLoop)
{
	FTimerHandle TimerHandle;

	if (UDMGameInstance::GetGameInstance() == nullptr)
		return TimerHandle;

	UWorld* pWorld = UDMGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return TimerHandle;

	pWorld->GetTimerManager().SetTimer(TimerHandle, InDelegate, InRate, InbLoop);
	return TimerHandle;
}

void DMActorUtil::ClearTimer(FTimerHandle& IN InTimerHandle)
{
	if (UDMGameInstance::GetGameInstance() == nullptr)
		return;

	UWorld* pWorld = UDMGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return;

	pWorld->GetTimerManager().ClearTimer(InTimerHandle);
}

void DMActorUtil::SetDOF(const float IN InValue)
{
	UWorld* World = UDMGameInstance::GetGameInstance()->GetWorld();
	for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	{
		APostProcessVolume* PostProcessVolume = *Iter;
		if (PostProcessVolume == nullptr)
			continue;

		FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
		FPostProcessSettings* PostProcessSettings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);

		if (InValue != 0)
		{
			PostProcessSettings->bOverride_DepthOfFieldFocalDistance = true;
			PostProcessSettings->DepthOfFieldFocalDistance = InValue;
		}
		else
		{
			PostProcessSettings->bOverride_DepthOfFieldFocalDistance = false;
			PostProcessSettings->DepthOfFieldFocalDistance = InValue;
		}
	}
}
