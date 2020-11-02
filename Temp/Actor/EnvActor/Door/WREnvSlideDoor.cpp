// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvSlideDoor.h"
#include "Components/BoxComponent.h"
#include "../WREnvVehicleBase.h"
#include "Manager/WREnvInteractionManager.h"
#include "../Elevator/WRElevator.h"
#include "Manager/WRUIManager.h"
#include "../WREnvDoorMountedVehicleBase.h"
#include "Manager/WRCharacterManager.h"
#include "Utility/WRActorUtility.h"

AWREnvSlideDoor::AWREnvSlideDoor() : AWREnvActor()
{
	
}

TArray<FString> AWREnvSlideDoor::MakeDebugString()
{
	TArray<FString> strText;
	// Index
	strText.Add(FString::Printf(TEXT("Position Index : %d"), PositionIndex));
	// AID
	strText.Add(FString::Printf(TEXT("Owner AID : %d"), OwnerVehicleAID));
	// Overlaped Component
	int32 nCount = 0;
	if (InteractionArea)
	{
		TArray<UPrimitiveComponent*> Components;
		InteractionArea->GetOverlappingComponents(Components);
		nCount = Components.Num();
	}
	strText.Add(FString::Printf(TEXT("Overlaped Component : %d"), nCount));

	return strText;
}

void AWREnvSlideDoor::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> BoxComponents = this->GetComponentsByTag(UBoxComponent::StaticClass(), "InteractionArea");
	if (BoxComponents.IsValidIndex(0))
	{
		InteractionArea = Cast<UBoxComponent>(BoxComponents[0]);
		InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &AWREnvSlideDoor::OnBeginOverlap_InteractionArea);
		InteractionArea->OnComponentEndOverlap.AddDynamic(this, &AWREnvSlideDoor::OnEndOverlap_InteractionArea);
	}
	else
	{
		ensureMsgf(false, TEXT("InteractionArea is not setted"));
	}

	SkeletalMesh = Cast<USkeletalMeshComponent>(this->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (SkeletalMesh)
	{
		SkeletalMesh->SetCollisionProfileName(TEXT("WREnvSlideDoor"));
		SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}

	// 외부 문인가?
	if (IsVehicleOutterDoor)
	{
		WREnvInteractionManager::Get()->AddVehicleOutterDoor(this);
	}

	// + Optimization
	WRActorUtility::SetEnableUpdateOverlapWhenFinalizeAnim(false, this->GetRootComponent());
}

void AWREnvSlideDoor::BeginDestroy()
{
	if (this->IsValidLowLevel())
	{
		if (WREnvInteractionManager::IsValid())
		{
			AWREnvDoorMountedVehicleBase* pOwnerVehicleBase = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvDoorMountedVehicleBase>(OwnerVehicleAID);
			if (pOwnerVehicleBase->IsValidLowLevel())
			{
				pOwnerVehicleBase->RemoveOutterDoor(this);
			}
		}
		if (WRUIManager::IsValid())
		{
			WRUIManager::Get()->CloseAllUIFromCharacter(this);
		}
	}

	Super::BeginDestroy();
}

void AWREnvSlideDoor::Tick(float DeltaTime)
{
	AWREnvActor::Tick(DeltaTime);

	if (IsOutterDoor())
	{
		FWRDebugUIData data;
		data.Flag = EWRDebugUIBitFlag::Env;
		data.Actor = this;
		data.CompareActor = Cast<AActor>(WRCharacterManager::Get()->GetMyCharacter());
		data.fDistance = 3000.f;
		data.ArrayContents = MakeDebugString();
		WRUIManager::Get()->UpdateDebugUI(data);
	}
}

void AWREnvSlideDoor::OnBeginOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsAutoOpen)
	{
		// 자동문이면 자동으로 열림
		OpenDoor();
	}
// 	else if (IsVehicleOutterDoor)
// 	{
// 		AWREnvVehicleBase* CastedActor = Cast<AWREnvVehicleBase>(Other);
// 		if (CastedActor == nullptr)
// 			return;
// 
// 		SetOwnerVehicle(CastedActor);	// [ 2020-4-2 : kyu ] OutterDoor와 Vehicle의 Overlap체크로 Owner지정하는 방식에서 Vehicle가 직접 상태변화에 따라 오버랩 된 OutterDoor를 가져오는 방식으로 변경함 (최적화)
//	}
}

void AWREnvSlideDoor::OnEndOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsAutoOpen)
	{
		// 자동문이면 자동으로 닫힘
		CloseDoor();
	}
