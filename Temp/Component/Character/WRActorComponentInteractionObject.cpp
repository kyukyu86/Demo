// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentInteractionObject.h"

#include "UI/Panel/WRUIPanel_Main.h"
#include "UI/Panel/WRUIPanel_IOPopup.h"

#include "Actor/Character/WRCharacterIO.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterPlayer.h"

#include "Enum/EWRUIEnum.h"

#include "Components/AudioComponent.h"
#include "Component/Character/WRActorComponentVFX.h"
#include "WRActorComponentInteraction.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRNoticeManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRTriggerManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInventoryManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRObjectInteractionManager.h"

#include "Utility/WRActorUtility.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"

#include "Core/WRGameInstance.h"

#include "GameFramework/PlayerController.h"
#include "UnrealMathUtility.h"

#include "Network/Share/Define/Define_Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Manager/WRInteriorManager.h"




// For Cpp
DEFINE_LOG_CATEGORY(WRActorComponentInteraction)




bool FWRIODirection::InvokeAction(class AWRCharacter* IN InIO, class AWRCharacter* IN InTarget, const FVector IN InLocation /* = FVector::ZeroVector */, const bool IN InUnusable /* = false */)
{
	bActivate = true;
	pOwnerIO = InIO;
	pTarget = InTarget;

	bool bImmediateResult = OnAnimation();
	if (InUnusable == false)
	{
		OnFXs(InLocation);
		OnForceFeedback();
		OnHandsIK();
	}
	return bImmediateResult;
}

void FWRIODirection::ReleaseAction()
{
	bActivate = false;

	OnAnimation();
	OnFXs();
	OnForceFeedback();
	OnHandsIK();

	pOwnerIO = nullptr;
	pTarget = nullptr;
}

void FWRIODirection::UpdateDirection()
{
	if (bActivate == false)
		return;

	OnHandsIK();
}

bool FWRIODirection::OnAnimation()
{
	if (pTarget == nullptr)
		return true;

	// Use Montage
	if (MontageTID != INVALID_TABLE_TID)
	{
		if (bActivate)
		{
			if (pTarget->IsIO())
			{
				pTarget->PlayAnimationMontage(MontageTID, AnimSection);
			}
			else
			{
				WRFSMInfo fsmInfo(EWRFSM::Interact, 0, 0, 0, false, pOwnerIO, AnimSection, MontageTID);
				pTarget->DetermineFSMSignal(fsmInfo);
			}
		}
		else
		{
			if (pTarget->IsIO())
			{
//				pTarget->PlayAnimationMontage(MontageTID, AnimSection);
			}
			else
			{
				if (AnimationManualStop == false)
					pTarget->DetermineFSMSignal(EWRFSM::Idle);
			}
		}
	}

	// Use Anim Sequence
	if (AnimAsset.IsNull() == false)
	{
		if (bActivate)
		{			
//			pTarget->PlaySlotAnimationAsDynamicMontage(AnimAsset.LoadSynchronous(), TEXT("DefaultSlot"));
			pTarget->PlayAnimationSequence(AnimAsset.LoadSynchronous());
		}
		else
		{
			pTarget->StopAnimationSequence();
		}

		return false;
	}
	return true;
}

void FWRIODirection::OnFXs(const FVector IN InLocation /* = FVector::ZeroVector */)
{
	if (pTarget == nullptr)
		return;

	if (VFX.IsNull() == false)
	{
		if (bActivate)
		{
			WRAsyncHandle AsyncHandle = WRAsyncHandle::CreateLambda([=](AWRVFX* _vfx)
			{
				pFX = _vfx;
				if (pFX != nullptr)
				{
					if (InLocation.IsZero() == false)
						pFX->SetActorLocation(InLocation);
				}
			});
			AsyncVFXKey = WRVFXManager::Get()->AsyncSpawn(VFX.ToString(), pTarget->GetActorID(), pTarget->GetActorID(), pTarget->GetUniqueID(), EWRVFX::None, FRotator::ZeroRotator, AsyncHandle);
		}
		else
		{
			if (VFXManualStop)
			{
				if (pFX != nullptr && pFX->IsValidLowLevel())
				{
					WRVFXManager::Get()->Remove(pFX);
				}
				else if(AsyncVFXKey != WRASYNCLOADMANAGER_INVALID)
				{
					WRVFXManager::Get()->AsyncRemove(AsyncVFXKey);
				}
			}
		}
	}
	if (SFX.IsNull() == false)
	{
		if (bActivate)
		{
			pAudioComp = WRSFXManager::Get()->Spawn2D(SFX.LoadSynchronous());
		}
		else
		{
			if (SFXManualStop && pAudioComp != nullptr && pAudioComp->IsValidLowLevel())
			{
				WRSFXManager::Get()->Remove(pAudioComp);
			}
		}
	}
}

void FWRIODirection::OnForceFeedback()
{
	if (ForceFeedbackTID != INVALID_TABLE_TID)
	{
		if (bActivate)
		{
			if (ForceFeefbackBroadCast == false)
			{
				AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(pTarget);
				if (CastedPlayer == nullptr || CastedPlayer->IsMe() == false)
					return;
			}

			WRInputSystemManager::Get()->ClientPlayForceFeedback(ForceFeedbackTID);
		}
		else
		{
			if (ForceFeefbackBroadCast == false)
			{
				AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(pTarget);
				if (CastedPlayer == nullptr || CastedPlayer->IsMe() == false)
					return;
			}

			if (ForceFeefbackManualStop)
			{
				WRInputSystemManager::Get()->ClientStopForceFeedback(ForceFeedbackTID);
			}
		}
	}
}

void FWRIODirection::OnHandsIK()
{
	if (pOwnerIO == nullptr)
		return;

	if (pTarget == nullptr)
		return;

	if (pTarget->IsPlayer() == false)
		return;

	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(pTarget);
	if (CastedPlayer == nullptr)
		return;

	FTransform LHandTf = FTransform::Identity;
	FTransform RHandTf = FTransform::Identity;
	if (bActivate)
	{
		// pOwnerIO의 소켓TF를 pTarget에게로 넘김
		USkeletalMeshComponent* pSkeletalMeshComp = pOwnerIO->GetMesh();
		if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
		{
			if (LHandIK)
			{
				LHandTf = pSkeletalMeshComp->GetSocketTransform(FName("IK_LHand"));
			}
			if (RHandIK)
			{
				RHandTf = pSkeletalMeshComp->GetSocketTransform(FName("IK_RHand"));
			}
		}
		else
		{
			TArray<UActorComponent*> LHandTransformComponents = pOwnerIO->GetComponentsByTag(USceneComponent::StaticClass(), "LHand");
			if (LHandTransformComponents.IsValidIndex(0))
			{
				USceneComponent* pCastedComponent = Cast<USceneComponent>(LHandTransformComponents[0]);
				LHandTf = pCastedComponent->GetComponentTransform();
			}
			TArray<UActorComponent*> RHandTransformComponents = pOwnerIO->GetComponentsByTag(USceneComponent::StaticClass(), "RHand");
			if (RHandTransformComponents.IsValidIndex(0))
			{
				USceneComponent* pCastedComponent = Cast<USceneComponent>(RHandTransformComponents[0]);
				RHandTf = pCastedComponent->GetComponentTransform();
			}
		}
		CastedPlayer->SetTransformForFABRIK(RHandTf, LHandTf);
	}
	else
	{
		CastedPlayer->SetTransformForFABRIK();
	}
}

//====================================================================================
//
//====================================================================================

AWRCharacterIO* UWRActorComponentInteractionObject::GetOwnerObject()
{
	if (GetOwnerCharacter() == nullptr)
		return nullptr;

	AWRCharacterIO* CastedChar = Cast<AWRCharacterIO>(GetOwnerCharacter());
	return CastedChar;
}

void UWRActorComponentInteractionObject::OnCreateComponent()
{
	Super::OnCreateComponent();

	USkeletalMeshComponent* pSkeletalMeshComp = GetOwnerCharacter()->GetMesh();
	if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
	{
		SrcRotate = FRotator(pSkeletalMeshComp->GetRelativeTransform().GetRotation());
	}
	else
	{
		SrcRotate = GetOwnerCharacter()->GetActorRotation();
	}

	// [ 2019-11-21 : kyu ] 일단 전부 통일. 창이 뜨면 UpdateTargetUI에 의해서 자동 셋팅
	TargetUIKind = EWRUIPanelKind::IOPopup;

}

void UWRActorComponentInteractionObject::OnDestroyComponent()
{
	Super::OnDestroyComponent();
}

void UWRActorComponentInteractionObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerCharacter() == nullptr)
		return;

	// 인지거리 업데이트
	UpdateRecognitionArea();

	// 상호작용 최소거리 업데이트
	UpdateInteractionMinDist();

	// 갈고리 사용가능 거리 업데이트
	UpdateHookArea();

	// 버튼 입력 지연시간 체크
	UpdatePressedTime(DeltaTime);

	// 연출 업데이트
	UpdateDirection();

	// 자이로 업데이트
	UpdateGyroTransform(DeltaTime);
}

void UWRActorComponentInteractionObject::SetInteractionTable(FWRInteractionObjectTable* IN InObjectTable)
{
	if (InObjectTable == nullptr)
		return;

	// [ 2019-10-22 : kyu ] *** 나중에 이걸 데이터 구조체로 저장하고 있게 할지...Owner 타입에 따라 다른 포인터를 쓸지..

	ObjectTable = InObjectTable;
}

