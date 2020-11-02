// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAnimNotifyState.h"
#include <Animation/AnimNotifies/AnimNotify.h>
#include "WRAnimInstance.h"

#include "../Character/WRCharacter.h"
#include "../Character/WRCharacterMine.h"

#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRObjectInteractionManager.h"
#include "Manager/WRUIManager.h"

#include "Utility/WRActorUtility.h"

#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"

void UWRAnimNotifyState_Blocking::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->BeginCombatGimmickAnimNotifyState(EWRCombatGimmickState::Blocking);
}

void UWRAnimNotifyState_Blocking::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->EndCombatGimmickAnimNotifyState(EWRCombatGimmickState::Blocking);
}

void UWRAnimNotifyState_LeftAttack::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	//애니 동기화 문제로 주석
// 	if (WRActorUtility::IsAttackableRange(OwnerCharacter, pMe) == false)
// 		return;

	AnimInstance->BeginCombatGimmickAnimNotifyState(EWRCombatGimmickState::AvoidingLeft);

	if (pMe->GetTargetCharacter() == OwnerCharacter)
	{	
		// [ 2019-9-19 : animal14 ] 포스트 프로세스 적용
		//WRMaterialParameterCollectionManager::Get()->SetRight();
		//WRMaterialParameterCollectionManager::Get()->SetEnpcAttack(true);

		WRInputSystemManager::Get()->SetHMDStartPositionForAvoid(true);
	}

	float AnimRate = WRConsoleManager::Get()->GetFloat(EWRConsole::Anim_AvoidSectionRate);
	OwnerCharacter->SetAnimMontageRate(AnimRate);
}

void UWRAnimNotifyState_LeftAttack::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->EndCombatGimmickAnimNotifyState(EWRCombatGimmickState::AvoidingLeft);

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (OwnerCharacter)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			// [ 2019-9-19 : animal14 ] 포스트 프로세스 적용
			//WRMaterialParameterCollectionManager::Get()->SetEnpcAttack(false);

			if (pMe->GetTargetCharacter() == OwnerCharacter)
			{
				WRInputSystemManager::Get()->SetHMDStartPositionForAvoid(false);
			}
		}

		OwnerCharacter->SetAnimMontageRate(1.0f);
	}
}

void UWRAnimNotifyState_RightAttack::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	//애니 동기화 문제로 주석
// 	if (WRActorUtility::IsAttackableRange(OwnerCharacter, pMe) == false)
// 		return;

	AnimInstance->BeginCombatGimmickAnimNotifyState(EWRCombatGimmickState::AvoidingRight);

	if (pMe->GetTargetCharacter() == OwnerCharacter)
	{
		// [ 2019-9-19 : animal14 ] 포스트 프로세스 적용
		//WRMaterialParameterCollectionManager::Get()->SetLeft();
		//WRMaterialParameterCollectionManager::Get()->SetEnpcAttack(true);

		WRInputSystemManager::Get()->SetHMDStartPositionForAvoid(true);
	}

	float AnimRate = WRConsoleManager::Get()->GetFloat(EWRConsole::Anim_AvoidSectionRate);
	OwnerCharacter->SetAnimMontageRate(AnimRate);
}

void UWRAnimNotifyState_RightAttack::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->EndCombatGimmickAnimNotifyState(EWRCombatGimmickState::AvoidingRight);

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (OwnerCharacter)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			// [ 2019-9-19 : animal14 ] 포스트 프로세스 적용
			//WRMaterialParameterCollectionManager::Get()->SetEnpcAttack(false);

			if (pMe->GetTargetCharacter() == OwnerCharacter)
			{
				WRInputSystemManager::Get()->SetHMDStartPositionForAvoid(false);
			}
		}

		OwnerCharacter->SetAnimMontageRate(1.0f);
	}
}

void UWRAnimNotifyState_Parrying::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	if (WRActorUtility::IsAttackableRange(OwnerCharacter, pMe) == false)
		return;
	
	const AWRCharacter* MyCharacterTarget = pMe->GetTargetCharacter();
	if (MyCharacterTarget != OwnerCharacter)
		return;

	AnimInstance->BeginCombatGimmickAnimNotifyState(EWRCombatGimmickState::Parrying);

	// [ 2019-9-19 : animal14 ] 머티리얼 적용
	if (OwnerCharacter->IsMe())
	{
	}
	else
	{
		OwnerCharacter->SetMaterialParameterValue("Parrying", 1.0F);
		OwnerCharacter->SetMaterialParameterValue("LeftRight", 1.0F);
	}

	if (pMe->GetTargetCharacter() == OwnerCharacter)
	{
		// [ 2019-9-19 : animal14 ] 포스트 프로세스 적용
		//WRMaterialParameterCollectionManager::Get()->SetRight();
		//WRMaterialParameterCollectionManager::Get()->SetParrying(true);
		//WRMaterialParameterCollectionManager::Get()->SetEnpcAttack(true);
	}

	// [firefirefox 20/01/06] : 추후 동기식 로드로 변경해야함
	// [firefirefox 20/03/03] : 기획 요처응로 삭제
	// WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Parrying, FTransform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, -80), FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr);
}

