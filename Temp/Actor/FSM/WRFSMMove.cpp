#include "WRFSMMove.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Animation/WRAnimInstance.h"
#include "../Animation/WRIKAnimInstance.h"
#include "../../Network/Share/Define/Define_Actor.h"
#include "../../Manager/WRCharacterManager.h"
#include "../../Table/WREnpcTable.h"
#include "../../Enum/EWRCharacter.h"
#include "../Character/WRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/Character/WRActorComponentFSM.h"



WRFSMMove::WRFSMMove()
	:WRFSMBase(EWRFSM::Move, EWRFSM::Move)
{
}


WRFSMMove::~WRFSMMove()
{
}

void WRFSMMove::Begin(const WRFSMInfo& FSMInfo)
{
	if (nullptr == FSMInfo.OwnerCharacter)
		return;

	FSMInfo.OwnerCharacter->StopCurrentAnimMontage();

	//블렌드 스페이스 사용하는 애들은 조건걸어서 아래 함수 안타게 추가
	//if (FSMInfo.OwnerCharacter->IsPlayer() == false)
	if (FSMInfo.OwnerCharacter->GetMoveAnimType() == EWRMoveAnimType::Montage)
	{
		ProcessAnimation(FSMInfo);
	}

	UWRIKAnimInstance* InIKAnimInstance = Cast<UWRIKAnimInstance>(FSMInfo.OwnerCharacter->GetAnimInstance());
	if (InIKAnimInstance != nullptr)
	{
		InIKAnimInstance->SetIsMoving(true);
	}

	// [ 2020-3-9 : animal14 ] 임시 코드
	FSMInfo.OwnerCharacter->MakeUnderWaterDisplay();
}

void WRFSMMove::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{
	if (OwnerCharacter != nullptr && OwnerCharacter->GetMoveAnimType() == EWRMoveAnimType::BlendSpace)
	{
		CalculateForwardDeltaAndRightDeltaForBlendSpace(OwnerCharacter);
	}
}

void WRFSMMove::End(const WRFSMInfo& InReservedFSM)
{
	if (nullptr == InReservedFSM.OwnerCharacter)
		return;

	if (InReservedFSM.OwnerCharacter->GetMoveAnimType() == EWRMoveAnimType::BlendSpace)
	{
		UWRAnimInstance* InAnimInstance = InReservedFSM.OwnerCharacter->GetAnimInstance();
		if (InAnimInstance == nullptr)
			return;

		InAnimInstance->SetForwardDeltaAndRightDelta(0.f, 0.f);
	}

	UWRIKAnimInstance* InIKAnimInstance = Cast<UWRIKAnimInstance>(InReservedFSM.OwnerCharacter->GetAnimInstance());
	if (InIKAnimInstance != nullptr)
	{
		InIKAnimInstance->SetIsMoving(false);
	}
	//이부분 호출될때 wraniminstance에 추가한 value값 2개 0으로 초기화

	const FWRNPCBaseTable* NpcBaseTable = WRCharacterManager::Get()->FindNpcBaseTable(InReservedFSM.OwnerCharacter->GetCharacterTableID());
	if (NpcBaseTable)
	{
		InReservedFSM.OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = NpcBaseTable->RunSpeed;
	}
}

void WRFSMMove::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{
}

void WRFSMMove::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

bool WRFSMMove::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	return true;
}

void WRFSMMove::CalculateForwardDeltaAndRightDeltaForBlendSpace(AWRCharacter * OwnerCharacter)
{
	if (OwnerCharacter == nullptr)
		return;

	UWRAnimInstance* InAnimInstance = OwnerCharacter->GetAnimInstance();

	if (InAnimInstance == nullptr)
		return;

	FRotator OwnerRotator = OwnerCharacter->GetActorRotation();

	FVector InNormalizeVec = OwnerCharacter->GetVelocity().GetSafeNormal();
	FVector InForwardVec = OwnerCharacter->GetActorForwardVector();
	FVector InRightVec = OwnerCharacter->GetActorRightVector();

	float ForwardDelta = FVector::DotProduct(InNormalizeVec, InForwardVec);
	float RightDelta = FVector::DotProduct(InNormalizeVec, InRightVec);

	InAnimInstance->SetForwardDeltaAndRightDelta(ForwardDelta, RightDelta);
};

