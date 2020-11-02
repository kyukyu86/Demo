// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentLadder.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"

#include "Utility/WRActorUtility.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRWeaponTable.h"
#include "Table/WRPlayerTable.h"

#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/EnvActor/Ladder/WRLadder.h"
#include "Actor/Animation/WRAnimInstance.h"

#include "Manager/WRInputSystemManager.h"
#include "Enum/EWRItem.h"
#include "Manager/WRItemManager.h"
#include "Network/IMMessageSender.h"



void UWRActorComponentLadder::OnCreateComponent()
{
	Super::OnCreateComponent();

}

void UWRActorComponentLadder::OnDestroyComponent()
{

	Super::OnDestroyComponent();
}

void UWRActorComponentLadder::OnFSMLadderBegin(const FWRLadderData& IN InData)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return;

	switch (LadderData.State)
	{
	case EWRLadderState::Start:
	{

	}
	break;
	case EWRLadderState::Move:
	{
		// Values
		bMove = true;
		fDeltaTime = 0.f;
		fMaxTime = CastedPlayer->GetLadderMoveTime();
		fMaxMove = GetUsingLadder()->GetHandleOffset();
		fSrcZ = CastedPlayer->GetActorLocation().Z;
	}
	break;
	case EWRLadderState::Finish:
	{

	}
	break;
	}
}

void UWRActorComponentLadder::OnFSMLadderTick(float DeltaTime)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return;

	switch (LadderData.State)
	{
	case EWRLadderState::Start:
	{
		if (CastedPlayer->IsMe())
		{
			if (LadderData.Direction == EWRLadderDirection::Down)
			{
				// [ 2020-2-21 : kyu ] 밑으로 내려갈 때 카메라 이동시켜주려고 했는데 HMD끼면 어차피 안됨
// 				USkeletalMeshComponent* pMesh = CastedPlayer->GetMesh();
// 				UCameraComponent* CameraComponent = Cast<UCameraComponent>(CastedPlayer->GetComponentByClass(UCameraComponent::StaticClass()));
// 				FVector vDstLocation = vSrcCameraRelativeLocation;
// 				vDstLocation.Z = pMesh->RootBoneTranslation.Z;
// 				CameraComponent->SetRelativeLocation(vDstLocation);
			}
			else
			{

			}
		}
	}
	break;
	case EWRLadderState::Move:
	{
		if (bMove == false)
			return;

		fDeltaTime += DeltaTime;
		float fMove = fMaxMove * (fDeltaTime / fMaxTime);
		if (fMove < fMaxMove)
		{
			FVector vLoc = CastedPlayer->GetActorLocation();
			if (LadderData.Direction == EWRLadderDirection::Up)
			{
				vLoc.Z = fSrcZ + fMove;
			}
			else if (LadderData.Direction == EWRLadderDirection::Down)
			{
				vLoc.Z = fSrcZ - fMove;
			}
			CastedPlayer->SetActorLocation(vLoc);
		}
		else
		{
			bMove = false;
		}
	}
	break;
	case EWRLadderState::Finish:
	{
		if (CastedPlayer->IsMe())
		{
			// [ 2020-2-21 : kyu ] 위로 올라갈 때 카메라 이동시켜주려고 했는데 HMD끼면 어차피 안됨
// 	 		USkeletalMeshComponent* pMesh = OwnerCharacter->GetMesh();
// 	 		UCameraComponent* CameraComponent = Cast<UCameraComponent>(OwnerCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
// 	 		FVector vDstLocation = vSrcCameraRelativeLocation;
// 	 		vDstLocation.Z = pMesh->RootBoneTranslation.Z;
// 	 		CameraComponent->SetRelativeLocation(vDstLocation);
		}
	}
	break;
	}
}

