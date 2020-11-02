// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvMovingWalkway.h"
#include "Components/ShapeComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "Manager/WRSFXManager.h"

#include "Component/SplineMesh/WRMovingwalkComponent.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WREnvInteractionManager.h"
#include "Network/IMMessageSender.h"
#include "UnrealMathUtility.h"
#include "Table/WREnvObjectTable.h"
#include "Table/Base/WRTableManager.h"

// Sets default values
AWREnvMovingWalkway::AWREnvMovingWalkway()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWREnvMovingWalkway::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	InMovingwalkLanes.Empty();
	this->ModifySplineMeshs();

}

void AWREnvMovingWalkway::BeginPlay()
{
	Super::BeginPlay();
	this->InitializeMovingWalkway();
	WREnvInteractionManager::Get()->AddMovingWalkway(this);

}

// Called every frame
void AWREnvMovingWalkway::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	this->SoundProcess();
	this->MovingWalkProcess(DeltaTime);
}

void AWREnvMovingWalkway::SpawnSoundEffect(const AActor& InActor)
{
	UAudioComponent* InMovingwalkSound = WRSFXManager::Get()->Find(InActor.GetUniqueID());
	if (InMovingwalkSound == nullptr)
	{
		FVector PlayerLocation = InRootSplineComp->FindLocationClosestToWorldLocation(InActor.GetActorLocation(), ESplineCoordinateSpace::World);

		InMovingwalkSound = WRSFXManager::Get()->SpawnAtLocation(SoundEffectPath, PlayerLocation, -1.f, -1.f, InActor.GetUniqueID());

		if (InMovingwalkSound != nullptr)
			InMovingwalkSound->SetMobility(EComponentMobility::Movable);
		//InMovingwalkSoundUID = InActor.GetUniqueID();
	}
}
#define CM_PER_ONEMETER 100.f
void AWREnvMovingWalkway::InitializeMovingWalkway()
{
	if (this->GetWorld()->IsPreviewWorld() == true)
	{
		return;
	}

	UDataTable * MovingWalkTable = WRTableManager::Get()->GetTable<FWRMovingWalkwayTable>();

	if (MovingWalkTable != nullptr)
	{
		TArray<FWRMovingWalkwayTable*> InMWTableArr;
		MovingWalkTable->GetAllRows(TEXT(""), InMWTableArr);

		for (FWRMovingWalkwayTable* It : InMWTableArr)
		{
			if (It == nullptr)
				return;

			FString InFilename = It->BlueprintPath;
			int32 SlashIndex;
			InFilename.FindLastChar('/', SlashIndex);
			InFilename = InFilename.Right(InFilename.Len() - SlashIndex - 1);

			int32 DotIndex;
			InFilename.FindLastChar('.', DotIndex);
			InFilename = InFilename.Left(DotIndex);

			FString ThisActorFileName = this->GetClass()->GetName();

			if (ThisActorFileName.Right(2) == TEXT("_C"))
			{
				ThisActorFileName = ThisActorFileName.LeftChop(2);
			}

			if (InFilename == ThisActorFileName)
			{
				this->SetTID(It->EnvObjectTID);
				this->SoundEffectPath = It->SoundEffectPath;
				this->MoveMeterPerSpeed = It->MoveMeterPerSpeed;
				this->DistanceToSpawnSound = It->DistanceToSpawnSound;
				break;
			}
		}
	}

	//FWRMovingWalkwayTable* NPCTable = WRTableManager::Get()->FindRow<FWRMovingWalkwayTable>(InTarget->GetCharacterTableID());

	float InMoveSpeed = this->MoveMeterPerSpeed * CM_PER_ONEMETER;
	float InSplineLength = InRootSplineComp->GetSplineLength();

	float InMoveDuration = InSplineLength / InMoveSpeed;

	if (InMoveDuration <= 0.f)
		return;

	this->InRootSplineComp->Duration = InMoveDuration;
}

void AWREnvMovingWalkway::ModifySplineMeshs()
{
	InRootSplineComp = Cast<USplineComponent>(this->GetRootComponent());
	if (InRootSplineComp == nullptr)
		return;

	InRootSplineComp->SetMobility(EComponentMobility::Movable);

	this->RegisterNodes();
}

void AWREnvMovingWalkway::RegisterNodes()
{
	this->ClearAllNode();

	int32 NumOfSplinePoint = InRootSplineComp->GetNumberOfSplinePoints();

	for (int32 InIndex = 0; InIndex < NumOfSplinePoint - 1; InIndex++)
	{
		this->CreateNode(InIndex);
	}
}

