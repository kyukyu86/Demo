// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvVehicleBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "../Character/WRCharacterMine.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRConsoleManager.h"
#include "../Character/WRCharacterPlayer.h"
#include "Component/Character/WRActorComponentDynamicMovement.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"


AWREnvVehicleBase::AWREnvVehicleBase() : AWREnvActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

AWREnvVehicleBase::AWREnvVehicleBase(EWREnvType InEnvType) : AWREnvActor(InEnvType)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWREnvVehicleBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> RecognitionComponents = this->GetComponentsByTag(UShapeComponent::StaticClass(), "RecognitionArea");
	if (RecognitionComponents.Num() != 0)
	{
		for (auto& Comp : RecognitionComponents)
		{
			UShapeComponent* CastedComp = Cast<UShapeComponent>(Comp);
			if (CastedComp != nullptr)
			{
				CastedComp->OnComponentBeginOverlap.AddDynamic(this, &AWREnvVehicleBase::OnBeginOverlap_RecognitionArea);
				CastedComp->OnComponentEndOverlap.AddDynamic(this, &AWREnvVehicleBase::OnEndOverlap_RecognitionArea);
				RecognitionList.Add(CastedComp);
			}
		}
	}
	else
	{
		ensureMsgf(false, TEXT("RecognitionArea is not setted"));
	}

	if (IsUseOverlapEventControl())
	{
		WRActorUtility::SetGenerateOverlapEventsbyActor(false, this, true); // [ 2020-4-3 : kyu ] ������ ������ ����
	}
}

void AWREnvVehicleBase::OnBeginOverlap_RecognitionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(Other);
	if (CastedMine == nullptr)
		return;

	CastedMine->OnBeginOverlap_EnvRecognitionArea(this);
}

void AWREnvVehicleBase::OnEndOverlap_RecognitionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(Other);
	if (CastedMine == nullptr)
		return;

	CastedMine->OnEndOverlap_EnvRecognitionArea(this);
}

void AWREnvVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateLocation(DeltaTime);
	UpdatePassengersCulling(DeltaTime);
}

//====================================================================================
//
//====================================================================================

void AWREnvVehicleBase::OnSpawnComplete(const FWREnvVehicleSpawnParams& IN InParams)
{
	SetTID(InParams.TableID);
	SetAID(InParams.AID);

	eState = InParams.State;
	CurrStation = InParams.CurrStation;
	NextStation = InParams.NextStation;
	StartTime = InParams.StartTime;

	std::list<AID_t>::const_iterator iter = InParams.PassengerList.begin();
	std::list<AID_t>::const_iterator iter_end = InParams.PassengerList.end();
	for (; iter != iter_end; ++iter)
	{
		Passengers.Add((AID_t)*iter);
	}

	SetActorLocation(InParams.Location);
	SetActorRotation(InParams.Rotation);
//	SetActorScale3D(InParam.Scale);

	vInitializeLocation = GetActorLocation();
}

void AWREnvVehicleBase::OnUpdatePassengersLocation()
{
	for (auto& _AID : Passengers)
	{
		AWRCharacterPlayer* FoundPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(_AID);
		if(FoundPlayer == nullptr)
			continue;

		FoundPlayer->UpdateEnvVehicleLocationOffset();
	}
}

//====================================================================================

void AWREnvVehicleBase::OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform)
{
	eState = (EWRVehicleState::en)InInform.vehicleState;
	CurrStation = InInform.curStation;
	NextStation = InInform.nextStation;

	Passengers.Empty();
	std::list<AID_t>::const_iterator iter = InInform.lstPassengers.begin();
	std::list<AID_t>::const_iterator iter_end = InInform.lstPassengers.end();
	for (; iter != iter_end; ++iter)
	{
		Passengers.Add((AID_t)*iter);
	}	

	// [ 2020-4-3 : kyu ] ž�¹� �߿����� ����ȭ ��뿩�ΰ� �ٸ���.
	if (IsUseOverlapEventControl())
	{
		// [ 2020-4-2 : kyu ] ����ȭ. ���������� ���� Move�϶��� OverlapEvent�� ���� ����ȭ. Move�� �ƴҶ��� OverlapEvent�� �Ѽ� OutterDoor�� ������Ʈ �� ���� ����ȭ�� �Ѵ�.
		if (eState == EWRVehicleState::Open)
		{
			WRActorUtility::SetGenerateOverlapEventsbyActor(true, this, true);	// [ 2020-4-3 : kyu ] ���� �� ������ Ȱ�� �� ������Ʈ
			UpdateOverlaps();
		}
		else if(eState == EWRVehicleState::Move)
		{
			//WRActorUtility::SetGenerateOverlapEventsbyActor(false, this, true); // [ 2020-4-3 : kyu ] �̵��� ������ ����
		}
	}	
}