void UWRAnimNotifyState_Parrying::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->EndCombatGimmickAnimNotifyState(EWRCombatGimmickState::Parrying);

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (OwnerCharacter)
	{
		// [ 2019-9-19 : animal14 ] 머티리얼 적용
	// [ 2019-9-19 : animal14 ] 머티리얼 적용
		if (OwnerCharacter->IsMe())
		{
		}
		else
		{
			OwnerCharacter->SetMaterialParameterValue("Parrying", 0.0F);
			OwnerCharacter->SetMaterialParameterValue("LeftRight", 1.0F);
		}

		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			// [ 2019-9-19 : animal14 ] 포스트 프로세스 적용
			//WRMaterialParameterCollectionManager::Get()->SetEnpcAttack(false);
			//WRMaterialParameterCollectionManager::Get()->SetParrying(false);
		}
	}

	// [firefirefox 20/03/03] : 기획 요처응로 삭제
	// WRUIManager::Get()->CloseUI(EWRUIPanelKind::Parrying);
}

void UWRAnimNotifyState_RightAttackingCollision::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	OwnerCharacter->SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart::RightHand, ECollisionEnabled::QueryOnly);

}

void UWRAnimNotifyState_RightAttackingCollision::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	OwnerCharacter->SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart::RightHand, ECollisionEnabled::NoCollision);
}

void UWRAnimNotifyState_LeftAttackingCollision::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	OwnerCharacter->SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart::LeftHand, ECollisionEnabled::QueryOnly);
}

void UWRAnimNotifyState_LeftAttackingCollision::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	OwnerCharacter->SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart::LeftHand, ECollisionEnabled::NoCollision);
}

void UWRAnimNotifyState_ForceFeedback::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	if (WRInputSystemManager::IsValid() == true && WRCharacterManager::IsValid() == true)
	{
		// [ 2019-11-20 : kyu ] 주변에게 알리지 않는 경우
		if (BroadCast == false)
		{
			// Owner가 내가 아니면 진동 울리지 않음
			if (OwnerCharacter->GetActorID() != WRCharacterManager::Get()->GetMyCharacter()->GetActorID())
			{
				return;
			}
		}

		WRInputSystemManager::Get()->ClientPlayForceFeedback(ForceFeedbackTID);
	}
}

void UWRAnimNotifyState_ForceFeedback::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	if (WRInputSystemManager::IsValid() == true && WRCharacterManager::IsValid() == true)
	{
		// [ 2019-11-20 : kyu ] 주변에게 알리지 않는 경우
		if (BroadCast == false)
		{
			// Owner가 내가 아니면 진동 울리지 않음
			if (OwnerCharacter->GetActorID() != WRCharacterManager::Get()->GetMyCharacter()->GetActorID())
			{
				return;
			}
		}

		WRInputSystemManager::Get()->ClientStopForceFeedback(ForceFeedbackTID);
	}
}

void UWRAnimNotifyState_InteractionFABRIK::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	UpdateFABRIK(BranchingPointPayload);
}

void UWRAnimNotifyState_InteractionFABRIK::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime)
{
	UpdateFABRIK(BranchingPointPayload);
}

void UWRAnimNotifyState_InteractionFABRIK::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	AWRCharacterPlayer* OwnerPlayer = Cast<AWRCharacterPlayer>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerPlayer)
		return;

// 	if (OwnerPlayer->IsMe() == false)
// 		return;

	OwnerPlayer->SetTransformForFABRIK();
}

void UWRAnimNotifyState_InteractionFABRIK::UpdateFABRIK(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	AWRCharacterPlayer* OwnerPlayer = Cast<AWRCharacterPlayer>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerPlayer)
		return;

// 	if (OwnerPlayer->IsMe() == false)
// 		return;

	FTransform LHandTf = FTransform::Identity;
	FTransform RHandTf = FTransform::Identity;

	AWRCharacter* pInteractionTarget = WRObjectInteractionManager::Get()->GetSingleInteractionTarget();
	if (pInteractionTarget == nullptr)
		return;

	// pInteractionTarget의 소켓TF를 pTarget에게로 넘김
	USkeletalMeshComponent* pSkeletalMeshComp = pInteractionTarget->GetMesh();
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
		TArray<UActorComponent*> LHandTransformComponents = pInteractionTarget->GetComponentsByTag(USceneComponent::StaticClass(), "LHand");
		if (LHandTransformComponents.IsValidIndex(0))
		{
			USceneComponent* pCastedComponent = Cast<USceneComponent>(LHandTransformComponents[0]);
			LHandTf = pCastedComponent->GetComponentTransform();
		}
		TArray<UActorComponent*> RHandTransformComponents = pInteractionTarget->GetComponentsByTag(USceneComponent::StaticClass(), "RHand");
		if (RHandTransformComponents.IsValidIndex(0))
		{
			USceneComponent* pCastedComponent = Cast<USceneComponent>(RHandTransformComponents[0]);
			RHandTf = pCastedComponent->GetComponentTransform();
		}
	}
	OwnerPlayer->SetTransformForFABRIK(RHandTf, LHandTf);
}
