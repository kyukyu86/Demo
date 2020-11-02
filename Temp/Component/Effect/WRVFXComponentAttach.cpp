// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVFXComponentAttach.h"

#include "Actor/Effect/WRVFX.h"

#include "Camera/CameraComponent.h"

#include "Components/SkinnedMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/Character.h"

#include "Utility/WRActorUtility.h"


void UWRVFXComponentAttach::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Ticks.Num() == 0)
	{
		return;
	}

	// [ 2019-8-16 : animal14 ] 해당 타입에서는 바인딩된 함수가 없으므로 반환
	if (EWRVFXAttach::None == Attach)
	{
		return;
	}

	// [ 2019-8-14 : animal14 ] 한번만 호출되는 경우
	if (IsLooping == false)
	{
		return;
	}

	Ticks[(int32)Attach].ExecuteIfBound();
}

void UWRVFXComponentAttach::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	WRActorUtility::SetComponentTickEnable(this, bNewActive);

	if (bNewActive)
	{
		Ticks.SetNum((int32)EWRVFXAttach::Max);
		Ticks[(int32)EWRVFXAttach::Spawner].BindUObject(this, &UWRVFXComponentAttach::Transform4Spawner);
		Ticks[(int32)EWRVFXAttach::Target].BindUObject(this, &UWRVFXComponentAttach::Transform4Target);
		Ticks[(int32)EWRVFXAttach::Parent].BindUObject(this, &UWRVFXComponentAttach::Transform4Parent);

		// [ 2019-8-16 : animal14 ] 해당 타입에서는 바인딩된 함수가 없으므로 반환
		if (Ticks[(int32)Attach].IsBound() == false)
		{
			return;
		}
		Ticks[(int32)Attach].ExecuteIfBound();
	}
	else
	{
		for (auto& Tick : Ticks)
		{
			Tick.Unbind();
		}
		Ticks.Empty();
	}
}

#if WITH_EDITOR
void UWRVFXComponentAttach::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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

FTransform UWRVFXComponentAttach::GetActorTransform(class AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FTransform::Identity;
	}

#if UE_EDITOR
	if (WRActorUtility::IsEditorPreview(InActor))
	{
		if (SocketName.IsNone() == false)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			if (SkeletalMeshComponent == nullptr)
			{
				return InActor->GetActorTransform();
			}

			if (IsComponentSpace == false)
			{
				return SkeletalMeshComponent->GetSocketTransform(SocketName);
			}
			else
			{
				FTransform Transform = SkeletalMeshComponent->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_Component);
				Transform.AddToTranslation(InActor->GetActorLocation());

				return Transform;
			}
		}
		if (BoneName.IsNone() == false)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			if (SkeletalMeshComponent == nullptr)
			{
				return InActor->GetActorTransform();
			}
			else
			{
				int32 Index = SkeletalMeshComponent->GetBoneIndex(BoneName);
				if (Index < 0)
				{
					return InActor->GetActorTransform();
				}
				else
				{
					return SkeletalMeshComponent->GetBoneTransform(Index);
				}
			}
		}
	}
	else
#else // UE_EDITOR
#endif // UE_EDITOR
	{
		if (ComponentToAttach != EWRVFXComponentToAttach::None && ComponentToAttach != EWRVFXComponentToAttach::Max)
		{
			return GetComponentTransform(InActor);
		}
		if (SocketName.IsNone() == false)
		{
			return WRActorUtility::GetSocketTransform(InActor, SocketName, IsComponentSpace);
		}
		if (BoneName.IsNone() == false)
		{
			return WRActorUtility::GetBoneTransform(InActor, BoneName);
		}
	}

	return InActor->GetActorTransform();
}

FTransform UWRVFXComponentAttach::GetComponentTransform(class AActor* InActor)
{
	if (InActor == nullptr)
	{
		return FTransform::Identity;
	}

	USceneComponent* SceneComponent = nullptr;
	switch (ComponentToAttach)
	{
	case EWRVFXComponentToAttach::Camera:
		SceneComponent = InActor->FindComponentByClass<UCameraComponent>();
		break;
	}

	if (SceneComponent == nullptr)
	{
		return FTransform::Identity;
	}

	return SceneComponent->GetComponentToWorld();
}

void UWRVFXComponentAttach::Transform(const FTransform& InTransform, class AActor* InActor)
{
	AActor* VFX = this->GetOwner();
	if (VFX == nullptr)
	{
		return;
	}

	if (IsAttachScale)
	{
		VFX->SetActorScale3D(InTransform.GetScale3D() * Scale);
	}

	if (IsAttachRotation)
	{
		VFX->SetActorRotation(InTransform.Rotator() + Rotation);
	}

	if (IsAttachLocation)
	{
		bool IsWorldSpace = SocketName.IsNone() && BoneName.IsNone() && ComponentToAttach == EWRVFXComponentToAttach::None;

		FVector RotatedLocation = (IsWorldSpace) ? InActor->GetActorRotation().RotateVector(Location) : InTransform.Rotator().RotateVector(Location);

		VFX->SetActorLocation(InTransform.GetLocation() + RotatedLocation);
	}

	if (IsSnapFloor)
	{
		FVector AdjustedLocation;
		AdjustedLocation.X = InTransform.GetLocation().X;
		AdjustedLocation.Y = InTransform.GetLocation().Y;
		AdjustedLocation.Z = WRActorUtility::ProjectPointToNavigation(InActor).Z;

		VFX->SetActorLocation(AdjustedLocation + FloorLocation);
	}
}

void UWRVFXComponentAttach::Transform4Spawner()
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
	FTransform Transform = this->GetActorTransform(Spawner);

	this->Transform(Transform, Spawner);
}

void UWRVFXComponentAttach::Transform4Target()
{
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
	FTransform Transform = this->GetActorTransform(Target);

	this->Transform(Transform, Target);
}

void UWRVFXComponentAttach::Transform4Parent()
{
	AWRVFX* VFX = this->GetOwnerVFX();
	if (VFX == nullptr)
	{
		return;
	}

	AWRVFX* Parent = VFX->GetParent();
	if (Parent == nullptr)
	{
		return;
	}
	FTransform Transform = Parent->GetActorTransform();

	this->Transform(Transform, Parent);
}