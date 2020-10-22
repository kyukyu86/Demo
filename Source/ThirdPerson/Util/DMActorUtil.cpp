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

bool DMActorUtil::IsTimerActive(FTimerHandle InHandle)
{
	if (UDMGameInstance::GetGameInstance() == nullptr)
		return false;

	UWorld* pWorld = UDMGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return false;

	return pWorld->GetTimerManager().IsTimerActive(InHandle);
}

float DMActorUtil::GetTimerRemaining(FTimerHandle InHandle)
{
	if (UDMGameInstance::GetGameInstance() == nullptr)
		return false;

	UWorld* pWorld = UDMGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return false;

	return pWorld->GetTimerManager().GetTimerRemaining(InHandle);
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

void DMActorUtil::SetActorTickEnable(class AActor* InActor, const bool bInEnabled)
{
	if (InActor == nullptr)
	{
		return;
	}

	if (bInEnabled)
	{
		if (InActor->PrimaryActorTick.IsTickFunctionRegistered() == false)
		{
			InActor->PrimaryActorTick.RegisterTickFunction(InActor->GetWorld()->GetCurrentLevel());
			InActor->PrimaryActorTick.Target = InActor;
		}

		InActor->PrimaryActorTick.bCanEverTick = true;
		InActor->SetActorTickEnabled(true);
	}
	else
	{
		InActor->SetActorTickEnabled(false);
		InActor->PrimaryActorTick.bCanEverTick = false;

		if (InActor->PrimaryActorTick.IsTickFunctionRegistered())
		{
			InActor->PrimaryActorTick.UnRegisterTickFunction();
		}
	}
}

void DMActorUtil::SetComponentTickEnable(class UActorComponent* InComponent, const bool bInEnabled)
{
	if (InComponent == nullptr)
	{
		return;
	}

	if (bInEnabled)
	{
		if (InComponent->PrimaryComponentTick.IsTickFunctionRegistered() == false)
		{
			InComponent->PrimaryComponentTick.RegisterTickFunction(InComponent->GetWorld()->GetCurrentLevel());
			InComponent->PrimaryComponentTick.Target = InComponent;
		}

		InComponent->PrimaryComponentTick.bCanEverTick = true;
		InComponent->PrimaryComponentTick.SetTickFunctionEnable(true);
	}
	else
	{
		InComponent->PrimaryComponentTick.bCanEverTick = true;
		InComponent->PrimaryComponentTick.SetTickFunctionEnable(false);
		InComponent->PrimaryComponentTick.bCanEverTick = false;

		if (InComponent->PrimaryComponentTick.IsTickFunctionRegistered())
		{
			InComponent->PrimaryComponentTick.UnRegisterTickFunction();
		}
	}
}

bool DMActorUtil::IsInnerActor(AActor* IN InStandardActor, AActor* IN InOtherActor, const float IN InDistance)
{
	if (InStandardActor == nullptr || InOtherActor == nullptr)
		return false;

	FVector Actor1Location = InStandardActor->GetActorLocation();
	FVector Actor2Location = InOtherActor->GetActorLocation();
	float fActorDistance = FVector::Distance(Actor1Location, Actor2Location);
	if (fActorDistance < InDistance)
		return true;

	return false;
}

bool DMActorUtil::IsInnerComponent(AActor* IN InStandardActor, UPrimitiveComponent* IN InComponent, const float IN InDistance)
{
	if (InStandardActor == nullptr || InComponent == nullptr)
		return false;

	FVector Actor1Location = InStandardActor->GetActorLocation();
	FVector Actor2Location = InComponent->GetComponentLocation();
	float fActorDistance = FVector::Distance(Actor1Location, Actor2Location);
	if (fActorDistance < InDistance)
		return true;

	return false;
}
