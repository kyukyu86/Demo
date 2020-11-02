// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvActor.h"

// Sets default values
AWREnvActor::AWREnvActor() : Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

AWREnvActor::AWREnvActor(EWREnvType InEnvType) : Super(), EnvType(InEnvType)
{
	
}

// Called when the game starts or when spawned
void AWREnvActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWREnvActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