// 	else if (IsVehicleOutterDoor)
// 	{
// 		AWREnvVehicleBase* CastedActor = Cast<AWREnvVehicleBase>(Other);
// 		if (CastedActor == nullptr)
// 			return;
// 
// 		SetOwnerVehicle(nullptr);	// [ 2020-4-2 : kyu ] OutterDoor와 Vehicle의 Overlap체크로 Owner지정하는 방식에서 Vehicle가 직접 상태변화에 따라 오버랩 된 OutterDoor를 가져오는 방식으로 변경함 (최적화)
//	}
}

//====================================================================================

void AWREnvSlideDoor::OpenDoor()
{
	if (OpenAnimAsset.IsNull() == false)
	{
		if (SkeletalMesh->IsValidLowLevel() == false)
			return;

		// [ 2020-4-7 : kyu ] Optimization - UpdateOverlaps() When Animation Finished
		WRActorUtility::SetEnableUpdateOverlapWhenFinalizeAnim(true, this->GetRootComponent());

		UAnimSequenceBase* pAnimSeq = OpenAnimAsset.LoadSynchronous();

		float fPosition = 0.f;
		if (SkeletalMesh->IsPlaying())
		{
			float CurPosition = SkeletalMesh->GetPosition();
			if (CurPosition != 0.f)
			{
				float MaxLength = pAnimSeq->GetPlayLength();
				fPosition = MaxLength - CurPosition;
				if (fPosition < 0.f)
					fPosition = 0.f;
			}
		}

//		SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		SkeletalMesh->SetAnimation(pAnimSeq);
		SkeletalMesh->Play(false);
		SkeletalMesh->SetPosition(fPosition);

		// Test 1
// 		UAnimInstance* pAnimInst = SkeletalMesh->GetAnimInstance();
// 		UAnimMontage* pCurActivateMontage = pAnimInst->GetCurrentActiveMontage();
// 		if (pCurActivateMontage != nullptr)
// 		{
// 			float CurrPosition = pAnimInst->Montage_GetPosition(pCurActivateMontage);
// 
// 			UAnimSequenceBase* pAnimSeq = OpenAnimAsset.LoadSynchronous();
// 			float MaxLength = pAnimSeq->GetPlayLength();
// 			float NewPosition = MaxLength -= CurrPosition;
// 			if (NewPosition < 0.f)
// 				NewPosition = 0.f;
// 
// 			UAnimMontage* pMontage = pAnimInst->PlaySlotAnimationAsDynamicMontage(OpenAnimAsset.LoadSynchronous(), TEXT("DefaultSlot"), 0.25f, 0.25f, 1.f, 1, -1.f, NewPosition);
// 			if (pMontage == nullptr)
// 				return;
// 		}
// 		else
// 		{
// 			UAnimMontage* pMontage = pAnimInst->PlaySlotAnimationAsDynamicMontage(OpenAnimAsset.LoadSynchronous(), TEXT("DefaultSlot"));
// 			if (pMontage == nullptr)
// 				return;
// 		}

		// Test 2
//		SkeletalMesh->PlayAnimation(OpenAnimAsset.LoadSynchronous(), false);
	}
}

void AWREnvSlideDoor::CloseDoor()
{
	if (CloseAnimAsset.IsNull() == false)
	{
		if (SkeletalMesh == nullptr)
			return;

		UAnimSequenceBase* pAnimSeq = CloseAnimAsset.LoadSynchronous();

		float fPosition = 0.f;
		if (SkeletalMesh->IsPlaying())
		{
			float CurPosition = SkeletalMesh->GetPosition();
			float MaxLength = pAnimSeq->GetPlayLength();
			fPosition = MaxLength - CurPosition;
			if (fPosition < 0.f)
				fPosition = 0.f;
		}

//		SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		SkeletalMesh->SetAnimation(pAnimSeq);
		SkeletalMesh->Play(false);
		SkeletalMesh->SetPosition(fPosition);

		// [ 2020-4-7 : kyu ] Optimization - UpdateOverlaps() When Animation Finished
		WRActorUtility::SetEnableUpdateOverlapWhenFinalizeAnim(false, this->GetRootComponent());
	}
}

//====================================================================================

void AWREnvSlideDoor::SetOwnerVehicle(class AWREnvVehicleBase* IN InOwnerVehicle)
{
	if (InOwnerVehicle == nullptr)
	{
		// Remove

		OwnerVehicleAID = INVALID_AID;
	}
	else
	{
		// Add

		OwnerVehicleAID = InOwnerVehicle->GetAID();
	}
}
