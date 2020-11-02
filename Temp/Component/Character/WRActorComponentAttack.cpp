// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentAttack.h"
#include "WRActorComponentFSM.h"

#include <Components/ActorComponent.h>
#include <Engine/World.h>
#include <TimerManager.h>

#include "Actor/Animation/WRAnimInstance.h"
#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterIO.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRWeaponTable.h"
#include "Table/WRSkillTable.h"

#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRVFXManager.h"

#include "Define/WRPathDefine.h"
#include "Manager/WRCombatManager.h"
#include "Network/IMMessageSender.h"
#include "Table/WREquipmentLookTable.h"
#include <GameFramework/Character.h>
#include "Manager/WRTutorialManager.h"
#include <DelegateSignatureImpl.inl>
#include "Manager/WRConsoleManager.h"
#include "Enum/EWRConsole.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WRSkillManager.h"
#include "Network/Share/Define/Define_Skill.h"
#include "WRActorComponentDynamicMovement.h"
#include "Network/Share/Protocol/Protocol_Game_User_Battle.h"
#include "WRActorComponentEquipmentLook.h"
#include "Manager/WREnvInteractionManager.h"
#include "Actor/EnvActor/WREnvVehicleBase.h"
#include "Utility/WRActorUtility.h"


/*
//��Ʈ ó�� ��ü
									npc			player
�Ϲ� ����(������ ���ݷ� ����)	:		Clinet		Clinet

����							:		Server		Server

so (�Ϲ�, ����)				:		Server		Server

���Ÿ�						:		Clinet		Clinet

*/

void UWRActorComponentAttack::OnCreateComponent()
{
	Super::OnCreateComponent();	
}

void UWRActorComponentAttack::OnDestroyComponent()
{
	Super::OnDestroyComponent();

	Initialize();
}

void UWRActorComponentAttack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWRActorComponentAttack::Initialize()
{
	FastAttackIndex = 0;
	StrongAttackIndex = 0;
	SubAttackIndex = 0;

	mapCurrentSkill.Reset();
}

void UWRActorComponentAttack::GenerateSkillInfo(WRTableID InWeaponTID)
{
	//���� ������ �÷��̾ ���⸦ �ٲٰ� �Ǹ� �ش� ���⿡ ���ӵǴ� ��Ÿ�ָ� �ε��Ѵ�
	WRCharacterManager::Get()->LoadPlayerWeaponSkillMontage(InWeaponTID);

	//���� �� �ɸ��͸� �����Ѵ�
	AWRCharacter* pChar = GetOwnerCharacter();
	if (nullptr == pChar || pChar->IsMe() == false)
		return;

	FString strTableName = "SkillPlayer_Atk";
	FWRWeaponTable* WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(InWeaponTID);
	if (nullptr == WeaponTable)
		return;

	switch (WeaponTable->WeaponType)
	{
	case EWRWeaponType::OneHandMain:
	case EWRWeaponType::OneHandSub:
	{
		TArray<EWRInput> removeInput;
		for (auto Iter = mapCurrentSkill.CreateIterator(); Iter; ++Iter)
		{
			mapTypeContainer mapSkill = Iter.Value();
			for (auto It = mapSkill.CreateIterator(); It; ++It)
			{
				TArray<WRTableID> arrSkill = It.Value();
				if (arrSkill.Num() != 0)
				{
					FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(arrSkill[0], strTableName);
					if (nullptr == SkillTable)
						continue;
						
					if (WeaponTable->WeaponType == EWRWeaponType::OneHandMain && SkillTable->EquipType == EWREquipPart::RightHand)
					{
						removeInput.Add(Iter.Key());
						break;
					}
					else if (WeaponTable->WeaponType == EWRWeaponType::OneHandSub && SkillTable->EquipType == EWREquipPart::LeftHand)
					{
						removeInput.Add(Iter.Key());
						break;
					}
				}
			}
		}

		for (int32 Index = 0; Index < removeInput.Num(); ++Index)
		{
			mapCurrentSkill.Remove(removeInput[Index]);
		}
	}
	break;
	case EWRWeaponType::TwoHand:
	{
		mapCurrentSkill.Reset();
	}
	break;
	default:
		break;
	}


	//���⿡ ���õ� ��ų ���̵�� ������ ����
	for (int32 Idx = 0; Idx < WeaponTable->SkillTID.Num(); ++Idx)
	{
		FWRPlayerSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRPlayerSkillTable>(WeaponTable->SkillTID[Idx], strTableName);
		if (nullptr == SkillTable)
			continue;
		
		if (mapCurrentSkill.Contains(SkillTable->InputKey))
		{
			mapTypeContainer* skillTypeContainer = mapCurrentSkill.Find(SkillTable->InputKey);
			if (skillTypeContainer->Contains(SkillTable->SkillType))
			{
				TArray<WRTableID>* SkillArray = skillTypeContainer->Find(SkillTable->SkillType);
				SkillArray->Add(WeaponTable->SkillTID[Idx]);
			}
			else
			{
				TArray<WRTableID> SkillArray;
				SkillArray.Add(WeaponTable->SkillTID[Idx]);
				skillTypeContainer->Add(SkillTable->SkillType, SkillArray);
			}
		}
		else
		{
			mapTypeContainer skillTypeContainer;

			TArray<WRTableID> SkillArray;
			SkillArray.Add(WeaponTable->SkillTID[Idx]);
			skillTypeContainer.Add(SkillTable->SkillType, SkillArray);

			mapCurrentSkill.Add(SkillTable->InputKey, skillTypeContainer);
		}
	}
}