void UWRActorComponentLadder::OnFSMLadderEnd()
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return;

	switch (LadderData.State)
	{
	case EWRLadderState::Start:
	{
		
	}
	break;
	case EWRLadderState::Move:
	{
		// 최종 위치 재설정
		FVector vLoc = CastedPlayer->GetActorLocation(); // 케릭터의 좌표에서
		vLoc.Z = GetUsingLadder()->GetCalculatedCharLocationZ(LadderData.TargetHandle);
		CastedPlayer->SetActorLocation(vLoc);

		bMove = false;
	}
	break;
	case EWRLadderState::Finish:
	{
		// Show Weapon
		CastedPlayer->SetEquipPartVisibility(EWREquipPart::LeftHand, true);
		CastedPlayer->SetEquipPartVisibility(EWREquipPart::RightHand, true);

		// Change State
		CastedPlayer->GetAnimInstance()->SetLadderIdle(false);
		CastedPlayer->GetAnimInstance()->SetLadderHand(EWRLadderHand::None);
		CastedPlayer->GetCharacterMovement()->GravityScale = 1.f;
		CastedPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		// Modify Transform
		if (LadderData.Transform.Equals(FTransform::Identity) == false)
		{
			FNavLocation OutNavLocation;
			bool bResult = WRActorUtility::ProjectPointToNavigation(CastedPlayer, LadderData.Transform.GetLocation(), OutNavLocation);
			if (bResult)
			{
				float OutHeight = 0.0f;
				if (WRActorUtility::ModifyZHeightForClient(CastedPlayer, OutNavLocation.Location, OutHeight))
				{
					OutNavLocation.Location.Z = OutHeight;
				}
				LadderData.Transform.SetLocation(OutNavLocation.Location);
			}
			CastedPlayer->SetActorTransform(LadderData.Transform);
		}

// 		if (CastedPlayer->IsMe())
// 		{
// 		 	USkeletalMeshComponent* pMesh = CastedPlayer->GetMesh();
// 		 	UCameraComponent* CameraComponent = Cast<UCameraComponent>(CastedPlayer->GetComponentByClass(UCameraComponent::StaticClass()));
// 		 	CameraComponent->SetRelativeLocation(vSrcCameraRelativeLocation);
// 		 	WRInputSystemManager::Get()->ResetHMDOrientation();
// 		}
	}
	break;
	}
}

//====================================================================================

void UWRActorComponentLadder::SetLadderOnAppear(const int32 IN InHandleIndex, const int32 IN InHand)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return;

	if (InHandleIndex == DEF_INVALID_LADDER_HANDLE_INDEX)
		return;

	UsingLadder = nullptr;
	TArray<AActor*> OverlapedActors;
	GetOwnerCharacter()->GetOverlappingActors(OverlapedActors, AWRLadder::StaticClass());
	if (OverlapedActors.IsValidIndex(0))
	{
		UsingLadder = Cast<AWRLadder>(OverlapedActors[0]);
	}

	if (UsingLadder == nullptr)
		return;

	LadderData.Reset();
	LadderData.Hand = (EWRLadderHand)InHand;
	LadderData.LadderHandleIndex = InHandleIndex;

	FWRLadderHandle* pHandle = UsingLadder->GetHandle(InHandleIndex);
	if (pHandle == nullptr)
		return;

	// 상태 변경 먼저
	GetOwnerCharacter()->GetAnimInstance()->SetLadderIdle(true);
	GetOwnerCharacter()->GetAnimInstance()->SetLadderHand((EWRLadderHand)InHand);
	GetOwnerCharacter()->GetCharacterMovement()->GravityScale = 0.f;
	CastedPlayer->SetForceStop();
	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::LeftHand, false);
	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::RightHand, false);
	GetOwnerCharacter()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 위치 조정
	FTransform tf = FTransform::Identity;
	if (UsingLadder->IsTopHandle(InHandleIndex))
	{
		tf = UsingLadder->GetTopStartPosition();
	}
	else
	{
		tf = UsingLadder->GetBottomPosition();
	}
	FVector ModifyLocation = tf.GetLocation();
	ModifyLocation.Z = UsingLadder->GetCalculatedCharLocationZ(pHandle); //pHandle->GetLocation().Z + UsingLadder->GetCharAttachOffset();
	tf.SetLocation(ModifyLocation);
	GetOwnerCharacter()->SetActorTransform(tf);

}

