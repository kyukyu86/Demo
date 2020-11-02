// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVoiceRecognitionVolume.h"
#include "Manager/WRVoiceContentsManager.h"

AWRVoiceRecognitionVolume::AWRVoiceRecognitionVolume(const FObjectInitializer& ObjectInitializer) : AWRVolume(ObjectInitializer)
{
	this->GetBrushComponent()->SetCollisionProfileName(TEXT("WRVoiceRecognition"));
}

void AWRVoiceRecognitionVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AWRVoiceRecognitionVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	WRVoiceContentsManager::Get()->AddAvailableVoiceCommand(VoiceCommandDataTIDs);
}

void AWRVoiceRecognitionVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	WRVoiceContentsManager::Get()->RemoveAvailableVoiceCommand(VoiceCommandDataTIDs);
}