bool UWRActorComponentInteractionObject::IsCheatedObject()
{
	return GetOwnerObject()->IsCheatedObject();
}

void UWRActorComponentInteractionObject::AfterProcess_SpawnCompleted()
{
	if (eState == EWRInteractionState::Unusable)
	{
		if (ObjectTable->DirectType == EWRIODirectType::KeyInput)
		{
			bool bImmediateResult = InvokeDirectionGroup(EWRInteractionGroup::DirectInteraction);
			if (bImmediateResult)
			{
				ReleaseInteractionInfo(); // 연출없이 즉시발동의 경우 인터렉션 데이터를 해제한다.
			}
			if (ObjectTable->DirectResult == EWRIOResultAction::Trigger)
			{
				WRTriggerManager::Get()->RunActiveTrigger(ObjectTable->DirectResultID);
			}
		}

		if (ObjectTable->HookType == EWRHookType::Activate)
		{
			bool bImmediateResult = InvokeDirectionGroup(EWRInteractionGroup::HookInteraction);
			if (bImmediateResult)
			{
				ReleaseInteractionInfo(); // 연출없이 즉시발동의 경우 인터렉션 데이터를 해제한다.
			}
			if (ObjectTable->HookResult == EWRIOResultAction::Trigger)
			{
				WRTriggerManager::Get()->RunActiveTrigger(ObjectTable->HookResultID);
			}
		}			

		UpdateTargetUI();
		AfterProcess_GyroResult();
		AfterProcess_Attached();
	}
}

//====================================================================================
//	기타
//====================================================================================

void UWRActorComponentInteractionObject::OnInteractionResult()
{
	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		DirectionLinkFunction(ObjectTable->DirectResult, ObjectTable->DirectResultID);

		// 행위 후 잔류여부
		if (ObjectTable->DirectResultAfter == EWRIOResultAfter::Retain)
		{
			
		}
		else if (ObjectTable->DirectResultAfter == EWRIOResultAfter::Delete
			|| ObjectTable->DirectResultAfter == EWRIOResultAfter::RetainHide)
		{
			WRUIManager::Get()->CloseAllUIFromCharacter(GetOwnerCharacter());
		}
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		DirectionLinkFunction(ObjectTable->HookResult, ObjectTable->HookResultID);

		// 행위 후 잔류여부
		if (ObjectTable->HookResultAfter == EWRIOResultAfter::Retain)
		{
			
		}
		else if (ObjectTable->HookResultAfter == EWRIOResultAfter::Delete
			|| ObjectTable->HookResultAfter == EWRIOResultAfter::RetainHide)
		{
			WRUIManager::Get()->CloseAllUIFromCharacter(GetOwnerCharacter());
		}
	}
	break;
	}

	UpdateTargetUI();
	AfterProcess_GyroResult();
	AfterProcess_Attached();

	bInteracting = false; // 결과 후 인터렉싱 해제
}

void UWRActorComponentInteractionObject::ReleaseInteractionInfo()
{	
	OnGyroEnd();			// 자이로 모드가 켜져있으면 끔
	ReleaseProgressing();	// 진행 연출있으면 끔

	bInteracting = false;
	bInteractionEnding = false;
	CurrentInteractionType = EWRIOInteractionType::None;
	InteractionPlayer = nullptr;

	UpdateTargetUI();		// 상호작용 가능 상태에 따라 출력중인 UI의 활성상태 변경해야한다.
}

void UWRActorComponentInteractionObject::AfterProcess_GyroResult()
{
	if (GyroSteps == nullptr || GyroSteps->Num() == 0)
		return;

	for (int32 nStep = 0; GyroSteps->Num(); ++nStep)
	{
		FWRIOGyro* pGyroStep = GetGyroStep(nStep);
		if (pGyroStep == nullptr)
			return;

		if (pGyroStep->GyroType != EWRIOGyroAction::Angle)
			continue;

		if (pGyroStep->Axis == EWRIOGyroAxis::X)
		{
			// Z to Yaw
			FRotator NewRot = SrcRotate + FRotator(0, 0, pGyroStep->GoalDegree);
			USkeletalMeshComponent* pSkeletalMeshComp = GetOwnerCharacter()->GetMesh();
			if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
			{
				pSkeletalMeshComp->SetRelativeRotation(NewRot);
			}
			else
			{
				GetOwnerCharacter()->SetRotation(NewRot);
			}
		}
		else if (pGyroStep->Axis == EWRIOGyroAxis::Y)
		{
			// Y to Pitch
			FRotator NewRot = SrcRotate + FRotator(pGyroStep->GoalDegree, 0, 0);
			USkeletalMeshComponent* pSkeletalMeshComp = GetOwnerCharacter()->GetMesh();
			if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
			{
				pSkeletalMeshComp->SetRelativeRotation(NewRot);
			}
			else
			{
				GetOwnerCharacter()->SetRotation(NewRot);
			}
		}
		else if (pGyroStep->Axis == EWRIOGyroAxis::Z)
		{
			// X to Roll
			FRotator NewRot = SrcRotate + FRotator(0, pGyroStep->GoalDegree, 0);
			USkeletalMeshComponent* pSkeletalMeshComp = GetOwnerCharacter()->GetMesh();
			if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
			{
				pSkeletalMeshComp->SetRelativeRotation(NewRot);
			}
			else
			{
				GetOwnerCharacter()->SetRotation(NewRot);
			}
		}
	}
}

void UWRActorComponentInteractionObject::AfterProcess_Attached()
{
	AWRCharacterIO* pObject = GetOwnerObject();
	if (pObject == nullptr)
		return;

	if (pObject->GetResultAttachTargetID() == INVALID_TID)
		return;

	AWRCharacterIO* pResultAttachTarget = WRCharacterManager::Get()->FindIOByResultAttachID(pObject->GetResultAttachTargetID());
	if (pResultAttachTarget == nullptr)
		return;

	USkeletalMeshComponent* pObjectSkeletalMeshComp = pObject->GetMesh();
	if (pObjectSkeletalMeshComp != nullptr && pObjectSkeletalMeshComp->SkeletalMesh != nullptr)
	{
		FVector vRelativeLocation = pObjectSkeletalMeshComp->GetRelativeTransform().GetLocation();
		pObjectSkeletalMeshComp->SetRelativeLocation(FVector::ZeroVector);
	}

	USkeletalMeshComponent* pTargetSkeletalMeshComp = pResultAttachTarget->GetMesh();
	if (pTargetSkeletalMeshComp != nullptr && pTargetSkeletalMeshComp->SkeletalMesh != nullptr)
	{
		pObject->AttachToComponent(pTargetSkeletalMeshComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false), pObject->GetResultAttachSocketName());
	}
}

//====================================================================================
//	:: Retain Count
//====================================================================================

bool UWRActorComponentInteractionObject::IsInteractionableCount(const EWRIOInteractionType IN InType)
{
	switch (InType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (ObjectTable->DirectType == EWRIODirectType::None)
			return false;

		if (ObjectTable->DirectRetainCount == 0)
			return true; // Count가 0이면 무제한

		if (ObjectTable->DirectRetainCount == DirectRetainCount)
			return false;
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		if (ObjectTable->HookType == EWRHookType::None)
			return false;

		if (ObjectTable->HookRetainCount == 0)
			return true; // Count가 0이면 무제한

		if (ObjectTable->HookRetainCount == HookRetainCount)
			return false;
	}
	break;
	}
	return true;
}

void UWRActorComponentInteractionObject::UpdateRetainCount(EWRIOInteractionType IN InType, const uint16 IN InCount)
{
	bool bEndCount = false;
	if (InType == EWRIOInteractionType::DirectInteraction)
	{
		DirectRetainCount = InCount;
		if (ObjectTable->DirectRetainCount != 0 && DirectRetainCount >= ObjectTable->DirectRetainCount)
		{
			bEndCount = true;
		}
	}
	else if (InType == EWRIOInteractionType::HookInteraction)
	{
		HookRetainCount = InCount;
		if (ObjectTable->HookRetainCount != 0 && HookRetainCount >= ObjectTable->HookRetainCount)
		{
			bEndCount = true;
		}
	}

	if (bEndCount)
	{
		if (ObjectTable->DirectResultAfter == EWRIOResultAfter::Delete)
		{
			eState = EWRInteractionState::Unusable;
		}
		else if (ObjectTable->DirectResultAfter == EWRIOResultAfter::RetainHide)
		{
			eState = EWRInteractionState::Unusable;
			GetOwnerCharacter()->SetActorHiddenInGame(true);
		}

		if (ObjectTable->HookResultAfter == EWRIOResultAfter::Delete)
		{
			eState = EWRInteractionState::Unusable;
		}
		else if (ObjectTable->HookResultAfter == EWRIOResultAfter::RetainHide)
		{
			eState = EWRInteractionState::Unusable;
			GetOwnerCharacter()->SetActorHiddenInGame(true);
		}
	}
}

//====================================================================================
//	:: Direction
//====================================================================================