bool UWRActorComponentLadder::DetermineLadder(EWRInput IN InInput, EWRLadderDirection IN InDirection)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return false;

	if (InInput != EWRInput::L1 && InInput != EWRInput::R1)
		return false; // 상단에서 다른 입력으로 내려옴 처리 될 것

	EWRFSM CurFSM = CastedPlayer->GetCurrentFSMType();
	if (CurFSM == EWRFSM::Ladder)
		return true;  // 입력 무시해야하기 때문

	switch (LadderData.Hand)
	{
	case EWRLadderHand::None:
	{
		if (OverlapedLadder == nullptr)
			return false;		

		int32 nHandleIndex = 0;
		FWRLadderHandle* pNearestHandle = OverlapedLadder->GetNearestHandle(CastedPlayer);
		if (OverlapedLadder->IsTopHandle(pNearestHandle))
		{
			nHandleIndex = OverlapedLadder->GetTopStartHandleNumber();
		}
		else
		{
			pNearestHandle = OverlapedLadder->GetBottomStartHandle();
			nHandleIndex = OverlapedLadder->GetBottomStartHandleNumber();
		}

		// [ 2020-2-10 : kyu ] *** 혹시나 점프 중 사다리 중간에 닿았을 경우 pNearestHandle의 HandleIndex를 구해서 셋팅해야한다. 일단은 Top 아니면 Bottom

		EWRLadderHand eHand = EWRLadderHand::None;
		if (InInput == EWRInput::L1)
			eHand = EWRLadderHand::Left;
		else if (InInput == EWRInput::R1)
			eHand = EWRLadderHand::Right;

		if (pNearestHandle->HandType != eHand)
			return false;

		if (OnBeginLadder(OverlapedLadder, nHandleIndex, eHand))
		{
			return true;
		}
	}
	break;
	case EWRLadderHand::Left:
	{
		if (UsingLadder == nullptr)
			return false;

		// [ 2020-2-20 : kyu ] 바라보는 방향의 손잡이의 입력은 밑을 볼때는 반대다.
		EWRInput NextInput = EWRInput::R1;
		if (InDirection == EWRLadderDirection::Down)
			NextInput = EWRInput::L1;

		if (InInput != NextInput)
			return true; // 입력 무시해야하기 때문

		if (CheckLadderFinishLine(InDirection))
		{
			// Top이거나 Bottom인 경우에만 들어온다.
			OnEndLadder(UsingLadder, LadderData.LadderHandleIndex);
		}
		else
		{
			if (InDirection == EWRLadderDirection::Up)
				++LadderData.LadderHandleIndex;
			else if (InDirection == EWRLadderDirection::Down)
				--LadderData.LadderHandleIndex;
			OnMoveLadder(UsingLadder, LadderData.LadderHandleIndex, EWRLadderHand::Right, InDirection);
		}
		return true;
	}
	break;
	case EWRLadderHand::Right:
	{
		if (UsingLadder == nullptr)
			return false;

		// [ 2020-2-20 : kyu ] 바라보는 방향의 손잡이의 입력은 밑을 볼때는 반대다.
		EWRInput NextInput = EWRInput::L1;
		if (InDirection == EWRLadderDirection::Down)
			NextInput = EWRInput::R1;

		if (InInput != NextInput)
			return true; // 입력 무시해야하기 때문

		if (CheckLadderFinishLine(InDirection))
		{
			// Top이거나 Bottom인 경우에만 들어온다.
			OnEndLadder(UsingLadder, LadderData.LadderHandleIndex);
		}
		else
		{
			if (InDirection == EWRLadderDirection::Up)
				++LadderData.LadderHandleIndex;
			else if (InDirection == EWRLadderDirection::Down)
				--LadderData.LadderHandleIndex;
			OnMoveLadder(UsingLadder, LadderData.LadderHandleIndex, EWRLadderHand::Left, InDirection);
		}
		return true;
	}
	break;
	}
	return false;
}

