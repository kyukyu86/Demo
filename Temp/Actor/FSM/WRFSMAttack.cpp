#include "WRFSMAttack.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"

#include "Enum/EWRCharacter.h"

#include "../Character/WRCharacterNPC.h"
#include "../Character/WRCharacter.h"

#include "Manager/WRConsoleManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRTutorialManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRCombatManager.h"
#include "CombatSystem/Combat/WRCombatInfo.h"
#include "Manager/WRCharacterManager.h"

WRFSMAttack::WRFSMAttack()
	:WRFSMBase(EWRFSM::Attack, EWRFSM::Idle, true)
{
}


WRFSMAttack::WRFSMAttack(EWRFSM InType, EWRFSM InLinkedFsmWhenAnimationEnd)
	: WRFSMBase(InType, InLinkedFsmWhenAnimationEnd, true)
{

}

WRFSMAttack::~WRFSMAttack()
{
}

void WRFSMAttack::Begin(const WRFSMInfo& FSMInfo)
{
	//최상위에서 호출
	FSMInfo.OwnerCharacter->BeginFSMAttack(FSMInfo);
	//공격 정보가 만들어진 이후에 호출되어야 함
	ProcessSoFX(FSMInfo);
	//이펙트 생성후에 애니메이션 시작 이라고 이카엠에서 진용이가 얘기해줌
	ProcessAnimation(FSMInfo);

	// [ 2019-11-29 : animal14 ] 임시 코드, 나중에 다같이 수정 예정
	FWRSkillTable* SkillTable = nullptr;
	if (FSMInfo.OwnerCharacter->GetCharacterType() == EWRCharacterType::Player)
	{		
		SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(FSMInfo.SkillTableID, FSMInfo.OwnerCharacter->GetSkillTableFileName());
	}
	else if (FSMInfo.OwnerCharacter->GetCharacterType() == EWRCharacterType::NPC
		|| FSMInfo.OwnerCharacter->GetCharacterType() == EWRCharacterType::ENPC)
	{
		SkillTable = WRTableManager::Get()->FindRow<FWREnpcSkillTable>(FSMInfo.SkillTableID);
	}

	if (SkillTable)
	{
		if (SkillTable->SkillType == EWRSkillType::Fast)
		{
			// [ 2019-11-18 : kyu ] 튜토리얼
			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::FastAttack);
		}
		else if (SkillTable->SkillType == EWRSkillType::Strong)
		{
			FSMInfo.OwnerCharacter->SetMaterialParameterValue("HeavyAttack", 1.0F);

			// [ 2019-11-18 : kyu ] 튜토리얼
			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::StrongAttack);
		}
	}

	// [ 2020-3-9 : animal14 ] 임시 코드
	FSMInfo.OwnerCharacter->MakeUnderWaterDisplay();
}

void WRFSMAttack::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	if (nullptr == OwnerCharacter)
		return;

	OwnerCharacter->TickFSMAttack(DeltaTime);

}

void WRFSMAttack::End(const WRFSMInfo& InReservedFSM)
{
	if (InReservedFSM.OwnerCharacter)
	{
		InReservedFSM.OwnerCharacter->EndFSMAttack(InReservedFSM);
	}

	if (InReservedFSM.OwnerCharacter == nullptr)
	{
		return;
	}

	UWRActorComponentFSM* ActorComponentFSM = InReservedFSM.OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (ActorComponentFSM == nullptr)
	{
		return;
	}
	
	
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(ActorComponentFSM->GetCurrentFSMInfo()->SkillTableID, InReservedFSM.OwnerCharacter->GetSkillTableFileName());
	if (SkillTable == nullptr)
	{
		return;
	}

	UWRActorComponentManageMaterial* ActorComponentManageMaterial = InReservedFSM.OwnerCharacter->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	if (SkillTable->SkillType == EWRSkillType::Strong)
	{
		float Duration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "HeavyAttackTime");

		ActorComponentManageMaterial->Spawn(EWRManageMaterial::HeavyAttack, Duration);
	}
}

bool WRFSMAttack::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	{
		UWRActorComponentFSM* FSMComponent = InFSMInfo.OwnerCharacter->GetComponent<UWRActorComponentFSM>();
		if (nullptr == FSMComponent)
			return false;

		return FSMComponent->IsSkipAnimation();
	}
	break;
	case EWRFSM::Skill:
	{
		//스킬은 어택을 씹고 나갈수 있음
		return true;
	}
	break;
	case EWRFSM::Arm:
	case EWRFSM::Disarm:
	case EWRFSM::Damage:
	{
		return false;
	}
	break;
	default:
	{

	}
	break;
	}

	return true;
}

EWRFSM WRFSMAttack::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	EWRFSM NextFSM = WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);

	return NextFSM;
}

void WRFSMAttack::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{
	if (nullptr == OwnerCharacter)
		return;

	UWRActorComponentFSM* FSMComponent = OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (FSMComponent)
	{
		FSMComponent->SetSkipAnimation(false);
	}
}

void WRFSMAttack::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{
	if (nullptr == OwnerCharacter)
		return;

	OwnerCharacter->SetWeaponCollisionBasedOnSide(EWREquipPart::LeftHand, ECollisionEnabled::NoCollision);
	OwnerCharacter->SetWeaponCollisionBasedOnSide(EWREquipPart::RightHand, ECollisionEnabled::NoCollision);

	UWRActorComponentFSM* FSMComponent = OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (FSMComponent)
	{
		FSMComponent->SetSkipAnimation(false);
	}
}

void WRFSMAttack::ProcessSoFX(const WRFSMInfo& InFSM)
{
	if (EWRCharacterType::SO != InFSM.OwnerCharacter->GetCharacterType())
		return;

	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InFSM.SkillTableID, InFSM.OwnerCharacter->GetSkillTableFileName());
	if (nullptr == SkillTable)
		return;

	AWRCharacter* AttackTarget = WRCharacterManager::Get()->FindbyAID(InFSM.OwnerCharacter->GetTargetAID());
	//WRVFXManager::Get()->Spawn(SkillTable->FxName.ToString(), InFSM.OwnerCharacter, AttackTarget);
	WRVFXManager::Get()->Spawn(SkillTable->FxName, InFSM.OwnerCharacter, AttackTarget);
}