void UWRActorComponentInteractionObject::OnInteractionGroup(const EWRInteractionGroup IN InType, const bool IN InOn /*= true*/, FVector IN InLocation /*= FVector::ZeroVector*/)
{
	switch (InType)
	{
	case EWRInteractionGroup::Creation:
	{
		InvokeDirectionGroup(InType);
	}
	break;
	case EWRInteractionGroup::DirectProgress:
	{
		if (InOn)
		{
			bPressedInput = true;
			fPressedTime = 0.0f;

			InvokeDirectionGroup(InType);
		}
		else
		{
			ReleaseDirectionGroup(InType);
		}
	}
	break;
	case EWRInteractionGroup::DirectInteraction:
	case EWRInteractionGroup::HookInteraction:
	{
		if (InOn)
		{
			// [ 2019-11-30 : kyu ] 인터렉션 시작해서 연출종료까지 타겟창 숨김
			bInteractionEnding = true;
			
			bool bImmediateResult = InvokeDirectionGroup(InType);
			if (bImmediateResult)
			{
				OnAnimationEnded();		// ... to InteractionFinish
			}
			else
			{
				// 연출이 있으면 연출동안 UI를 끈다. 타겟도 불가.
				WRUIManager::Get()->CloseAllUIFromCharacter(GetOwnerCharacter());
			}
		}
		else
		{

		}
	}
	break;
	case EWRInteractionGroup::DirectInteractionFinish:
	case EWRInteractionGroup::HookInteractionFinish:
	{
		// [ 2020-1-7 : kyu ] 연출시작과 동시에 Result 발동. Trigger는 지연시간으로 미룬다.
		OnInteractionResult();

		InvokeDirectionGroup(InType);

		if (bInteractionEnding)
		{
			// 인터렉션 모두 해제
			ReleaseInteractionInfo();
		}
	}
	break;
	case EWRInteractionGroup::Gyro:
	{
		if (InOn)
		{
			if (bGyroMode)
				return;

			bGyroMode = true;

			InvokeDirectionGroup(InType);
			// + 타겟UI 끔
			ShowOffTargetUI();

			if (InteractionPlayer != nullptr && InteractionPlayer->IsMe())
			{
				// + 자이로 UI 띄움
				ShowOnGyroInteraction();
				WRInputSystemManager::Get()->SetOnlyMoveInput(true);
			}
		}
		else
		{
			if (bGyroMode == false)
				return;

			bGyroMode = false;

			ReleaseGyroData();
			ReleaseDirectionGroup(InType);

			if (InteractionPlayer != nullptr && InteractionPlayer->IsMe())
			{
				// + 자이로 UI 닫음
				ShowOffGyroInteraction();
				WRInputSystemManager::Get()->SetOnlyMoveInput(false);
			}
		}
	}
	break;
	case EWRInteractionGroup::HookArrival:
	{
		InvokeDirectionGroup(InType, InLocation);

		// 갈고리 도달시 UI 업데이트
		UpdateTargetUI();
	}
	break;
	}
}

bool UWRActorComponentInteractionObject::InvokeDirectionGroup(EWRInteractionGroup IN InGroupType, FVector IN InLocation /* = FVector::ZeroVector */)
{
	// Release Prev Direction
	if (eInteractionGroup != EWRInteractionGroup::None)
	{
		ReleaseDirectionGroup(eInteractionGroup);
		eInteractionGroup = EWRInteractionGroup::None;
	}

	auto InvokeDirectionLambda = [&](EWRIODirectionType IN InType, AWRCharacter* IN InTarget = nullptr, FVector IN InLocation = FVector::ZeroVector)
	{
		FWRIODirection* Direction = GetDirection(InType);
		if (Direction != nullptr)
		{
			return Direction->InvokeAction(GetOwnerCharacter(), InTarget, InLocation);
		}
		return true;
	};

	eInteractionGroup = InGroupType;

	// Invoke New Direction
	switch (InGroupType)
	{
	case EWRInteractionGroup::Creation:
	{
		return InvokeDirectionLambda(EWRIODirectionType::Creation);
	}
	break;
	case EWRInteractionGroup::Deletion:
	{
		return InvokeDirectionLambda(EWRIODirectionType::Deletion);
	}
	break;	
	case EWRInteractionGroup::DirectProgress:
	{
		InvokeDirectionLambda(EWRIODirectionType::CharacterProgress, InteractionPlayer);
		return InvokeDirectionLambda(EWRIODirectionType::ObjectProgress, GetOwnerCharacter());
	}
	break;
	case EWRInteractionGroup::DirectInteraction:
	{
		InvokeDirectionLambda(EWRIODirectionType::CharacterInteraction, InteractionPlayer);
		return InvokeDirectionLambda(EWRIODirectionType::ObjectInteraction, GetOwnerCharacter());
	}
	break;
	case EWRInteractionGroup::DirectInteractionFinish:
	{
// 		InvokeDirectionLambda(EWRIODirectionType::CharacterInteraction, InteractionPlayer);
// 		return InvokeDirectionLambda(EWRIODirectionType::ObjectInteraction, GetOwnerCharacter());
	}
	break;
	case EWRInteractionGroup::Gyro:
	{
		InvokeDirectionLambda(EWRIODirectionType::CharacterGyroStart, InteractionPlayer);
		return InvokeDirectionLambda(EWRIODirectionType::ObjecGyroStart, GetOwnerCharacter());
	}
	break;
	case EWRInteractionGroup::HookInteraction:
	{
		return InvokeDirectionLambda(EWRIODirectionType::HookInteraction, GetOwnerCharacter());
	}
	break;
	case EWRInteractionGroup::HookInteractionFinish:
	{
//		return InvokeDirectionLambda(EWRIODirectionType::HookInteraction, GetOwnerCharacter());
	}
	break;
	case EWRInteractionGroup::HookArrival:
	{
		return InvokeDirectionLambda(EWRIODirectionType::HookArrival, GetOwnerCharacter());
	}
	break;	
	}

	return false;
}

void UWRActorComponentInteractionObject::ReleaseDirectionGroup(EWRInteractionGroup IN InGroupType, FVector IN InLocation /* = FVector::ZeroVector */)
{
	auto ReleaseDirectionLambda = [&](EWRIODirectionType IN InType, AWRCharacter* IN InTarget = nullptr, FVector IN InLocation = FVector::ZeroVector)
	{
		FWRIODirection* Direction = GetDirection(InType);
		if (Direction != nullptr)
		{
			Direction->ReleaseAction();
		}
	};

	switch (InGroupType)
	{
	case EWRInteractionGroup::Creation:
	{
		ReleaseDirectionLambda(EWRIODirectionType::Creation);
	}
	break;
	case EWRInteractionGroup::Deletion:
	{
		ReleaseDirectionLambda(EWRIODirectionType::Deletion);
	}
	break;
	case EWRInteractionGroup::DirectProgress:
	{
		ReleaseDirectionLambda(EWRIODirectionType::CharacterProgress);
		ReleaseDirectionLambda(EWRIODirectionType::ObjectProgress);
	}
	break;
	case EWRInteractionGroup::DirectInteraction:
	{
		ReleaseDirectionLambda(EWRIODirectionType::CharacterInteraction);
		ReleaseDirectionLambda(EWRIODirectionType::ObjectInteraction);
	}
	break;
	case EWRInteractionGroup::DirectInteractionFinish:
	{
		ReleaseDirectionLambda(EWRIODirectionType::CharacterInteraction);
		ReleaseDirectionLambda(EWRIODirectionType::ObjectInteraction);
	}
	break;
	case EWRInteractionGroup::Gyro:
	{
		ReleaseDirectionLambda(EWRIODirectionType::CharacterGyroStart);
		ReleaseDirectionLambda(EWRIODirectionType::ObjecGyroStart);
	}
	break;
	case EWRInteractionGroup::HookInteraction:
	{
		ReleaseDirectionLambda(EWRIODirectionType::HookInteraction);
	}
	break;
	case EWRInteractionGroup::HookInteractionFinish:
	{
		ReleaseDirectionLambda(EWRIODirectionType::HookInteraction);
	}
	break;
	case EWRInteractionGroup::HookArrival:
	{
		ReleaseDirectionLambda(EWRIODirectionType::HookArrival);
	}
	break;
	}
}

void UWRActorComponentInteractionObject::OnAnimationEnded()
{
	// AWRCharacterIO::OnAnimEnded 에서 호출됨

	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		OnInteractionGroup(EWRInteractionGroup::DirectInteractionFinish);
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		OnInteractionGroup(EWRInteractionGroup::HookInteractionFinish);
	}
	break;
	}
}

void UWRActorComponentInteractionObject::DirectionLinkFunction(EWRIOResultAction IN InAction, int32 IN InID)
{
	if (InAction == EWRIOResultAction::Trigger)
	{
		// [ 2019-12-4 : kyu ] 치트로 만들어진 오브젝트가 아닐때만 트리거 발동
		if (IsCheatedObject() == false)
		{
			WRTriggerManager::Get()->RunActiveTrigger(InID);
		}
	}
	else if (InAction == EWRIOResultAction::SingleTrigger)
	{
		if (IsCheatedObject() == false)
		{
			if (InteractionPlayer != nullptr && InteractionPlayer->IsMe())
			{
				WRTriggerManager::Get()->RunActiveTrigger(InID);
			}
		}
	}
}

void UWRActorComponentInteractionObject::SaveDirections(TMap<EWRIODirectionType, FWRIODirection>* IN InDirections)
{
	if (InDirections == nullptr || InDirections->Num() == 0)
	{
		Directions = nullptr;
		return;
	}

	Directions = InDirections;
}

FWRIODirection* UWRActorComponentInteractionObject::GetDirection(EWRIODirectionType IN InType)
{
	if ((*Directions).Contains(InType))
	{
		return &(*Directions)[InType];
	}
	return nullptr;
}