bool UWRActorComponentLadder::OnBeginLadder(class AWRLadder* IN InLadder, const int32 IN InHandleIndex, const EWRLadderHand IN InHand)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return false;

	if (GetOwnerCharacter()->IsMe())	
	{
		// 위에서는 아래를 봐야한다
		if (InLadder->IsTopHandle(InHandleIndex))
		{
			if (WRObjectInteractionManager::Get()->GetLadderDirection() == EWRLadderDirection::Up)
				return false;
		}
		// 아래에서는 위를 봐야한다.
// 	 	else if (InLadder->IsBottomHandle(InHandleIndex))
// 	 	{
// 	 		if (WRObjectInteractionManager::Get()->GetLadderDirection() == EWRLadderDirection::Down)
// 	 			return false;
// 	 	}

		// 갈고리가 연결되어있으면 사다리 사용 불가	
		if (CastedPlayer->GetConnectedHookCableComponent() != nullptr)
			return false;	
	}	

	UsingLadder = InLadder;
	EWRLadderDirection LadderDirection = EWRLadderDirection::None;
	
	FString strSectionName = "";
	FTransform tf = FTransform::Identity;
	if (UsingLadder->IsTopHandle(InHandleIndex))
	{
		tf = InLadder->GetTopStartPosition();
		strSectionName = "StartTop";
		LadderDirection = EWRLadderDirection::Down;
	}
	else
	{
		tf = InLadder->GetBottomPosition();
		strSectionName = "StartBottom";
		LadderDirection = EWRLadderDirection::Up;
	}

	FWRLadderHandle* pHandle = UsingLadder->GetHandle(InHandleIndex);
	if (pHandle == nullptr)
		return false;

	UsingLadder->SetHandleIndex(InHandleIndex);

	// 상태 변경 먼저
	GetOwnerCharacter()->GetAnimInstance()->SetLadderIdle(true);
	GetOwnerCharacter()->GetAnimInstance()->SetLadderHand(InHand);
	GetOwnerCharacter()->GetCharacterMovement()->GravityScale = 0.f;
	CastedPlayer->SetForceStop();
	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::LeftHand, false);
	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::RightHand, false);
	GetOwnerCharacter()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 위치 조정
	FVector ModifyLocation = tf.GetLocation();
	ModifyLocation.Z = UsingLadder->GetCalculatedCharLocationZ(pHandle);
	tf.SetLocation(ModifyLocation);
	GetOwnerCharacter()->SetActorTransform(tf);
	USceneComponent* MeshParentComponent = GetOwnerCharacter()->GetMeshParentComponent();
	if (MeshParentComponent != nullptr)
	{
		MeshParentComponent->SetWorldRotation(tf.GetRotation());
	}

	// 애니
	LadderData.Reset();
	LadderData.State = EWRLadderState::Start;
	LadderData.TargetHandle = pHandle;
	LadderData.Direction = LadderDirection;
	LadderData.Hand = InHand;
	LadderData.LadderHandleIndex = InHandleIndex;
	WRFSMInfo fsmInfo(EWRFSM::Ladder, 0, 0, 0, false, &LadderData, strSectionName);
	GetOwnerCharacter()->DetermineFSMSignal(fsmInfo);

	if (GetOwnerCharacter()->IsMe())
	{
		WRCharacterManager::Get()->ResetVRRotateOrientation();
		//WRInputSystemManager::Get()->ResetHMDOrientation();
		// [ 2020-3-4 : kyu ] 카메라 회전에 대한 케릭터 Body회전 락
		WRInputSystemManager::Get()->SetRotationLock(true);
		// [ 2020-3-12 : kyu ] 사다리 탈 때 임시장비 버림
		WRItemManager::Get()->ThrowTemporaryItemAway(EWRReasonForThrowTemporaryItemAway::Ladder);

		UsingLadder->UpdateHandGuideOnlyForMe(InHandleIndex);
		UsingLadder->SetHandGuideVisibility(true);
		IMMessageSender::RequestBeginLadder(GetOwnerCharacter()->GetActorID(), InHandleIndex, 1);
	}

	return true;
}

