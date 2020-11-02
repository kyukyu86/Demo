#include "WRSkillManager.h"
#include "Table/WRSkillTable.h"
#include "Table/Base/WRTableManager.h"
#include <Animation/AnimMontage.h>
#include "Actor/Character/WRCharacter.h"
#include "Actor/Animation/WRAnimInstance.h"



WRSkillManager::WRSkillManager()
{
}


WRSkillManager::~WRSkillManager()
{
}

void WRSkillManager::OnInitialize()
{

}

void WRSkillManager::OnShutdown()
{

}

void WRSkillManager::OnLoadLevelStart()
{

}

void WRSkillManager::OnLoadLevelComplete()
{

}

void WRSkillManager::OnRenderStart()
{

}

bool WRSkillManager::IsGuardBreakSkill(WRTableID InTableID)
{
	// [ 2019-11-29 : animal14 ] 임시 코드, 나중에 다같이 수정 예정
	FWREnpcSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWREnpcSkillTable>(InTableID);
	if (nullptr == SkillTable)
		return false;

	if (SkillTable->GuardBreak)
	{
		return true;
	}

	return false;
}

bool WRSkillManager::IsPlayingMontageGuardBreakSkill(class AWRCharacter* InCharacter)
{
	if (nullptr == InCharacter->GetAnimInstance())
		return false;
	
	FAnimMontageInstance* OtherAnimMontageInstance = InCharacter->GetAnimInstance()->GetActiveMontageInstance();
	if (nullptr == OtherAnimMontageInstance)
		return false;

	FName OtherSectionName = OtherAnimMontageInstance->GetCurrentSection();
	WRTableID SkillTID = FCString::Atoi(*OtherSectionName.ToString());

	return IsGuardBreakSkill(SkillTID);
}

EWRFSM WRSkillManager::GetFSMTypeDependOnSkillTable(AWRCharacter* InActor, WRTableID InTID)
{
	FWRSkillTable* SkillRecord = WRTableManager::Get()->FindRow<FWRSkillTable>(InTID, InActor->GetSkillTableFileName());
	if (nullptr == SkillRecord)
		return EWRFSM::Attack;

	return SkillRecord->FSMType;
// 	switch (SkillRecord->SkillType)
// 	{
// 	case EWRSkillType::Defense:
// 	{
// 		return EWRFSM::Defense;
// 	}
// 	break;
// 	default:
// 	{
// 		return EWRFSM::Attack;
// 	}
// 	break;
// 	}
}

bool WRSkillManager::IsStarterSkill(WRTableID InTID, AWRCharacter* InCaster)
{
	if (nullptr == InCaster)
		return false;

	FWRPlayerSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRPlayerSkillTable>(InTID, InCaster->GetSkillTableFileName());
	if (nullptr == SkillTable)
		return false;

	if (SkillTable->PrecedingSkillTID == InTID)
		return true;

	return false;
}

bool WRSkillManager::IsInRectangle(AWRCharacter* InCaster, AWRCharacter* InTarget, float InWidth, float InLength)
{
	if (nullptr == InCaster || nullptr == InTarget)
		return false;

	//make box	
	FVector BoxPoint[4];
	BoxPoint[0] = FVector(0.0f, -InWidth, 0.0f);
	BoxPoint[1] = FVector(InLength, -InWidth, 0.0f);
	BoxPoint[2] = FVector(InLength, InWidth, 0.0f);
	BoxPoint[3] = FVector(0.0f, InWidth, 0.0f);

	//rotate boxpoint and move
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	UKismetMathLibrary::GetYawPitchFromVector(InCaster->GetActorForwardVector(), Yaw, Pitch);
	for (int32 Index = 0; Index < 4; ++Index)
	{
		BoxPoint[Index] = UKismetMathLibrary::RotateAngleAxis(BoxPoint[Index], Yaw, FVector::UpVector) + InCaster->GetActorLocation();
	}

	//make edges
	FVector Edges[4];
	for (int32 Index = 0; Index < 4; ++Index)
	{
		int32 NextIndex = (Index + 1) % 4;

		Edges[Index] = BoxPoint[NextIndex] - BoxPoint[Index];
	}

	for (int32 index = 0; index < 4; ++index)
	{
		FVector Dir = InTarget->GetActorLocation() - BoxPoint[index];
		float dot = UKismetMathLibrary::Dot_VectorVector(Edges[index], Dir);
		if (dot < 0.0f)
		{
			return false;
		}
	}

	return true;
}