void UWRActorComponentInteractionObject::UpdateDirection()
{
	if (Directions == nullptr)
		return;

	for (auto& EachDirection : (*Directions))
	{
		FWRIODirection& DirectionInfo = EachDirection.Value;
		if (DirectionInfo.bActivate == false)
			continue;

		DirectionInfo.UpdateDirection();
	}
}

//====================================================================================
//	:: Occupation
//====================================================================================

void UWRActorComponentInteractionObject::UpdateToOccupationSync(class AWRCharacterPlayer* IN InPlayer)
{
	InteractionPlayer = InPlayer;
	bInteracting = true;
	CurrentInteractionType = EWRIOInteractionType::DirectInteraction;

	// [ 2020-4-21 : kyu ] 점유 설정 - 내부에서 AttachToComponent로 위치조정까지 한다
	UWRActorComponentInteraction* pInteractionComp = InteractionPlayer->GetComponent<UWRActorComponentInteraction>();
	if (pInteractionComp != nullptr)
	{
		pInteractionComp->SetOccupiedIO(GetOwnerObject());
	}

	// 상호작용 위치 강제 조정
// 	FTransform tf = GetInteractionTransform();
// 	if (tf.Equals(FTransform::Identity) == false)
// 	{
// 		FNavLocation OutNavLocation;
// 		bool bResult = WRActorUtility::ProjectPointToNavigation(InPlayer, tf.GetLocation(), OutNavLocation);
// 		if (bResult)
// 		{
// 			float OutHeight = 0.0f;
// 			if (WRActorUtility::ModifyZHeightForClient(InPlayer, OutNavLocation.Location, OutHeight))
// 			{
// 				OutNavLocation.Location.Z = OutHeight;
// 			}
// 			tf.SetLocation(OutNavLocation.Location);
// 		}
// 		InPlayer->SetActorTransform(tf);
// 	}

	// 상호작용 결과의 연출 시도 (일단 의자에 대해서만 테스트)
	OnInteractionGroup(EWRInteractionGroup::DirectInteraction);
}

//====================================================================================
//	:: 인지 거리
//====================================================================================

bool UWRActorComponentInteractionObject::IsRecognizableObject()
{
	// Q. 인지 할 수 있는 오브젝트인가?

	if (GetInteractionState() == EWRInteractionState::Unusable)
		return false; // 상호작용 불가 상태

	if (bInteractionEnding)
		return false; // 상호작용 종료중이면 불가

 	if (ObjectTable == nullptr)
 		return false; // 데이터 없음
 
 	if (ObjectTable->MainType != EWRIOMainType::Interaction)
 		return false; // MainType이 Interaction이 아니면 인지 불가
 
 	if (ObjectTable->DirectType == EWRIODirectType::None
 		&& ObjectTable->HookType == EWRHookType::None)
 		return false; // 직접, 갈고리 상호작용 선택지 하나도 없는 경우
 
 	if (IsInteractionableCount(EWRIOInteractionType::DirectInteraction) == false
 		&& IsInteractionableCount(EWRIOInteractionType::HookInteraction) == false)
 		return false; // 액션 가능한 상태가 하나도 없음

	if (WRConsoleManager::Get()->GetBool(EWRConsole::IO_IgnoreQuest) == false 
		&& IsCheatedObject() == false 
		&& ObjectTable->DirectConditionQuestProgress != 0)
	{
		// + Quest TID 연동 여부
		if (WRQuestManager::Get()->IsProgressQuest(ObjectTable->DirectConditionQuestProgress) == false)
			return false;
	}

	if (GetOwnerObject()->GetOccupiedPlayer() != nullptr)
		return false; // 점유중이면 미출력

	return true;
}

bool UWRActorComponentInteractionObject::IsRecognizableState()
{
	if (IsRecognizableObject() == false)
		return false; // 인지 할 수 없는 오브젝트

	if (bRecognitionArea == false)
		return false; // 인지 상태인가?

	return true;
}

void UWRActorComponentInteractionObject::UpdateRecognitionArea()
{
	if (WRCharacterManager::IsValid() == false)
		return;

	AWRCharacterMine* pMyPlayer = WRCharacterManager::Get()->GetMyCharacter();
	if (pMyPlayer == nullptr)
		return;

	if (IsRecognizableObject() == false)
	{
		RecognitionAreaOUT();
		return; // 인지 할 수 없는 오브젝트
	}

	if (pMyPlayer->IsRecognizableRange(GetOwnerCharacter()))
	{
		RecognitionAreaIN();	// 인지 거리 안
	}
	else
	{
		RecognitionAreaOUT();	// 인지 거리 밖
	}
}

void UWRActorComponentInteractionObject::RecognitionAreaIN()
{
	if (bRecognitionArea == false)
	{
		bRecognitionArea = true;

		// [ 2019-12-2 : kyu ] 상호작용 영역안에서 퀘스트를 늦게 받아 타겟UI가 업데이트 안되는 현상때문에 bInteractionableArea 상태 강제 업데이트
		if (bInteractionableArea == false)
		{
			UpdateInteractionableAreaOverappingState();
		}

		// 인지범위 밖에서 타겟팅 중에 들어왔을 때 업데이트
		UpdateTargetUI();
		UpdateHighLight();
	}	
}

void UWRActorComponentInteractionObject::RecognitionAreaOUT()
{
	if (bRecognitionArea == true)
	{
		bRecognitionArea = false;

		bInteractionableArea = false;
		bHookableArea = false;

		// 타겟팅 중 인지 범위 벗어났을 때 끄기위해 업데이트
		UpdateTargetUI();
		UpdateHighLight();
	}
}

//====================================================================================
//	:: 상호작용 거리
//====================================================================================

FTransform UWRActorComponentInteractionObject::GetInteractionTransform()
{
	FTransform WCTransform = GetOwnerCharacter()->GetInteractionTransform();

	return WCTransform;
}

// Q. 상호작용 할 수 있는 오브젝트인가?
bool UWRActorComponentInteractionObject::IsInteractionableObject()
{
	if (IsRecognizableState() == false)
		return false; // 인지 할 수 없는 [상태]의 오브젝트
	
	return true;
}

// Q. 상호작용할 수 있는 상태인가?
bool UWRActorComponentInteractionObject::IsInteractionableState()
{
	if (IsInteractionableObject() == false)
		return false; // 상호작용할 수 없는 오브젝트	

	if (bInteractionableArea == false)
		return false; // 상호작용 가능한가?

	if (GetOwnerObject()->IsDirectOccupiedState())
		return false; // [ 2020-3-5 : kyu ] 점유중이면 패스

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return false;

	float fInteractionMinDist = GetOwnerObject()->GetInteractionMinDist();
	if (fInteractionMinDist != 0.f)
	{
		float fDist = FVector::Dist(pMine->GetActorLocation(), GetOwnerCharacter()->GetActorLocation());
		if (fDist < fInteractionMinDist)
			return false;	// [ 2020-4-23 : kyu ] 인터렉션 최소거리 체크		
	}

	if (pMine->GetConnectedHookCableComponent() != nullptr)
		return false;	// [ 2020-5-7 : kyu ] 갈고리가 연결되어있으면 사다리 사용 불가	

	return true;
}

void UWRActorComponentInteractionObject::OnBeginOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsInteractionableObject() == false)
		return;

	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(Other);
	if (CastedPlayer == nullptr)
		return;

	InteractionAreaIN(CastedPlayer);
}

void UWRActorComponentInteractionObject::OnEndOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsInteractionableObject() == false)
		return;

	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(Other);
	if (CastedPlayer == nullptr)
		return;

	InteractionAreaOUT(CastedPlayer);
}

void UWRActorComponentInteractionObject::InteractionAreaIN(class AWRCharacterPlayer* IN InPlayer)
{
	// 나에 대해서만 처리
	if (InPlayer->IsMe() == false)
		return;

	if (bInteractionableArea == false)
	{
		bInteractionableArea = true;

		// 상호작용 가능 상태에 따라 출력중인 UI의 활성상태 변경해야한다.
		UpdateTargetUI();
	}
}

void UWRActorComponentInteractionObject::InteractionAreaOUT(class AWRCharacterPlayer* IN InPlayer)
{
	// 나에 대해서만 처리
	if (InPlayer->IsMe() == false)
		return;

	// [ 2020-4-21 : kyu ] 점거중인 대상은 Collision이 꺼져서 OUT 처리가 되므로 여기선 예외처리한다.
	if (GetOwnerObject()->GetOccupiedPlayer() == InPlayer)
		return;

	if (bInteractionableArea == true)
	{
		bInteractionableArea = false;

		// 인터렉션 중이고 인터렉션플레이어가 나면 중단요청
		if (bInteracting && InteractionPlayer->IsMe())
		{
			OnReq_InteractionStop();
		}

		// 상호작용 가능 상태에 따라 출력중인 UI의 활성상태 변경해야한다.
		UpdateTargetUI();
	}
}

void UWRActorComponentInteractionObject::UpdateInteractionableAreaOverappingState()
{
	if (GetOwnerObject() == nullptr)
		return;

	if (IsInteractionableObject() == false)
		return;
	
	bInteractionableArea = GetOwnerObject()->IsOverlappingInteractionableArea();
}

