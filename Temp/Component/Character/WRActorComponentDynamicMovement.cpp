// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentDynamicMovement.h"
#include <Components/SplineComponent.h>
#include <GameFramework/Actor.h>
#include "Actor/Character/WRCharacter.h"
#include <Components/CapsuleComponent.h>
#include "Table/Base/WRTableManager.h"
#include "Table/WRDynamicMovementTable.h"
#include "Utility/WRActorUtility.h"
#include <NavigationSystem.h>
#include <DrawDebugHelpers.h>
#include "Network/IMMessageSender.h"
#include "Manager/WRCharacterManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WREnvInteractionManager.h"
#include "Actor/EnvActor/WREnvVehicleBase.h"


void UWRActorComponentDynamicMovement::OnCreateComponent()
{
	Super::OnCreateComponent();

	CreateSplineComponent();
}

void UWRActorComponentDynamicMovement::OnDestroyComponent()
{
	Super::OnDestroyComponent();

	DestroySplineComponent();
}

void UWRActorComponentDynamicMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickCalculateMovement(DeltaTime);
}

void UWRActorComponentDynamicMovement::CreateSplineComponent()
{
	SplineComponent = NewObject<USplineComponent>(GetOwner());
	if (nullptr == SplineComponent)
		return;
	SplineComponent->AddToRoot();
	SplineComponent->RegisterComponent();

	SplineComponent->bStationaryEndpoints = true;

	SplineComponent->bDrawDebug = true;
}

void UWRActorComponentDynamicMovement::DestroySplineComponent()
{
	if (SplineComponent == nullptr)
		return;

	if (SplineComponent->IsRegistered() == true)
		SplineComponent->UnregisterComponent();

	SplineComponent->RemoveFromRoot();

	if (GetOwnerCharacter())
	{
		GetOwnerCharacter()->RemoveOwnedComponent(SplineComponent);
	}
	SplineComponent->DestroyComponent();
}

void UWRActorComponentDynamicMovement::TickCalculateMovement(float DeltaTime)
{
	if (nullptr == SplineComponent)
		return;

	if (DynamicMovementInfo.IsValid())
	{
		SplineComponent->UpdateSpline();
		FVector CurrLocation = SplineComponent->GetWorldLocationAtTime(DynamicMovementInfo.UpdateElapsedTime(DeltaTime));

		//bSweep�� true�� �浹�� �Ͼ�� �ش� ��ġ���� ���߰� �ȴ�
		bool bResult = GetOwnerCharacter()->SetActorLocation(CurrLocation, DynamicMovementInfo.bSweep);
	}
}

