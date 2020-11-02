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
		// pOwnerIO�� ����TF�� pTarget���Է� �ѱ�
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

	// [ 2019-11-21 : kyu ] �ϴ� ���� ����. â�� �߸� UpdateTargetUI�� ���ؼ� �ڵ� ����
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

	// �����Ÿ� ������Ʈ
	UpdateRecognitionArea();

	// ��ȣ�ۿ� �ּҰŸ� ������Ʈ
	UpdateInteractionMinDist();

	// ���� ��밡�� �Ÿ� ������Ʈ
	UpdateHookArea();

	// ��ư �Է� �����ð� üũ
	UpdatePressedTime(DeltaTime);

	// ���� ������Ʈ
	UpdateDirection();

	// ���̷� ������Ʈ
	UpdateGyroTransform(DeltaTime);
}

void UWRActorComponentInteractionObject::SetInteractionTable(FWRInteractionObjectTable* IN InObjectTable)
{
	if (InObjectTable == nullptr)
		return;

	// [ 2019-10-22 : kyu ] *** ���߿� �̰� ������ ����ü�� �����ϰ� �ְ� ����...Owner Ÿ�Կ� ���� �ٸ� �����͸� ����..

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
				ReleaseInteractionInfo(); // ������� ��ùߵ��� ��� ���ͷ��� �����͸� �����Ѵ�.
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
				ReleaseInteractionInfo(); // ������� ��ùߵ��� ��� ���ͷ��� �����͸� �����Ѵ�.
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
//	��Ÿ
//====================================================================================

void UWRActorComponentInteractionObject::OnInteractionResult()
{
	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		DirectionLinkFunction(ObjectTable->DirectResult, ObjectTable->DirectResultID);

		// ���� �� �ܷ�����
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

		// ���� �� �ܷ�����
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

	bInteracting = false; // ��� �� ���ͷ��� ����
}

void UWRActorComponentInteractionObject::ReleaseInteractionInfo()
{	
	OnGyroEnd();			// ���̷� ��尡 ���������� ��
	ReleaseProgressing();	// ���� ���������� ��

	bInteracting = false;
	bInteractionEnding = false;
	CurrentInteractionType = EWRIOInteractionType::None;
	InteractionPlayer = nullptr;

	UpdateTargetUI();		// ��ȣ�ۿ� ���� ���¿� ���� ������� UI�� Ȱ������ �����ؾ��Ѵ�.
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
			return true; // Count�� 0�̸� ������

		if (ObjectTable->DirectRetainCount == DirectRetainCount)
			return false;
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		if (ObjectTable->HookType == EWRHookType::None)
			return false;

		if (ObjectTable->HookRetainCount == 0)
			return true; // Count�� 0�̸� ������

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
			// [ 2019-11-30 : kyu ] ���ͷ��� �����ؼ� ����������� Ÿ��â ����
			bInteractionEnding = true;
			
			bool bImmediateResult = InvokeDirectionGroup(InType);
			if (bImmediateResult)
			{
				OnAnimationEnded();		// ... to InteractionFinish
			}
			else
			{
				// ������ ������ ���⵿�� UI�� ����. Ÿ�ٵ� �Ұ�.
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
		// [ 2020-1-7 : kyu ] ������۰� ���ÿ� Result �ߵ�. Trigger�� �����ð����� �̷��.
		OnInteractionResult();

		InvokeDirectionGroup(InType);

		if (bInteractionEnding)
		{
			// ���ͷ��� ��� ����
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
			// + Ÿ��UI ��
			ShowOffTargetUI();

			if (InteractionPlayer != nullptr && InteractionPlayer->IsMe())
			{
				// + ���̷� UI ���
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
				// + ���̷� UI ����
				ShowOffGyroInteraction();
				WRInputSystemManager::Get()->SetOnlyMoveInput(false);
			}
		}
	}
	break;
	case EWRInteractionGroup::HookArrival:
	{
		InvokeDirectionGroup(InType, InLocation);

		// ���� ���޽� UI ������Ʈ
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
	// AWRCharacterIO::OnAnimEnded ���� ȣ���

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
		// [ 2019-12-4 : kyu ] ġƮ�� ������� ������Ʈ�� �ƴҶ��� Ʈ���� �ߵ�
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

	// [ 2020-4-21 : kyu ] ���� ���� - ���ο��� AttachToComponent�� ��ġ�������� �Ѵ�
	UWRActorComponentInteraction* pInteractionComp = InteractionPlayer->GetComponent<UWRActorComponentInteraction>();
	if (pInteractionComp != nullptr)
	{
		pInteractionComp->SetOccupiedIO(GetOwnerObject());
	}

	// ��ȣ�ۿ� ��ġ ���� ����
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

	// ��ȣ�ۿ� ����� ���� �õ� (�ϴ� ���ڿ� ���ؼ��� �׽�Ʈ)
	OnInteractionGroup(EWRInteractionGroup::DirectInteraction);
}

//====================================================================================
//	:: ���� �Ÿ�
//====================================================================================

bool UWRActorComponentInteractionObject::IsRecognizableObject()
{
	// Q. ���� �� �� �ִ� ������Ʈ�ΰ�?

	if (GetInteractionState() == EWRInteractionState::Unusable)
		return false; // ��ȣ�ۿ� �Ұ� ����

	if (bInteractionEnding)
		return false; // ��ȣ�ۿ� �������̸� �Ұ�

 	if (ObjectTable == nullptr)
 		return false; // ������ ����
 
 	if (ObjectTable->MainType != EWRIOMainType::Interaction)
 		return false; // MainType�� Interaction�� �ƴϸ� ���� �Ұ�
 
 	if (ObjectTable->DirectType == EWRIODirectType::None
 		&& ObjectTable->HookType == EWRHookType::None)
 		return false; // ����, ���� ��ȣ�ۿ� ������ �ϳ��� ���� ���
 
 	if (IsInteractionableCount(EWRIOInteractionType::DirectInteraction) == false
 		&& IsInteractionableCount(EWRIOInteractionType::HookInteraction) == false)
 		return false; // �׼� ������ ���°� �ϳ��� ����

	if (WRConsoleManager::Get()->GetBool(EWRConsole::IO_IgnoreQuest) == false 
		&& IsCheatedObject() == false 
		&& ObjectTable->DirectConditionQuestProgress != 0)
	{
		// + Quest TID ���� ����
		if (WRQuestManager::Get()->IsProgressQuest(ObjectTable->DirectConditionQuestProgress) == false)
			return false;
	}

	if (GetOwnerObject()->GetOccupiedPlayer() != nullptr)
		return false; // �������̸� �����

	return true;
}

bool UWRActorComponentInteractionObject::IsRecognizableState()
{
	if (IsRecognizableObject() == false)
		return false; // ���� �� �� ���� ������Ʈ

	if (bRecognitionArea == false)
		return false; // ���� �����ΰ�?

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
		return; // ���� �� �� ���� ������Ʈ
	}

	if (pMyPlayer->IsRecognizableRange(GetOwnerCharacter()))
	{
		RecognitionAreaIN();	// ���� �Ÿ� ��
	}
	else
	{
		RecognitionAreaOUT();	// ���� �Ÿ� ��
	}
}

void UWRActorComponentInteractionObject::RecognitionAreaIN()
{
	if (bRecognitionArea == false)
	{
		bRecognitionArea = true;

		// [ 2019-12-2 : kyu ] ��ȣ�ۿ� �����ȿ��� ����Ʈ�� �ʰ� �޾� Ÿ��UI�� ������Ʈ �ȵǴ� ���󶧹��� bInteractionableArea ���� ���� ������Ʈ
		if (bInteractionableArea == false)
		{
			UpdateInteractionableAreaOverappingState();
		}

		// �������� �ۿ��� Ÿ���� �߿� ������ �� ������Ʈ
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

		// Ÿ���� �� ���� ���� ����� �� �������� ������Ʈ
		UpdateTargetUI();
		UpdateHighLight();
	}
}

//====================================================================================
//	:: ��ȣ�ۿ� �Ÿ�
//====================================================================================

FTransform UWRActorComponentInteractionObject::GetInteractionTransform()
{
	FTransform WCTransform = GetOwnerCharacter()->GetInteractionTransform();

	return WCTransform;
}

// Q. ��ȣ�ۿ� �� �� �ִ� ������Ʈ�ΰ�?
bool UWRActorComponentInteractionObject::IsInteractionableObject()
{
	if (IsRecognizableState() == false)
		return false; // ���� �� �� ���� [����]�� ������Ʈ
	
	return true;
}

// Q. ��ȣ�ۿ��� �� �ִ� �����ΰ�?
bool UWRActorComponentInteractionObject::IsInteractionableState()
{
	if (IsInteractionableObject() == false)
		return false; // ��ȣ�ۿ��� �� ���� ������Ʈ	

	if (bInteractionableArea == false)
		return false; // ��ȣ�ۿ� �����Ѱ�?

	if (GetOwnerObject()->IsDirectOccupiedState())
		return false; // [ 2020-3-5 : kyu ] �������̸� �н�

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return false;

	float fInteractionMinDist = GetOwnerObject()->GetInteractionMinDist();
	if (fInteractionMinDist != 0.f)
	{
		float fDist = FVector::Dist(pMine->GetActorLocation(), GetOwnerCharacter()->GetActorLocation());
		if (fDist < fInteractionMinDist)
			return false;	// [ 2020-4-23 : kyu ] ���ͷ��� �ּҰŸ� üũ		
	}

	if (pMine->GetConnectedHookCableComponent() != nullptr)
		return false;	// [ 2020-5-7 : kyu ] ������ ����Ǿ������� ��ٸ� ��� �Ұ�	

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
	// ���� ���ؼ��� ó��
	if (InPlayer->IsMe() == false)
		return;

	if (bInteractionableArea == false)
	{
		bInteractionableArea = true;

		// ��ȣ�ۿ� ���� ���¿� ���� ������� UI�� Ȱ������ �����ؾ��Ѵ�.
		UpdateTargetUI();
	}
}

void UWRActorComponentInteractionObject::InteractionAreaOUT(class AWRCharacterPlayer* IN InPlayer)
{
	// ���� ���ؼ��� ó��
	if (InPlayer->IsMe() == false)
		return;

	// [ 2020-4-21 : kyu ] �������� ����� Collision�� ������ OUT ó���� �ǹǷ� ���⼱ ����ó���Ѵ�.
	if (GetOwnerObject()->GetOccupiedPlayer() == InPlayer)
		return;

	if (bInteractionableArea == true)
	{
		bInteractionableArea = false;

		// ���ͷ��� ���̰� ���ͷ����÷��̾ ���� �ߴܿ�û
		if (bInteracting && InteractionPlayer->IsMe())
		{
			OnReq_InteractionStop();
		}

		// ��ȣ�ۿ� ���� ���¿� ���� ������� UI�� Ȱ������ �����ؾ��Ѵ�.
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
		return; // ��ȣ�ۿ��� �� ���� ������Ʈ	

	if (bInteractionableArea == false)
		return; // ��ȣ�ۿ� �����Ѱ�?

	if (GetOwnerObject()->IsDirectOccupiedState())
		return; // [ 2020-3-5 : kyu ] �������̸� �н�

	float fInteractionMinDist = GetOwnerObject()->GetInteractionMinDist();
	if (fInteractionMinDist != 0.f)
	{
		UpdateTargetUI();
	}
}

//====================================================================================
//	:: ���� ��밡�� ����
//====================================================================================

// Q. ���� �� �� �ִ� ������Ʈ�ΰ�?
bool UWRActorComponentInteractionObject::IsHookableObject()
{
	if (IsRecognizableState() == false)
		return false; // ���� �� �� ���� [����]�� ������Ʈ

	return true;
}

// Q. ���� �� �� �ִ� �����ΰ�?
bool UWRActorComponentInteractionObject::IsHookableState()
{
	if (WRCharacterManager::IsValid() == false)
		return false;

	if (WRCharacterManager::Get()->IsEquippedHook() == false)
		return false; // ���� ��� �������� ���� ���

	if (WRCharacterManager::Get()->IsHookableRange(GetOwnerCharacter()) == false)
		return false; // �Ÿ�üũ

	if (IsHookableObject() == false)
		return false; // ���� �� �� ���� ������Ʈ

	if (bHookableArea == false)
		return false; // ���� �����Ѱ�?

	return true;
}


void UWRActorComponentInteractionObject::OnHookArrival(const EWRConnectCableEvent IN InEvent, const FVector IN InImpactLocation, AWRCharacter* IN InHookCaster)
{
	// ���� ���� ���� �߻� !

	OnInteractionGroup(EWRInteractionGroup::HookArrival, true, InImpactLocation);
}

void UWRActorComponentInteractionObject::OnHookCutted()
{
	UpdateTargetUI();

}

void UWRActorComponentInteractionObject::OnHookEventToTarget(class AWRCharacter* IN InHookCaster)
{
	// ���� ����� ���� �߻� !

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
		return; // ���� �� �� ���� ������Ʈ
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

		// ���� ���� ���¿� ���� ������� UI�� Ȱ������ �����ؾ��Ѵ�.
		UpdateTargetUI();
	}
}

void UWRActorComponentInteractionObject::HookAreaOUT()
{
	if (bHookableArea == true)
	{
		bHookableArea = false;

		// ���� ���� ���¿� ���� ������� UI�� Ȱ������ �����ؾ��Ѵ�.
		UpdateTargetUI();
	}
}

//====================================================================================
//	Ÿ����
//====================================================================================


bool UWRActorComponentInteractionObject::IsTargetableObject()
{
	if (IsRecognizableState() == false)
		return false; // ���� �Ұ��� ����

	return true;
}

void UWRActorComponentInteractionObject::OnTargeted(const bool IN InMulti /*= false*/, const bool IN InByMe /*= true*/)
{
	if (IsTargetableObject() == false)
		return;

	// ���̷��߿��� Ÿ���� ����� �ʴ´�.
	if (bGyroMode)
		return;

	ShowOnTargetUI();
}

void UWRActorComponentInteractionObject::ReleasedTarget(const bool IN InMulti /* = false */, const bool IN InByMe /* = true */)
{
	if (IsTargetableObject() == false)
		return;

	// ���̷��߿��� Ÿ���� ����� �ʴ´�.
	if (bGyroMode)
		return;

	// [ 2019-12-5 : kyu ] Press�Է��߿��� Ÿ�� ������ �ʴ´�
	if (bPressedInput)
		return;

	ShowOffTargetUI();
}

void UWRActorComponentInteractionObject::ShowOnTargetUI()
{
	// [ 2019-8-30 : kyu ] UI�� �ߴ� ���� �ֱ� ������ ��û �� ������¸� �Ǵ�.
	if (bReservedUIOpen)
		return;

	if (WRCharacterManager::Get()->GetMyCharacter()->GetCurrentFSMType() == EWRFSM::Interact)
		return;

	// [ 2019-10-16 : kyu ] Ÿ�� �����Ҷ��� �Ҵ�
	if (IsTargetableObject() == false)
		return;

	// ���� UI�� ������ ����
	if (WRUIManager::Get()->FindUIData(GetOwnerCharacter(), TargetUIKind) != nullptr)
		return;


	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
		{
			// [ 2019-8-28 : kyu ] ������ ���뿡 ���� â ũ�Ⱑ �������̾�� �Ѵ�.
			InCombinedPanelData.WidgetComponentBase->SetDrawAtDesiredSize(true);
			//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::IO_TargetUIFixedSize) * 0.0001f);

			// ������� ����
			bReservedUIOpen = false;

			// ��ȣ�ۿ� ����, ���� ���� ���ο� ���� UI Ȱ��/��Ȱ��
			UpdateTargetUI();

			// ���̶���Ʈ ������Ʈ
			UpdateHighLight();
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::IOPopup, GetWidgetComponentShowType(), &Delegate, GetOwnerCharacter());
	WidgetInfo.SetTransform(GetWidgetTransform());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//FTransform WCTransform = GetWidgetTransform();
	//BitFlag16 WidgetCompFlag = GetWidgetComponentShowType();
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::IOPopup, GetOwnerCharacter(), WCTransform, WidgetCompFlag, &Delegate);

	// ������� ����
	bReservedUIOpen = true;
}

