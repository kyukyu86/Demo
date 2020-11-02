// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVoiceRecommendVolume.h"
#include "Manager/WRVoiceContentsManager.h"

AWRVoiceRecommendVolume::AWRVoiceRecommendVolume(const FObjectInitializer& ObjectInitializer) : AWRVolume(ObjectInitializer)
{
	this->GetBrushComponent()->SetCollisionProfileName(TEXT("WRVoiceRecognition"));
}

void AWRVoiceRecommendVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AWRVoiceRecommendVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	WRVoiceContentsManager::Get()->AddRecommendedVoiceCommand(RecommendedVoiceCommandTIDs);
}

void AWRVoiceRecommendVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	WRVoiceContentsManager::Get()->RemoveRecommendedVoiceCommand(RecommendedVoiceCommandTIDs);
}