bool UWRActorComponentDynamicMovement::DetermineDynamicMovement(WRTableID InTableID, AWRCharacter* InChar, bool bSweep)
{
	FWRDynamicMovementTable* DynamicMovementTable = WRTableManager::Get()->FindRow<FWRDynamicMovementTable>(InTableID);
	if (nullptr == DynamicMovementTable)
		return false;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return false;
	
	FVector DestLocation = FVector::ZeroVector;
	switch (DynamicMovementTable->DynamicMovementType)
	{
	case EWRDynamicMovementType::FrontMoveToPosition:
	{
		FVector MoveDirection = OwnerCharacter->GetActorForwardVector().GetSafeNormal();

		DestLocation = (MoveDirection * DynamicMovementTable->Distance) + OwnerCharacter->GetActorLocation();
	}
	break;
	case EWRDynamicMovementType::FrontMoveToActor:
	case EWRDynamicMovementType::PulledToActor:
	{
		if (nullptr == InChar)
			return false;

		FVector DirToTarget = InChar->GetActorLocation() - OwnerCharacter->GetActorLocation();
		DirToTarget = DirToTarget.GetSafeNormal();

		float DistanceToTarget = WRActorUtility::GetLengthBetweenActor(InChar, OwnerCharacter, true);
		DistanceToTarget -= DynamicMovementTable->Distance;

		DestLocation = (DirToTarget * DistanceToTarget) + OwnerCharacter->GetActorLocation();
	}
	break;
	case EWRDynamicMovementType::BackMoveToPosition:
	{
		FVector MoveDirection = OwnerCharacter->GetActorForwardVector() * -1;
		MoveDirection = MoveDirection.GetSafeNormal();

		DestLocation = (MoveDirection * DynamicMovementTable->Distance) + OwnerCharacter->GetActorLocation();
	}
	break;
	case EWRDynamicMovementType::BackMoveToActor:
	{
		if (nullptr == InChar)
			return false;

		//1
		FVector BackMoveDir = OwnerCharacter->GetActorLocation() - InChar->GetActorLocation();
		BackMoveDir = BackMoveDir.GetSafeNormal();

		float DistanceToTarget = WRActorUtility::GetLengthBetweenActor(InChar, OwnerCharacter, true);
		float DistanceToMove = DynamicMovementTable->Distance - DistanceToTarget;
		if (DistanceToMove <= 0.0f)	//������ �Ÿ��� �ڷ� �̵��ؾ� �ϴ� �Ÿ����� �̹� �ָ� ������
		{
			return false;
		}

		DestLocation = (BackMoveDir * DistanceToMove) + OwnerCharacter->GetActorLocation();
	}
	break;
	case EWRDynamicMovementType::PushedToPosition:
	{
		if (nullptr == InChar)
			return false;

		//�ڷ� �з����� �ϹǷ� �� ��ġ���� ����� ��ġ�� ����
		FVector MoveDirection = OwnerCharacter->GetActorLocation() - InChar->GetActorLocation();
		MoveDirection = MoveDirection.GetSafeNormal();

		DestLocation = (MoveDirection * DynamicMovementTable->Distance) + OwnerCharacter->GetActorLocation();
	}
	break;
	case EWRDynamicMovementType::MoveToInputDirection:
	{
		if (OwnerCharacter->IsMe() == false)
			return false;

		FVector ForwardAcc = OwnerCharacter->GetActorForwardVector() * OwnerCharacter->MoveForwardValue;
		FVector RightAcc = OwnerCharacter->GetActorRightVector() * OwnerCharacter->MoveRightValue;
		FVector MoveDirection = ForwardAcc + RightAcc;
		if (FVector::ZeroVector == MoveDirection)
		{
			MoveDirection = OwnerCharacter->GetActorForwardVector();
		}

		DestLocation = (MoveDirection.GetSafeNormal() * DynamicMovementTable->Distance) + OwnerCharacter->GetActorLocation();
	}
	break;
	default:
	{
		return false;
	}
	break;
	}

	return InvokeDynamicMovement(InTableID, DestLocation, bSweep);
}

