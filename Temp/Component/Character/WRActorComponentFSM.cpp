// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentFSM.h"
#include "Manager/WRFSMManager.h"
#include <Animation/AnimMontage.h>
#include <TimerManager.h>
#include "Actor/Character/WRCharacter.h"
#include "Actor/Animation/WRAnimInstance.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Character/WRCharacterMine.h"
#include <Components/SkeletalMeshComponent.h>
#include "Actor/Character/WRCharacterPlayer.h"
#include "Manager/WRInputSystemManager.h"

void UWRActorComponentFSM::OnCreateComponent()
{
	Super::OnCreateComponent();

	//�� �ɸ��Ϳ� npc���� fsmComp�� �����ɶ� �ٷ� ��������Ʈ �����
	//�� �̿��� �÷��̾���� AWRCharacter::OnAnimInitializedDele() ���⼭ ó��


	InvokeFSM(EWRFSM::Idle, true);
}

void UWRActorComponentFSM::OnDestroyComponent()
{
	Super::OnDestroyComponent();

	UWorld* pWorld = GetWorld();
	if (pWorld)
	{
		pWorld->GetTimerManager().ClearTimer(PauseAnimationTimerHandle);
	}
}

void UWRActorComponentFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentFSM)
	{
		CurrentFSM->Tick(GetOwnerCharacter(), DeltaTime);
	}
}

// void UWRActorComponentFSM::RegistFuncOnMontageDelegate()
// {
// 	if (GetOwnerCharacter())
// 	{
// 		UWRAnimInstance* AnimInstance = GetOwnerCharacter()->GetAnimInstance();
// 		if (AnimInstance)
// 		{
// 			AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAnimMontageEnded);
// 			AnimInstance->OnMontageBlendingOut.AddDynamic(this, &ThisClass::OnAnimMontageBlendingOut);
// 		}	
// 	}
// }
// 
// void UWRActorComponentFSM::RegistFuncOnMontageDelegate4Me()
// {
// 	AWRCharacterMine* pMe = Cast<AWRCharacterMine>(GetOwnerCharacter());
// 	if (pMe)
// 	{
// 		UWRAnimInstance* MyAnimInstance = Cast<UWRAnimInstance>(pMe->Get3rdSkeletalMeshComponent()->GetAnimInstance());
// 		if (MyAnimInstance)
// 		{
// 			MyAnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAnimMontageEnded);
// 			MyAnimInstance->OnMontageBlendingOut.AddDynamic(this, &ThisClass::OnAnimMontageBlendingOut);
// 		}
// 	}
// }

void UWRActorComponentFSM::PauseAnimation(float InPauseTime, bool bSkipAnimPauseTime)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	UWRAnimInstance* AnimInstance = OwnerCharacter->GetAnimInstance();
	if (nullptr == AnimInstance)
		return;

	UWorld* pWorld = GetWorld();
	if (nullptr == pWorld)
		return;

	UAnimMontage* Montage = nullptr;
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance();
	if (MontageInstance)
	{
		Montage = MontageInstance->Montage;
		MontageInstance->Pause();
	}
	else
	{
		OwnerCharacter->GetMesh()->GlobalAnimRateScale = 0.0f;
	}

	FTimerDelegate TimeDele;
	TimeDele.BindUObject(this, &ThisClass::ResumeAnimation, Montage, InPauseTime, bSkipAnimPauseTime);
	pWorld->GetTimerManager().SetTimer(PauseAnimationTimerHandle, TimeDele, InPauseTime, false);

	
/*	// ���������� �Ẹ�ٰ� �ȵǸ� �̰� Ǯ��
	UAnimMontage* CurrMontage = AnimInstance->GetCurrentActiveMontage();
	if (CurrMontage)
	{
		AnimInstance->Montage_Pause(CurrMontage);
	}
	else
	{
		OwnerCharacter->GetMesh()->GlobalAnimRateScale = 0.0f;
	}

	FTimerDelegate TimeDele;
	TimeDele.BindUObject(this, &ThisClass::ResumeAnimation, CurrMontage, InPauseTime, bSkipAnimPauseTime);
	pWorld->GetTimerManager().SetTimer(PauseAnimationTimerHandle, TimeDele, InPauseTime, false);
*/
}

