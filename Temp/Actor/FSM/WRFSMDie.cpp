#include "WRFSMDie.h"

#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRTutorialManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRTriggerManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRConsoleManager.h"

#include "../Character/WRCharacterMine.h"
#include "../Character/WRCharacterNPC.h"

#include "Utility/WRActorUtility.h"

#include "Define/WRPathDefine.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WREnpcTable.h"

#include "Component/Character/WRActorComponentRevive.h"

WRFSMDie::WRFSMDie()
	:WRFSMBase(EWRFSM::Die, EWRFSM::Dead)
{
}


WRFSMDie::~WRFSMDie()
{
}

void WRFSMDie::Begin(const WRFSMInfo& FSMInfo)
{
	//ProcessAnimation(FSMInfo);

	FSMInfo.OwnerCharacter->BeginFSMDie(FSMInfo);

	WRCombatManager::Get()->UnregisterAll(FSMInfo.OwnerCharacter->GetActorID());

	// [ 2019-10-18 : hansang88 ] 내가 죽거나 내 타겟이 죽을경우 
	if (FSMInfo.OwnerCharacter->IsMe() == false)
	{
		if (WRCharacterManager::Get()->GetMyTarget() == FSMInfo.OwnerCharacter)
		{
			WRInputSystemManager::Get()->ReleaseTargetLock();
		}

		WRCharacterManager::Get()->AddDeadCharacter(FSMInfo.OwnerCharacter);
	}
	else
	{
		WRInputSystemManager::Get()->ReleaseTargetLock();
	}

	WRTriggerManager::Get()->RemoveFollowerID(FSMInfo.OwnerCharacter->GetTriggerSpawnTid());


	if (FSMInfo.OwnerCharacter->IsMe())
	{
		AWRCharacterMine* Me = Cast<AWRCharacterMine>(FSMInfo.OwnerCharacter);
		if (Me == nullptr)
		{
			return;
		}
		UWRActorComponentRevive* ActorComponentRevive = Me->GetComponent<UWRActorComponentRevive>();
		if (ActorComponentRevive == nullptr)
		{
			return;
		}
		ActorComponentRevive->ReviveOut();
	}
	else
	{
		this->MakeVFX(FSMInfo.OwnerCharacter, FSMInfo.OwnerCharacter->GetAttackDirection().Rotation());
		this->MakeSFX(FSMInfo.OwnerCharacter);
		this->MakePhysics(FSMInfo.OwnerCharacter);

		// [ 2019-11-18 : kyu ] 튜토리얼
		WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::MonsterKill, FSMInfo.OwnerCharacter->GetCharacterTableID());

		if (FSMInfo.OwnerCharacter->IsPlayer() == false)
		{
			// [ 2019-11-28 : animal14 ] 액터 삭제
			WRCharacterManager::Get()->Remove(FSMInfo.OwnerCharacter, FSMInfo.OwnerCharacter->GetRemoveDisplay());
		}
	}
}

void WRFSMDie::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
}

void WRFSMDie::End(const WRFSMInfo& InReservedFSM)
{
	InReservedFSM.OwnerCharacter->EndFSMDie(InReservedFSM);
}

void WRFSMDie::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMDie::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMDie::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Dead:
	case EWRFSM::Spawn:
	case EWRFSM::Revival:
	{
		return true;
	}
	break;
	default:
	{
		return false;
	}
	break;
	}
}

void WRFSMDie::MakeVFX(AWRCharacter* InCharacter, const FRotator& InDirection)
{
	if (InCharacter->IsMe())
	{
		AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_PATH_4_HIT, InCharacter, nullptr);
		if (VFX == nullptr)
		{
			return;
		}
		VFX->SetActorLocationAndRotation(InCharacter->GetActorLocation(), InDirection);
	}
	else
	{
		if (InCharacter->GetCharacterType() == EWRCharacterType::NPC)
		{
			FWRNpcTable* Table = WRTableManager::Get()->FindRow<FWRNpcTable>(InCharacter->GetCharacterTableID());
			if (Table == nullptr)
			{
				return;
			}

			//AWRVFX* VFX = WRVFXManager::Get()->Spawn(Table->DMGFX.ToString(), InCharacter, nullptr);
			AWRVFX* VFX = WRVFXManager::Get()->Spawn(Table->DMGFX, InCharacter, nullptr);
			if (VFX == nullptr)
			{
				return;
			}
			VFX->SetActorLocationAndRotation(InCharacter->GetActorLocation(), InDirection);
		}
		if (InCharacter->GetCharacterType() == EWRCharacterType::ENPC)
		{
			FWREnpcTable* Table = WRTableManager::Get()->FindRow<FWREnpcTable>(InCharacter->GetCharacterTableID());
			if (Table == nullptr)
			{
				return;
			}

			//AWRVFX* VFX = WRVFXManager::Get()->Spawn(Table->DMGFX.ToString(), InCharacter, nullptr);
			AWRVFX* VFX = WRVFXManager::Get()->Spawn(Table->DMGFX, InCharacter, nullptr);
			if (VFX == nullptr)
			{
				return;
			}
			VFX->SetActorLocationAndRotation(InCharacter->GetActorLocation(), InDirection);
		}
	}
}

void WRFSMDie::MakeSFX(AWRCharacter* InCharacter)
{
	if (InCharacter->IsMe())
	{
		WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_DEAD, InCharacter->GetActorLocation());
	}
	else
	{
		if (InCharacter->GetCharacterType() == EWRCharacterType::NPC)
		{
			FWRNpcTable* Table = WRTableManager::Get()->FindRow<FWRNpcTable>(InCharacter->GetCharacterTableID());
			if (Table == nullptr)
			{
				return;
			}

			//WRSFXManager::Get()->SpawnAtLocation(Table->DeadSound.LoadSynchronous(), InCharacter->GetActorLocation());
			WRSFXManager::Get()->SpawnAtLocation(Table->DeadSound, InCharacter->GetActorLocation());
		}
		if (InCharacter->GetCharacterType() == EWRCharacterType::ENPC)
		{
			FWREnpcTable* Table = WRTableManager::Get()->FindRow<FWREnpcTable>(InCharacter->GetCharacterTableID());
			if (Table == nullptr)
			{
				return;
			}

			//WRSFXManager::Get()->SpawnAtLocation(Table->DeadSound.LoadSynchronous(), InCharacter->GetActorLocation());
			WRSFXManager::Get()->SpawnAtLocation(Table->DeadSound, InCharacter->GetActorLocation());
		}
	}
}

void WRFSMDie::MakePhysics(AWRCharacter* InCharacter)
{
	if (InCharacter->IsMe() == false)
	{
		USkeletalMeshComponent* InSkelmeshComp = InCharacter->GetMesh();
		if (InSkelmeshComp != nullptr)
			InSkelmeshComp->SetCollisionProfileName(FName("Ragdoll"));

	}
	// [ 2019-10-21 : animal14 ] 캡슐 충돌 비활성화
	WRActorUtility::SetComponentCollisionEnabled(InCharacter->GetRootComponent(), ECollisionEnabled::NoCollision);

	// [ 2019-10-21 : animal14 ] 피직스 활성화
	InCharacter->SetSimulatePhysics(true);

	// [ 2019-12-5 : animal14 ] 충격량
	InCharacter->AddImpulseToAllBodiesBelow(0.0F);
}