void UWRActorComponentLadder::OnMoveLadder(class AWRLadder* IN InLadder, const int32 IN InHandleIndex, const EWRLadderHand IN InHand, const EWRLadderDirection IN InDirection)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return;

	UsingLadder = InLadder;

	FString strSectionName;
	if (InHand == EWRLadderHand::Left)
	{
		if (InDirection == EWRLadderDirection::Up)
			strSectionName = "MoveLeftUp";
		else if (InDirection == EWRLadderDirection::Down)
			strSectionName = "MoveRightDown";
	}
	else if (InHand == EWRLadderHand::Right)
	{
		if (InDirection == EWRLadderDirection::Up)
			strSectionName = "MoveRightUp";
		else if (InDirection == EWRLadderDirection::Down)
			strSectionName = "MoveLeftDown";
	}
	else
		return;

	FWRLadderHandle* pHandle = UsingLadder->GetHandle(InHandleIndex);
	if (pHandle == nullptr)
		return;
	UsingLadder->SetHandleIndex(InHandleIndex);

	// 상태 변경
	GetOwnerCharacter()->GetAnimInstance()->SetLadderIdle(true);
	GetOwnerCharacter()->GetAnimInstance()->SetLadderHand(InHand);
	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::LeftHand, false);
	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::RightHand, false);

	// 위치 조정 없음. FSMLadderMove 에서 처리함

	// 애니
	LadderData.Reset();
	LadderData.State = EWRLadderState::Move;
	LadderData.TargetHandle = pHandle;
	LadderData.Direction = InDirection;
	LadderData.Hand = InHand;
	LadderData.LadderHandleIndex = InHandleIndex;
	WRFSMInfo fsmInfo(EWRFSM::Ladder, 0, 0, 0, false, &LadderData, strSectionName);
	GetOwnerCharacter()->DetermineFSMSignal(fsmInfo);

	if (GetOwnerCharacter()->IsMe())
	{
		UsingLadder->UpdateHandGuideOnlyForMe(InHandleIndex);
		IMMessageSender::RequestMoveLadder(GetOwnerCharacter()->GetActorID(), InHandleIndex, (int32)InHand, (int32)InDirection);
	}
}

void UWRActorComponentLadder::OnEndLadder(class AWRLadder* IN InLadder, const int32 IN InHandleIndex)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return;

	FString strSectionName = "";
	FTransform tf = FTransform::Identity;
	if (InLadder->IsTopHandle(InHandleIndex))
	{
		tf = InLadder->GetTopEndPosition();
		strSectionName = "FinishTop";
	}
	else if (InLadder->IsBottomHandle(InHandleIndex))
	{
		tf = InLadder->GetBottomPosition();
		strSectionName = "FinishBottom";
	}
	else
		return;

	// 상태 변경 먼저
// 	this->GetAnimInstance()->SetLadderIdle(false);
// 	this->GetAnimInstance()->SetLadderHand(EWRLadderHand::None);
//	this->GetCharacterMovement()->GravityScale = 1.f;
//	this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
//	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 위치 조정
// 	if (tf.Equals(FTransform::Identity) == false)
// 	{
// 		FNavLocation OutNavLocation;
// 		bool bResult = WRActorUtility::ProjectPointToNavigation(this, tf.GetLocation(), OutNavLocation);
// 		if (bResult)
// 		{
// 			float OutHeight = 0.0f;
// 			if (WRActorUtility::ModifyZHeightForClient(this, OutNavLocation.Location, OutHeight))
// 			{
// 				OutNavLocation.Location.Z = OutHeight;
// 			}
// 			tf.SetLocation(OutNavLocation.Location);
// 		}
// 		this->SetActorTransform(tf);
// 		if (IsMe())
// 		{
// 			WRInputSystemManager::Get()->ResetHMDOrientation();
// 		}
// 	}

	// 애니
	LadderData.Reset();
	LadderData.State = EWRLadderState::Finish;
	LadderData.Transform = tf;
	WRFSMInfo fsmInfo(EWRFSM::Ladder, 0, 0, 0, false, &LadderData, strSectionName);
	GetOwnerCharacter()->DetermineFSMSignal(fsmInfo);

	if (GetOwnerCharacter()->IsMe())
	{
		// [ 2020-3-4 : kyu ] 카메라 회전에 대한 케릭터 Body회전 락
		WRInputSystemManager::Get()->SetRotationLock(false);
		UsingLadder->ReleaseFromMe(CastedPlayer);
		IMMessageSender::RequestEndLadder(GetOwnerCharacter()->GetActorID(), InHandleIndex);
	}

	UsingLadder = nullptr;
}