void UWRActorComponentAttack::RemoveSkillInfo(EWREquipPart InPart)
{
	FString strTableName = "SkillPlayer_Atk";
	switch (InPart)
	{
	case EWREquipPart::RightHand:
	case EWREquipPart::LeftHand:
	{
		TArray<EWRInput> removeInput;
		for (auto Iter = mapCurrentSkill.CreateIterator(); Iter; ++Iter)
		{
			mapTypeContainer mapSkill = Iter.Value();
			for (auto It = mapSkill.CreateIterator(); It; ++It)
			{
				TArray<WRTableID> arrSkill = It.Value();
				if (arrSkill.Num() != 0)
				{
					FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(arrSkill[0], strTableName);
					if (nullptr == SkillTable)
						continue;

					if (SkillTable->EquipType == InPart)
					{
						removeInput.Add(Iter.Key());
						break;
					}
				}
			}
		}

		for (int32 Index = 0; Index < removeInput.Num(); ++Index)
		{
			mapCurrentSkill.Remove(removeInput[Index]);
		}
	}
	break;
	default:
	{
		mapCurrentSkill.Reset();
	}
	break;
	}
}

bool UWRActorComponentAttack::DetermineParrying()
{
	AWRCharacter* pChar = GetOwnerCharacter();
	if (nullptr == pChar)
		return false;

	AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(pChar->GetTargetAID());
	if (nullptr == Target)
		return false;

	UWRAnimInstance* TargetAnimIns = Target->GetAnimInstance();
	if (nullptr == TargetAnimIns)
		return false;

	if (TargetAnimIns->IsIncludedCombatGimmickState(EWRCombatGimmickState::Parrying) == false)
		return false;

	return true;
}

void UWRActorComponentAttack::TestShield()
{
	if (bThrowingShield)
	{
		return;
	}

	AWRCharacter* pChar = GetOwnerCharacter();
	if (nullptr == pChar)
		return;

	UWRActorComponentFSM* FSMComponent = pChar->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return;

	if (FSMComponent->IsPossibleEndCurrentFSM(EWRFSM::Attack) == false)
		return;

	//���д����� �׽�Ʈ��
	TArray<WRTableID> arrProjectile = GetTIDArrBySkillType(EWRSkillType::ChainProjectile, EWREquipPart::LeftHand);
	if (arrProjectile.Num() != 0)
	{
		FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(arrProjectile[0], "SkillPlayer_Atk");
		if (SkillTable && (SkillTable->TargetingType == EWRTargetingType::Multi))
		{
			WRFSMInfo fsmInfo(EWRFSM::Attack, arrProjectile[0], 0, 0);
			pChar->DetermineFSMSignal(fsmInfo);

			if (pChar->GetTargetAID() != INVALID_AID)
			{
				TArray<AID_t> arrChainTarget;
				arrChainTarget.Add(pChar->GetTargetAID());
				WRCombatInfo* ShieldCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(pChar->GetActorID());
				if (ShieldCombatInfo)
				{
					ShieldCombatInfo->SetChainTargetInfo(arrChainTarget);
				}


				UseSkillParam UseParam;
				UseParam.Caster = pChar;
				UseParam.SetSkillTID(arrProjectile[0], pChar->GetSkillTableFileName());
				UseParam.TargetList = arrChainTarget;
				IMMessageSender::RequestUseSkillBegin(UseParam);

				SetThrowingShieldFlag(true);
				return;
			}
			else
			{
				WRCombatInfo* ShieldCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(pChar->GetActorID());
				if (ShieldCombatInfo)
				{
					WRCombatManager::Get()->Unregister(ShieldCombatInfo, true);
				}
			}
		}
	}
}