void AWREnvMovingWalkway::CreateNode(int32 InNodeIndex)
{
	FVector StartPos, StartTangent, EndPos, EndTangent;
	InRootSplineComp->GetLocationAndTangentAtSplinePoint(InNodeIndex, StartPos, StartTangent, ESplineCoordinateSpace::Local);
	InRootSplineComp->GetLocationAndTangentAtSplinePoint(InNodeIndex + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

	TArray<EWREnvSplineInfo> KeyArr;
	SplineMeshs.GenerateKeyArray(KeyArr);

	for (int32 InIndex = 0; InIndex < KeyArr.Num(); InIndex++)
	{
		EWREnvSplineInfo SplineInfo = KeyArr[InIndex];
		if (SplineInfo == EWREnvSplineInfo::None)
			continue;

		UWRMovingwalkComponent* InMovingwalkMesh = NewObject<UWRMovingwalkComponent>(InRootSplineComp);
		if (InMovingwalkMesh == nullptr)
			continue;

		UStaticMesh* InStaticMesh = nullptr;

		TSoftObjectPtr<UStaticMesh>* InSoftStaticMeshPtr = SplineMeshs.Find(SplineInfo);
		if (InSoftStaticMeshPtr == nullptr)
			continue;

		if (SplineMeshs.Find(SplineInfo)->IsNull() == false)
			InStaticMesh = SplineMeshs.Find(SplineInfo)->LoadSynchronous();

		if (InStaticMesh == nullptr)
			continue; // 다른리스트가 활성화 되어 있으면 그냥 진행되게 변경 

		//bool EnableMove = false;
		switch (SplineInfo)
		{
		case EWREnvSplineInfo::PhysicsMesh:
		{
			InMovingwalkMesh->SetVisibility(this->GetDrawPhysicsCollision());
		}
		break;
		case EWREnvSplineInfo::ForwardLane:
		case EWREnvSplineInfo::ReverseLane:
		{
			InMovingwalkMesh->SetVisibility(this->GetDrawOverlapCollision());
			InMovingwalkLanes.Add(InMovingwalkMesh);
			//EnableMove = true;
		}
		break;
		}

		InMovingwalkMesh->RegisterSplineMesh(InNodeIndex, InRootSplineComp, InStaticMesh, ForwardAxis.GetValue(), SplineInfo, TranslucencySortPriority, StartPos, StartTangent, EndPos, EndTangent);// , EnableMove);

		InMovingwalkComps.Add(InMovingwalkMesh);
	}
}

void AWREnvMovingWalkway::DestroyNode(USplineMeshComponent* InNode)
{
	if (InNode == nullptr)
		return;

	if (InNode->IsRegistered() == true)
		InNode->UnregisterComponent();

	InNode->RemoveFromRoot();
	InNode->DestroyComponent();
}

void AWREnvMovingWalkway::ClearAllNode()
{
	for (UWRMovingwalkComponent* It : InMovingwalkComps)
	{
		this->DestroyNode(It);
	}
	InMovingwalkLanes.Empty();
	InMovingwalkComps.Empty();
}

void AWREnvMovingWalkway::SoundProcess()
{
	if (DistanceToSpawnSound < 0.f)
		return;

	AActor* InActor = WRCharacterManager::Get()->GetMyCharacter();
	if (InActor == nullptr)
		return;

	FVector PointLocation = InRootSplineComp->FindLocationClosestToWorldLocation(InActor->GetActorLocation(), ESplineCoordinateSpace::World);

	float DistFromPoint = UKismetMathLibrary::Vector_Distance(PointLocation, InActor->GetActorLocation());

	UAudioComponent* InMovingwalkSound = WRSFXManager::Get()->Find(InActor->GetUniqueID());
	if (DistFromPoint <= DistanceToSpawnSound)
	{
		if (InMovingwalkSound == nullptr)
		{
			this->SpawnSoundEffect(*InActor);
		}
		else
		{
			InMovingwalkSound->SetWorldLocation(PointLocation);
		}
	}
	else
	{
		/*if (InMovingwalkSound != nullptr)
		{
			WRSFXManager::Get()->Remove(InActor->GetUniqueID());
		}*/
	}
}

void AWREnvMovingWalkway::StartRideZipline(AWRCharacter* InCharacter)
{
	if (this->GetCantMoveInput() == false)
		return;

	if (InCharacter == nullptr || InRootSplineComp == nullptr)
		return;
	if (InCharacter->GetCurrentFSMType() == EWRFSM::Zipline)
		return;

	FVector SetStartLocation = InRootSplineComp->FindLocationClosestToWorldLocation(InCharacter->GetActorLocation(), ESplineCoordinateSpace::World);
	InCharacter->SetActorLocation(SetStartLocation);

	InCharacter->DetermineFSMSignal(EWRFSM::Zipline);

	if (InCharacter->IsMe() == true)
	{
		WRInputSystemManager::Get()->SetLocationLock(true);
	}
}

void AWREnvMovingWalkway::EndRideZipline(AActor* InActor)
{
	if (this->GetCantMoveInput() == false)
		return;

	AWRCharacter* InCharacter = Cast<AWRCharacter>(InActor);

	if (InCharacter == nullptr || InRootSplineComp == nullptr)
		return;

	if (InCharacter->GetCharacterType() != EWRCharacterType::Player)
		return;

	InCharacter->DetermineFSMSignal(EWRFSM::Idle);

	if (InCharacter->IsMe() == true)
	{
		WRInputSystemManager::Get()->SetLocationLock(false);
	}
}

void AWREnvMovingWalkway::MovingWalkProcess(float DeltaTime)
{
	if (InRootSplineComp == nullptr)
		return;

	TArray<AActor*> InActors;
	this->GetOverlappingActors(InActors);
	for (AActor * It : InActors)
	{
		AWRCharacter* InCharacter = Cast<AWRCharacter>(It);

		if (InCharacter == nullptr)
		{
			continue;
		}

		float CurrentLocationRatio = InRootSplineComp->FindInputKeyClosestToWorldLocation(InCharacter->GetActorLocation()) / static_cast<float>(InRootSplineComp->GetNumberOfSplinePoints() - 1);
		CurrentLocationRatio = FMath::Clamp(CurrentLocationRatio, 0.f, static_cast<float>(InRootSplineComp->GetNumberOfSplinePoints() - 1));

		float InCurrentTime = InRootSplineComp->Duration * CurrentLocationRatio;
		UWRMovingwalkComponent* InMovingwalkComp = this->GetOverlappingComp(InCharacter);

		if (InMovingwalkComp == nullptr)
			continue;

		// [ 2020-06-16 EZ : On Off 되는 과정에서 GetOff 패킷이 늦게 도착하여 탑승하자마자 내려지는 경우 방지 ]
		if (this->GetOverlappedActors().Contains(InCharacter->GetActorID()) == false)
		{
			AWREnvMovingWalkway* AnotherMovingwalk = WREnvInteractionManager::Get()->FindOverlappedMWByCharacterAID(InCharacter->GetActorID());
			if (AnotherMovingwalk == nullptr)
			{
				AWRCharacterMine* InMine = Cast<AWRCharacterMine>(InCharacter);
				if (InMine != nullptr)
				{
					if (InMine->GetIsBoardingEnv() == false)
						WREnvInteractionManager::Get()->OnReq_GetOnMW_U2G();
				}
				continue;
			}
		}

		float InDeltaTime = DeltaTime;

		if (InMovingwalkComp->GetSplineInfo() == EWREnvSplineInfo::ForwardLane)
			InDeltaTime *= -1.f;

		float NewCurrentTime = InCurrentTime + InDeltaTime;

		NewCurrentTime = FMath::Clamp(NewCurrentTime, 0.f, InRootSplineComp->Duration);

		//UE_LOG(CharacterManager, Display, TEXT("AWREnvMovingWalkway::MovingWalkProcess : NewCurrentTime : %f"), NewCurrentTime);

		FVector CurrentCharacterLocation = InCharacter->GetActorLocation();
		FVector CurrentLocationAtTime = InRootSplineComp->GetLocationAtTime(InCurrentTime, ESplineCoordinateSpace::World);

		FVector CurrentOffset = CurrentCharacterLocation - CurrentLocationAtTime;


		FRotator NewRotator = FRotator(0.f, InRootSplineComp->GetRotationAtTime(NewCurrentTime, ESplineCoordinateSpace::World).Yaw, 0.f);
		FRotator CurrentRotator = FRotator(0.f, InRootSplineComp->GetRotationAtTime(InCurrentTime, ESplineCoordinateSpace::World).Yaw, 0.f);
		FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(NewRotator, CurrentRotator);

		FRotator ResultRotator = InCharacter->GetActorRotation() + DeltaRotator;
		//UE_LOG(CharacterManager, Display, TEXT("AWREnvMovingWalkway::MovingWalkProcess : ResultRotator : %f, %f, %f"), ResultRotator.Pitch, ResultRotator.Yaw, ResultRotator.Roll);
		CurrentOffset = DeltaRotator.RotateVector(CurrentOffset);

		FVector NewLocation = InRootSplineComp->GetLocationAtTime(NewCurrentTime, ESplineCoordinateSpace::World) + CurrentOffset;
		InCharacter->SetLocationAndRotation(NewLocation, ResultRotator);

		if (InCharacter->IsMe() == true)
		{
			this->SpawnSoundEffect(*InCharacter);
		}
	}
}

bool AWREnvMovingWalkway::IsBoardingMovingwalk(AWRCharacter* InCharacter)
{
	for (UWRMovingwalkComponent*It : InMovingwalkLanes)
	{
		if (It != nullptr)
		{
			if (It->IsOverlappingActor(InCharacter) == true)
				return true;
		}
	}
	return false;
}

UWRMovingwalkComponent* AWREnvMovingWalkway::GetOverlappingComp(AWRCharacter* InCharacter)
{
	for (UWRMovingwalkComponent*It : InMovingwalkLanes)
	{
		if (It != nullptr)
		{
			if (It->IsOverlappingActor(InCharacter) == true)
				return It;
		}
	}
	return nullptr;
}

void AWREnvMovingWalkway::AddOverlappedActors(const AID_t& aid, const stSmallDateTime IN InStartTime)
{
	if (OverlappedActors.Contains(aid) == false)
	{
		AWRCharacter* InCharacter = WRCharacterManager::Get()->FindbyAID(aid);
		if (InCharacter == nullptr)
			return;

		OverlappedActors.Add(aid, InStartTime);
	}
}