void UWRActorComponentInteractionObject::UpdateInteractionMinDist()
{
	if (IsInteractionableObject() == false)
		return; // 상호작용할 수 없는 오브젝트	

	if (bInteractionableArea == false)
		return; // 상호작용 가능한가?

	if (GetOwnerObject()->IsDirectOccupiedState())
		return; // [ 2020-3-5 : kyu ] 점유중이면 패스

	float fInteractionMinDist = GetOwnerObject()->GetInteractionMinDist();
	if (fInteractionMinDist != 0.f)
	{
		UpdateTargetUI();
	}
}

//====================================================================================
//	:: 갈고리 사용가능 영역
//====================================================================================

// Q. 갈고리 걸 수 있는 오브젝트인가?
bool UWRActorComponentInteractionObject::IsHookableObject()
{
	if (IsRecognizableState() == false)
		return false; // 인지 할 수 없는 [상태]의 오브젝트

	return true;
}

// Q. 갈고리 걸 수 있는 상태인가?
bool UWRActorComponentInteractionObject::IsHookableState()
{
	if (WRCharacterManager::IsValid() == false)
		return false;

	if (WRCharacterManager::Get()->IsEquippedHook() == false)
		return false; // 갈고리 장비 착용하지 않은 경우

	if (WRCharacterManager::Get()->IsHookableRange(GetOwnerCharacter()) == false)
		return false; // 거리체크

	if (IsHookableObject() == false)
		return false; // 갈고리 걸 수 없는 오브젝트

	if (bHookableArea == false)
		return false; // 갈고리 가능한가?

	return true;
}


void UWRActorComponentInteractionObject::OnHookArrival(const EWRConnectCableEvent IN InEvent, const FVector IN InImpactLocation, AWRCharacter* IN InHookCaster)
{
	// 갈고리 박힘 연출 발생 !

	OnInteractionGroup(EWRInteractionGroup::HookArrival, true, InImpactLocation);
}

void UWRActorComponentInteractionObject::OnHookCutted()
{
	UpdateTargetUI();

}

void UWRActorComponentInteractionObject::OnHookEventToTarget(class AWRCharacter* IN InHookCaster)
{
	// 갈고리 당겨짐 연출 발생 !

	OnReq_InteractionStart(EWRIOInteractionType::HookInteraction);
}

void UWRActorComponentInteractionObject::UpdateHookArea()
{
	if (WRCharacterManager::IsValid() == false)
		return;

	AWRCharacterMine* pMyPlayer = WRCharacterManager::Get()->GetMyCharacter();
	if (pMyPlayer == nullptr)
		return;

	if (IsHookableObject() == false)
	{
		HookAreaOUT();
		return; // 갈고리 걸 수 없는 오브젝트
	}

	if (pMyPlayer->IsHookableRange(GetOwnerCharacter()))
	{
		HookAreaIN();
	}
	else
	{
		HookAreaOUT();
	}
}

void UWRActorComponentInteractionObject::HookAreaIN()
{
	if (bHookableArea == false)
	{
		bHookableArea = true;

		// 갈고리 가능 상태에 따라 출력중인 UI의 활성상태 변경해야한다.
		UpdateTargetUI();
	}
}

void UWRActorComponentInteractionObject::HookAreaOUT()
{
	if (bHookableArea == true)
	{
		bHookableArea = false;

		// 갈고리 가능 상태에 따라 출력중인 UI의 활성상태 변경해야한다.
		UpdateTargetUI();
	}
}

//====================================================================================
//	타겟팅
//====================================================================================


bool UWRActorComponentInteractionObject::IsTargetableObject()
{
	if (IsRecognizableState() == false)
		return false; // 인지 불가능 상태

	return true;
}

void UWRActorComponentInteractionObject::OnTargeted(const bool IN InMulti /*= false*/, const bool IN InByMe /*= true*/)
{
	if (IsTargetableObject() == false)
		return;

	// 자이로중에는 타겟팅 띄우지 않는다.
	if (bGyroMode)
		return;

	ShowOnTargetUI();
}

void UWRActorComponentInteractionObject::ReleasedTarget(const bool IN InMulti /* = false */, const bool IN InByMe /* = true */)
{
	if (IsTargetableObject() == false)
		return;

	// 자이로중에는 타겟팅 띄우지 않는다.
	if (bGyroMode)
		return;

	// [ 2019-12-5 : kyu ] Press입력중에는 타겟 지우지 않는다
	if (bPressedInput)
		return;

	ShowOffTargetUI();
}

void UWRActorComponentInteractionObject::ShowOnTargetUI()
{
	// [ 2019-8-30 : kyu ] UI가 뜨는 텀이 있기 때문에 요청 시 예약상태를 건다.
	if (bReservedUIOpen)
		return;

	if (WRCharacterManager::Get()->GetMyCharacter()->GetCurrentFSMType() == EWRFSM::Interact)
		return;

	// [ 2019-10-16 : kyu ] 타겟 가능할때만 켠다
	if (IsTargetableObject() == false)
		return;

	// 받은 UI가 있으면 리턴
	if (WRUIManager::Get()->FindUIData(GetOwnerCharacter(), TargetUIKind) != nullptr)
		return;


	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
		{
			// [ 2019-8-28 : kyu ] 컨텐츠 내용에 따라 창 크기가 유동적이어야 한다.
			InCombinedPanelData.WidgetComponentBase->SetDrawAtDesiredSize(true);
			//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::IO_TargetUIFixedSize) * 0.0001f);

			// 예약상태 해제
			bReservedUIOpen = false;

			// 상호작용 영역, 갈고리 영역 여부에 따른 UI 활성/비활성
			UpdateTargetUI();

			// 하이라이트 업데이트
			UpdateHighLight();
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::IOPopup, GetWidgetComponentShowType(), &Delegate, GetOwnerCharacter());
	WidgetInfo.SetTransform(GetWidgetTransform());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//FTransform WCTransform = GetWidgetTransform();
	//BitFlag16 WidgetCompFlag = GetWidgetComponentShowType();
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::IOPopup, GetOwnerCharacter(), WCTransform, WidgetCompFlag, &Delegate);

	// 예약상태 저장
	bReservedUIOpen = true;
}

void UWRActorComponentInteractionObject::ShowOffTargetUI()
{
	WRUIManager::Get()->CloseUI(GetOwnerCharacter(), TargetUIKind);

	// 예약상태 해제
	bReservedUIOpen = false;

	// 하이라이트 업데이트
	UpdateHighLight();
}

void UWRActorComponentInteractionObject::ShowOnProgress(const float IN InGoalTime)
{
	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		UWRUIPanel_IOPopup* pWidget = WRUIManager::Get()->FindUI<UWRUIPanel_IOPopup>(GetOwnerCharacter(), EWRUIPanelKind::IOPopup);
		if (pWidget != nullptr)
		{
			pWidget->ShowOnProgress(InGoalTime / 1000.0f);
		}

		OnInteractionGroup(EWRInteractionGroup::DirectProgress, true);
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{

	}
	break;
	}
}

void UWRActorComponentInteractionObject::ShowOffProgress(const bool IN InComplete)
{
	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		UWRUIPanel_IOPopup* pWidget = WRUIManager::Get()->FindUI<UWRUIPanel_IOPopup>(GetOwnerCharacter(), EWRUIPanelKind::IOPopup);
		if (pWidget != nullptr)
		{
			pWidget->ShowOffProgress(InComplete);
		}

		OnInteractionGroup(EWRInteractionGroup::DirectProgress, false);
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{

	}
	break;
	}
}

FTransform UWRActorComponentInteractionObject::GetWidgetTransform()
{
	FTransform WCTransform = GetOwnerCharacter()->GetWidgetTransform();

	return WCTransform;
}

EWRUIWidgetCreationBitFlag UWRActorComponentInteractionObject::GetWidgetComponentShowType()
{
	EWRUIWidgetCreationBitFlag WidgetCompFlag = EWRUIWidgetCreationBitFlag::None;

	WidgetCompFlag = GetOwnerCharacter()->GetWidgetComponentShowType();

	return WidgetCompFlag;
}

void UWRActorComponentInteractionObject::UpdateTargetUI()
{
// 	if (WRCharacterManager::IsValid() == false)
// 		return;
// 
// 	// TargetUI는 인터렉션 플레이어가 '내'가 아니면 처리할 필요가 없다
// 	if (InteractionPlayer != WRCharacterManager::Get()->GetMyCharacter())
// 		return;
	 
	const AWRCharacter* pTarget = WRCharacterManager::Get()->GetMyTarget();
	if (pTarget == GetOwnerCharacter() && IsTargetableObject())
	{
		switch (ObjectTable->MainType)
		{
		case EWRIOMainType::Interaction:
		{
			UWRUIPanel_IOPopup* pWidget = WRUIManager::Get()->FindUI<UWRUIPanel_IOPopup>(GetOwnerCharacter(), EWRUIPanelKind::IOPopup);
			if (pWidget != nullptr)
			{
				AWRCharacterIO* CastedOwner = GetOwnerObject();

				pWidget->SetDataFromTable(ObjectTable, this, CastedOwner->GetISN());
				return;
			}
		}
		break;
		}
		
		ShowOnTargetUI();
	}
	else
	{
		ShowOffTargetUI();
	}
}

//====================================================================================
//	:: 입력
//====================================================================================