void UWRActorComponentAttack::SetThrowingShieldFlag(bool bFlag)
{
	bThrowingShield = bFlag;
}

void UWRActorComponentAttack::CallbackFuncFastAttack()
{
	FastAttackIndex = 0;
}

void UWRActorComponentAttack::CallbackFuncStrongAttack()
{
	StrongAttackIndex = 0;
}

void UWRActorComponentAttack::OnProcessExecuteSkill(WRTableID InSkillTID)
{
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InSkillTID, "SkillPlayer_Atk");
	if (nullptr == SkillTable)
		return;

	if (SkillTable->ComboSkill)
	{
		switch (SkillTable->SkillType)
		{
		case EWRSkillType::Fast:
		{
			TArray<WRTableID> arrSkill = GetTIDArrBySkillType(EWRSkillType::Fast, SkillTable->EquipType);
			++FastAttackIndex;
			if (FastAttackIndex < arrSkill.Num())
			{
			}
			else
			{
				FastAttackIndex = 0;
			}
		}
		break;
		case EWRSkillType::Strong:
		{
			TArray<WRTableID> arrSkill = GetTIDArrBySkillType(EWRSkillType::Strong, SkillTable->EquipType);
			++StrongAttackIndex;
			if (StrongAttackIndex < arrSkill.Num())
			{
			}
			else
			{
				StrongAttackIndex = 0;
			}
		}
		break;
		default:
			break;
		}
	}
	else
	{
		switch (SkillTable->SkillType)
		{
		case EWRSkillType::Fast:
		{
			FastAttackIndex = 0;
		}
		break;
		case EWRSkillType::Strong:
		{
			StrongAttackIndex = 0;		
		}
		break;
		default:
			break;
		}
	}

}

void UWRActorComponentAttack::OnProcessNormalAttackInitialization(WRTableID InSkillTID)
{
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InSkillTID, "SkillPlayer_Atk");
	if (nullptr == SkillTable)
		return;

	switch (SkillTable->SkillType)
	{
	case EWRSkillType::Fast:
	{
		if (SkillTable->ComboSkill)
		{
			FTimerDelegate DelTimer = FTimerDelegate::CreateUObject(this, &UWRActorComponentAttack::CallbackFuncFastAttack);
			SetTimer(FastAttackComboTimer, DelTimer, 2.0f);
		}
		else
		{
			WRActorUtility::ClearTimer(FastAttackComboTimer);
		}
	}
	break;
	case EWRSkillType::Strong:
	{
		if (SkillTable->ComboSkill)
		{
			FTimerDelegate DelTimer = FTimerDelegate::CreateUObject(this, &UWRActorComponentAttack::CallbackFuncStrongAttack);
			SetTimer(StrongAttackComboTimer, DelTimer, 2.0f);
		}
		else
		{
			WRActorUtility::ClearTimer(StrongAttackComboTimer);
		}
	}
	break;
	default:
		break;
	}
}

void UWRActorComponentAttack::SetTimer(FTimerHandle& InTimerHandle, FTimerDelegate const& InDelegate, float InRate)
{
	if (GetWorld())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(InTimerHandle))
		{
			ClearTimer(InTimerHandle);
		}

		GetWorld()->GetTimerManager().SetTimer(InTimerHandle, InDelegate, InRate, false);
	}
}

void UWRActorComponentAttack::ClearTimer(FTimerHandle& InTimerHandle)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(InTimerHandle);
	}
}

