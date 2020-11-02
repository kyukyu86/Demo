// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentInteraction.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"

#include "Utility/WRActorUtility.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRWeaponTable.h"
#include "Table/WRPlayerTable.h"

#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Actor/EnvActor/Ladder/WRLadder.h"
#include "Actor/Animation/WRAnimInstance.h"

#include "Enum/EWRItem.h"
#include "Network/IMMessageSender.h"

#include "Manager/WRItemManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRObjectInteractionManager.h"
#include "Manager/WRCharacterManager.h"

#include "WRActorComponentInteractionObject.h"
#include "WRActorComponentVR.h"

#include "UI/Panel/WRUIPanel_CrossHair.h"
#include "Manager/WRConsoleManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/WRInventoryManager.h"
#include "WRActorComponentMic.h"




void UWRActorComponentInteraction::OnCreateComponent()
{
	Super::OnCreateComponent();

}

void UWRActorComponentInteraction::OnDestroyComponent()
{
	// [ 2020-3-5 : kyu ] 삭제시 점유 해제
	if (OccupiedIO->IsValidLowLevel())
	{
		OccupiedIO->SetOccupiedPlayer(nullptr);
	}

	Super::OnDestroyComponent();
}

void UWRActorComponentInteraction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Debug_UpdateRecognitionIO(DeltaTime);
}

class AWRCharacterPlayer* UWRActorComponentInteraction::GetOwnerPlayer()
{
	if (GetOwnerCharacter()->IsPlayer() == false)
		return nullptr;

	return Cast<AWRCharacterPlayer>(GetOwnerCharacter());
}

//====================================================================================
//	Recognition
//====================================================================================

void UWRActorComponentInteraction::Debug_UpdateRecognitionIO(float IN InDeltaTime)
{
	if (GetOwnerCharacter()->IsMe() == false)
		return;

	AWRCharacterMine* OwnerMine = Cast<AWRCharacterMine>(GetOwnerCharacter());
	if (OwnerMine == nullptr)
		return;

	bool bCollisionShow = WRConsoleManager::Get()->GetBool(EWRConsole::IO_CollisionShow);;
	if (bCollisionShow == false)
		return;

	fRecognitionDistanceShowDelay += InDeltaTime;
	if (fRecognitionDistanceShowDelay > 1.0f)
	{
		fRecognitionDistanceShowDelay = 0.0f;
		UKismetSystemLibrary::DrawDebugCircle(OwnerMine, OwnerMine->GetActorLocation(), OwnerMine->GetRecognitionDistance(), 100, FColor::Cyan, 0.5f, 2.0f, FVector(0.0f, 1.0f, 0.0f), FVector(1.0f, 0.0f, 0.0f), true);	// 가로
	}
}

//====================================================================================
//	Interaction
//====================================================================================

void UWRActorComponentInteraction::OnInteractionStart(class AWRCharacterIO* IN InIO)
{
	AWRCharacterPlayer* OwnerPlayer = GetOwnerPlayer();
	if (OwnerPlayer == nullptr)
		return;

	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::LeftHand, false);
	GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::RightHand, false);

	if (OwnerPlayer->IsMe())
	{
		WRObjectInteractionManager::Get()->SetSingleInteractionTarget(InIO);
	}



	if (InIO != nullptr)
	{
		// Modify Transform
		FTransform tf = InIO->GetInteractionTransform();
		if (tf.Equals(FTransform::Identity) == false)
		{
			// Actor
//			OwnerPlayer->SetActorTransform(tf);

			// Mesh
// 			USceneComponent* MeshParentComponent = OwnerPlayer->GetMeshParentComponent();
// 			if (MeshParentComponent != nullptr)
// 			{
// 				MeshParentComponent->SetWorldRotation(tf.GetRotation());
// 			}

			if (OwnerPlayer->IsMe())
			{
				// HMD Rotation Sync
				AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(OwnerPlayer);
				if (MyCharacter != nullptr)
				{
					FRotator MyCharacterRotator = MyCharacter->GetActorRotation();
					FRotator DstRotatot = tf.Rotator();
					MyCharacter->GetRotator()->RotateCharacterWithHMD(DstRotatot);
				}
				//WRCharacterManager::Get()->ResetVRRotateOrientation();

				// Body Lock
				if (InIO->IsBodyLock())
				{
					WRInputSystemManager::Get()->SetRotationLock(true);
				}
			}
		}
	}	


	// [ 2020-4-21 : kyu ] 점유 설정. 내부에서 InteractionTransform으로 Attach한다
	SetOccupiedIO(InIO);
}