bool UWRActorComponentInteractionObject::OnPressedEvent(const EWRInput IN InInputType)
{
	// ! 대상을 바라본 상태로 입력이 들어옴

	if (IsInteracting())
	{
		if (InteractionPlayer != nullptr && InteractionPlayer->IsMe() == false)
		{
			WRNoticeManager::Get()->AddNotice(FString(""));
		}
		return true;
	}

	bInteracting = false;

	// 입력에 맞는 상호작용이 있는가?
	EWRIOInteractionType MatchInteractionType = FindMatchKeyEvent(InInputType);
	switch (MatchInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (IsInteractionableState() == false)
		{
			return false; // 상호작용 불가능한 상태
		}
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		if (IsHookableState() == false)
		{
			return false; // 갈고리 불가능한 상태
		}
	}
	break;

	case EWRIOInteractionType::None:
	{
		// + 배치된 인테리어용 IO : [회수]
		ISN_t InteriorISN = GetOwnerObject()->GetISN();
		if (InteriorISN != INVALID_ISN)
		{
			if (WRInteriorManager::Get()->IsSpawnedByMe(InteriorISN))
			{
				// 회수는 X키만
				if (InInputType == EWRInput::Cross)
				{
					WRInteriorManager::Get()->RequestRemoveInteriorItem(InteriorISN);
					return true;
				}
			}
		}
		return false;
	}
	break;
	}

	// [ 2019-11-27 : kyu ] 실제 입력이 들어가면 그때부터 상호작용 시작인것이다.

	// 상호작용 시도
	OnReq_InteractionStart(MatchInteractionType);

	return true;
}

bool UWRActorComponentInteractionObject::OnReleasedEvent(const EWRInput IN InInputType)
{
	if (IsInteracting() == false)
		return false;

	// 입력에 맞는 상호작용이 있는가?
	EWRIOInteractionType MatchInteractionType = FindMatchKeyEvent(InInputType);
	switch (MatchInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (IsInteractionableState() == false)
		{
			return false; // 상호작용 불가능한 상태
		}
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		if (IsHookableState() == false)
		{
			return false; // 갈고리 불가능한 상태
		}
	}
	break;
	default:
	{
		return false;
	}
	break;
	}

	// + PressTime 체크 추가
	if (ObjectTable->DirectKeyPressTime != 0.0f)
	{
//		ReleaseProgressing();

		// 중지일 때 중지 패킷 보냄
		OnReq_InteractionStop();
	}

	return true;
}

void UWRActorComponentInteractionObject::OnPadEvent(const EWRInput IN InEvent)
{
	// 자이로 모드일 때만 입력 받음
	if (bGyroMode == false)
		return;

	if (FindMatchPadEvent(InEvent) == false)
		return; // 입력에 맞는 상호작용이 있는가?

	if (IsInteractionableState() == false)
		return; // 상호작용 불가능한 상태

	// 자이로 모드 종료
	OnGyroEnd();

	// 상호작용 완료 요청
	OnReq_InteractionEnd();
}

void UWRActorComponentInteractionObject::UpdatePressedTime(float IN InDeltaTime)
{
	if (ObjectTable == nullptr)
		return;

	if (ObjectTable->DirectKeyPressTime == 0.0f)
		return;

	// [ 2019-12-4 : kyu ] 입력중에 이동시 입력 해제
	if (CheckMoveDuringInteraction())
		return;

	if (bPressedInput)
	{
		fPressedTime += InDeltaTime;

		float fGoalTime = ObjectTable->DirectKeyPressTime / 1000.0f;
		if (fPressedTime >= fGoalTime)
		{
			bPressedInput = false;
			fPressedTime = 0.0f;

			if (IsUseGyro())
			{
				OnGyroStart();
			}
			else
			{
				OnReq_InteractionEnd(); // 상호작용 완료 요청
			}

			ShowOffProgress(true);	// 프로그레스 완료
		}
	}
}

void UWRActorComponentInteractionObject::ReleaseProgressing()
{
	if (ObjectTable->DirectKeyPressTime == 0.0f)
		return;

	bPressedInput = false;
	fPressedTime = 0.0f;

	ShowOffProgress(false); // 프로그레스 취소
}

EWRIOInteractionType UWRActorComponentInteractionObject::FindMatchKeyEvent(const EWRInput IN InInputType)
{
	if (ObjectTable->DirectKey == InInputType)
	{
		return EWRIOInteractionType::DirectInteraction;
	}

// [ 2019-9-30 : kyu ] *** Hook에 대한 키 입력은 스킬데이터를 참조해야한다.
// 	if (pTableData->HookType == InInputType)
// 	{
// 		OutInvokeInteractionType = EWRIOInteractionType::HookInteraction;
// 		return true;
//	}

	return EWRIOInteractionType::None;
}

bool UWRActorComponentInteractionObject::FindMatchPadEvent(const EWRInput IN InEvent)
{
	FWRIOGyro* pGyroStep = GetGyroStep(GyroStep);
	if (pGyroStep == nullptr)
		return false;

	switch (InEvent)
	{
	case EWRInput::JoystickPull:
	{
		if (pGyroStep->GyroType == EWRIOGyroAction::Pull)
			return true;
	}
	break;
	case EWRInput::JoystickPush:
	{
		if (pGyroStep->GyroType == EWRIOGyroAction::Push)
			return true;
	}
	break;
	case EWRInput::TouchPadClick:
	{

	}
	break;
	case EWRInput::TouchPadDragUpY:
	{

	}
	break;
	}
	return false;
}

//====================================================================================
// :: Gyro
//====================================================================================

void UWRActorComponentInteractionObject::SaveGyroSteps(TArray<FWRIOGyro>* IN InGyroSteps)
{
	if (InGyroSteps == nullptr || InGyroSteps->Num() == 0)
		return;

	GyroSteps = InGyroSteps;
}

void UWRActorComponentInteractionObject::OnGyroStart()
{
	OnInteractionGroup(EWRInteractionGroup::Gyro, true);
}

void UWRActorComponentInteractionObject::OnGyroEnd()
{
	OnInteractionGroup(EWRInteractionGroup::Gyro, false);
}

bool UWRActorComponentInteractionObject::IsUseGyro()
{
	if (GyroSteps == nullptr || GyroSteps->Num() == 0)
		return false;

	if (IsRecognizableState() == false)
		return false; // 인지 불가능 상태

	return true;
}

void UWRActorComponentInteractionObject::ShowOnGyroInteraction()
{
	FWRIOGyro* pCurStep = GetGyroStep(GyroStep);
	if (pCurStep == nullptr)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(InCombinedPanelData.UIPanel);
			if (Panel_Main)
			{
				Panel_Main->OpenGyroInteraction(pCurStep->Icon.ToString(), pCurStep->Description);
			}
		}
	});

	if (WRUIManager::IsValid())
	{
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
}

void UWRActorComponentInteractionObject::ShowOffGyroInteraction()
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(InCombinedPanelData.UIPanel);
			if (Panel_Main)
			{
				Panel_Main->CloseGyroInteraction();
			}
		}
	});

	if (WRUIManager::IsValid())
	{
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
}

FWRIOGyro* UWRActorComponentInteractionObject::GetGyroStep(int32 IN InStep)
{
	if (GyroSteps == nullptr)
		return nullptr;

	if ((*GyroSteps).IsValidIndex(InStep))
	{
		return &(*GyroSteps)[InStep];
	}
	return nullptr;
}


