#include "WRFSMBase.h"
#include "../Animation/WRAnimInstance.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Table/WRWeaponTable.h"
#include "Utility/WREnumUtility.h"
#include "../Character/WRCharacterMine.h"
#include "Manager/WRInputSystemManager.h"
#include "Table/WRPlayerTable.h"
#include "Enum/EWRCharacter.h"
#include "Manager/WRAnimManager.h"

WRFSMBase::WRFSMBase(EWRFSM InType, EWRFSM InLinkedFsmWhenAnimationEnd, bool InbLayeredBlendPerBone)
	:CurrentFSMType(InType)
	, LinkedFSMWhenAnimationEnd(InLinkedFsmWhenAnimationEnd)
	, bLayeredBlendperBone(InbLayeredBlendPerBone)
{

}

WRFSMBase::~WRFSMBase()
{
}

EWRFSM WRFSMBase::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{	
	if (nullptr == InChar)
		return EWRFSM::Idle;

	if (InChar->IsPlayer())
	{
		AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(InChar);
		if (Player && Player->IsMoving())
		{
			return EWRFSM::Move;
		}
	}
	else
	{
		if (InChar->IsAIMoveTo())
		{
			return EWRFSM::Move;
		}
	}


	return LinkedFSMWhenAnimationEnd;
}

bool WRFSMBase::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	//플레이어일경우 상하체 분리 적용할지 결정
	//블렌드 가능한 fsm일때
	if (InFSMInfo.OwnerCharacter->IsPlayer() && bLayeredBlendperBone)
	{
		//다음 실행하는 fsm이 아래와 같다면
		//현재 fsm 을 유지하기 위해서 fsm을 적용시키지 않는다.
		switch (InFSMInfo.FSMType)
		{
		case EWRFSM::Move:
		{
			return false;
		}
		default:
			break;
		}
	}

	return true;
}

FString WRFSMBase::GetBaseSectionName(EWRFSM InFSMType)
{
	return WREnumUtility::EnumToString("EWRFSM", InFSMType);
}

FString WRFSMBase::GetAttackHeightSuffix(EWRAttackHeightType InType)
{
	switch (InType)
	{
	case EWRAttackHeightType::Upper:
	{
		return "_upper";
	}
	break;
	case EWRAttackHeightType::Lower:
	{
		return "_lower";
	}
	break;
	case EWRAttackHeightType::Middle:
	{
		return "_middle";
	}
	break;
	default:
	{
	}
	break;
	}

	return "";
}

void WRFSMBase::GetMontageInfoForPlaying(const WRFSMInfo& InFSMInfo, FWRMontageInfo& OutMontageInfo)
{
	FString SectionName = "";
	int32 MontageID = INVALID_TABLE_TID;
	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Attack:
	case EWRFSM::Parrying:
	case EWRFSM::Skill:
	case EWRFSM::MaintainingSkill:
	case EWRFSM::Defense:
	{
		FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InFSMInfo.SkillTableID, InFSMInfo.OwnerCharacter->GetSkillTableFileName());
		if (SkillTable)
		{
			SectionName = FString::FromInt(InFSMInfo.SkillTableID);
			MontageID = SkillTable->SkillGroupMontageTID;

			if (InFSMInfo.OwnerCharacter->IsPlayer())
			{
				if (InFSMInfo.OwnerCharacter->IsMe())
				{
					AWRCharacterMine* pMe = Cast<AWRCharacterMine>(InFSMInfo.OwnerCharacter);
					if (pMe && pMe->IsShowing3rdCharacter())
					{
						MontageID = (static_cast<FWRPlayerSkillTable*>(SkillTable))->SkillGroupSubMontageTID;
						MontageID += InFSMInfo.OwnerCharacter->GetMontageBandwidth();
					}

					//상중하 공격에 대한설정
					if (SkillTable->IsNoneHeightFlag() == false)
					{
						SectionName += GetAttackHeightSuffix(WRInputSystemManager::Get()->GetAttackHeightType());
					}
				}
				else
				{
					MontageID = (static_cast<FWRPlayerSkillTable*>(SkillTable))->SkillGroupSubMontageTID;
					MontageID += InFSMInfo.OwnerCharacter->GetMontageBandwidth();

					//상중하 공격에 대한설정
					if (InFSMInfo.VoidPtr)
					{
						EWRAttackHeightType HeightType = *((EWRAttackHeightType*)InFSMInfo.VoidPtr);
						SectionName += GetAttackHeightSuffix(HeightType);
					}
				}
			}
		}
	}
	break;
	default:
	{
		if (InFSMInfo.JumpToSectionName.IsEmpty())
		{
			SectionName = GetBaseSectionName(InFSMInfo.FSMType);
		}
		else
		{
			SectionName = InFSMInfo.JumpToSectionName;
		}
		MontageID = InFSMInfo.OwnerCharacter->GetMontageTID(InFSMInfo.FSMType, SectionName);
		if (InFSMInfo.MontageTID != INVALID_TABLE_TID)
			MontageID = InFSMInfo.MontageTID;

		// [ 2020-8-19 : kyu ] 신규 대역대 적용 BaseTID 기반
		// 나중에 else는 안써야하나...일단 모르겠다
		if (MontageID < DEF_BANDWIDTH_NORMAL_PLAYER_TID)
		{
			MontageID = InFSMInfo.OwnerCharacter->ConvertMontageTIDFromBaseTID(MontageID);
		}
		else
		{
			MontageID += InFSMInfo.OwnerCharacter->GetMontageBandwidth();
		}
	}
	break;
	}

	OutMontageInfo.MontageID = MontageID;
	OutMontageInfo.SectionName = SectionName;
}

