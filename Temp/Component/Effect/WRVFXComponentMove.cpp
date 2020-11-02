// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVFXComponentMove.h"

#include "Manager/WRConsoleManager.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Effect/WRVFX.h"

#include "Enum/EWRVFX.h"

#include "Utility/WRActorUtility.h"

#include "Components/SplineComponent.h"

#if UE_EDITOR
#include "DrawDebugHelpers.h"
#else // UE_EDITOR
#endif // UE_EDITOR

void UWRVFXComponentMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// [ 2019-8-16 : animal14 ] 해당 타입에서는 바인딩된 함수가 없으므로 반환
	if (EWRVFXMove::None == Type)
	{
		return;
	}

	this->Tick4Transform(DeltaTime);
}

void UWRVFXComponentMove::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	WRActorUtility::SetComponentTickEnable(this, bNewActive);

	if (bNewActive)
	{
		this->LoadTable();

		Radius = (WRConsoleManager::IsValid() == false) ? 60.0F : WRConsoleManager::Get()->GetFloat(EWRConsole::CCD_Radius);

		Movements.Reset();

		Initializes.SetNum((int32)EWRVFXMove::Max);
		Initializes[(int32)EWRVFXMove::Spawner].BindUObject(this, &UWRVFXComponentMove::Initialize4Spawner);
		Initializes[(int32)EWRVFXMove::SpawnerToTarget].BindUObject(this, &UWRVFXComponentMove::Initialize4SpawnerToTarget);
		Initializes[(int32)EWRVFXMove::TraceTarget].BindUObject(this, &UWRVFXComponentMove::Initialize4TraceTarget);

		// [ 2019-8-16 : animal14 ] 해당 타입에서는 바인딩된 함수가 없으므로 반환
		if (Initializes[(int32)Type].IsBound() == false)
		{
			return;
		}
		Initializes[(int32)Type].ExecuteIfBound();
	}
	else
	{
		for (auto& Initialize : Initializes)
		{
			Initialize.Unbind();
		}
		Initializes.Empty();
	}
}