bool UWRActorComponentDynamicMovement::InvokeDynamicMovement(WRTableID InTableID, FVector InDestLocation, bool bSweep)
{
	FWRDynamicMovementTable* DynamicMovementTable = WRTableManager::Get()->FindRow<FWRDynamicMovementTable>(InTableID);
	if (nullptr == DynamicMovementTable)
		return false;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return false;

	FNavLocation OutNavLocation;
	bool bResult = WRActorUtility::ProjectPointToNavigation(OwnerCharacter, InDestLocation, OutNavLocation);
	if (bResult)
	{
		float OutHeight = 0.0f;
		if (WRActorUtility::ModifyZHeightForClient(OwnerCharacter, OutNavLocation.Location, OutHeight))
		{
			OutNavLocation.Location.Z = OutHeight;
		}
		InDestLocation = OutNavLocation.Location;

		//DrawDebugSphere(GetWorld(), OutNavLocation.Location, 10.0f, 16, FColor::Green, false, 2.0f);
	}
	else
	{
		//���� ������ �ʿ� �ҵ��Ͽ� ������� ����
		//���� ��ġ�� �׺� �������� ������� 1���;� ���鼭 ��ġ üũ�� �ؼ� ���� ��ġ�� ����
// 		bool bCheckNavi = true;
// 		FVector Dir = OwnerCharacter->GetActorLocation() - InDestLocation;
// 		Dir = Dir.GetSafeNormal();
// 		while (bCheckNavi)
// 		{
// 			InDestLocation += Dir * 100.0f;
// 			FNavLocation OutNav;
// 			bool bResult = WRActorUtility::ProjectPointToNavigation(OwnerCharacter, InDestLocation, OutNav);
// 			if (bResult)
// 			{
// 				InDestLocation = OutNav.Location;
// 				break;
// 			}
// 		}
	}


	//1	//������ġ�� �׺� ���� ����
	TArray<FVector> SplineMoveLocation;
	SplineMoveLocation.Add(InDestLocation);


	//2	//��̵��� ��� �ش� ��ǥ ���ؼ� ����
	if (DynamicMovementTable->Height != 0.0f)
	{
		FVector DirToDest = InDestLocation - OwnerCharacter->GetActorLocation();
		float HalfDistanceToDest = DirToDest.Size() / 2.0f;
		DirToDest = DirToDest.GetSafeNormal();
		FVector MiddleLocation = DirToDest * HalfDistanceToDest + OwnerCharacter->GetActorLocation();
		MiddleLocation.Z += DynamicMovementTable->Height;

		SplineMoveLocation.Add(MiddleLocation);
	}
	else //�����̵��� ��� �߰���ǥ�� 2���͸��� ���ؼ� ������ ������ ����
	{
		FVector Dir = OwnerCharacter->GetActorLocation() - InDestLocation;
		float Distance = Dir.Size();
		Dir = Dir.GetSafeNormal();

		FVector CheckLocation = InDestLocation;
		int32 CheckPointNum = Distance / 200.0f;	//2���͸��� üũ�ϰ�
		for (int32 Idx = 0; Idx < CheckPointNum - 1; ++Idx)
		{
			CheckLocation += Dir * 200.0f;

			FNavLocation OutNav;
			if (false == WRActorUtility::ProjectPointToNavigation(OwnerCharacter, CheckLocation, OutNav))
				continue;

			float OutHeight = 0.0f;
			if (WRActorUtility::ModifyZHeightForClient(OwnerCharacter, OutNav.Location, OutHeight))
			{
				OutNav.Location.Z = OutHeight;
			}

			SplineMoveLocation.Add(OutNav.Location);

			//DrawDebugSphere(GetWorld(), OutNav.Location, 10.0f, 16, FColor::Red, false, 2.0f);
		}
	}

	//3 //������ġ
	SplineMoveLocation.Add(OwnerCharacter->GetActorLocation());

	DynamicMovementInfo.Reset();
	if (SplineMoveLocation.Num() != 0)
	{
		float Distance = (OwnerCharacter->GetActorLocation() - InDestLocation).Size() / 100.0f;	//���ͷ� ����
		SplineComponent->Duration = Distance / DynamicMovementTable->Velocity;
		SplineComponent->SetSplinePoints(SplineMoveLocation, ESplineCoordinateSpace::World);

		DynamicMovementInfo.DynamicMovementTID = InTableID;
		DynamicMovementInfo.StartingLocation = OwnerCharacter->GetActorLocation();
		DynamicMovementInfo.DestLocation = InDestLocation;
		DynamicMovementInfo.bSweep = bSweep;
		DynamicMovementInfo.ElapsedTime = SplineComponent->Duration;

		FTimerDelegate DelTimer = FTimerDelegate::CreateUObject(this, &UWRActorComponentDynamicMovement::OnDynamicMoveFinished);
		WRActorUtility::SetTimer(DynamicMovementTimer, DelTimer, SplineComponent->Duration, false);

		WRActorUtility::SetComponentTickEnable(this, true);
	}

	//�Է¹��⿡ ���� �̵��� ���, �� �ڽ��϶� ������ ��Ŷ ����
	if (OwnerCharacter->IsMe())
	{
		if (EWRDynamicMovementType::MoveToInputDirection == DynamicMovementTable->DynamicMovementType)
		{
			IMMessageSender::RequestDynamicMove(OwnerCharacter, InDestLocation, InTableID);
		}
	}

	return true;
}

