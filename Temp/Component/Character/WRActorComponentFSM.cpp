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

	//내 케릭터와 npc들은 fsmComp가 생성될때 바로 델리게이트 등록함
	//나 이외의 플레이어들은 AWRCharacter::OnAnimInitializedDele() 여기서 처리


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

	
/*	// 윗버전으로 써보다가 안되면 이것 풀것
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
			//전체 몽타주에서 현재 재생시점의 시간 (애님 A + B 로 된 몽타주일때, B지점에서 호출되면 A애님시간 + 흐른 B애님시간 이 됨)
			float CurrPosition = AnimInstance->Montage_GetPosition(InMontage); 
			int32 SectionIdx = InMontage->GetSectionIndexFromPosition(CurrPosition);
			float CurrSectionLength = InMontage->GetSectionLength(SectionIdx); //현재 섹션의 

			CurrPosition += InPauseTime;
			const int32 JumpedSectionIdx = InMontage->GetSectionIndexFromPosition(CurrPosition);
			if (JumpedSectionIdx == SectionIdx)	//++한 시간이 같은 섹션이면 해당 섹션으로 애니세팅
			{
				AnimInstance->Montage_SetPosition(InMontage, CurrPosition);
				AnimInstance->Montage_Resume(InMontage);
			}
			else //다른 섹션일 경우 몽타주 자체를 종료 시킴
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
		//AI이동을 할때 ai이동을 먼저 호출하고 fsm을 호출하기 때문에
		//ai이동을 취소하지 않도록 아래와 같이 작성
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
			//이동하면서 뭔가를 할 수 없는 행동이면 자동이동은 취소 시킨다
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
		//이동하면서 뭔가를 할 수 없는 행동이면 자동이동은 취소 시킨다
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

	//FSMInfo에 케릭터 세팅
	FSMInfo.OwnerCharacter = OwnerCharacter;

	if (IsPossibleEndCurrentFSM(FSMInfo.FSMType, FSMInfo.bImmediateUpdate) == false)
		return false;

	// 현재 FSM 종료 처리
	FSMInfo.PreviousFSMType = CurrentFSM->GetFSMType();	//현재 실행해야 하는 fsm에서 이전 fsm상태를 체크해야 할 경우가 잇으므로
	FSMInfo.PreviousSkillTableID = CurrentFSMInfo.SkillTableID;
	CurrentFSM->End(FSMInfo);

	//fsm이 바뀔때 처리할 그 어떤것들 ui같은거라던가..
	FSMInfo.OwnerCharacter->DetermineTerminateSomethingWhenChangeFSM(FSMInfo);

	//상하체 적용 종료
	AnimInstance->SetLayeredBlendPerBone(false);

#ifdef DEF_FIX_CLEARAIMOVETO
	//자동이동 종료처리
	DetermineAIMoveTo(FSMInfo);
#endif // DEF_FIX_CLEARAIMOVETO

	if (bAnimationEnded)
	{
		//애니가 재생종료될때
		CurrentFSM->ProcessWhenAnimationEnd(OwnerCharacter);
	}
	else
	{
		//애니가 끝나지 않은 상태에서 FSM이 종료될때
		CurrentFSM->ProcessWhenAnimationInterrupted(OwnerCharacter);
	}

	//새로운 FSM 시작
	CurrentFSM = WRFSMManager::Get()->GetFSM(FSMInfo.FSMType);
	CurrentFSMInfo = FSMInfo;
	CurrentFSM->Begin(FSMInfo);

	//상하체 적용할지 체크
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

		//입력받은 것중 첫번째 실행 시도
		if (InvokeFSM(SequenceFSMInfo[0], false))
		{
			//성공시 해당정보 삭제
			SequenceFSMInfo.Remove(InFSMInfo[0]);
		}
	}
	else
	{
		//기존 sequence에 추가
		SequenceFSMInfo.Append(InFSMInfo);
	}
}

void UWRActorComponentFSM::EmptySequenceFSMInfo()
{
	SequenceFSMInfo.Empty();
}