void AWREnvVehicleBase::OnMoveToPos(const struct pt_Game_User::Inf_ActorMoveToPos_G2U& IN InInform)
{
	vSrcLocation = FVector(InInform.start_posX, InInform.start_posY, InInform.start_posZ);
	vDstLocation = FVector(InInform.target_posX, InInform.target_posY, InInform.target_posZ);

	bMove = true;
	fElapsedTime = 0.f;
}

void AWREnvVehicleBase::OnMoveStop(const struct pt_Game_User::Inf_ActorMoveStop_G2U& IN InInform)
{
	FVector vStopLocation(InInform.stop_posX, InInform.stop_posY, InInform.stop_posZ);

	SetActorLocation(vStopLocation);

	vSrcLocation = vStopLocation;
	vDstLocation = vStopLocation;

	bMove = false;
}

void AWREnvVehicleBase::AddPassenger(const AID_t IN InAID)
{
	if (IsAlreadyPassenger(InAID) == false)
	{
		Passengers.AddUnique(InAID);
	}

	// *** ���� ó���� �־�� �ұ�?
}

void AWREnvVehicleBase::RemovePassenger(const AID_t IN InAID)
{
	Passengers.Remove(InAID);

	// *** �߹� ó���� �־�� �ұ�?
}

//====================================================================================

