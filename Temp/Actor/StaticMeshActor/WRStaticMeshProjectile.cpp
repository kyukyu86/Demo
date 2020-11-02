#include "WRStaticMeshProjectile.h"
#include "../Character/WRCharacter.h"
#include "Manager/WRCharacterManager.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Enum/EWRCharacter.h"
#include "Define/WRPathDefine.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Network/IMMessageSender.h"
#include "Manager/WRCombatManager.h"
#include "CombatSystem/Combat/WRCombatInfo.h"



FName AWRStaticMeshProjectile::ProjectileMovementComponentName(TEXT("ProjMoveComp"));
AWRStaticMeshProjectile::AWRStaticMeshProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// 	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(AWRStaticMeshProjectile::ProjectileMovementComponentName);
	// 	if (ProjectileMovement)
	// 	{
	// 		ProjectileMovement->bSweepCollision = false;
	// 	}

}

void AWRStaticMeshProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWRCharacter* HitChar = Cast<AWRCharacter>(Other);
	if (nullptr == HitChar)
		return;

	AWRCharacter* OwnerChar = WRCharacterManager::Get()->FindbyAID(OwnerAID);
	if (nullptr == OwnerChar)
		return;

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(OwnerAID, CombatSeqID);
	if (nullptr == CombatInfo)
		return;

	if (CombatInfo->IsValidNextTarget(HitChar->GetActorID()) == false)
		return;

	//내가 쏜 투사체면 충돌시 서버에 히트 노티를 해야 함
	if (OwnerChar->IsMe())
	{
		if (CombatInfo->IsGettingBackProjectile())
		{
			IMMessageSender::RequestUseSkillEnd(OwnerChar, CombatInfo->GetCombatSeqID());

			WRCombatManager::Get()->Unregister(CombatInfo, true);
			//CombatInfo->Destroy();	//투사체가 주인한테 되돌아 가는중이고 주인과 투사체가 충돌나면 해당 공격정보는 삭제
		}
		else
		{
			//타겟을 지우면 combatInfo의 다음 틱에서 다음 대상에 대한 위치 업데이트를 실행
			if (HitChar != OwnerChar)
			{
				UWRCapsuleCollisionComponent* HitComponent = Cast<UWRCapsuleCollisionComponent>(OtherComp);
				CombatInfo->DetermineHit(HitChar->GetActorID(), HitComponent);

				CombatInfo->SetNextChainTaretIndex(HitChar->GetActorID());
			}
		}
	}
	else
	{
		//타겟을 지우면 combatInfo의 다음 틱에서 다음 대상에 대한 위치 업데이트를 실행
		CombatInfo->SetNextChainTaretIndex(HitChar->GetActorID());
	}
}


void AWRStaticMeshProjectile::Tick(float DeltaSeconds)
{
	TickDistance2Spawner(DeltaSeconds);
}

void AWRStaticMeshProjectile::SetIdendifier(AID_t InOwnerAID, SKILL_SEQ_t InCombatSeqID)
{
	OwnerAID = InOwnerAID;
	CombatSeqID = InCombatSeqID;
}

void AWRStaticMeshProjectile::Initialize()
{
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_PATH_4_SHIELD, this, nullptr);
	if (VFX == nullptr)
	{
		return;
	}

	UID = VFX->GetUniqueID();
}

void AWRStaticMeshProjectile::Release()
{
	if (DestoryHandle.ExecuteIfBound(this))
	{
		DestoryHandle.Unbind();
	}

	WRVFXManager::Get()->Remove(UID);
}

void AWRStaticMeshProjectile::TickDistance2Spawner(float DeltaSeconds)
{
	//받는 애니를 재생했다면 더이상 업뎃하지 않음
	if (bPlayingTakeAnim)
		return;

	AWRCharacter* OwnerChar = WRCharacterManager::Get()->FindbyAID(OwnerAID);
	if (nullptr == OwnerChar)
		return;

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(OwnerAID, CombatSeqID);
	if (nullptr == CombatInfo)
		return;

	if (CombatInfo->IsGettingBackProjectile() == false)
		return;

	float fLength = (OwnerChar->GetActorLocation() - GetActorLocation()).Size2D();
	if (fLength < 400.0f)
	{
		switch (OwnerChar->GetCurrentFSMType())
		{
		case EWRFSM::Move:
		case EWRFSM::Idle:
		{
			WRFSMInfo FSMInfo;
			FSMInfo.FSMType = EWRFSM::WeaponChange;
			FSMInfo.JumpToSectionName = "TakeShield";
			EWRItemSubType Type = EWRItemSubType::Shield;
			FSMInfo.VoidPtr = &Type;
			OwnerChar->DetermineFSMSignal(FSMInfo);
		}
		break;
		default:
		{

		}
		break;
		}

		bPlayingTakeAnim = true;
	}
}