void UWRActorComponentInteractionObject::ShowOffTargetUI()
{
	WRUIManager::Get()->CloseUI(GetOwnerCharacter(), TargetUIKind);

	// ������� ����
	bReservedUIOpen = false;

	// ���̶���Ʈ ������Ʈ
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
// 	// TargetUI�� ���ͷ��� �÷��̾ '��'�� �ƴϸ� ó���� �ʿ䰡 ����
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
//	:: �Է�
//====================================================================================

bool UWRActorComponentInteractionObject::OnPressedEvent(const EWRInput IN InInputType)
{
	// ! ����� �ٶ� ���·� �Է��� ����

	if (IsInteracting())
	{
		if (InteractionPlayer != nullptr && InteractionPlayer->IsMe() == false)
		{
			WRNoticeManager::Get()->AddNotice(FString(""));
		}
		return true;
	}

	bInteracting = false;

	// �Է¿� �´� ��ȣ�ۿ��� �ִ°�?
	EWRIOInteractionType MatchInteractionType = FindMatchKeyEvent(InInputType);
	switch (MatchInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (IsInteractionableState() == false)
		{
			return false; // ��ȣ�ۿ� �Ұ����� ����
		}
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		if (IsHookableState() == false)
		{
			return false; // ���� �Ұ����� ����
		}
	}
	break;

	case EWRIOInteractionType::None:
	{
		// + ��ġ�� ���׸���� IO : [ȸ��]
		ISN_t InteriorISN = GetOwnerObject()->GetISN();
		if (InteriorISN != INVALID_ISN)
		{
			if (WRInteriorManager::Get()->IsSpawnedByMe(InteriorISN))
			{
				// ȸ���� XŰ��
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

	// [ 2019-11-27 : kyu ] ���� �Է��� ���� �׶����� ��ȣ�ۿ� �����ΰ��̴�.

	// ��ȣ�ۿ� �õ�
	OnReq_InteractionStart(MatchInteractionType);

	return true;
}

bool UWRActorComponentInteractionObject::OnReleasedEvent(const EWRInput IN InInputType)
{
	if (IsInteracting() == false)
		return false;

	// �Է¿� �´� ��ȣ�ۿ��� �ִ°�?
	EWRIOInteractionType MatchInteractionType = FindMatchKeyEvent(InInputType);
	switch (MatchInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (IsInteractionableState() == false)
		{
			return false; // ��ȣ�ۿ� �Ұ����� ����
		}
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		if (IsHookableState() == false)
		{
			return false; // ���� �Ұ����� ����
		}
	}
	break;
	default:
	{
		return false;
	}
	break;
	}

	// + PressTime üũ �߰�
	if (ObjectTable->DirectKeyPressTime != 0.0f)
	{
//		ReleaseProgressing();

		// ������ �� ���� ��Ŷ ����
		OnReq_InteractionStop();
	}

	return true;
}

void UWRActorComponentInteractionObject::OnPadEvent(const EWRInput IN InEvent)
{
	// ���̷� ����� ���� �Է� ����
	if (bGyroMode == false)
		return;

	if (FindMatchPadEvent(InEvent) == false)
		return; // �Է¿� �´� ��ȣ�ۿ��� �ִ°�?

	if (IsInteractionableState() == false)
		return; // ��ȣ�ۿ� �Ұ����� ����

	// ���̷� ��� ����
	OnGyroEnd();

	// ��ȣ�ۿ� �Ϸ� ��û
	OnReq_InteractionEnd();
}

void UWRActorComponentInteractionObject::UpdatePressedTime(float IN InDeltaTime)
{
	if (ObjectTable == nullptr)
		return;

	if (ObjectTable->DirectKeyPressTime == 0.0f)
		return;

	// [ 2019-12-4 : kyu ] �Է��߿� �̵��� �Է� ����
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
				OnReq_InteractionEnd(); // ��ȣ�ۿ� �Ϸ� ��û
			}

			ShowOffProgress(true);	// ���α׷��� �Ϸ�
		}
	}
}