WRTableID UWRActorComponentAttack::GetValidTID(EWRInput InType)
{
	int32 TestFlag = WRConsoleManager::Get()->GetInt(EWRConsole::Test_TID);
	if (TestFlag != 0)
		return TestFlag;

	//��ư�� �Ҵ�� ��ų �����̳� ������
	mapTypeContainer SkillTypeContainer = mapCurrentSkill.FindRef(InType);
	if (SkillTypeContainer.Num() == 0)
		return INVALID_TABLE_TID;

	EWRSkillType SkillType = EWRSkillType::None;
	TArray<WRTableID> arrSkill;
	for (auto Iter = SkillTypeContainer.CreateIterator(); Iter; ++Iter)
	{
		//�ش� ��ư�� Ư�� �������� �ߵ��Ǹ�, �� ������ �����Ǵ� ��ų�� �ִ��� ���� üũ
		EWRSkillType CheckSkillType = Iter.Key();

		bool bSet = false;
		switch (CheckSkillType)
		{
		case EWRSkillType::Parrying:
		{
			if (DetermineParrying())
			{
				//������ �����صΰ�
				bSet = true;
			}
		}
		break;
		/*
		//����� ������ �̷��� üũ�ؾ� �ϴ� ��ų�ϰ�� ���⼭ üũ �ؼ� �� ���� ����
		*/
		default:
		{
			//������ �����صΰ�
			SkillType = Iter.Key();
			arrSkill = Iter.Value();
		}
		break;
		}

		if (bSet)
		{
			SkillType = Iter.Key();
			arrSkill = Iter.Value();
			break;
		}
	}

	switch (SkillType)
	{
	case EWRSkillType::Fast:
	{
		if (FastAttackIndex < arrSkill.Num())
		{
			return arrSkill[FastAttackIndex];
		}
	}
	break;
	case EWRSkillType::Strong:
	{
		if (StrongAttackIndex < arrSkill.Num())
		{
			return arrSkill[StrongAttackIndex];
		}
	}
	break;
	case EWRSkillType::Defense:
	{
		//���⽺ų�� �ΰ� �̻��϶��� 
		//�޼տ� �� ������ ���⸦ �켱���� �����Ѵ�
		if (1 < arrSkill.Num())
		{
			for (int32 Idx = 0; Idx < arrSkill.Num(); ++Idx)
			{
				FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(arrSkill[Idx], "SkillPlayer_Atk");
				if (nullptr == SkillTable)
					continue;
				
				if (SkillTable->EquipType == EWREquipPart::LeftHand)
				{
					return arrSkill[Idx];
				}
			}
		}
		else
		{
			//�� �ܿ��� �׳� �ٷ� ����
			return arrSkill[0];
		}

		return INVALID_TABLE_TID;
	}
	break;
	case EWRSkillType::Hook:
	{
		return GetValidHookTID(SkillType, arrSkill);
	}
	break;
	default:
	{
		return GetValidDefaultTID(SkillType, arrSkill);
	}
	break;
	}


	return INVALID_TABLE_TID;

}

TArray<WRTableID> UWRActorComponentAttack::GetTIDArrBySkillType(EWRSkillType InType, EWREquipPart InEquipType)
{
	TArray<WRTableID> reVal;
	for (auto Iter = mapCurrentSkill.CreateIterator(); Iter; ++Iter)
	{
		TArray<WRTableID> temp = Iter.Value().FindRef(InType);
		if (temp.Num() == 0)
			continue;

		FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(temp[0], "SkillPlayer_Atk");
		if (nullptr == SkillTable)
			continue;

		if (SkillTable->EquipType != InEquipType)
			continue;
			
		reVal = temp;
	}

	return reVal;
}


WRTableID UWRActorComponentAttack::GetValidHookTID(EWRSkillType InSkillType, const TArray<WRTableID>& InArrSkill)
{
	AWRCharacter* pOwner = GetOwnerCharacter();
	if (nullptr == pOwner)
		return INVALID_TABLE_TID;

	AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(pOwner);
	if (nullptr == CastedMine)
		return INVALID_TABLE_TID;

	// [ 2020-1-21 : kyu ] ���� �ٶ󺸴� Ÿ���� ��� ���̺� Ÿ�ٿ� ���� üũ�Ǿ���Ѵ�
	AWRCharacter* pTarget = WRCharacterManager::Get()->FindbyAID(pOwner->GetTargetAID());
	if (nullptr == pTarget)
	{
		pTarget = CastedMine->GetHookTargetCharacter();
		if(nullptr == pTarget)
			return INVALID_TABLE_TID;
	}

	// ���� ��Ȳ���� �ߵ� �Ǿ�� �� ���� �̺�Ʈ (������ or �׼�)
	EWRHookEvent HookEvent = DetermineHookEvent();

	/*
	�Ϲ������� �ϳ��� Ű�� ������ųŸ���� ���� ��ų�� ���õ� ����
	���ེų�� üũ �ؾ� ��
	*/
	if (1 < InArrSkill.Num())
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(pOwner->GetActorID());
		for (int32 Idx = 0; Idx < InArrSkill.Num(); ++Idx)
		{
			FWRPlayerSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRPlayerSkillTable>(InArrSkill[Idx], "SkillPlayer_Atk");
			if (nullptr == SkillTable)
				continue;

			switch (SkillTable->TargetType)
			{
			case EWRInvokeSkillTargetType::Enemy:
			{
				if (pTarget->GetCharacterType() != EWRCharacterType::Player &&
					pTarget->GetCharacterType() != EWRCharacterType::ENPC)
					continue;
			}
			break;
			case EWRInvokeSkillTargetType::IO:
			{
				if (pTarget->GetCharacterType() != EWRCharacterType::IO)
					continue;
			}
			break;
			default:
				break;
			}

			//�̹� ���� �ܰ谡 ���õ� ���¸�, ���� ��ų�� ������ ��ų�� ã�Ƽ� ����
			switch (HookEvent)
			{
			case EWRHookEvent::Throw:
			{
				//�Ϲ����� ���¸� ���� ���� ��ų�� ����
				if (WRSkillManager::Get()->IsStarterSkill(InArrSkill[Idx], pOwner))
				{
					return InArrSkill[Idx];
				}
				else
				{
					continue;
				}
			}
			break;
			case EWRHookEvent::Action:
			{
				if (pOwner->GetCurrentFSMType() == EWRFSM::MaintainingSkill)
				{
					if (WRSkillManager::Get()->IsStarterSkill(InArrSkill[Idx], pOwner))
						continue;

					if (CombatInfo)
					{
						if (SkillTable->PrecedingSkillTID == CombatInfo->GetCombatSeqParam().SkillTID)
						{
							return InArrSkill[Idx];
						}
					}
				}
			}
			break;
			default:
			{
				continue;
			}
			break;
			}

			if (SkillTable->PrecedingSkillTID != 0)
			{
				return InArrSkill[Idx];
			}
		}
	}
	else
	{
		FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InArrSkill[0], "SkillPlayer_Atk");
		if (SkillTable == nullptr)
			return INVALID_TABLE_TID;

		switch (SkillTable->AttackType)
		{
		case EWRAttackType::Throw:
		{
			if (HookEvent == EWRHookEvent::Throw)
				return INVALID_TABLE_TID;
		}
		break;
		case EWRAttackType::Pull:
		{
			if (HookEvent == EWRHookEvent::Action)
				return INVALID_TABLE_TID;
		}
		break;
		default:
			break;
		}

		//�� �ܿ��� �׳� �ٷ� ����
		return InArrSkill[0];
	}

	return INVALID_TABLE_TID;

}

