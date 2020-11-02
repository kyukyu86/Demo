// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRTODVolume.h"

#include "Core/WRGameInstance.h"

#include "Components/ArrowComponent.h"
#include "Components/BrushComponent.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WREnumUtility.h"

#include "Manager/WRTODManager.h"
#include "Manager/WRCharacterManager.h"

#include "Enum/EWRTOD.h"

AWRTODVolume::AWRTODVolume(const FObjectInitializer& ObjectInitializer) : AWRVolume(ObjectInitializer)
{
#if UE_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
#else // UE_EDITOR
#endif // UE_EDITOR

	this->GetBrushComponent()->SetCollisionProfileName(TEXT("WRTOD"));

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent0"));
	if (ArrowComponent == nullptr)
	{
		return;
	}

	ArrowComponent->ArrowColor = FColor(150, 200, 255);
	ArrowComponent->bTreatAsASprite = true;
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->bIsScreenSizeScaled = true;
	ArrowComponent->bUseInEditorScaling = true;
#endif // WITH_EDITORONLY_DATA
}

void AWRTODVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if UE_EDITOR
	if (WRTODManager::IsValid() == false)
	{
		return;
	}

	CurrentDayCycle = WRTODManager::Get()->GetCurrent();
#else // UE_EDITOR
#endif // UE_EDITOR
}

bool AWRTODVolume::ShouldTickIfViewportsOnly() const
{
#if UE_EDITOR
	return true;
#else // 
	return false;
#endif // UE_EDITOR
}

void AWRTODVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AWRCharacter* Character = Cast<AWRCharacter>(OtherActor);
	if (Character == nullptr)
	{
		return;
	}

	if (Character->IsMe() == false)
	{
		return;
	}

	if (IsOverlap)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Already overlap"), TEXT("AWRTODVolume::NotifyActorBeginOverlap()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	FWRWind LocalWind;
	LocalWind.Strength = Strength;
	LocalWind.Speed = Speed;
	LocalWind.MinGustAmount = MinGustAmount;
	LocalWind.MaxGustAmount = MaxGustAmount;
	LocalWind.Direction = this->GetActorRotation();

	FWRTODETC LocalETC;
	LocalETC.Raining = Raining;
	LocalETC.Thundering = Thundering;
	LocalETC.Lightning = Lightning;

	WRTODManager::Get()->SpawnTOD(DayCycle, Priority, Duration, LocalETC, LocalWind);

	IsOverlap = true;
}

void AWRTODVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	AWRCharacter* Character = Cast<AWRCharacter>(OtherActor);
	if (Character == nullptr)
	{
		return;
	}

	//WRTODManager::Get()->RemoveTOD(DayCycle, Duration);
	//WRTODManager::Get()->RemoveFX(DayCycle, (EWRTODFX)FX);

	IsOverlap = false;
}

#if WITH_EDITOR
void AWRTODVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Priority = FMath::Clamp(Priority, 1, SHORT_MAX);

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#else // WITH_EDITOR
#endif // WITH_EDITOR

void AWRTODVolume::BeginPlay()
{
	Super::BeginPlay();
}