// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvDoor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Manager/WRSFXManager.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"

void AWREnvDoor::BeginPlay()
{
	Super::BeginPlay();

	InBoxComp = Cast<UBoxComponent>(this->GetComponentByClass(UBoxComponent::StaticClass()));
	
	if (InBoxComp == nullptr)
		return;

	InBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InBoxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
	InBoxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel5, ECR_Ignore);
	//InBoxComp->OnComponentHit.AddDynamic(this, &AWRDynamicFoliage::OnComponentHit);
	InBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AWREnvDoor::OnOverlapBegin);
	InBoxComp->OnComponentEndOverlap.AddDynamic(this, &AWREnvDoor::OnOverlapEnd);
}

void AWREnvDoor::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
}

void AWREnvDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsOpened == false)
	{
		bIsOpened = true;

		if (OpenSound.IsNull() == false)
		{
			USoundBase* OpenSoundBase = Cast<USoundBase>(OpenSound.LoadSynchronous());

			WRSFXManager::Get()->SpawnAtLocation(OpenSoundBase, this->GetActorLocation());
		}
	}
}

void AWREnvDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsOpened == true)
	{
		bIsOpened = false;
		if (CloseSound.IsNull() == false)
		{
			USoundBase* CloseSoundBase = Cast<USoundBase>(CloseSound.LoadSynchronous());

			WRSFXManager::Get()->SpawnAtLocation(CloseSoundBase, this->GetActorLocation());
		}
	}
}