WRTableID UWRActorComponentAttack::GetValidDefaultTID(EWRSkillType InSkillType, const TArray<WRTableID>& InArrSkill)
{
	AWRCharacter* pOwner = GetOwnerCharacter();
	if (nullptr == pOwner)
		return INVALID_TABLE_TID;
	
	//ù��° ��ų�� �����Ϸ��� �ϴµ� �� ��ų�� �̹� ����� ��ų�̸� ���� ��ų�� üũ�ϴ� ���
	/*
	�Ϲ������� �ϳ��� Ű�� ������ųŸ���� ���� ��ų�� ���õ� ����
	���ེų�� üũ �ؾ� ��
	*/
	if (1 < InArrSkill.Num())
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(pOwner->GetActorID());
		for (int32 Idx = 0; Idx < InArrSkill.Num(); ++Idx)
		{
			FWRPlayerSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRPlayerSkillTable>(InArrSkill[Idx], "SkillPlayer_Atk");
			if (nullptr == SkillTable)
				continue;

			//�̹� ���� �ܰ谡 ���õ� ���¸�, ���� ��ų�� ������ ��ų�� ã�Ƽ� ����
			if (pOwner->GetCurrentFSMType() == EWRFSM::MaintainingSkill)
			{
				if (WRSkillManager::Get()->IsStarterSkill(InArrSkill[Idx], pOwner))
					continue;

				if (CombatInfo)
				{
					if (SkillTable->PrecedingSkillTID == CombatInfo->GetCombatSeqParam().SkillTID)
					{
						return InArrSkill[Idx];
					}
				}
			}
			else //�Ϲ����� ���¸� ���� ���� ��ų�� ����
			{
				if (WRSkillManager::Get()->IsStarterSkill(InArrSkill[Idx], pOwner))
				{
					return InArrSkill[Idx];
				}
				else
				{
					continue;
				}
			}

			if (SkillTable->PrecedingSkillTID != 0)
			{
				return InArrSkill[Idx];
			}
		}
	}
	else
	{
		//�� �ܿ��� �׳� �ٷ� ����
		return InArrSkill[0];
	}

	return INVALID_TABLE_TID;

}

bool UWRActorComponentAttack::IsActiveCoolTime(WRTableID InTID)
{
	int32 TestFlag = WRConsoleManager::Get()->GetInt(EWRConsole::Test_NoCool);
	if (1 == TestFlag)
		return false;

	if (CoolTimeContainer.Contains(InTID))
	{
		if (WRActorUtility::IsTimerActive(CoolTimeContainer.FindRef(InTID)))
		{
			return true;
		}
	}

	return false;
}

