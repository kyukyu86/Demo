// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRDanceVolume.h"

#include "Core/WRGameInstance.h"

#include "Components/ArrowComponent.h"
#include "Components/BrushComponent.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WREnumUtility.h"

#include "Manager/WRTODManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRUIManager.h"

#include "Enum/EWRTOD.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"

#include "UI/Panel/WRUIPanel_DanceMode.h"
#include "../Character/WRCharacterMine.h"


AWRDanceVolume::AWRDanceVolume(const FObjectInitializer& ObjectInitializer) : AWRVolume(ObjectInitializer)
{
	this->GetBrushComponent()->SetCollisionProfileName(TEXT("WRDance"));
}

void AWRDanceVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AWRDanceVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AWRCharacter* Character = Cast<AWRCharacter>(OtherActor);
	if (Character == nullptr)
		return;

	if (Character->IsMe() == false)
		return;

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(Character);
	if (MyCharacter == nullptr)
		return;

	UWRActorComponentDanceModeForMine* DanceModeComponent = Character->GetComponent<UWRActorComponentDanceModeForMine>();
	if (DanceModeComponent == nullptr)
		return;

	DanceModeComponent->SetEnterDanceAreaByVolume(true);
	bEnter = true;

	if (DanceModeComponent->IsDanceModeOn())
		return;

	DanceModeComponent->ShowStartDanceUI();
}

void AWRDanceVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	AWRCharacter* Character = Cast<AWRCharacter>(OtherActor);
	if (Character == nullptr)
		return;

	if (Character->IsMe() == false)
		return;

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(Character);
	if (MyCharacter == nullptr)
		return;

	UWRActorComponentDanceModeForMine* DanceModeComponent = Character->GetComponent<UWRActorComponentDanceModeForMine>();
	if (DanceModeComponent == nullptr)
		return;

	DanceModeComponent->SetEnterDanceAreaByVolume(false);
	bEnter = false;
}

void AWRDanceVolume::BeginPlay()
{
	Super::BeginPlay();
}

bool AWRDanceVolume::OnStartDanceButtonClicked()
{

	return false;
}