void UWRActorComponentDynamicMovement::OnDynamicMoveFinished()
{
	DynamicMovementInfo.Reset();
	SplineComponent->ClearSplinePoints();

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	if (OwnerCharacter->GetCurrentFSMType() == EWRFSM::MaintainingSkill)
	{
		OwnerCharacter->DetermineFSMSignal(EWRFSM::Idle, true);
	}

	SyncDynamicMoveLocation();

	WRActorUtility::SetComponentTickEnable(this, false);

	if (OwnerCharacter->IsMe() && 
		OwnerCharacter->GetCharacterMovement() && 
		OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		AWRCharacterMine* pMe = Cast<AWRCharacterMine>(OwnerCharacter);
		if (pMe && pMe->IsReqBeginFallPacket() == false)
		{
			pMe->DetermineFallPacket();
		}
	}
}

FVector UWRActorComponentDynamicMovement::GetSplineDestLocation()
{
	return DynamicMovementInfo.DestLocation;
}

void UWRActorComponentDynamicMovement::SyncDynamicMoveLocation()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	bool bSend = false;
	AWRCharacter* pChild = OwnerCharacter->GetChild();
	if (pChild->IsPlayer())
	{
		//PC�϶� ���϶��� ������ ����
		if (pChild->IsMe())
		{
			bSend = true;
		}
	}
	else //NPC�϶�
	{
		AWRCharacter* OwnerTarget = WRCharacterManager::Get()->FindbyAID(OwnerCharacter->GetTargetAID());
		if (OwnerTarget)
		{
			if (OwnerTarget->IsMe()) //Ÿ���� ���϶�
			{
				bSend = true;
			}
			else if (OwnerTarget->GetChild()->IsPlayer())
			{
				//����� Ÿ���������� ��������
			}
			else //Ÿ���� ���� ����
			{
				bSend = false;
			}
		}
		else
		{
			bSend = false;
		}
	}

	if (bSend)
	{
		IMMessageSender::RequestSyncDynamicMoveLocation(GetOwnerCharacter());
	}
}

bool UWRActorComponentDynamicMovement::IsValidDynamicMovement()
{
	return DynamicMovementInfo.IsValid();
}

void UWRActorComponentDynamicMovement::UpdateLocationOnVehicle(FVector InLocationToModify)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	//DynamicMove ���°� �ƴϸ� ����
	if (IsValidDynamicMovement() == false)
		return;

	//�÷��̾ üũ
	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (nullptr == Player)
		return;

	//ž�¹� ���� �ƴϸ� ����
	AWREnvVehicleBase* Vehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(Player->GetEnvVehicleAID());
	if (nullptr == Vehicle)
		return;

	//OwnerCharacter->
	FVector test = OwnerCharacter->GetActorLocation();
	FVector ModifiedLocation = OwnerCharacter->GetActorLocation() + InLocationToModify;

	//ž�¹��� �̵��� ��ŭ �� �ɸ��� ��ǥ ����
//	GetOwnerCharacter()->SetActorLocation(ModifiedLocation, DynamicMovementInfo.bSweep);

	//ž�¹��� �̵��� ��ŭ �� dynamicMove dstlocation�� ������ ����� ��
	//������Ʈ ���� ����
	DynamicMovementInfo.DestLocation += InLocationToModify;

	//���ö��� ������Ʈ�� ������ǥ�� ����
	//SplineComponent->SetSplinePoints(SplineMoveLocation, ESplineCoordinateSpace::World);
	FInterpCurveVector InterpVector = SplineComponent->GetSplinePointsPosition();
	for (int32 Idx = 0; Idx < InterpVector.Points.Num(); ++Idx)
	{
		FVector SplinePoint = InterpVector.Points[Idx].OutVal;
		SplinePoint += InLocationToModify;

		SplineComponent->SetWorldLocationAtSplinePoint(Idx, SplinePoint);
	}
}

void UWRActorComponentDynamicMovement::OnHit(UPrimitiveComponent* HitComponent, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{
	int32 aa = 0;
}
