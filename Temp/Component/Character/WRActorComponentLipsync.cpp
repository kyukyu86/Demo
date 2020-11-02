// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentLipsync.h"
#include "Sound/SoundBase.h"
#include "Curves/CurveFloat.h"
#include "Manager/WRSFXManager.h"
#include "Components/AudioComponent.h"
#include "Actor/Character/WRCharacter.h"

void UWRActorComponentLipsync::OnCreateComponent()
{

}

void UWRActorComponentLipsync::OnDestroyComponent()
{
	this->Stop();
}

void UWRActorComponentLipsync::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ElapsedTime += DeltaTime;

	float Min, Max = 0.0f;
	LipsyncCurve->GetTimeRange(Min, Max);

	if (Max < ElapsedTime)
	{
		this->Stop();
	}
}

bool UWRActorComponentLipsync::Start(class USoundBase* InSound, class UCurveFloat* InLipsyncCurve, bool bIn3DSound /*= false*/)
{
	if (IsTalking() == true)
		return false;

	if (InSound->IsValidLowLevel() == false || InLipsyncCurve->IsValidLowLevel() == false)
		return false;

	if (bIn3DSound)
	{
		AWRCharacter* OwnerCharacter = GetOwnerCharacter();
		if (OwnerCharacter == nullptr)
			return false;

		LipsyncAudioComponent = WRSFXManager::Get()->SpawnAttached(InSound, OwnerCharacter->GetRootComponent());
	}
	else
	{
		LipsyncAudioComponent = WRSFXManager::Get()->Spawn2D(InSound);
	}
	
	if (LipsyncAudioComponent->IsValidLowLevel() == false)
		return false;

	ElapsedTime = 0.0f;

	LipsyncCurve = InLipsyncCurve;
	LipsyncCurve->AddToRoot();

	// [ 2019-11-20 : magedoga ] On Tick
	this->SetEnableTick(true);

	return true;
}

void UWRActorComponentLipsync::Stop()
{
	if (IsTalking() == false)
		return;

	WRSFXManager::Get()->Remove(LipsyncAudioComponent);
	LipsyncAudioComponent = nullptr;

	if (LipsyncCurve->IsValidLowLevel())
		LipsyncCurve->RemoveFromRoot();
	LipsyncCurve = nullptr;

	ElapsedTime = 0.0f;

	this->SetEnableTick(false);
}



bool UWRActorComponentLipsync::IsTalking() const
{
	return LipsyncAudioComponent != nullptr;
}

float UWRActorComponentLipsync::GetLipsyncAlpha()
{
	if (this->IsTalking() == false)
		return 0.0f;

	return LipsyncCurve->GetFloatValue(ElapsedTime);
}
