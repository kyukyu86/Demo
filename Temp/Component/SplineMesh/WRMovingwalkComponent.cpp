// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRMovingwalkComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Actor/EnvActor/MovingWalkway/WREnvMovingWalkway.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Manager/WREnvInteractionManager.h"
#include "Network/IMMessageSender.h"

#define LANE_HEIGHT 100.f
void UWRMovingwalkComponent::RegisterSplineMesh(int32 InNodeIndex, USplineComponent* InRootSplineComp, UStaticMesh* InStaticMesh, TEnumAsByte<ESplineMeshAxis::Type> InForwardAxis, EWREnvSplineInfo InLaneInfo, int32 InTranslucencySortPriority, const FVector & InStartLocation, const FVector & InStartTangent, const FVector & InEndLocation, const FVector & InEndTangent)//, bool bEnableMove)
{
	if (InRootSplineComp == nullptr || InStaticMesh == nullptr)
		return;
	PrimaryComponentTick.bCanEverTick = false;

	NodeIndex = InNodeIndex;

	MovingwalkPtr = Cast<AWREnvMovingWalkway>(InRootSplineComp->GetOwner());
	if (MovingwalkPtr == nullptr)
		return;

	LaneInfo = InLaneInfo;

	this->SetWorldTransform(MovingwalkPtr->GetActorTransform());

	this->SetMobility(EComponentMobility::Movable);
	this->WeldTo(InRootSplineComp);

	this->SetCollisionProfileName(FName("WREnvObject"));

	this->SetStaticMesh(InStaticMesh);
	this->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->SetForwardAxis(InForwardAxis, true);
	this->SetCastShadow(false);

	FVector StartLaneUpVector = FVector::ZeroVector;
	FVector EndLaneUpVector = FVector::ZeroVector;
	if (InLaneInfo == EWREnvSplineInfo::ForwardLane || InLaneInfo == EWREnvSplineInfo::ReverseLane)
	{
		StartLaneUpVector = FVector::UpVector * LANE_HEIGHT;
		EndLaneUpVector = FVector::UpVector * LANE_HEIGHT;
		this->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		this->SetCollisionResponseToAllChannels(ECR_Overlap);
	}

	if (InLaneInfo == EWREnvSplineInfo::ReverseLane)
	{
		this->SetStartAndEnd(InEndLocation + EndLaneUpVector, InEndTangent*-1.f, InStartLocation + StartLaneUpVector, InStartTangent*-1.f);
	}
	else
	{
		this->SetStartAndEnd(InStartLocation + StartLaneUpVector, InStartTangent, InEndLocation + EndLaneUpVector, InEndTangent);
	}
	this->SetTranslucentSortPriority(TranslucencySortPriority);

	this->RegisterComponent();
}


void UWRMovingwalkComponent::UpdateOverlappedActors(AActor* OverlappedActor)
{
	if (OverlappedActor == nullptr || MovingwalkPtr == nullptr)
		return;

	AWRCharacter* InCharacter = Cast<AWRCharacter>(OverlappedActor);

	if (InCharacter == nullptr)
		return;

	if (InCharacter->IsMe() && MovingwalkPtr->IsBoardingMovingwalk(InCharacter) == false)
	{
		if (MovingwalkPtr->IsOverlappedActor(InCharacter->GetActorID()) == true)
		{
			WREnvInteractionManager::Get()->OnReq_GetOffMW_U2G();
		}
		else
		{
			WREnvInteractionManager::Get()->OnReq_GetOnMW_U2G();
		}
	}
}

void UWRMovingwalkComponent::BeginPlay()
{
	Super::BeginPlay();
	if (LaneInfo == EWREnvSplineInfo::ForwardLane || LaneInfo == EWREnvSplineInfo::ReverseLane)
	{
		this->OnComponentBeginOverlap.AddUniqueDynamic(this, &UWRMovingwalkComponent::OnBeginOverlap);
		this->OnComponentEndOverlap.AddUniqueDynamic(this, &UWRMovingwalkComponent::OnEndOverlap);
		this->SetComponentTickEnabled(true);
	}
}

void UWRMovingwalkComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	this->SetComponentTickEnabled(false);
	if (this->OnComponentBeginOverlap.IsBound())
		this->OnComponentBeginOverlap.RemoveAll(this);

	if (this->OnComponentEndOverlap.IsBound())
		this->OnComponentEndOverlap.RemoveAll(this);
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UWRMovingwalkComponent::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * Other, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	this->UpdateOverlappedActors(Other);
}

void UWRMovingwalkComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	this->UpdateOverlappedActors(Other);
}

