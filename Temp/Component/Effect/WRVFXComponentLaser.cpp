// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVFXComponentLaser.h"

#include "Enum/EWRVFX.h"

#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Utility/WRActorUtility.h"

#include "Actor/Effect/WRVFX.h"

#include "ParticleEmitterInstances.h"

void UWRVFXComponentLaser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->Tick4Transform();
}

void UWRVFXComponentLaser::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	WRActorUtility::SetComponentTickEnable(this, bNewActive);

	if (bNewActive)
	{
	}
	else
	{
	}
}

FTransform UWRVFXComponentLaser::GetActorTransform()
{
	AActor* Spawner = this->GetOwnerVFX()->GetSpawner();
	if (Spawner == nullptr)
	{
		return FTransform::Identity;
	}

#if UE_EDITOR
	if (WRActorUtility::IsEditorPreview(Spawner))
	{
		if (SocketName.IsNone() == false)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Spawner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			if (SkeletalMeshComponent == nullptr)
			{
				return Spawner->GetActorTransform();
			}
			else
			{
				return SkeletalMeshComponent->GetSocketTransform(SocketName);
			}
		}
		if (BoneName.IsNone() == false)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Spawner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
			if (SkeletalMeshComponent == nullptr)
			{
				return Spawner->GetActorTransform();
			}
			else
			{
				int32 Index = SkeletalMeshComponent->GetBoneIndex(BoneName);
				if (Index < 0)
				{
					return Spawner->GetActorTransform();
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
		if (SocketName.IsNone() == false)
		{
			return WRActorUtility::GetSocketTransform(Spawner, SocketName, false);
		}
		if (BoneName.IsNone() == false)
		{
			return WRActorUtility::GetBoneTransform(Spawner, BoneName);
		}
	}
	return Spawner->GetActorTransform();
}

void UWRVFXComponentLaser::Tick4Transform()
{
	UParticleSystemComponent* ParticleSystemComponent = this->GetOwnerVFX()->GetCastedComponent<UParticleSystemComponent>(EWRVFXComponent::FX);
	if (ParticleSystemComponent == nullptr)
	{
		return;
	}
	
	for (int32 i = 0; i < ParticleSystemComponent->EmitterInstances.Num(); ++i)
	{
		if (ParticleSystemComponent->EmitterInstances[i]->bIsBeam == false)
		{
			continue;
		}

		ParticleSystemComponent->SetBeamSourcePoint(i, this->GetActorTransform().GetLocation(), 0);
		ParticleSystemComponent->SetBeamEndPoint(i, this->GetOwnerVFX()->GetActorLocation());
	}
}