void UWRActorComponentInteractionObject::UpdateGyroTransform(float IN InDeltaTime)
{
	if (bGyroMode == false)
		return;

	// [ 2019-12-4 : kyu ] 자이로중에 이동시 자이로 해제
	if (CheckMoveDuringInteraction())
		return;

	FWRIOGyro* pGyroStep = GetGyroStep(GyroStep);
	if (pGyroStep == nullptr)
	{
		OnGyroEnd();
		return;
	}

	if (pGyroStep->GyroType == EWRIOGyroAction::None)
		return;

	// GyroType이 Pull이거나 Push는 Update가 아닌 별도의 입력에서 얻는다
	if (pGyroStep->GyroType == EWRIOGyroAction::Pull
		|| pGyroStep->GyroType == EWRIOGyroAction::Push)
		return;

	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetWorld()->GetFirstPlayerController();
	if (PlayerController->IsValidLowLevel())
	{
		FVector Empty;
		FVector Gravity;
		PlayerController->GetInputMotionState(Empty, Empty, Gravity, Empty);

		FVector GyroVector = Gravity.GetSafeNormal();

		// FRotator : Pitch(Y), Yaw(Z), Roll(X)
		if (pGyroStep->Axis == EWRIOGyroAxis::X)
		{
			if (CalcGyroLerpDegree(InDeltaTime, GyroVector.X, pGyroStep))
			{
				//--- 움직이는 중
				UpdateGyroDirection(fLerpDegree, pGyroStep);
				// Z to Yaw
				FRotator NewRot = SrcRotate + FRotator(0, 0, fLerpDegree);
				USkeletalMeshComponent* pSkeletalMeshComp = GetOwnerCharacter()->GetMesh();
				if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
				{
					pSkeletalMeshComp->SetRelativeRotation(NewRot);
				}
				else
				{
					GetOwnerCharacter()->SetRotation(NewRot);
				}
			}
			else
			{
				//--- 멈춤
				StopGyroDirection(pGyroStep);
			}
		}
		else if (pGyroStep->Axis == EWRIOGyroAxis::Y)
		{
			if (CalcGyroLerpDegree(InDeltaTime, GyroVector.Z, pGyroStep))
			{
				//--- 움직이는 중
				UpdateGyroDirection(fLerpDegree, pGyroStep);
				// Y to Pitch
				FRotator NewRot = SrcRotate + FRotator(fLerpDegree, 0, 0);
				USkeletalMeshComponent* pSkeletalMeshComp = GetOwnerCharacter()->GetMesh();
				if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
				{
					pSkeletalMeshComp->SetRelativeRotation(NewRot);
				}
				else
				{
					GetOwnerCharacter()->SetRotation(NewRot);
				}
			}
			else
			{
				//--- 멈춤
				StopGyroDirection(pGyroStep);
			}
		}
		else if (pGyroStep->Axis == EWRIOGyroAxis::Z)
		{
			if (CalcGyroLerpDegree(InDeltaTime, GyroVector.Y, pGyroStep))
			{
				//--- 움직이는 중
				UpdateGyroDirection(fLerpDegree, pGyroStep);
				// X to Roll
				FRotator NewRot = SrcRotate + FRotator(0, fLerpDegree, 0);
				USkeletalMeshComponent* pSkeletalMeshComp = GetOwnerCharacter()->GetMesh();
				if (pSkeletalMeshComp != nullptr && pSkeletalMeshComp->SkeletalMesh != nullptr)
				{
					pSkeletalMeshComp->SetRelativeRotation(NewRot);
				}
				else
				{
					GetOwnerCharacter()->SetRotation(NewRot);
				}
			}
			else
			{
				//--- 멈춤
				StopGyroDirection(pGyroStep);
			}
		}


		// Goal Check
		if (FMath::Abs(fLerpDegree - pGyroStep->GoalDegree) < 5.f)
		{
			int32 NextGyroStep = GyroStep + 1;
			FWRIOGyro* pNextGyroStep = GetGyroStep(NextGyroStep);
			if (pNextGyroStep == nullptr)
			{
				OnGyroEnd();
				
				// 상호작용 완료 요청
				OnReq_InteractionEnd();
			}
			else
			{
				if (pNextGyroStep->Axis == EWRIOGyroAxis::None)
				{
					OnGyroEnd();
					
					// 다음 자이로 설정 안되어있음 ? 상호작용 완료 요청
					OnReq_InteractionEnd();
				}
				else
				{
					// 다음 자이로 동작으로 !
					GyroStep = NextGyroStep;

					// UI 갱신
					ShowOnGyroInteraction();
				}
			}
		}
	}
}

bool UWRActorComponentInteractionObject::CalcGyroLerpDegree(float IN InDeltaTime, float IN InGyroValue, FWRIOGyro* IN InGyroInfo)
{
	float fCurDegree = FMath::Clamp(InGyroValue * InGyroInfo->LimitDegree, InGyroInfo->MinDegree, InGyroInfo->MaxDegree);
// 	if (GyroVector.Y < 0.f)
// 	{
// 		fDegree *= -1.f;
// 	}
// 	fDegree = (int32)fDegree / (int32)InGyroInfo->LerpTermDegree;
// 	fDegree *= InGyroInfo->LerpTermDegree;

	// [ 2019-11-21 : kyu ] 
	// 자이로 회전 시 각도값을 Gravity로 추정하여 가져오는데
	// 패드 진동 시 Gravity값이 널뛰는 문제로 각도값이 출렁여서 크게 움직인것으로 판정, 진동중에 멈추질 않는다

	// [ 2019-12-12 : kyu ] 최초시작 시 Lerp만큼 벌어지면 실제 이동 시작하도록 수정
	if (bGyroLerpMove == false)
	{
		if (fStartDegree == INVALID_START_DEGREE)
			fStartDegree = fCurDegree;

		float fStartDiff = FMath::Abs(fStartDegree - fCurDegree);
		if (FMath::Abs(fStartDiff) > InGyroInfo->LerpTermDegree)
			bGyroLerpMove = true;
		else
			return false;
	}

	float fDiffDegree = FMath::Abs(fDstDegree - fCurDegree);
	if (fDiffDegree > InGyroInfo->LerpTermDegree)
	{
		// LerpTermDegree만큼의 갭이 생겨서 시작각도 설정 및 목표각도를 현재각도로 지정한다.
		fElapsedTime = 0.f;
		fSrcDegree = fLerpDegree;
		fDstDegree = fCurDegree;
	}

	if (fElapsedTime < InGyroInfo->LerpTime)
	{
		fElapsedTime += InDeltaTime;
		if (fElapsedTime >= InGyroInfo->LerpTime)
			fElapsedTime = InGyroInfo->LerpTime;
		fLerpDegree = FMath::Lerp(fSrcDegree, fDstDegree, FMath::Clamp(fElapsedTime, 0.f, InGyroInfo->LerpTime) / InGyroInfo->LerpTime);

		InGyroInfo->bActive = true;
		return true;
	}

	return false;
}

void UWRActorComponentInteractionObject::UpdateGyroDirection(float IN InCurDegree, FWRIOGyro* IN InGyroInfo)
{
	float fGoalDegree = InGyroInfo->GoalDegree;
	float fDiff = FMath::Abs(fGoalDegree - InCurDegree); // 절대적인 차이

	float fPrev = 0.f;
	float fNext = 0.f;
	for (FWRIOGyroDirection& GyroDirection : InGyroInfo->Directions)
	{
		fNext = GyroDirection.RoundDegree;
		if (fPrev < fDiff && fNext > fDiff)
		{
			// 구간 중복체크 방지
			if (fLerpSectionMin == fPrev && fLerpSectionMax == fNext)
				return;

			fLerpSectionMin = fPrev;
			fLerpSectionMax = fNext;

			// 진동
			ChangeForceFeedback(&GyroDirection);
			// VFX
			ChangeVFX(&GyroDirection);
			// SFX
			ChangeSFX(&GyroDirection);

			// 진동TID가 있고, 이미 플레이 중인 TID와 다를때만 다시 플레이
// 			if (GyroDirection.ForceFeedbackTID != INVALID_TABLE_TID
// 				&& GyroDirection.ForceFeedbackTID != GyroForceFeedbackTID)
// 			{
// 				WRInputSystemManager::Get()->ClientStopForceFeedback(GyroForceFeedbackTID);
// 
// 				GyroForceFeedbackTID = GyroDirection.ForceFeedbackTID;
// 				WRInputSystemManager::Get()->ClientPlayForceFeedback(GyroForceFeedbackTID);
// 			}
			// 이펙트 데이터가 있고, 이미 스폰한 데이터와 다를때만 삭제하고 다시 스폰
// 			if (GyroDirection.VFX.IsNull() == false
// 				&& GyroDirection.VFX.GetAssetName() != GyroRotateVFX)
// 			{
// 				if (GyroDirection.pFX != nullptr)
// 				{
// 					WRVFXManager::Get()->Remove(GyroDirection.pFX);
// 				}
// 				GyroRotateVFX = GyroDirection.VFX.GetAssetName();
// 				GyroDirection.pFX = WRVFXManager::Get()->Spawn(GyroDirection.VFX.LoadSynchronous(), GetOwnerCharacter(), nullptr);
// 			}
			// 사운드 데이터가 있고, 이미 스폰한 데이터와 다를때만 삭제하고 다시 스폰
// 			if (GyroDirection.SFX.IsNull() == false
// 				&& GyroDirection.SFX.GetAssetName() != GyroRotateSFX)
// 			{
// 				if (GyroDirection.pAudioComp != nullptr)
// 				{
// 					WRSFXManager::Get()->Remove(GyroDirection.pAudioComp);
// 				}
// 				GyroRotateSFX = GyroDirection.SFX.GetAssetName();
// 				GyroDirection.pAudioComp = WRSFXManager::Get()->Spawn2D(GyroDirection.SFX.LoadSynchronous());
// 			}

			CurGyroDirection = &GyroDirection;

			return;
		}
		else
		{
			fPrev = fNext;
		}
	}
}

void UWRActorComponentInteractionObject::ChangeForceFeedback(FWRIOGyroDirection* IN InNewDirection)
{
	// 이전 진동 해제
	if (CurGyroDirection != nullptr)
	{
		if (CurGyroDirection->ForceFeedbackTID != INVALID_TABLE_TID)
		{
			if (CurGyroDirection->ForceFeedbackTID != InNewDirection->ForceFeedbackTID)
			{
				WRInputSystemManager::Get()->ClientStopForceFeedback(CurGyroDirection->ForceFeedbackTID);
			}
			else
			{
				return;
			}
		}
	}

	// 새 진동 출력
	if (InNewDirection->ForceFeedbackTID != INVALID_TABLE_TID)
	{
		WRInputSystemManager::Get()->ClientPlayForceFeedback(InNewDirection->ForceFeedbackTID);
	}
}

void UWRActorComponentInteractionObject::ChangeVFX(FWRIOGyroDirection* IN InNewDirection)
{
	// 이전 VFX 해제
	if (CurGyroDirection != nullptr)
	{
		if (CurGyroDirection->VFX.IsNull() == false)
		{
			FString strCur = CurGyroDirection->VFX.GetAssetName();
			FString strNew = InNewDirection->VFX.GetAssetName();
			if (strCur.Equals(strNew) == false)
			{
				WRVFXManager::Get()->Remove(CurGyroDirection->pFX);
			}
			else
			{
				// 다음 효과가 승계받음
				InNewDirection->pFX = CurGyroDirection->pFX;
				return;
			}
		}
	}

	// 새 VFX 출력
	if (InNewDirection->VFX.IsNull() == false)
	{
		InNewDirection->pFX = WRVFXManager::Get()->Spawn(InNewDirection->VFX.LoadSynchronous(), GetOwnerCharacter(), nullptr);
	}
}