void WRFSMBase::ProcessAnimation(AWRCharacter* InCharacter, int32 InMontageID, FString InAnimSection, float InAnimTime /*= 0*/, float InAnimScale /*= 1.0f*/)
{
	InMontageID += InCharacter->GetMontageBandwidth();
	bool bSuccess = ProcessAnimationAsMontage(InCharacter, InMontageID, InAnimSection, InAnimScale);
	if (false == bSuccess)
	{
		InCharacter->DetermineFSMSignal(EWRFSM::Idle, true);
	}
}

void WRFSMBase::ProcessAnimation(const WRFSMInfo& InFSMInfo)
{
	FWRMontageInfo MontageInfo;
	GetMontageInfoForPlaying(InFSMInfo, MontageInfo);

	float AnimScale = 1.0f;
	bool bSuccess = ProcessAnimationAsMontage(InFSMInfo.OwnerCharacter, MontageInfo.MontageID, MontageInfo.SectionName, AnimScale);
	if (bSuccess)
	{
		if (InFSMInfo.JumpToSectionName.IsEmpty() == false)
		{
			InFSMInfo.OwnerCharacter->GetAnimInstance()->Montage_JumpToSection(FName(*InFSMInfo.JumpToSectionName));
		}
	}
	else
	{
		switch (InFSMInfo.FSMType)
		{
		case EWRFSM::Idle:
		{
			bool bPlayMontage = false;
			WRFSMInfo FSMInfo;
			FSMInfo.FSMType = EWRFSM::Idle;
			FSMInfo.VoidPtr = &bPlayMontage;
			FSMInfo.bImmediateUpdate = true;

			InFSMInfo.OwnerCharacter->DetermineFSMSignal(FSMInfo);
		}
		break;
		default:
		{
			//탑승 중인데 탑승 아이들이 실패 했다 -> 몽타주에 해당 섹션이 없어 보인다.
			if (InFSMInfo.OwnerCharacter->GetBoardingVehicleAID() != INVALID_AID)
			{
				
			}

			//실패 했으므로 강제로 아이들을 세팅
			InFSMInfo.OwnerCharacter->DetermineFSMSignal(EWRFSM::Idle, true);
		}
		break;
		}
	}
}

bool WRFSMBase::IsLayeredBlendFSM(AWRCharacter* InCharacter)
{
	// [ 2020-7-24 : kyu ] 특수 상황에서는 상하체 블렌딩이 무조건 적용되어야함. 구현 확장에 대해서는 다음에...
	if (InCharacter->GetTemporaryItemTID() != INVALID_ISN)
	{
		return true;
	}
	return bLayeredBlendperBone;
}

//특정 에셋을 몽타쥬처럼 플레이 함. 성공하면 몽타쥬를 리턴
//AnimInstance->PlaySlotAnimationAsDynamicMontage()
bool WRFSMBase::ProcessAnimationAsMontage(AWRCharacter* InCharacter, int32 MontageID, FString InSectionName, float InAnimScale)
{
	return InCharacter->PlayAnimationMontage(MontageID, InSectionName, InAnimScale);
}

EWRFSM WRFSMBase::GetFSMType()
{
	return CurrentFSMType;
}