void UWRActorComponentInteraction::OnInteractionStop()
{
	if (GetOwnerCharacter()->IsMe())
	{
		WRObjectInteractionManager::Get()->SetInteractionType(EWRIOInteractionType::None);
		WRObjectInteractionManager::Get()->SetSingleInteractionTarget(nullptr);
	}

	if (OccupiedIO == nullptr)
	{
		if (GetOwnerCharacter()->IsInBattle())
		{
			GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::LeftHand, true);
			GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::RightHand, true);
		}
	}

	SetOccupiedIO(nullptr);
}

void UWRActorComponentInteraction::OnInteractionEnd()
{
	if (GetOwnerCharacter()->IsMe())
	{
		WRObjectInteractionManager::Get()->SetInteractionType(EWRIOInteractionType::None);
		WRObjectInteractionManager::Get()->SetSingleInteractionTarget(nullptr);
	}

	if (OccupiedIO == nullptr)
	{
		if (GetOwnerCharacter()->IsInBattle())
		{
			GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::LeftHand, true);
			GetOwnerCharacter()->SetEquipPartVisibility(EWREquipPart::RightHand, true);
		}
	}
}

//====================================================================================
//	Occupation
//====================================================================================

void UWRActorComponentInteraction::SetOccupiedIO(class AWRCharacterIO* IN InNewIO, const bool IN InIODestroy /*= false*/)
{
	AWRCharacterPlayer* OwnerPlayer = GetOwnerPlayer();
	if (OwnerPlayer == nullptr)
		return;
		
	if (InNewIO == nullptr)
	{
		// Release
		if (OccupiedIO != nullptr)
		{
			// [ 2020-8-4 : kyu ] 점유 이후 추가 작업 (by SubType)
			OnOccupationDeactivate();

			if (InIODestroy == false)
			{
				OccupiedIO->SetOccupiedPlayer(nullptr);
				// [ 2020-9-1 : kyu ] 점유중인 IO의 TargetArea 비활성->활성
				OccupiedIO->SetTargetAreaEnable(true);
			}

			if (OwnerPlayer->IsMe())
			{
				IMMessageSender::RequestUnoccupyIO();

				// Modify End Position
				TransformToOccupationEndPosition();

				UWRActorComponentInteractionObject* pIOComp = OccupiedIO->GetComponent<UWRActorComponentInteractionObject>();
				if (pIOComp != nullptr)
				{
					pIOComp->UpdateTargetUI();
				}

				// Body Rock Release
				if (OccupiedIO->IsBodyLock())
				{
					WRInputSystemManager::Get()->SetRotationLock(false);
				}

				// + 나의 점유 해제 됨

				// Show Cross Hair
				UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
				if (Panel_CrossHair)
					Panel_CrossHair->ShowCrossHairPanel();

				// Camera Offset
				if (OccupiedIO->GetCameraOffset() != FVector::ZeroVector)
				{
					AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(GetOwnerCharacter());
					if (CastedMine != nullptr)
					{
						UWRActorComponentVR* CompVR = CastedMine->GetComponent<UWRActorComponentVR>();
						if (CompVR != nullptr)
						{
							CompVR->SetCameraOffset(EWRCameraOffset::None, OccupiedIO->GetCameraOffset());
						}
					}
				}

				// Fade
// 				FWRInteractionObjectTable* IOTable = OccupiedIO->GetIOTable();
// 				if (IOTable && IOTable->UseFadeInOut && IOTable->FadeOutInExitTime != 0.f)
// 				{
// 					FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
// 					if (ScreenFader != nullptr)
// 					{
// 						ScreenFader->ScreenFadeOutIn(UWRGameInstance::GetGameInstance(), IOTable->FadeOutInExitTime, IOTable->FadeOutInExitHoldTime, nullptr, FLinearColor::Black, true, false);
// 					}
// 				}
			}
			
			// ++ 순서 주의
			OwnerPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			SetOwnerGravity(true);
			OwnerPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			OccupiedIO = nullptr;

			// [ 2020-7-23 : kyu ] Idle Type Change for Animation Blue Print
			UWRAnimInstance* AnimInstance = OwnerPlayer->GetAnimInstance();
			if (AnimInstance && AnimInstance->GetIdleType() != EWRIdleType::None)
			{
				AnimInstance->SetIdleType(EWRIdleType::None);
				AnimInstance->SetLayeredBlendPerBone(false);
			}			
		}
	}
	else
	{
		// Set
		if (InNewIO->IsDirectOccupiable() == false)
			return;

		OccupiedIO = InNewIO;

		OccupiedIO->SetOccupiedPlayer(OwnerPlayer);
		// [ 2020-9-1 : kyu ] 점유중인 IO의 TargetArea 비활성
		OccupiedIO->SetTargetAreaEnable(false);

		// [ 2020-7-23 : kyu ] Idle Type Change for Animation Blue Print
		UWRAnimInstance* AnimInstance = OwnerPlayer->GetAnimInstance();
		if (AnimInstance && OccupiedIO->GetIdleType() != EWRIdleType::None)
		{
			AnimInstance->SetIdleType(OccupiedIO->GetIdleType());
			AnimInstance->SetLayeredBlendPerBone(true);
		}
				
		OwnerPlayer->SetForceStop();

		// ++ 순서 주의
		//OwnerPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OwnerPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		USceneComponent* pInteractionComp = InNewIO->GetInteractionComponent();
		if (pInteractionComp != nullptr)
		{
			OwnerPlayer->AttachToComponent(pInteractionComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
			SetOwnerGravity(false);

			if (OwnerPlayer->IsMe())
			{
				// + Input Lock Timer
				WRInputSystemManager::Get()->SetLocationLock(true);
				OcccupyTimerDelegate = FTimerDelegate::CreateUObject(this, &UWRActorComponentInteraction::OnOccupyTimerFinished);
				WRActorUtility::SetTimer(OccupyTimer, OcccupyTimerDelegate, 1.0f, true);
			}			
		}

		if (OwnerPlayer->IsMe())
		{
			UWRActorComponentInteractionObject* pIOComp = OccupiedIO->GetComponent<UWRActorComponentInteractionObject>();
			if (pIOComp != nullptr)
			{
				pIOComp->UpdateTargetUI();
			}
			WRItemManager::Get()->ThrowTemporaryItemAway(EWRReasonForThrowTemporaryItemAway::IOOccupy);

			// Hide Cross Hair
			UWRUIPanel_CrossHair* Panel_CrossHair = WRUIManager::Get()->FindUI<UWRUIPanel_CrossHair>(EWRUIPanelKind::CrossHair);
			if (Panel_CrossHair)
				Panel_CrossHair->HideCrossHairPanel();

			// Camera Offset
			if (OccupiedIO->GetCameraOffset() != FVector::ZeroVector)
			{
				AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(GetOwnerCharacter());
				if (CastedMine != nullptr)
				{
					UWRActorComponentVR* CompVR = CastedMine->GetComponent<UWRActorComponentVR>();
					if (CompVR != nullptr)
					{
						CompVR->SetCameraOffset(EWRCameraOffset::New, OccupiedIO->GetCameraOffset());
					}
				}
			}

			// Fade
// 			FWRInteractionObjectTable* IOTable = OccupiedIO->GetIOTable();
// 			if (IOTable && IOTable->UseFadeInOut && IOTable->FadeOutInEnterTime != 0.f)
// 			{
// 				FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
// 				if (ScreenFader != nullptr)
// 				{
// 					ScreenFader->ScreenFadeOutIn(UWRGameInstance::GetGameInstance(), IOTable->FadeOutInEnterTime, IOTable->FadeOutInEnterHoldTime, nullptr, FLinearColor::Black, true, false);					
// 				}
// 			}

			// [ 2020-7-27 : kyu ] 방향 동기화용
			IMMessageSender::RequestPlayerMoveStop(OwnerPlayer);
		}

		// [ 2020-8-4 : kyu ] 점유 이후 추가 작업 (by SubType)
		OnOccupationActivate();
	}
}

void UWRActorComponentInteraction::OnOccupyTimerFinished()
{
	WRInputSystemManager::Get()->SetLocationLock(false);
	WRActorUtility::ClearTimer(OccupyTimer);
	OcccupyTimerDelegate.Unbind();
}

void UWRActorComponentInteraction::OnOccupationActivate()
{
	if (OccupiedIO == nullptr)
		return;

	FWRInteractionObjectTable* IOTable = OccupiedIO->GetIOTable();
	if (IOTable == nullptr)
		return;

	switch (IOTable->SubType)
	{
	case EWRIOSubType::Mic:
	{
		UWRActorComponentMic* MicComponent = GetOwnerPlayer()->GetComponent<UWRActorComponentMic>();
		if (MicComponent)
		{
			MicComponent->ActivateMic(OccupiedIO->GetActorID());
		}
	}
	break;
	}
}

void UWRActorComponentInteraction::OnOccupationDeactivate()
{
	if (OccupiedIO == nullptr)
		return;

	FWRInteractionObjectTable* IOTable = OccupiedIO->GetIOTable();
	if (IOTable == nullptr)
		return;

	switch (IOTable->SubType)
	{
	case EWRIOSubType::Mic:
	{
		UWRActorComponentMic* MicComponent = GetOwnerPlayer()->GetComponent<UWRActorComponentMic>();
		if (MicComponent)
		{
			MicComponent->DeactivateMic(OccupiedIO->GetActorID());
		}
	}
	break;
	}
}

bool UWRActorComponentInteraction::IsOccupiedIO(const AID_t IN InAID)
{
	if (OccupiedIO == nullptr)
		return false;

	if (OccupiedIO->GetActorID() == InAID)
		return true;

	return false;
}

void UWRActorComponentInteraction::TransformToOccupationEndPosition()
{
	if (OccupiedIO == nullptr)
		return;

	// + Occupation End Transform
	USceneComponent* pEndOccupiedTransform = OccupiedIO->GetOccupationEndComponent();
	if (pEndOccupiedTransform != nullptr)
	{
		FTransform tf = pEndOccupiedTransform->GetComponentToWorld();
		if (tf.Equals(FTransform::Identity) == false)
		{
			// Modify Location
			FNavLocation OutNavLocation;
			bool bResult = WRActorUtility::ProjectPointToNavigation(GetOwnerCharacter(), tf.GetLocation(), OutNavLocation);
			if (bResult)
			{
				float OutHeight = 0.0f;
				if (WRActorUtility::ModifyZHeightForClient(GetOwnerCharacter(), OutNavLocation.Location, OutHeight))
				{
					OutNavLocation.Location.Z = OutHeight;
				}
				tf.SetLocation(OutNavLocation.Location);
			}
			GetOwnerCharacter()->SetActorTransform(tf);

			// Exception
			if (GetOwnerCharacter()->IsMe())
			{
				USceneComponent* MeshParentComponent = GetOwnerCharacter()->GetMeshParentComponent();
				if (MeshParentComponent != nullptr)
				{
					MeshParentComponent->SetWorldRotation(tf.GetRotation());
				}
				WRCharacterManager::Get()->ResetVRRotateOrientation();
			}
		}
	}
}

void UWRActorComponentInteraction::SetOwnerGravity(const bool IN InSet)
{
	if (InSet)
	{
		GetOwnerCharacter()->GetCharacterMovement()->GravityScale = fSrcGravityScale;
		GetOwnerCharacter()->GetCapsuleComponent()->SetEnableGravity(bSrcGravityEnable);
	}
	else
	{
		fSrcGravityScale = GetOwnerCharacter()->GetCharacterMovement()->GravityScale;
		bSrcGravityEnable = GetOwnerCharacter()->GetCapsuleComponent()->IsGravityEnabled();

		GetOwnerCharacter()->GetCharacterMovement()->GravityScale = 0.f;
		GetOwnerCharacter()->GetCapsuleComponent()->SetEnableGravity(false);
	}
}