void UWRActorComponentInteractionObject::ChangeSFX(FWRIOGyroDirection* IN InNewDirection)
{
	// 이전 SFX 해제
	if (CurGyroDirection != nullptr)
	{
		if (CurGyroDirection->SFX.IsNull() == false)
		{
			FString strCur = CurGyroDirection->SFX.GetAssetName();
			FString strNew = InNewDirection->SFX.GetAssetName();
			if (strCur.Equals(strNew) == false)
			{
				WRSFXManager::Get()->Remove(CurGyroDirection->pAudioComp);
			}
			else
			{
				// 다음 사운드가 승계받음
				InNewDirection->pAudioComp = CurGyroDirection->pAudioComp;
				return;
			}
		}
	}

	// 새 SFX 출력
	if (InNewDirection->SFX.IsNull() == false)
	{
		InNewDirection->pAudioComp = WRSFXManager::Get()->Spawn2D(InNewDirection->SFX.LoadSynchronous());
	}
}

void UWRActorComponentInteractionObject::StopGyroDirection(FWRIOGyro* IN InGyroInfo)
{
	if (InGyroInfo->bActive == false)
		return;

	InGyroInfo->bActive = false;

	ReleaseGyroData();
}

void UWRActorComponentInteractionObject::ReleaseGyroData()
{
	if (CurGyroDirection != nullptr)
	{
		CurGyroDirection->ReleaseGyroDirection();
		CurGyroDirection = nullptr;
	}

	bGyroLerpMove = false;
	GyroStep = 0;

	fLerpSectionMin = 0.f;
	fLerpSectionMax = 0.f;
	fStartDegree = INVALID_START_DEGREE;
}

//====================================================================================
// Etc.
//====================================================================================

bool UWRActorComponentInteractionObject::CheckMoveDuringInteraction()
{
	if (WRCharacterManager::Get()->IsMoving())
	{
		// 인터렉션 중이고 인터렉션플레이어가 나면 중단요청
		if (bInteracting && InteractionPlayer->IsMe())
		{
			OnReq_InteractionStop();
			return true;
		}
	}
	return false;
}

void UWRActorComponentInteractionObject::UpdateHighLight()
{
	if (IsRecognizableState())
	{
		const AWRCharacter* pTarget = WRCharacterManager::Get()->GetMyTarget();
		if (pTarget == GetOwnerCharacter())
		{
			GetOwnerCharacter()->SetHighLight(EWRObjectHighLight::Targeted);
		}
		else
		{
			GetOwnerCharacter()->SetHighLight(EWRObjectHighLight::Recognition);
		}
	}
	else
	{
		GetOwnerCharacter()->SetHighLight(EWRObjectHighLight::None);
	}
}

void UWRActorComponentInteractionObject::OnSkipToTest()
{
	OnGyroEnd();			// 자이로 종료
	OnReq_InteractionEnd();	// 상호작용 완료 요청
}

//====================================================================================
// For Cheat
//====================================================================================

void UWRActorComponentInteractionObject::AddResultCount(const EWRIOInteractionType IN InType)
{
	switch (InType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (ObjectTable->DirectRetainCount == 0)
			return;

		if (ObjectTable->DirectRetainCount == DirectRetainCount)
			return;

		++DirectRetainCount;
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		if (ObjectTable->HookRetainCount == 0)
			return;

		if (ObjectTable->HookRetainCount == HookRetainCount)
			return;

		++HookRetainCount;
	}
	break;
	}
}

//====================================================================================
//
//====================================================================================

void FWRIOGyroDirection::ReleaseGyroDirection()
{
	if (ForceFeedbackTID != INVALID_TABLE_TID)
	{
		WRInputSystemManager::Get()->ClientStopForceFeedback(ForceFeedbackTID);
	}

	if (VFX.IsNull() == false && pFX != nullptr)
	{
		WRVFXManager::Get()->Remove(pFX);
	}

	if (SFX.IsNull() == false && pAudioComp != nullptr)
	{
		WRSFXManager::Get()->Remove(pAudioComp);
	}
}


//====================================================================================
//	:: Packet
//====================================================================================

void UWRActorComponentInteractionObject::OnReq_InteractionStart(const EWRIOInteractionType IN InInvokeInteractionType)
{
	AID_t CharAID = WRCharacterManager::Get()->GetMyAID();
	AID_t ObjectAID = GetOwnerCharacter()->GetActorID();
	WRObjectInteractionManager::Get()->OnReq_BeginInteraction(CharAID, ObjectAID, InInvokeInteractionType);
}

void UWRActorComponentInteractionObject::OnInf_InteractionStart(const AID_t IN InCharID, const EWRIOInteractionType IN InInvokeInteractionType)
{
	InteractionPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InCharID);
	if (InteractionPlayer == nullptr)
		return;

	UWRActorComponentInteraction* pInteractionComp = InteractionPlayer->GetComponent<UWRActorComponentInteraction>();
	if (pInteractionComp != nullptr)
	{
		pInteractionComp->OnInteractionStart(GetOwnerObject());
	}

	bInteracting = true;
	CurrentInteractionType = InInvokeInteractionType;

	// 프레싱, 자이로 등 중간 [과정]이 더 있는 경우는 추가 작업 완료 후 End를 보낸다.
	// 그 외 애니 등 연출로 때워질 경우 바로 End를 보낸다.
	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (ObjectTable->DirectKeyPressTime != 0.0f)
		{
			ShowOnProgress(ObjectTable->DirectKeyPressTime); // 상호작용 프레싱
		}
		else
		{			
			if (IsUseGyro())
			{
				OnGyroStart(); // 자이로 정보가 있으면 자이로 시작
			}
			else
			{
				OnReq_InteractionEnd();	// 상호작용 완료 요청
			}
		}
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		OnReq_InteractionEnd();	// 상호작용 완료 요청 (갈고리는 당김과 동시에 Start->End 직행. 박혀있을 때 상호작용중이라던지 상태가 잡히면 안되기 때문)
	}
	break;
	}
}

void UWRActorComponentInteractionObject::OnReq_InteractionStop()
{
	AID_t CharAID = WRCharacterManager::Get()->GetMyAID();
	AID_t ObjectAID = GetOwnerCharacter()->GetActorID();
	WRObjectInteractionManager::Get()->OnReq_StopInteraction(CharAID, ObjectAID);
}

void UWRActorComponentInteractionObject::OnInf_InteractionStop(const AID_t IN InCharID)
{
	AWRCharacterPlayer* pPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InCharID);
	if (pPlayer == nullptr)
		return;

	UWRActorComponentInteraction* pInteractionComp = pPlayer->GetComponent<UWRActorComponentInteraction>();
	if (pInteractionComp != nullptr)
	{
		pInteractionComp->OnInteractionStop();
	}

	ReleaseInteractionInfo();
}

void UWRActorComponentInteractionObject::OnReq_InteractionEnd()
{
	if (InteractionPlayer == nullptr)
		return;

	if (InteractionPlayer->IsMe() == false)
		return;

	AID_t CharAID = WRCharacterManager::Get()->GetMyAID();
	AID_t ObjectAID = GetOwnerCharacter()->GetActorID();
	WRObjectInteractionManager::Get()->OnReq_EndInteraction(CharAID, ObjectAID, CurrentInteractionType);
}

void UWRActorComponentInteractionObject::OnInf_InteractionEnd(const AID_t IN InCharID, const AID_t IN InObjectID, const uint16 IN InRetainCount, const uint8 IN InInteractionType)
{
	AWRCharacterPlayer* pPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InCharID);
	if (pPlayer == nullptr)
		return;
	
	// 횟수 체크해서 Unsuable로 변경
	UpdateRetainCount((EWRIOInteractionType)InInteractionType, InRetainCount);

	UpdateTargetUI();

	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		OnInteractionGroup(EWRInteractionGroup::DirectInteraction);
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		OnInteractionGroup(EWRInteractionGroup::HookInteraction);
	}
	break;
	}
	
	UWRActorComponentInteraction* pInteractionComp = pPlayer->GetComponent<UWRActorComponentInteraction>();
	if (pInteractionComp != nullptr)
	{
		pInteractionComp->OnInteractionEnd();
	}

	if (pPlayer->IsMe())
	{
		// [ 2019-11-29 : kyu ] 누구든 갈고리 인터렉션을 끝냈을 때 나와 다른 사람들의 갈고리를 끊어준다.
		if ((EWRIOInteractionType)InInteractionType == EWRIOInteractionType::HookInteraction)
		{
			WRCharacterManager::Get()->HookCutAllRelatedTarget(GetOwnerCharacter(), true);
		}
	}
	else
	{
		// [ 2019-11-29 : kyu ] 누구든 갈고리 인터렉션을 끝냈을 때 나와 다른 사람들의 갈고리를 끊어준다.
		if ((EWRIOInteractionType)InInteractionType == EWRIOInteractionType::HookInteraction)
		{
			WRCharacterManager::Get()->HookCutAllRelatedTarget(GetOwnerCharacter(), false);
		}

		// 타인이 작동 시 대상의 연출을 강제로 실행한다
		AfterProcess_GyroResult();
		AfterProcess_Attached();
	}	
}