// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvDestructible.h"

#include "Component/Destructible/WRDestructibleComponent.h"
#include "Components/CapsuleComponent.h"

AWREnvDestructible::AWREnvDestructible()
{

}

void AWREnvDestructible::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
}

void AWREnvDestructible::BeginPlay()
{
	Super::BeginPlay();

	pDestructibleComp = Cast<UWRDestructibleComponent>(this->GetComponentByClass(UWRDestructibleComponent::StaticClass()));
	pDestructibleComp->OnComponentFracture.AddUniqueDynamic(this, &AWREnvDestructible::OnTouchedFracture);
	pCapsuleCollisionComp = Cast<UCapsuleComponent>(this->GetComponentByClass(UCapsuleComponent::StaticClass()));
	//pDestructibleComp->end
	if (pDestructibleComp == nullptr || pCapsuleCollisionComp == nullptr)
		return;

	pCapsuleCollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWREnvDestructible::OnBeginOverlapCapsuleHitting);
}

void AWREnvDestructible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWREnvDestructible::OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (pDestructibleComp == nullptr)
		return;
	
	AWRCharacter* InCharacter = Cast<AWRCharacter>(Other);

	if (InCharacter == nullptr)
		return;	

	pDestructibleComp->BeginDestruction();
}

void AWREnvDestructible::OnTouchedFracture(const FVector& HitPosition, const FVector& HitDirection)
{
	UE_LOG(EnvInteractionManager, Display, TEXT(" AWREnvDestructible::OnTouchedFracture"));
}
