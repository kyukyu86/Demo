// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVFXComponentSpawn.h"

#include "Manager/WRVFXManager.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WREnumUtility.h"

#include "Engine/World.h"

void UWRVFXComponentSpawn::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& VFXSpawn : VFXSpawns)
	{
		if (VFXSpawn.IsSpawnWhenCreate)
		{
			continue;
		}
		if (VFXSpawn.IsSpawnWhenCollide)
		{
			continue;
		}

#if UE_EDITOR
		if (VFXSpawn.Delay < 0.0F)
		{
		}
		else
		{
			if (this->GetOwnerVFX()->GetVFX().Lifespan < VFXSpawn.Delay)
			{
				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Delay of Spawn Component is larger than Lifespan of BP"), TEXT("UWRVFXComponentSpawn::SetActive()"));
			}
		}
#else // UE_EDITOR
#endif // UE_EDITOR

		bool IsSpawned = VFXSpawn.Tick(DeltaTime);
		if (IsSpawned != VFXSpawn.IsSpawned)
		{
			VFXSpawn.IsSpawned = IsSpawned;

#if UE_EDITOR
			if (WRActorUtility::IsEditorPreview(this->GetOwnerVFX()->GetSpawner()))
			{
				FActorSpawnParameters Parameters;
				Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AWRVFX* VFX = GetWorld()->SpawnActor<AWRVFX>(VFXSpawn.GeneratedClass, FVector::ZeroVector, FRotator::ZeroRotator, Parameters);
				if (VFX == nullptr)
				{
					return;
				}

				this->OnComplete(VFX, VFXSpawn.AdditionalAngle);
			}
			else
#else // UE_EDITOR
#endif // UE_EDITOR
			{
				if (this->GetOwner()->PrimaryActorTick.bCanEverTick)
				{
				}
				else
				{
					return;
				}
				
				AWRVFX* Parent = this->GetOwnerVFX();
				if (Parent == nullptr)
				{
					return;
				}

				bool IsAsync = WREnumUtility::IsBit((uint64)Parent->Bits, (uint64)EWRVFX::Async);
				if (IsAsync)
				{
					FSoftObjectPath Temp = FSoftObjectPath(VFXSpawn.GeneratedClass);

					FString Key = WRVFXManager::Get()->AsyncSpawn(Temp.GetAssetPathString(), Parent->GetSpawnerAID(), Parent->GetTargetAID(), Parent->GetUniqueID(), EWRVFX::Async, VFXSpawn.AdditionalAngle);
				}
				else
				{
					// [ 2019-8-27 : animal14 ] Parent 파라미터는 WRVFXComponentSpawn에서만 사용
					AWRVFX* VFX = WRVFXManager::Get()->Spawn(VFXSpawn.GeneratedClass, this->GetOwnerVFX()->GetSpawner(), this->GetOwnerVFX()->GetTarget(), this->GetOwnerVFX(), EWRVFX::None, VFXSpawn.AdditionalAngle);
					if (VFX == nullptr)
					{
						return;
					}
				}
			}
		}
		else
		{

		}
	}
}

void UWRVFXComponentSpawn::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	WRActorUtility::SetComponentTickEnable(this, bNewActive);

	if (bNewActive)
	{
		this->LoadTable();

		for (auto& VFXSpawn : VFXSpawns)
		{
			VFXSpawn.Reset();

			if (VFXSpawn.IsSpawnWhenCreate == false)
			{
				continue;
			}
#if UE_EDITOR
			if (WRActorUtility::IsEditorPreview(this->GetOwnerVFX()->GetSpawner()))
			{
				FActorSpawnParameters Parameters;
				Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AWRVFX* VFX = GetWorld()->SpawnActor<AWRVFX>(VFXSpawn.GeneratedClass, FVector::ZeroVector, FRotator::ZeroRotator, Parameters);
				if (VFX == nullptr)
				{
					return;
				}

				this->OnComplete(VFX, VFXSpawn.AdditionalAngle);
			}
			else
#else // UE_EDITOR
#endif // UE_EDITOR
			{
				AWRVFX* Parent = this->GetOwnerVFX();
				if (Parent == nullptr)
				{
					return;
				}

				bool IsAsync = WREnumUtility::IsBit((uint64)Parent->Bits, (uint64)EWRVFX::Async);
				if (IsAsync)
				{
					FSoftObjectPath Temp = FSoftObjectPath(VFXSpawn.GeneratedClass);

					FString Key = WRVFXManager::Get()->AsyncSpawn(Temp.GetAssetPathString(), Parent->GetSpawnerAID(), Parent->GetTargetAID(), Parent->GetUniqueID(), EWRVFX::Async, VFXSpawn.AdditionalAngle);
				}
				else
				{
					// [ 2019-8-27 : animal14 ] Parent 파라미터는 WRVFXComponentSpawn에서만 사용
					AWRVFX* VFX = WRVFXManager::Get()->Spawn(VFXSpawn.GeneratedClass, this->GetOwnerVFX()->GetSpawner(), this->GetOwnerVFX()->GetTarget(), this->GetOwnerVFX(), EWRVFX::None, VFXSpawn.AdditionalAngle);
					if (VFX == nullptr)
					{
						return;
					}
				}
			}
		}
	}
	else
	{
		// [ 2019-8-12 : animal14 ] VFX는 전적으로 Lifespan에 따라 활성화 및 비활성화
		// [ 2019-8-12 : animal14 ] 그러므로 SFX와 달리 해당 부분에서 삭제 처리X
	}
}