void AWREnvVehicleBase::MakeDebugString(TArray<FString>& OUT OutString)
{
	// AID 
	OutString.Add(FString::Printf(TEXT("AID : %d"), GetAID()));

	// TID 
	OutString.Add(FString::Printf(TEXT("TID : %d"), GetTID()));

	// State
	OutString.Add(FString::Printf(TEXT("State : %s"), *EWRVehicleState::GetStr(eState)));

	// Location
	OutString.Add(FString::Printf(TEXT("Location : %.0f, %.0f, %.0f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));

	// Passengers
	OutString.Add(FString::Printf(TEXT("Passengers : %d"), Passengers.Num()));

	// Recognition Players
	int32 nCount = 0;
	for (auto& RecognitionArea : RecognitionList)
	{
		TArray<AActor*> Actors;
		RecognitionArea->GetOverlappingActors(Actors, AWRCharacterPlayer::StaticClass());
		nCount += Actors.Num();
	}
	OutString.Add(FString::Printf(TEXT("Recognition Players : %d"), nCount));

	// Move Speed
	OutString.Add(FString::Printf(TEXT("Move Speed : %.1f"), GetMoveSpeed()));

	// Move Vector
	OutString.Add(FString::Printf(TEXT("Move Vector : (%.1f), (%.1f), (%.1f)"), GetMoveVector().X, GetMoveVector().Y, GetMoveVector().Z));
}

void AWREnvVehicleBase::UpdateLocation(float DeltaTime)
{
	if (bMove == false)
		return;

	fElapsedTime += DeltaTime;

	//���� ���͸� ��ġ�̵� ��Ű�� �� ���� ��ǥ�� �̸� �޾Ƶ�
	FVector CurrentVehicleLocation = GetActorLocation();
	FVector NextVehicleLocation = vDstLocation;

	float fMaxDist = FVector::Dist(vSrcLocation, vDstLocation);
	float fMoveDist = fElapsedTime * fMoveSpeed;
	if (fMoveDist > fMaxDist)
	{
		SetActorLocation(vDstLocation);
		bMove = false;
		fElapsedTime = 0.f;
	}
	else
	{
		FVector vDirection = vDstLocation - vSrcLocation;
		FVector vMoveLocation = vSrcLocation + (vDirection.GetSafeNormal() * fMoveDist);
		SetActorLocation(vMoveLocation);

		NextVehicleLocation = vMoveLocation;
	}

	float DistToNextLocation = FVector::Dist(CurrentVehicleLocation, NextVehicleLocation);
	FVector LocationToModify = (NextVehicleLocation - CurrentVehicleLocation).GetSafeNormal() * DistToNextLocation;
	UpdatePassengerDynamicMovementLocation(DeltaTime, LocationToModify);

}

void AWREnvVehicleBase::UpdatePassengersCulling(float DeltaTime)
{
	if (Passengers.Num() == 0)
	{
		if (CullingRefreshTime != 0.f)
			CullingRefreshTime = 0.f;

		return;
	}

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return;

	// ���� ž���ڸ� �ø�üũ ��������.
	if (IsAlreadyPassenger(pMine->GetActorID()))
		return;


	float RefreshTime = WRConsoleManager::Get()->GetFloat(EWRConsole::CullingRefreshTime);
	if (CullingRefreshTime > RefreshTime)
	{
		CullingRefreshTime = 0.0F;
	}
	else
	{
		CullingRefreshTime += DeltaTime;
		return;
	}
	
#if UE_EDITOR
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return;
	}
#else // UE_EDITOR
#endif // UE_EDITOR

	float CullingDistance = FVector::Dist(this->GetActorLocation(), pMine->GetActorLocation());
	float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CullingDistance);
	if (CullingDistance < Distance)
	{
		for (auto& PassengerAID : Passengers)
		{
			AWRCharacterPlayer* pPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(PassengerAID);
			if (pPlayer == nullptr)
				continue;

#ifdef REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
			if (pPlayer->IsVisibilityBits(EWRVisibility::DistanceCulling) == false)
#else // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
			if (pPlayer->IsCullingState() == false)
#endif // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
				continue;

			// [ 2020-4-28 : kyu ] �ø� ���� ������ ������ �׶����� �� �÷��̾���� �������� �����Ͽ� WRCharacterManager::TickDistanceCulling ���� ������ϵ��� �Ѵ�.

			FVector vPlayerLocation = pPlayer->GetActorLocation();
			FVector vVehicleLocation = this->GetActorLocation();

			pPlayer->UpdateEnvVehicleLocationOffset();

			vPlayerLocation = pPlayer->GetActorLocation();
			vVehicleLocation = this->GetActorLocation();

			int32 a = 0;
		}
	}
}

void AWREnvVehicleBase::UpdatePassengerDynamicMovementLocation(float DeltaTime, FVector InLocationToModify)
{
	for (int32 Idx = 0; Idx < Passengers.Num(); ++Idx)
	{
		AWRCharacter* pPassenger = WRCharacterManager::Get()->FindbyAID(Passengers[Idx]);
		if (nullptr == pPassenger)
			continue;

		//dynamicMove��ġ ������Ʈ
		UWRActorComponentDynamicMovement* DynamicMovementComponent = pPassenger->GetComponent<UWRActorComponentDynamicMovement>();
		if (DynamicMovementComponent)
		{
			DynamicMovementComponent->UpdateLocationOnVehicle(InLocationToModify);
		}

		//�罽�� ������̶�� �ش� �罽 ��ǥ�� ������Ʈ ����� �Ѵ�
		if (pPassenger->IsPlayer())
		{
			UWRActorComponentConnectbyCable* CableComponent = pPassenger->GetComponent<UWRActorComponentConnectbyCable>();
			if (CableComponent)
			{
				CableComponent->UpdateCableLocationOnVehicle(InLocationToModify);
			}
		}
	}
}

FVector AWREnvVehicleBase::GetMoveVector()
{
	FVector vMoveVector = FVector::ZeroVector;
	if (bMove)
	{
		vMoveVector = vDstLocation - vSrcLocation;
	}
	return vMoveVector;
}

FVector AWREnvVehicleBase::GetSrcLocation()
{
	return vSrcLocation;
}

FVector AWREnvVehicleBase::GetDstLocation()
{
	return vDstLocation;
}

bool AWREnvVehicleBase::IsAlreadyPassenger(const AID_t IN InCharAID)
{
	int32 nIndex = 0;
	return Passengers.Find(InCharAID, nIndex);
}

void AWREnvVehicleBase::TestMove(FVector InDstLocation)
{
	vSrcLocation = GetActorLocation();
	vDstLocation = InDstLocation;

	bMove = true;
	fElapsedTime = 0.f;
}