// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVehicleSpawnVolume.h"
#include "../Vehicle/WRVehicle.h"


AWRVehicleSpawnVolume::AWRVehicleSpawnVolume(const FObjectInitializer& ObjectInitializer) : AWRVolume(ObjectInitializer)
{
	this->GetBrushComponent()->SetCollisionProfileName(TEXT("WRVehicleSpawn"));
}

void AWRVehicleSpawnVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AWRVehicleSpawnVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AWRVehicle* Vehicle = Cast<AWRVehicle>(OtherActor);
	if (Vehicle == nullptr)
		return;

	Vehicle->SetVehicleInSpawnVolume(true);
}

void AWRVehicleSpawnVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	AWRVehicle* Vehicle = Cast<AWRVehicle>(OtherActor);
	if (Vehicle == nullptr)
		return;

	Vehicle->SetVehicleInSpawnVolume(false);
}

void AWRVehicleSpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}