void UWRActorComponentAttack::SetCoolTime(AWRCharacter* InActor, WRTableID InTID)
{
	if (nullptr == InActor)
		return;

	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InTID, InActor->GetSkillTableFileName());
	if (nullptr == SkillTable)
		return;

	if (SkillTable->GetCoolTime() <= 0)
		return;

	if (IsActiveCoolTime(InTID))
		return;
	   
	FTimerHandle CooldownTimer;
	FTimerDelegate DeleTimer = FTimerDelegate::CreateUObject(this, &UWRActorComponentAttack::CallBackCoolTimeEnd, InTID);
	WRActorUtility::SetTimer(CooldownTimer, DeleTimer, SkillTable->GetCoolTime(), false);

	CoolTimeContainer.Add(InTID, CooldownTimer);
}

void UWRActorComponentAttack::CallBackCoolTimeEnd(WRTableID InTID)
{
	CoolTimeContainer.Remove(InTID);
}

int32 UWRActorComponentAttack::GetCoolTime(WRTableID InTID)
{
	if (IsActiveCoolTime(InTID))
	{
		FTimerHandle TimeHandle = CoolTimeContainer.FindRef(InTID);
		return WRActorUtility::GetTimerRemaining(TimeHandle);
	}

	return 0;
}

WRTableID UWRActorComponentAttack::GetCurrentHookTID(const AWRCharacter* InTarget)
{
	if (InTarget == nullptr)
		return INVALID_TABLE_TID;

	EWRInvokeSkillTargetType TargetType = EWRInvokeSkillTargetType::None;
	switch (InTarget->GetCharacterType())
	{
	case EWRCharacterType::IO:
	{
		TargetType = EWRInvokeSkillTargetType::IO;
	}
	break;
	case EWRCharacterType::NPC:
	case EWRCharacterType::ENPC:
	case EWRCharacterType::Player:
	{
		TargetType = EWRInvokeSkillTargetType::Enemy;
	}
	break;
	default:
	{
		return false;
	}
	break;
	}

	TArray<WRTableID> HookArray = GetTIDArrBySkillType(EWRSkillType::Hook, EWREquipPart::LeftHand);
	if (HookArray.Num() != 0)
	{
		for (auto TableID : HookArray)
		{
			FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(TableID, "SkillPlayer_Atk");
			if (SkillTable == nullptr)
				continue;

			if (SkillTable->TargetType != TargetType)
				continue;

			return TableID;
		}

// 		int32 nIndex = DetermineHookIndex();
// 		if (HookArray.IsValidIndex(nIndex) == false)
// 			return 0;
//		return HookArray[0];
	}
	return INVALID_TABLE_TID;
}

EWRHookEvent UWRActorComponentAttack::DetermineHookEvent()
{
	AWRCharacter* pChar = GetOwnerCharacter();
	if (nullptr == pChar)
		return EWRHookEvent::None;

	AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(pChar);
	if (CastedMine == nullptr)
		return EWRHookEvent::None;

	UWRActorComponentConnectbyCable* CableComponent = pChar->GetComponent<UWRActorComponentConnectbyCable>();
	if (CableComponent == nullptr)
		return EWRHookEvent::None;

	AWRCharacter* pTarget = const_cast<AWRCharacter*>(GetOwnerCharacter()->GetTargetCharacter());
	if (pTarget == nullptr)
	{
		if (CableComponent->IsWorking())
			return EWRHookEvent::Action;	// ����� ��� �������̸� [����� ���] ���� ����
		else
			return EWRHookEvent::None;		// ����� ���µ� ���ᵵ �ȵǾ� ������ �ƹ��͵� �ȵ�
	}

	switch (pTarget->GetCharacterType())
	{
	case EWRCharacterType::IO:
	{
		AWRCharacterIO* CastedChar = Cast<AWRCharacterIO>(pTarget);
		if (CastedChar == nullptr)
		{
			if (CableComponent->IsWorking())
				return EWRHookEvent::Action;		// Ȥ�� Ÿ�� ��ȯ�� �����ص� �������̸� [����� ���] ���� ����
			else
				return EWRHookEvent::None;			// Ÿ�� ��ȯ ���� & ���ᵵ �ȵ� : �ƹ��͵� �ȵ�
		}

		if (CableComponent->IsWorking())
		{
			if (CableComponent->GetCableTarget() == pTarget)
			{
				return EWRHookEvent::Action;		// ���� ��� : [����� ���] ����
			}
			else
			{
				if (CastedChar->IsHookableState())
				{
					return EWRHookEvent::Throw;		// �ٸ� ���(����) : [�ٸ� ���] ������
				}
				else
				{
					return EWRHookEvent::Action;	// �ٸ� ���(�Ұ���) : [����� ���] ����
				}
			}
		}
		else
		{
			if (CastedChar->IsHookableState())
			{
				return EWRHookEvent::Throw;			// �ٸ� ���(����) : [�ٸ� ���] ������
			}
		}
	}
	break;
	case EWRCharacterType::ENPC:
	case EWRCharacterType::NPC:
	case EWRCharacterType::Player:
	{
		if (CableComponent->IsWorking())
		{
			if (CableComponent->GetCableTarget() == pTarget)
			{
				if (CableComponent->GetCablePart() == CastedMine->GetCurrentTargetedHookPart())
				{
					return EWRHookEvent::Action;	// ���� ���, ���� ���� : [������ ���] ���� ����
				}
				else
				{
					return EWRHookEvent::Throw;		// ���� ���, �ٸ� ���� : [������ ���] ������ ����
				}
			}
			else
			{
				if (pTarget->IsHookableState() && CastedMine->IsHookTargeted())
				{
					CastedMine->SaveCurrentHookPart();	// �ٸ� ���(����) : [�ٸ� ���] ������ ����
					return EWRHookEvent::Throw;
				}
				else
				{
					return EWRHookEvent::Action;		// �ٸ� ���(�Ұ���) : [������ ���] ���� ����
				}
			}
		}
		else
		{
			if (pTarget->IsHookableState() && CastedMine->IsHookTargeted())
			{
				CastedMine->SaveCurrentHookPart();	// �ٸ� ���(����) : [�ٸ� ���] ������ ���� 
				return EWRHookEvent::Throw;
			}
		}
	}
	break;
	}

	return EWRHookEvent::None;
}

