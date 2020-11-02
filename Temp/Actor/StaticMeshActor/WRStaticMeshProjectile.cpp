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

	//���� �� ����ü�� �浹�� ������ ��Ʈ ��Ƽ�� �ؾ� ��
	if (OwnerChar->IsMe())
	{
		if (CombatInfo->IsGettingBackProjectile())
		{
			IMMessageSender::RequestUseSkillEnd(OwnerChar, CombatInfo->GetCombatSeqID());

			WRCombatManager::Get()->Unregister(CombatInfo, true);
			//CombatInfo->Destroy();	//����ü�� �������� �ǵ��� �������̰� ���ΰ� ����ü�� �浹���� �ش� ���������� ����
		}
		else
		{
			//Ÿ���� ����� combatInfo�� ���� ƽ���� ���� ��� ���� ��ġ ������Ʈ�� ����
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
		//Ÿ���� ����� combatInfo�� ���� ƽ���� ���� ��� ���� ��ġ ������Ʈ�� ����
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
	//�޴� �ִϸ� ����ߴٸ� ���̻� �������� ����
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