void UWRVFXComponentSpawn::OnComponentBeginOverlap()
{
	for (auto& VFXSpawn : VFXSpawns)
	{
		VFXSpawn.Reset();

		if (VFXSpawn.IsSpawnWhenCollide == false)
		{
			continue;
		}

#if UE_EDITOR
		if (WRActorUtility::IsEditorPreview(this->GetOwnerVFX()->GetSpawner()))
		{
			FActorSpawnParameters Parameters;
			Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AWRVFX* VFX = GetWorld()->SpawnActor<AWRVFX>(VFXSpawn.GeneratedClass, FVector::ZeroVector, FRotator::ZeroRotator, Parameters);
			if (VFX == nullptr)
			{
				return;
			}

			this->OnComplete(VFX, VFXSpawn.AdditionalAngle);
		}
		else
#else // UE_EDITOR
#endif // UE_EDITOR
		{
			AWRVFX* Parent = this->GetOwnerVFX();
			if (Parent == nullptr)
			{
				return;
			}

			bool IsAsync = WREnumUtility::IsBit((uint64)Parent->Bits, (uint64)EWRVFX::Async);
			if (IsAsync)
			{
				FSoftObjectPath Temp = FSoftObjectPath(VFXSpawn.GeneratedClass);

				FString Key = WRVFXManager::Get()->AsyncSpawn(Temp.GetAssetPathString(), Parent->GetSpawnerAID(), Parent->GetTargetAID(), Parent->GetUniqueID(), EWRVFX::Async, VFXSpawn.AdditionalAngle);
			}
			else
			{
				// [ 2019-8-27 : animal14 ] Parent 파라미터는 WRVFXComponentSpawn에서만 사용
				AWRVFX* VFX = WRVFXManager::Get()->Spawn(VFXSpawn.GeneratedClass, this->GetOwnerVFX()->GetSpawner(), this->GetOwnerVFX()->GetTarget(), this->GetOwnerVFX(), EWRVFX::None, VFXSpawn.AdditionalAngle);
				if (VFX == nullptr)
				{
					return;
				}
			}
		}
	}
}

void UWRVFXComponentSpawn::LoadTable()
{
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

	//FWRVFXSpawns Temp;
	//Temp.GeneratedClass = nullptr;
}

#if UE_EDITOR
void UWRVFXComponentSpawn::OnComplete(AWRVFX* InVFX, const FRotator& InAdditionalAngle)
{
	InVFX->Adopted(this->GetOwnerVFX());

	InVFX->Initialize(this->GetOwnerVFX()->GetSpawner(), this->GetOwnerVFX()->GetTarget(), this->GetOwnerVFX(), EWRVFX::None, InAdditionalAngle);
	InVFX->SetLifespan4Preview();
}
#else // UE_EDITOR
#endif // UE_EDITOR

bool FWRVFXSpawns::Tick(float DeltaTime)
{
	if (Delay < 0.0F)
	{
		return false;
	}

	AccDelay += DeltaTime;
	if (AccDelay < Delay)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void FWRVFXSpawns::Reset()
{
	AccDelay = 0.0F;

	IsSpawned = false;
}