void UWRActorComponentAttack::PressBtn(EWRInput InInputType)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	WRTableID SkillTID = GetValidTID(InInputType);
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(SkillTID, "SkillPlayer_Atk");
	if (nullptr == SkillTable)
		return;
	
	UWRActorComponentFSM* FSMComponent = OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return;

	if (FSMComponent->IsPossibleEndCurrentFSM(SkillTable->FSMType) == false)
		return;

	//FSMó��
	switch (SkillTable->SkillType)
	{
	case EWRSkillType::Parrying:
	{
		AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(OwnerCharacter->GetTargetAID());
		if (Target)
		{
			WRFSMInfo fsmInfo(SkillTable->FSMType, SkillTID, 0, 0);
			OwnerCharacter->DetermineFSMSignal(fsmInfo);

			WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(OwnerCharacter->GetActorID());
			if (CombatInfo)
			{
				CombatInfo->ExecuteParrying(Target->GetActorID());
			}
		}
	}
	break;
	case EWRSkillType::Dash:
	{
		if (WRInputSystemManager::Get()->IsLocationLock())
			return;

		if (WRTutorialManager::Get()->IsTutorialMovableBlock())
			return;

// 		AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
// 		if (Player)
// 		{
// 			AWREnvVehicleBase* Vehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(Player->GetEnvVehicleAID());
// 			if (Vehicle)
// 			{
// 				return;
// 			}
// 		}

		if (IsActiveCoolTime(SkillTID))
		{
			WRUIManager::Get()->OpenSkillCoolTimeUI(GetCoolTime(SkillTID));
		}
		else
		{
			WRFSMInfo fsmInfo(SkillTable->FSMType, SkillTID, 0, 0);
			OwnerCharacter->DetermineFSMSignal(fsmInfo);
		}
	}
	break;
	case EWRSkillType::Defense:
	{
		if (bThrowingShield)
			return;

		WRFSMInfo fsmInfo(SkillTable->FSMType, SkillTID, 0, 0);
		OwnerCharacter->DetermineFSMSignal(fsmInfo);
	}
	break;
	case EWRSkillType::ChainProjectile:
	{
		if (bThrowingShield)
			return;

		if (SkillTable->TargetingType == EWRTargetingType::Multi)
		{
			//���⼭ �� ���� �ʴ´�
			FWRPlayerSkillTable* pPlayerSkillTable = static_cast<FWRPlayerSkillTable*>(SkillTable);
			if (pPlayerSkillTable)
			{
				WRInputSystemManager::Get()->SetMultiTargetMode(true, pPlayerSkillTable->MultiCount);
			}
		}
	}
	break;
	case EWRSkillType::Hook:
	{
		if (IsActiveCoolTime(SkillTID))
		{
			WRUIManager::Get()->OpenSkillCoolTimeUI(GetCoolTime(SkillTID));
		}
		else
		{
			WRFSMInfo fsmInfo(SkillTable->FSMType, SkillTID, 0, 0);
			OwnerCharacter->DetermineFSMSignal(fsmInfo);
		}
	}
	break;
	default:
	{
		int32 TestFlag = WRConsoleManager::Get()->GetInt(EWRConsole::Test_Throw_Shield);
		if (0 < TestFlag)
		{
			TestShield();
		}
		else
		{
			WRFSMInfo fsmInfo(SkillTable->FSMType, SkillTID, 0, 0);
			OwnerCharacter->DetermineFSMSignal(fsmInfo);
		}
	}
	break;
	}

	//��Ŷ ����
	UseSkillParam UseParam;
	UseParam.Caster = OwnerCharacter;
	UseParam.SetSkillTID(SkillTID, OwnerCharacter->GetSkillTableFileName());
	AWRCharacter* pTarget = WRCharacterManager::Get()->FindbyAID(OwnerCharacter->GetTargetAID());
	if (pTarget)
	{
		UseParam.TargetList.Add(pTarget->GetActorID());
	}

	switch (SkillTable->SkillType)
	{
	case EWRSkillType::Defense:
	{
		IMMessageSender::RequestUseSkillBegin(UseParam);
	}
	break;
	case EWRSkillType::Hook:
	{
		AWRCharacterMine* pMine = Cast<AWRCharacterMine>(OwnerCharacter);
		if (SkillTable->FSMType == EWRFSM::MaintainingSkill)
		{
			UseParam.CollisionType = pMine->GetHookPart();
			IMMessageSender::RequestUseSkillBegin(UseParam);
		}		
		else
		{
			UseParam.CollisionType = pMine->GetHookPart();
			IMMessageSender::RequestUseSkill(UseParam);
		}
	}
	break;
	default:
	{
		IMMessageSender::RequestUseSkill(UseParam);
	}
	break;
	}
}