void UWRActorComponentFSM::ResumeAnimation(UAnimMontage* InMontage, float InPauseTime, bool bSkipAnim)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	UWRAnimInstance* AnimInstance = OwnerCharacter->GetAnimInstance();
	if (nullptr == AnimInstance)
		return;

	if (InMontage)
	{
		if (bSkipAnim)
		{
			//��ü ��Ÿ�ֿ��� ���� ��������� �ð� (�ִ� A + B �� �� ��Ÿ���϶�, B�������� ȣ��Ǹ� A�ִԽð� + �帥 B�ִԽð� �� ��)
			float CurrPosition = AnimInstance->Montage_GetPosition(InMontage); 
			int32 SectionIdx = InMontage->GetSectionIndexFromPosition(CurrPosition);
			float CurrSectionLength = InMontage->GetSectionLength(SectionIdx); //���� ������ 

			CurrPosition += InPauseTime;
			const int32 JumpedSectionIdx = InMontage->GetSectionIndexFromPosition(CurrPosition);
			if (JumpedSectionIdx == SectionIdx)	//++�� �ð��� ���� �����̸� �ش� �������� �ִϼ���
			{
				AnimInstance->Montage_SetPosition(InMontage, CurrPosition);
				AnimInstance->Montage_Resume(InMontage);
			}
			else //�ٸ� ������ ��� ��Ÿ�� ��ü�� ���� ��Ŵ
			{
				AnimInstance->Montage_Stop(0.1f, InMontage);
			}
		}
		else
		{
			AnimInstance->Montage_Resume(InMontage);
		}
	}
	else
	{
		if (OwnerCharacter->GetMesh())
		{
			OwnerCharacter->GetMesh()->GlobalAnimRateScale = 1.0f;
		}
	}
}

void UWRActorComponentFSM::SetSkipAnimation(bool bFlag)
{
	if (nullptr == CurrentFSM)
		return;

	bSkipAnimation = bFlag;

}

bool UWRActorComponentFSM::IsSkipAnimation()
{
	return bSkipAnimation;
}

bool UWRActorComponentFSM::IsPossibleEndCurrentFSM(EWRFSM InType, bool bImmediateUpdate /*= false*/, void* InVoidPtr /*=nullptr*/)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return false;

	if (nullptr == CurrentFSM)
		return true;

	if (bImmediateUpdate)
		return true;

	WRFSMInfo InReserveFSM;
	InReserveFSM.OwnerCharacter = OwnerCharacter;
	InReserveFSM.FSMType = InType;
	InReserveFSM.VoidPtr = InVoidPtr;

	return CurrentFSM->IsHighPriorityThanCurrentFSM(InReserveFSM);
}

EWRFSM UWRActorComponentFSM::GetFSMType()
{
	return GetCurrentFSM()->GetFSMType();
}

void UWRActorComponentFSM::OnAnimMontageEnded(UAnimMontage* rtnMontage, bool bInterrupted)
{

}

void UWRActorComponentFSM::OnAnimMontageBlendingOut(UAnimMontage* rtnMontage, bool bInterrupted)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	if (false == bInterrupted)
	{
		if (OwnerCharacter->IsPlayer())
		{
			if (GetFSMType() != EWRFSM::Move)
			{
				InvokeLinkedFSMWhenAnimationEnd();
			}
		}
		else
		{
			InvokeLinkedFSMWhenAnimationEnd();
		}

	}
}



void UWRActorComponentFSM::InvokeLinkedFSMWhenAnimationEnd()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	if (nullptr == CurrentFSM)
		return;

	if (SequenceFSMInfo.Num() != 0)
	{
		WRFSMInfo tempFSMInfo = SequenceFSMInfo[0];
		if (InvokeFSM(tempFSMInfo, true))
		{
			SequenceFSMInfo.Remove(tempFSMInfo);
		}
	}
	else
	{
		EWRFSM NextFSM = CurrentFSM->GetNextFSMWhenAnimationEnd(OwnerCharacter);

		InvokeFSM(NextFSM, true, true);
	}
}

void UWRActorComponentFSM::DetermineAIMoveTo(WRFSMInfo InFSMInfo)
{
	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Move:
	{
		//AI�̵��� �Ҷ� ai�̵��� ���� ȣ���ϰ� fsm�� ȣ���ϱ� ������
		//ai�̵��� ������� �ʵ��� �Ʒ��� ���� �ۼ�
		bool bClearAIMoveTo = true;
		if (InFSMInfo.VoidPtr != nullptr)
			bClearAIMoveTo = *((bool*)(InFSMInfo.VoidPtr));

		if (bClearAIMoveTo)
		{
			InFSMInfo.OwnerCharacter->ClearAIMoveTo();
		}
	}
	break;
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	case EWRFSM::MaintainingSkill:
	{
		if (InFSMInfo.OwnerCharacter->IsPlayer())
		{
			//�̵��ϸ鼭 ������ �� �� ���� �ൿ�̸� �ڵ��̵��� ��� ��Ų��
			WRFSMBase* BlendFSM = WRFSMManager::Get()->GetFSM(InFSMInfo.FSMType);
			if (BlendFSM)
			{
				if (BlendFSM->IsLayeredBlendFSM(InFSMInfo.OwnerCharacter) == false)
				{
					InFSMInfo.OwnerCharacter->ClearAIMoveTo();
				}
			}
		}
		else
		{
			InFSMInfo.OwnerCharacter->ClearAIMoveTo();
		}
	}
	break;
	default:
	{
		//�̵��ϸ鼭 ������ �� �� ���� �ൿ�̸� �ڵ��̵��� ��� ��Ų��
		WRFSMBase* BlendFSM = WRFSMManager::Get()->GetFSM(InFSMInfo.FSMType);
		if (BlendFSM)
		{
			if (BlendFSM->IsLayeredBlendFSM(InFSMInfo.OwnerCharacter) == false)
			{
				InFSMInfo.OwnerCharacter->ClearAIMoveTo();
			}
		}
	}
	break;
	}
}