void UWRActorComponentLadder::CancelLadder()
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (CastedPlayer == nullptr)
		return;

	if (UsingLadder != nullptr)
	{
		// 상태 변경
		GetOwnerCharacter()->GetAnimInstance()->SetLadderIdle(false);
		GetOwnerCharacter()->GetCharacterMovement()->GravityScale = 1.f;
//		GetOwnerCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetOwnerCharacter()->DetermineFSMSignal(EWRFSM::Idle);
		GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::LeftHand, true);
		GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::RightHand, true);
		GetOwnerCharacter()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		if (GetOwnerCharacter()->IsMe())
		{
			// [ 2020-3-4 : kyu ] 카메라 회전에 대한 케릭터 Body회전 락
			WRInputSystemManager::Get()->SetRotationLock(false);
			UsingLadder->ReleaseFromMe(CastedPlayer);
			IMMessageSender::RequestCancelLadder(GetOwnerCharacter()->GetActorID());
		}

		LadderData.Reset();
		UsingLadder = nullptr;
	}
}

void UWRActorComponentLadder::SetOverlapLadderArea(AWRLadder* IN InLadder)
{
	OverlapedLadder = InLadder;
}

bool UWRActorComponentLadder::IsUsingLadder()
{
	if (GetUsingLadder() != nullptr || GetOwnerCharacter()->GetCurrentFSMType() == EWRFSM::Ladder)
		return true;

	return false;
}

bool UWRActorComponentLadder::CheckLadderFinishLine(EWRLadderDirection IN InDirection)
{
	if (InDirection == EWRLadderDirection::Up)
	{
		return UsingLadder->IsTopHandle(LadderData.LadderHandleIndex);
	}
	else if (InDirection == EWRLadderDirection::Down)
	{
		return UsingLadder->IsBottomHandle(LadderData.LadderHandleIndex);
	}
	return false;
}

void UWRActorComponentLadder::OnAck_BeginLadder(const int32 IN InHandleIndex, const int32 IN InHand)
{
	AWRLadder* TargetLadder = nullptr;
	TArray<AActor*> OverlapedActors;
	GetOwnerCharacter()->GetOverlappingActors(OverlapedActors, AWRLadder::StaticClass());
	if (OverlapedActors.IsValidIndex(0))
		TargetLadder = Cast<AWRLadder>(OverlapedActors[0]);
	if (TargetLadder == nullptr)
		return;

	OnBeginLadder(TargetLadder, InHandleIndex, (EWRLadderHand)InHand);
}

void UWRActorComponentLadder::OnAck_MoveLadder(const int32 IN InHandleIndex, const int32 IN InHand, const int32 IN InDirection)
{
	AWRLadder* TargetLadder = nullptr;
	TArray<AActor*> OverlapedActors;
	GetOwnerCharacter()->GetOverlappingActors(OverlapedActors, AWRLadder::StaticClass());
	if (OverlapedActors.IsValidIndex(0))
		TargetLadder = Cast<AWRLadder>(OverlapedActors[0]);
	if (TargetLadder == nullptr)
		return;

	OnMoveLadder(TargetLadder, InHandleIndex, (EWRLadderHand)InHand, (EWRLadderDirection)InDirection);
}

void UWRActorComponentLadder::OnAck_EndLadder(const int32 IN InHandleIndex)
{
	AWRLadder* TargetLadder = nullptr;
	TArray<AActor*> OverlapedActors;
	GetOwnerCharacter()->GetOverlappingActors(OverlapedActors, AWRLadder::StaticClass());
	if (OverlapedActors.IsValidIndex(0))
		TargetLadder = Cast<AWRLadder>(OverlapedActors[0]);
	if (TargetLadder == nullptr)
		return;

	OnEndLadder(TargetLadder, InHandleIndex);
}

void UWRActorComponentLadder::OnAck_CancelLadder()
{
	CancelLadder();
}