void UWRActorComponentAttack::ReleaseBtn(EWRInput InInputType)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	WRTableID SkillTID = GetValidTID(InInputType);
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(SkillTID, "SkillPlayer_Atk");
	if (nullptr == SkillTable)
		return;

	switch (SkillTable->SkillType)
	{
	case EWRSkillType::Defense:
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(OwnerCharacter->GetActorID());
		if (CombatInfo)
		{
			IMMessageSender::RequestUseSkillEnd(OwnerCharacter, CombatInfo->GetCombatSeqID());
		}

		//OwnerCharacter->DetermineFSMSignal(EWRFSM::Idle, true);
		if (OwnerCharacter->GetCurrentFSMType() == EWRFSM::Defense)
		{
			AWRCharacterMine* pMe = Cast<AWRCharacterMine>(OwnerCharacter);
			if (pMe)
			{
				if (pMe->IsMoving())
				{
					OwnerCharacter->DetermineFSMSignal(EWRFSM::Move, true);
				}
				else
				{
					OwnerCharacter->DetermineFSMSignal(EWRFSM::Idle, true);
				}
			}

		}
	}
	break;
	case EWRSkillType::ChainProjectile:
	{
		if (bThrowingShield)
			return;

		if (SkillTable->TargetingType != EWRTargetingType::Multi)
			return;

		if (WRInputSystemManager::Get()->IsActiveMultiTargetMode() == false)
			return;

		if (IsActiveCoolTime(SkillTID))
			return;

		WRFSMInfo fsmInfo(SkillTable->FSMType, SkillTID, 0, 0);
		OwnerCharacter->DetermineFSMSignal(fsmInfo);

		TArray<AID_t> arrChainTarget;
		TArray<ACharacter*> MultiTargets = WRInputSystemManager::Get()->GetMultiTarget();
		for (int32 Idx = 0; Idx < MultiTargets.Num(); ++Idx)
		{
			AWRCharacter* pTarget = Cast<AWRCharacter>(MultiTargets[Idx]);
			if (nullptr == pTarget)
				continue;

			if (pTarget->GetCharacterType() == EWRCharacterType::IO)
				continue;
			
			if (arrChainTarget.Contains(pTarget->GetActorID()))
				continue;

			arrChainTarget.Add(pTarget->GetActorID());
		}

		if (arrChainTarget.Num() != 0)
		{
			WRCombatInfo* ShieldCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(OwnerCharacter->GetActorID());
			if (ShieldCombatInfo)
			{
				ShieldCombatInfo->SetChainTargetInfo(arrChainTarget);

				UseSkillParam UseParam;
				UseParam.Caster = OwnerCharacter;
				UseParam.SetSkillTID(SkillTID, OwnerCharacter->GetSkillTableFileName());
				UseParam.TargetList = arrChainTarget;

				IMMessageSender::RequestUseSkillBegin(UseParam);

				SetThrowingShieldFlag(true);
			}
		}

		WRInputSystemManager::Get()->SetMultiTargetMode(false);
	}
	break;
	default:
	{
	}
	break;
	}
}