// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvZipline.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Components/SceneComponent.h"
#include "Manager/WRObjectInteractionManager.h"

AWREnvZipline::AWREnvZipline() : AWREnvActor()
{

}

void AWREnvZipline::BeginPlay()
{
	Super::BeginPlay();

	Zipline = Cast<USplineComponent>(this->GetComponentByClass(USplineComponent::StaticClass()));
	if (Zipline == nullptr)
	{
		ensureMsgf(false, TEXT("Zipline is not setted"));
	}
	
	TArray<UActorComponent*> StartPointBoxComponents = this->GetComponentsByTag(UBoxComponent::StaticClass(), "StartPoint");
	if (StartPointBoxComponents.IsValidIndex(0))
	{
		StartPoint = Cast<UBoxComponent>(StartPointBoxComponents[0]);
//		StartPoint->OnComponentBeginOverlap.AddDynamic(this, &AWREnvZipline::OnBeginOverlap_InteractionableArea);
	}
	else
	{
		ensureMsgf(false, TEXT("StartPoint is not setted"));
	}
	TArray<UActorComponent*> EndPointBoxComponents = this->GetComponentsByTag(UBoxComponent::StaticClass(), "EndPoint");
	if (EndPointBoxComponents.IsValidIndex(0))
	{
		EndPoint = Cast<UBoxComponent>(EndPointBoxComponents[0]);
	}
	else
	{
		ensureMsgf(false, TEXT("EndPoint is not setted"));
	}
	TArray<UActorComponent*> AttachComponents = this->GetComponentsByTag(USceneComponent::StaticClass(), "AttachComponent");
	if (AttachComponents.IsValidIndex(0))
	{
		AttachComponent = Cast<USceneComponent>(AttachComponents[0]);
	}
	else
	{
		ensureMsgf(false, TEXT("Attach Component is not setted"));
	}

//	WRObjectInteractionManager::Get()->AddZipline(this);
}

void AWREnvZipline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Movement
	if (bMovement == false)
		return;

	// move AttachComponent
}

//====================================================================================

void AWREnvZipline::AttachZiplineHandle(AWRCharacterIO* IN InZiplineHandle)
{
	AttachedHandle = InZiplineHandle;
	AttachedHandle->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void AWREnvZipline::ActiveZipline()
{
	bMovement = true;
}

void AWREnvZipline::ArrivalEndPoint()
{
	bMovement = false;
	AttachedHandle->ArrivalZiplineEndPoint();
}