#if WITH_EDITOR
void UWRVFXComponentMove::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (SocketName.IsNone() == false && BoneName.IsNone() == false)
	{
		if (PropertyChangedEvent.GetPropertyName() == TEXT("SocketName"))
		{
			SocketName = NAME_None;
		}
		if (PropertyChangedEvent.GetPropertyName() == TEXT("BoneName"))
		{
			BoneName = NAME_None;
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#else // WITH_EDITOR
#endif // WITH_EDITOR

void UWRVFXComponentMove::LoadTable()
{
	if (bOverrideMovements == false)
	{
		return;
	}

	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	AWRCharacter* SpawnerCharacter = Cast<AWRCharacter>(VFX->GetSpawner());
	if (SpawnerCharacter == nullptr)
	{
		return;
	}

	//UWRActorComponentFSM* ActorComponentFSM = SpawnerCharacter->GetComponent<UWRActorComponentFSM>();
	//if (ActorComponentFSM == nullptr)
	//{
	//	return;
	//}

	//FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(ActorComponentFSM->GetCurrentFSMInfo()->SkillTableID, SpawnerCharacter->GetSkillTableFileName());
	//if (SkillTable == nullptr)
	//{
	//	return;
	//}

	//Movements.StartVelocity = 0.0F;
	//Movements.FinishVelocity = 0.0F;
}

void UWRVFXComponentMove::Initialize4Spawner()
{
	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	AActor* Spawner = VFX->GetSpawner();
	if (Spawner == nullptr)
	{
		return;
	}

	//Direction = Spawner->GetActorForwardVector();
	Direction = AdditionalAngle.RotateVector(Spawner->GetActorForwardVector());

	this->Initialize4Parabola(VFX->GetActorLocation(), VFX->GetActorLocation() + (Direction * Movements.FinishVelocity));
}

//void UWRVFXComponentMove::Initialize4Target()
//{
//}

void UWRVFXComponentMove::Initialize4SpawnerToTarget()
{
	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	AActor* Target = VFX->GetTarget();
	if (Target == nullptr)
	{
		AActor* Spawner = VFX->GetSpawner();
		if (Spawner == nullptr)
		{
			return;
		}

		//Direction = Spawner->GetActorForwardVector();
		Direction = AdditionalAngle.RotateVector(Spawner->GetActorForwardVector());

		this->Initialize4Parabola(VFX->GetActorLocation(), VFX->GetActorLocation() + (Direction * Movements.FinishVelocity));
	}
	else
	{
		FVector Location = Target->GetActorLocation();

#if UE_EDITOR
		if (WRActorUtility::IsEditorPreview(Target))
		{
			if (SocketName.IsNone() == false)
			{
				Location = FVector::ZeroVector;
			}

			if (BoneName.IsNone() == false)
			{
				Location = FVector::ZeroVector;
			}
		}
		else
#else // UE_EDITOR
#endif // UE_EDITOR
		{
			if (SocketName.IsNone() == false)
			{
				Location = WRActorUtility::GetSocketTransform(Target, SocketName).GetLocation();
			}

			if (BoneName.IsNone() == false)
			{
				Location = WRActorUtility::GetBoneTransform(Target, BoneName).GetLocation();
			}
		}

		//Direction = WRActorUtility::GetDirection(VFX->GetActorLocation(), Location);
		Direction = AdditionalAngle.RotateVector(WRActorUtility::GetDirection(VFX->GetActorLocation(), Location));

		this->Initialize4Parabola(VFX->GetActorLocation(), Location);
	}
}

void UWRVFXComponentMove::Initialize4TraceTarget()
{
	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	AActor* Spawner = VFX->GetSpawner();
	if (Spawner == nullptr)
	{
		return;
	}

	AActor* Target = VFX->GetTarget();
	if (Target == nullptr)
	{
		//Direction = Spawner->GetActorForwardVector();
		Direction = AdditionalAngle.RotateVector(Spawner->GetActorForwardVector());

		this->Initialize4Parabola(VFX->GetActorLocation(), VFX->GetActorLocation() + (Direction * Movements.FinishVelocity));
	}
	else
	{
		FVector Location = Target->GetActorLocation();

#if UE_EDITOR
		if (WRActorUtility::IsEditorPreview(Target))
		{
			if (SocketName.IsNone() == false)
			{
				Location = FVector::ZeroVector;
			}

			if (BoneName.IsNone() == false)
			{
				Location = FVector::ZeroVector;
			}
		}
		else
#else // UE_EDITOR
#endif // UE_EDITOR
		{
			if (SocketName.IsNone() == false)
			{
				Location = WRActorUtility::GetSocketTransform(Target, SocketName).GetLocation();
			}

			if (BoneName.IsNone() == false)
			{
				Location = WRActorUtility::GetBoneTransform(Target, BoneName).GetLocation();
			}
		}

		//Direction = WRActorUtility::GetDirection(VFX->GetActorLocation(), Location);
		Direction = AdditionalAngle.RotateVector(WRActorUtility::GetDirection(VFX->GetActorLocation(), Location));
	}
}

void UWRVFXComponentMove::Tick4Transform(float InDeltaTime)
{
	if (IsMoving == false)
	{
		return;
	}

	AActor* MyOwner = this->GetOwner();
	if (MyOwner == nullptr)
	{
		return;
	}

	FVector CurrentDirection = this->GetDirection(InDeltaTime);
	float CurrentSpeed = this->GetSpeed();

	this->Tick4CCD(CurrentDirection, CurrentSpeed, InDeltaTime);

	FVector Location = MyOwner->GetActorLocation() + CurrentDirection * CurrentSpeed;
	FRotator Rotation = CurrentDirection.Rotation();

	MyOwner->SetActorLocationAndRotation(Location, Rotation);

#if UE_EDITOR
	this->Debug(CurrentDirection, CurrentSpeed, InDeltaTime);
#else // UE_EDITOR
#endif // UE_EDITOR
}

void UWRVFXComponentMove::Tick4CCD(const FVector& InCurrentDirection, const float InCurrentSpeed, const float InDeltaTime)
{
	if (IsCCD == false)
	{
		return;
	}

	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	AActor* Target = VFX->GetTarget();
	if (Target == nullptr)
	{
		return;
	}

	FVector CurrentLocation = VFX->GetActorLocation();
	FVector OriginLocation = Target->GetActorLocation();

	if (FMath::LineSphereIntersection(CurrentLocation, InCurrentDirection, InCurrentSpeed * InDeltaTime, OriginLocation, Radius) == false)
	{
		return;
	}

#if UE_EDITOR
	if (WRActorUtility::IsEditorPreview(Target))
	{
	}
	else
#else // UE_EDITOR
#endif // UE_EDITOR
	{
		VFX->NotifyActorBeginOverlap(Target);
	}
}

#define WR_MIN_VALUE 0.0F
#define WR_MAX_VALUE 1.0F

FVector UWRVFXComponentMove::GetDirection(float InDeltaTime)
{
	float PreviousTime = FMath::Clamp(Movements.DeltaTime * DistanceRatio, WR_MIN_VALUE, WR_MAX_VALUE);
	Movements.Tick(InDeltaTime);
	float CurrentTime = FMath::Clamp(Movements.DeltaTime * DistanceRatio, WR_MIN_VALUE, WR_MAX_VALUE);

	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return FVector::ZeroVector;
	}

	bool IsTracing = (Type == EWRVFXMove::TraceTarget) ? true : false;
	if (IsAttaching || IsTracing)
	{
		AActor* Target = VFX->GetTarget();
		if (Target == nullptr)
		{
			AActor* Spawner = VFX->GetSpawner();
			if (Spawner == nullptr)
			{
				return FVector::ZeroVector;
			}

			return Direction * InDeltaTime;
		}
		else
		{
			FVector Location = Target->GetActorLocation();

#if UE_EDITOR
			if (WRActorUtility::IsEditorPreview(Target))
			{
				if (SocketName.IsNone() == false)
				{
					Location = FVector::ZeroVector;
				}

				if (BoneName.IsNone() == false)
				{
					Location = FVector::ZeroVector;
				}
			}
			else
#else // UE_EDITOR
#endif // UE_EDITOR
			{
				if (SocketName.IsNone() == false)
				{
					Location = WRActorUtility::GetSocketTransform(Target, SocketName).GetLocation();
				}

				if (BoneName.IsNone() == false)
				{
					Location = WRActorUtility::GetBoneTransform(Target, BoneName).GetLocation();
				}
			}

			return Direction = WRActorUtility::GetDirection(VFX->GetActorLocation(), Location) * InDeltaTime;
		}
	}

	// [ 2019-8-16 : animal14 ] 포물선이 끝난 후 움직임
	if (CurrentTime >= WR_MAX_VALUE)
	{
		USplineComponent* SplineComponent = VFX->GetCastedComponent<USplineComponent>(EWRVFXComponent::Spline);
		if (SplineComponent == nullptr)
		{
			return Direction * InDeltaTime;
		}
		else
		{
			return CalculatedParabolaDirection;
		}
	}

	return this->GetParabolaDirection(CurrentTime, PreviousTime);
}

float UWRVFXComponentMove::GetSpeed()
{
	float Ratio = FMath::Clamp(Movements.DeltaTime / Movements.Acceleration, WR_MIN_VALUE, WR_MAX_VALUE);

	return FMath::Lerp(Movements.StartVelocity, Movements.FinishVelocity, Ratio) * SplineRatio;
}

void UWRVFXComponentMove::Initialize4Parabola(const FVector& InFrom, const FVector& InTo)
{
	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	USplineComponent* SplineComponent = VFX->GetCastedComponent<USplineComponent>(EWRVFXComponent::Spline);
	if (SplineComponent == nullptr)
	{
		return;
	}

	float SplineDistance = FVector::Dist(SplineComponent->GetLocationAtTime(WR_MAX_VALUE, ESplineCoordinateSpace::Local), SplineComponent->GetLocationAtTime(WR_MIN_VALUE, ESplineCoordinateSpace::Local));
	SplineRatio = 1.0F / SplineDistance;

	//float ActorDistance = FVector::Dist2D(InFrom, InTo);
	float ActorDistance = FVector::Dist(InFrom, InTo);

	bool IsIncreasing = (Movements.FinishVelocity > Movements.StartVelocity) ? true : false;
	if (IsIncreasing)
	{
		DistanceRatio = WR_MAX_VALUE / (ActorDistance / Movements.FinishVelocity);
	}
	else
	{
		DistanceRatio = WR_MAX_VALUE / (ActorDistance / Movements.StartVelocity);
	}
}

FVector UWRVFXComponentMove::GetParabolaDirection(const float InCurrentTime, const float InPreviousTime)
{
	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return Direction * (InCurrentTime - InPreviousTime);
	}

	USplineComponent* SplineComponent = VFX->GetCastedComponent<USplineComponent>(EWRVFXComponent::Spline);
	if (SplineComponent == nullptr)
	{
		return Direction * (InCurrentTime - InPreviousTime);
	}

	//FRotator CalculatedDirection = Direction.GetSafeNormal2D().Rotation();
	FRotator CalculatedDirection = Direction.Rotation();

	FVector ParabolaDirection = SplineComponent->GetLocationAtTime(InCurrentTime, ESplineCoordinateSpace::Local) - SplineComponent->GetLocationAtTime(InPreviousTime, ESplineCoordinateSpace::Local);
	return CalculatedParabolaDirection = CalculatedDirection.RotateVector(ParabolaDirection) / DistanceRatio;
}

#if UE_EDITOR
void UWRVFXComponentMove::Debug(const FVector& InCurrentDirection, const float InCurrentSpeed, const float InDeltaTime)
{
	if (IsDebug == false)
	{
		return;
	}

	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	FColor Color = FColor::Green;
	if (Movements.DeltaTime >= WR_MAX_VALUE)
	{
		Color = FColor::Red;
	}

	DrawDebugLine(this->GetWorld(), VFX->GetActorLocation(), VFX->GetActorLocation() + InCurrentDirection * InCurrentSpeed, Color, false, 1.0F);

	AActor* Target = VFX->GetTarget();
	if (Target == nullptr)
	{
		return;
	}	
}
#else // UE_EDITOR
#endif // UE_EDITOR

float FWRMoves::Tick(float InDeltaTime)
{
	DeltaTime += InDeltaTime;

	return DeltaTime;
}

void FWRMoves::Reset()
{
	DeltaTime = 0.0F;
}