void UWRActorComponentInteractionObject::ReleaseProgressing()
{
	if (ObjectTable->DirectKeyPressTime == 0.0f)
		return;

	bPressedInput = false;
	fPressedTime = 0.0f;

	ShowOffProgress(false); // ���α׷��� ���
}

EWRIOInteractionType UWRActorComponentInteractionObject::FindMatchKeyEvent(const EWRInput IN InInputType)
{
	if (ObjectTable->DirectKey == InInputType)
	{
		return EWRIOInteractionType::DirectInteraction;
	}

// [ 2019-9-30 : kyu ] *** Hook�� ���� Ű �Է��� ��ų�����͸� �����ؾ��Ѵ�.
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
		return false; // ���� �Ұ��� ����

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

	// [ 2019-12-4 : kyu ] ���̷��߿� �̵��� ���̷� ����
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

	// GyroType�� Pull�̰ų� Push�� Update�� �ƴ� ������ �Է¿��� ��´�
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
				//--- �����̴� ��
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
				//--- ����
				StopGyroDirection(pGyroStep);
			}
		}
		else if (pGyroStep->Axis == EWRIOGyroAxis::Y)
		{
			if (CalcGyroLerpDegree(InDeltaTime, GyroVector.Z, pGyroStep))
			{
				//--- �����̴� ��
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
				//--- ����
				StopGyroDirection(pGyroStep);
			}
		}
		else if (pGyroStep->Axis == EWRIOGyroAxis::Z)
		{
			if (CalcGyroLerpDegree(InDeltaTime, GyroVector.Y, pGyroStep))
			{
				//--- �����̴� ��
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
				//--- ����
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
				
				// ��ȣ�ۿ� �Ϸ� ��û
				OnReq_InteractionEnd();
			}
			else
			{
				if (pNextGyroStep->Axis == EWRIOGyroAxis::None)
				{
					OnGyroEnd();
					
					// ���� ���̷� ���� �ȵǾ����� ? ��ȣ�ۿ� �Ϸ� ��û
					OnReq_InteractionEnd();
				}
				else
				{
					// ���� ���̷� �������� !
					GyroStep = NextGyroStep;

					// UI ����
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
	// ���̷� ȸ�� �� �������� Gravity�� �����Ͽ� �������µ�
	// �е� ���� �� Gravity���� �ζٴ� ������ �������� �ⷷ���� ũ�� �����ΰ����� ����, �����߿� ������ �ʴ´�

	// [ 2019-12-12 : kyu ] ���ʽ��� �� Lerp��ŭ �������� ���� �̵� �����ϵ��� ����
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
		// LerpTermDegree��ŭ�� ���� ���ܼ� ���۰��� ���� �� ��ǥ������ ���簢���� �����Ѵ�.
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
	float fDiff = FMath::Abs(fGoalDegree - InCurDegree); // �������� ����

	float fPrev = 0.f;
	float fNext = 0.f;
	for (FWRIOGyroDirection& GyroDirection : InGyroInfo->Directions)
	{
		fNext = GyroDirection.RoundDegree;
		if (fPrev < fDiff && fNext > fDiff)
		{
			// ���� �ߺ�üũ ����
			if (fLerpSectionMin == fPrev && fLerpSectionMax == fNext)
				return;

			fLerpSectionMin = fPrev;
			fLerpSectionMax = fNext;

			// ����
			ChangeForceFeedback(&GyroDirection);
			// VFX
			ChangeVFX(&GyroDirection);
			// SFX
			ChangeSFX(&GyroDirection);

			// ����TID�� �ְ�, �̹� �÷��� ���� TID�� �ٸ����� �ٽ� �÷���
// 			if (GyroDirection.ForceFeedbackTID != INVALID_TABLE_TID
// 				&& GyroDirection.ForceFeedbackTID != GyroForceFeedbackTID)
// 			{
// 				WRInputSystemManager::Get()->ClientStopForceFeedback(GyroForceFeedbackTID);
// 
// 				GyroForceFeedbackTID = GyroDirection.ForceFeedbackTID;
// 				WRInputSystemManager::Get()->ClientPlayForceFeedback(GyroForceFeedbackTID);
// 			}
			// ����Ʈ �����Ͱ� �ְ�, �̹� ������ �����Ϳ� �ٸ����� �����ϰ� �ٽ� ����
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
			// ���� �����Ͱ� �ְ�, �̹� ������ �����Ϳ� �ٸ����� �����ϰ� �ٽ� ����
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
	// ���� ���� ����
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

	// �� ���� ���
	if (InNewDirection->ForceFeedbackTID != INVALID_TABLE_TID)
	{
		WRInputSystemManager::Get()->ClientPlayForceFeedback(InNewDirection->ForceFeedbackTID);
	}
}

void UWRActorComponentInteractionObject::ChangeVFX(FWRIOGyroDirection* IN InNewDirection)
{
	// ���� VFX ����
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
				// ���� ȿ���� �°����
				InNewDirection->pFX = CurGyroDirection->pFX;
				return;
			}
		}
	}

	// �� VFX ���
	if (InNewDirection->VFX.IsNull() == false)
	{
		InNewDirection->pFX = WRVFXManager::Get()->Spawn(InNewDirection->VFX.LoadSynchronous(), GetOwnerCharacter(), nullptr);
	}
}

void UWRActorComponentInteractionObject::ChangeSFX(FWRIOGyroDirection* IN InNewDirection)
{
	// ���� SFX ����
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
				// ���� ���尡 �°����
				InNewDirection->pAudioComp = CurGyroDirection->pAudioComp;
				return;
			}
		}
	}

	// �� SFX ���
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
		// ���ͷ��� ���̰� ���ͷ����÷��̾ ���� �ߴܿ�û
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
	OnGyroEnd();			// ���̷� ����
	OnReq_InteractionEnd();	// ��ȣ�ۿ� �Ϸ� ��û
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

	// ������, ���̷� �� �߰� [����]�� �� �ִ� ���� �߰� �۾� �Ϸ� �� End�� ������.
	// �� �� �ִ� �� ����� ������ ��� �ٷ� End�� ������.
	switch (CurrentInteractionType)
	{
	case EWRIOInteractionType::DirectInteraction:
	{
		if (ObjectTable->DirectKeyPressTime != 0.0f)
		{
			ShowOnProgress(ObjectTable->DirectKeyPressTime); // ��ȣ�ۿ� ������
		}
		else
		{			
			if (IsUseGyro())
			{
				OnGyroStart(); // ���̷� ������ ������ ���̷� ����
			}
			else
			{
				OnReq_InteractionEnd();	// ��ȣ�ۿ� �Ϸ� ��û
			}
		}
	}
	break;
	case EWRIOInteractionType::HookInteraction:
	{
		OnReq_InteractionEnd();	// ��ȣ�ۿ� �Ϸ� ��û (������ ���� ���ÿ� Start->End ����. �������� �� ��ȣ�ۿ����̶���� ���°� ������ �ȵǱ� ����)
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
	
	// Ƚ�� üũ�ؼ� Unsuable�� ����
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
		// [ 2019-11-29 : kyu ] ������ ���� ���ͷ����� ������ �� ���� �ٸ� ������� ������ �����ش�.
		if ((EWRIOInteractionType)InInteractionType == EWRIOInteractionType::HookInteraction)
		{
			WRCharacterManager::Get()->HookCutAllRelatedTarget(GetOwnerCharacter(), true);
		}
	}
	else
	{
		// [ 2019-11-29 : kyu ] ������ ���� ���ͷ����� ������ �� ���� �ٸ� ������� ������ �����ش�.
		if ((EWRIOInteractionType)InInteractionType == EWRIOInteractionType::HookInteraction)
		{
			WRCharacterManager::Get()->HookCutAllRelatedTarget(GetOwnerCharacter(), false);
		}

		// Ÿ���� �۵� �� ����� ������ ������ �����Ѵ�
		AfterProcess_GyroResult();
		AfterProcess_Attached();
	}	
}