bool UWRActorComponentFSM::InvokeFSM(EWRFSM eType, bool bImmediateUpdate /*= false*/, bool bAnimationEnded)
{
	return InvokeFSM(WRFSMInfo(eType, bImmediateUpdate), bAnimationEnded);
}

bool UWRActorComponentFSM::InvokeFSM(WRFSMInfo FSMInfo, bool bAnimationEnded)
{
	if (WRFSMManager::Get() == nullptr)
		return false;

	if (nullptr == CurrentFSM)
	{
		CurrentFSM = WRFSMManager::Get()->GetFSM(EWRFSM::Idle);
	}

	if (FSMInfo.FSMType == EWRFSM::None)
		return false;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return false;

	UWRAnimInstance* AnimInstance = OwnerCharacter->GetAnimInstance();
	if (nullptr == AnimInstance)
		return false;

	//FSMInfo�� �ɸ��� ����
	FSMInfo.OwnerCharacter = OwnerCharacter;

	if (IsPossibleEndCurrentFSM(FSMInfo.FSMType, FSMInfo.bImmediateUpdate) == false)
		return false;

	// ���� FSM ���� ó��
	FSMInfo.PreviousFSMType = CurrentFSM->GetFSMType();	//���� �����ؾ� �ϴ� fsm���� ���� fsm���¸� üũ�ؾ� �� ��찡 �����Ƿ�
	FSMInfo.PreviousSkillTableID = CurrentFSMInfo.SkillTableID;
	CurrentFSM->End(FSMInfo);

	//fsm�� �ٲ� ó���� �� ��͵� ui�����Ŷ����..
	FSMInfo.OwnerCharacter->DetermineTerminateSomethingWhenChangeFSM(FSMInfo);

	//����ü ���� ����
	AnimInstance->SetLayeredBlendPerBone(false);

#ifdef DEF_FIX_CLEARAIMOVETO
	//�ڵ��̵� ����ó��
	DetermineAIMoveTo(FSMInfo);
#endif // DEF_FIX_CLEARAIMOVETO

	if (bAnimationEnded)
	{
		//�ִϰ� �������ɶ�
		CurrentFSM->ProcessWhenAnimationEnd(OwnerCharacter);
	}
	else
	{
		//�ִϰ� ������ ���� ���¿��� FSM�� ����ɶ�
		CurrentFSM->ProcessWhenAnimationInterrupted(OwnerCharacter);
	}

	//���ο� FSM ����
	CurrentFSM = WRFSMManager::Get()->GetFSM(FSMInfo.FSMType);
	CurrentFSMInfo = FSMInfo;
	CurrentFSM->Begin(FSMInfo);

	//����ü �������� üũ
	AnimInstance->SetLayeredBlendPerBone(CurrentFSM->IsLayeredBlendFSM(OwnerCharacter));

	return true;
}

void UWRActorComponentFSM::RegistSequenceFSM(EWRFSM InType)
{
	RegistSequenceFSM(WRFSMInfo(InType, 0, 0, 0, false));
}

void UWRActorComponentFSM::RegistSequenceFSM(WRFSMInfo FSMInfo)
{
	SequenceFSMInfo.Add(FSMInfo);
}

void UWRActorComponentFSM::RegistSequenceFSM(const TArray<WRFSMInfo>& InFSMInfo, bool bImmediatelyInvokeFirstIndexFSM)
{
	if (InFSMInfo.Num() == 0)
		return;

	if (bImmediatelyInvokeFirstIndexFSM)
	{
		//reset pre sequence info
		EmptySequenceFSMInfo();

		//set new sequence fsm info
		SequenceFSMInfo = InFSMInfo;

		//�Է¹��� ���� ù��° ���� �õ�
		if (InvokeFSM(SequenceFSMInfo[0], false))
		{
			//������ �ش����� ����
			SequenceFSMInfo.Remove(InFSMInfo[0]);
		}
	}
	else
	{
		//���� sequence�� �߰�
		SequenceFSMInfo.Append(InFSMInfo);
	}
}

void UWRActorComponentFSM::EmptySequenceFSMInfo()
{
	SequenceFSMInfo.Empty();
}
