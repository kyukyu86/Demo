// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAnimInstance.h"
#include "Enum/EWRPath.h"
#include "Manager/WRPathManager.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "../Character/WRCharacter.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Table/WRMontageTable.h"
#include "Manager/WRAnimManager.h"
#include "../Character/WRCharacterPlayer.h"
#include "Manager/WRConsoleManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Table/WREnpcTable.h"
#include "Manager/WRInputSystemManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Component/Character/WRActorComponentLipsync.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "../Character/WRCharacterMine.h"
#include "Manager/WRCharacterManager.h"
#include "UnrealString.h"
#include "../Vehicle/WRVehicle.h"
#include "Manager/WRVehicleManager.h"

// For CPP
DECLARE_CYCLE_STAT(TEXT("WRAnimInstance_SettingFABRIK"), STAT_WRAnimInstance_SettingFABRIK, STATGROUP_WRAnimInstance);
DECLARE_CYCLE_STAT(TEXT("WRAnimInstance_SettingLookIK"), STAT_WRAnimInstance_SettingLookIK, STATGROUP_WRAnimInstance);

// For Cpp
DEFINE_LOG_CATEGORY(WRAnimInstance)

UWRAnimInstance::UWRAnimInstance()
{

}

//montage를 스킬, 평타 등등으로 나누고 각 스킬id를 섹션 인덱스로 세팅.
//montage를 만들때 해당 montage가 재생될 슬롯을 설정 할수 있음
//즉 montage_play를 호출했을때 state machine에 세팅될 여러개의 슬롯중에서 특정 슬롯에서만 재생되게 가능
float UWRAnimInstance::PlayMontage(int32 InMontageID, FString InSectionName, float InRate)
{
	UAnimMontage* AnimMontage = WRAnimManager::Get()->FindMontage(InMontageID);
	if (nullptr == AnimMontage)
		return 0.0f;

	float Duration = 0.0f;
	if (InSectionName.IsEmpty())
	{
		Duration = Montage_Play(AnimMontage, InRate);
	}
	else
	{
		if (AnimMontage->IsValidSectionName(FName(*InSectionName)))
		{
			Duration = Montage_Play(AnimMontage, InRate);
			if (0.f < Duration)
			{
				Montage_JumpToSection(FName(*InSectionName), AnimMontage);
			}
			else
			{
				if (Montage_IsPlaying(AnimMontage))
				{
					Montage_Stop(0.0f, AnimMontage);
				}
			}
		}
		else
		{
			if (InSectionName != "Spawn")
			{
				FString MSG = FString::Format(TEXT("Invalid section name : {0}"), { *InSectionName });
				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("UWRAnimInstance::PlayMontage"));
			}
		}
	}

	return Duration;
}

AWRCharacter* UWRAnimInstance::GetOwnerCharacter()
{
	return Cast<AWRCharacter>(GetOwningActor());
}

void UWRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	//====================================================================================
	// [ 2019-11-14 : magedoga ] (추후 AnimInstance 세분화 요망)

	//아래 함수 순서 변경 금지 by yul
	ProcessFABRIK(OwnerCharacter);
	ProcessFABRIK();	//추후에 이 함수로 통일하는게 나음. ProcessFABRIK(OwnerCharacter); 이거는 기존꺼 영향갈까봐 일단 살려둠

	this->ProcessLookIK(OwnerCharacter);
	this->ProcessLipsync(OwnerCharacter);
	this->ProcessHMDHeadSync(Cast<class AWRCharacterPlayer>(OwnerCharacter));
	//====================================================================================

	UpdateMoveValue();


}

void UWRAnimInstance::AnimNotify_HitTiming()
{
	//CombatInfo쪽에서 처리 하게 함
}

void UWRAnimInstance::AnimNotify_SkipAnimation()
{
	//노티파이 설정
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	UWRActorComponentFSM* FSMComponent = OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return;

	FSMComponent->SetSkipAnimation(true);
}

bool UWRAnimInstance::IsIncludedCombatGimmickState(EWRCombatGimmickState InState)
{
	if ((CombatGimmickState & InState) != EWRCombatGimmickState::None)
	{
		return true;
	}

	return false;
}

void UWRAnimInstance::BeginCombatGimmickAnimNotifyState(EWRCombatGimmickState InState)
{
	CombatGimmickState |= InState;
}

void UWRAnimInstance::EndCombatGimmickAnimNotifyState(EWRCombatGimmickState InState)
{
	CombatGimmickState &= ~InState;
}

FString UWRAnimInstance::GetMontageSectionName()
{
	FAnimMontageInstance* AnimMontageInstance = GetActiveMontageInstance();
	if (nullptr == AnimMontageInstance)
		return "";

	FString CurrentSection = AnimMontageInstance->GetCurrentSection().ToString();

	return CurrentSection;
}

void UWRAnimInstance::OnAnimMontageEnded(UAnimMontage* rtnMontage, bool bInterrupted)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	UWRActorComponentFSM* FSMComponent = OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return;

	FSMComponent->OnAnimMontageEnded(rtnMontage, bInterrupted);
}

void UWRAnimInstance::OnAnimMontageBlendingOut(UAnimMontage* rtnMontage, bool bInterrupted)
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (nullptr == OwnerCharacter)
		return;

	UWRActorComponentFSM* FSMComponent = OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return;

	FSMComponent->OnAnimMontageBlendingOut(rtnMontage, bInterrupted);
}

bool UWRAnimInstance::IsIdentityTR(const FTransform& InTransform)
{
	return InTransform.GetLocation() == FVector::ZeroVector && InTransform.GetRotation().Rotator() == FRotator::ZeroRotator;
}


void UWRAnimInstance::ProcessFABRIK()
{
	//SCOPE_CYCLE_COUNTER(STAT_WRAnimInstance_SettingFABRIK);

	if (nullptr == SkeletalMeshComponentToFABRIK)
		return;

	LHandTransform_FABRIK = SkeletalMeshComponentToFABRIK->GetSocketTransform(FName("IK_LHand"));
	RHandTransform_FABRIK = SkeletalMeshComponentToFABRIK->GetSocketTransform(FName("IK_RHand"));

	LHandAlpha_FABRIK = IsIdentityTR(LHandTransform_FABRIK) == true ? 0.0f : 1.0f;
	RHandAlpha_FABRIK = IsIdentityTR(RHandTransform_FABRIK) == true ? 0.0f : 1.0f;
}


//====================================================================================
// [ 2019-11-20 : magedoga ] Process Function
void UWRAnimInstance::ProcessFABRIK(AWRCharacter* InOwnerCharacter)
{
	if (InOwnerCharacter == nullptr)
		return;

	AWRCharacterPlayer* PlayerCharacter = Cast<AWRCharacterPlayer>(InOwnerCharacter);
	if (PlayerCharacter == nullptr)
		return;

	SCOPE_CYCLE_COUNTER(STAT_WRAnimInstance_SettingFABRIK);


	// [ 2019-11-13 : magedoga ] Get transform for FABRIK
	PlayerCharacter->GetTransformForFABRIK(RHandTransform_FABRIK, LHandTransform_FABRIK);

	// [ 2019-11-14 : magedoga ] Check on/off
	RHandAlpha_FABRIK = IsIdentityTR(RHandTransform_FABRIK) == true ? 0.0f : 1.0f;
	LHandAlpha_FABRIK = IsIdentityTR(LHandTransform_FABRIK) == true ? 0.0f : 1.0f;


	// [ 2019-11-14 : magedoga ] Calculate angle
	USkeletalMeshComponent* SkeletalMeshComponent = PlayerCharacter->GetMesh();
	checkf(SkeletalMeshComponent != nullptr, TEXT("UWRAnimInstance::NativeUpdateAnimation - Null SkeletalMesh of PlayerCharacter."));
	   
	auto CalculateHandAngle = [&](FName InHandName, FVector InTargetLocation) {

		FTransform TRHand = SkeletalMeshComponent->GetSocketTransform(InHandName);
		FVector ToTargetDirection = (InTargetLocation - TRHand.GetLocation()).GetSafeNormal();

		// [ 2019-11-14 : magedoga ] Target
		//UKismetSystemLibrary::DrawDebugSphere(PlayerCharacter, InTargetLocation, 10.0f, 12, FLinearColor::Red);


		// [ 2019-11-14 : magedoga ] Hand To Target
		UKismetSystemLibrary::DrawDebugArrow(PlayerCharacter, TRHand.GetLocation(), InTargetLocation, 5.0f, FLinearColor::Red, 0.0f, 1.0f);
		// [ 2019-11-14 : magedoga ] Hand To Target by Direction
		UKismetSystemLibrary::DrawDebugArrow(PlayerCharacter, TRHand.GetLocation(), TRHand.GetLocation() + (ToTargetDirection*100.0f), 5.0f, FLinearColor::Green, 0.0f, 1.0f);
		// [ 2019-11-14 : magedoga ] Hand Forward Vector
		UKismetSystemLibrary::DrawDebugArrow(PlayerCharacter, TRHand.GetLocation(), TRHand.GetLocation() + TRHand.GetRotation().GetForwardVector() * 100.0f, 5.0f, FLinearColor::Blue, 0.0f, 1.0f);

		return (ToTargetDirection - TRHand.GetRotation().GetForwardVector()).Rotation();
	};

	// 			RHandAngle = RHandAlpha_FABRIK != 0.0f ? CalculateHandAngle(FName("hand_r"), RHandTransform_FABRIK.GetLocation()) : FRotator::ZeroRotator;
	// 			LHandAngle = LHandAlpha_FABRIK != 0.0f ? CalculateHandAngle(FName("hand_l"), LHandTransform_FABRIK.GetLocation()) : FRotator::ZeroRotator;
}

//====================================================================================

void UWRAnimInstance::ProcessLookIK(AWRCharacter* InOwnerCharacter)
{
	if (InOwnerCharacter == nullptr)
		return;

	if (WRConsoleManager::IsValid() == false)
		return;

	AWRCharacter* OwnerCharacter = InOwnerCharacter;

	if (OwnerCharacter->GetCharacterType() != EWRCharacterType::NPC || WRTableManager::IsValid() == false)
		return;

	if (OwnerCharacter->IsLookingTarget() == false)
	{
		LookAngle = FMath::RInterpTo(LookAngle, FRotator::ZeroRotator, GetWorld()->GetDeltaSeconds(), WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKBlendSpeed));
		return;
	}

	WRTableID TableID = OwnerCharacter->GetCharacterTableID();
	if (TableID == INVALID_TABLE_TID)
		return;

	FWRNpcTable* NPCTable = WRTableManager::Get()->FindRow<FWRNpcTable>(OwnerCharacter->GetCharacterTableID());
	if (NPCTable == nullptr)
		return;

	//====================================================================================
	// [ 2019-11-13 : magedoga ] Look IK
	SCOPE_CYCLE_COUNTER(STAT_WRAnimInstance_SettingLookIK);

	DestAngle = FRotator::ZeroRotator;

	// [ 2019-11-14 : magedoga ] Get Looking Location
	AWRCharacter* LookTarget = nullptr;
	FName SocketName = NAME_None;
	OwnerCharacter->GetLookIKTarget(LookTarget, SocketName);
	FVector LookLocation = FVector::ZeroVector;
	if (SocketName.ToString().Contains("head") == true && LookTarget->IsMe() == true)
	{
		LookLocation = WRInputSystemManager::Get()->GetHMDLocation();
	}
	else
	{
		LookLocation = LookTarget->GetMesh()->GetSocketLocation(SocketName);
	}


	// [ 2019-11-14 : magedoga ] Get My Head Bone Transform
	FTransform HeadTransform = OwnerCharacter->GetMesh()->GetSocketTransform(NPCTable->HeadBoneName);

	// [ 2019-11-14 : magedoga ] 0. Check Looking Distance
	float MaxDistance = NPCTable->LookIKDist * WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKDistanceRatio);
	if (FVector::Dist(HeadTransform.GetLocation(), LookLocation) < MaxDistance)
	{
		float MaxPitch = WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitPitch);
		float MaxYaw = WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitYaw);

		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(HeadTransform.GetLocation(), LookLocation);
		FRotator Gap = LookRotation - OwnerCharacter->GetActorRotation();
		Gap.Normalize();
		if (FMath::Abs(Gap.Pitch) < WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitPitch) &&
			FMath::Abs(Gap.Yaw) < WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitYaw))
		{

			// [ 2019-11-22 : magedoga ] UE wiki
			auto RemapLooatRotator = [&](FRotator& OutRotator) {

				auto Remap360To180 = [&](float InAxis)
				{
					float Result = 0.0f;
					if (InAxis > 180)
					{
						Result = InAxis - 360.0f;
					}
					else
					{
						Result = InAxis;
					}

					if (Result < -180.0f)
					{
						Result += 360.0f;
					}

					return Result;
				};

				OutRotator.Yaw = Remap360To180(OutRotator.Yaw);
				OutRotator.Pitch = Remap360To180(OutRotator.Pitch);
				OutRotator.Roll = Remap360To180(OutRotator.Roll);
			};

			FRotator OwnerRotation = OwnerCharacter->GetActorRotation();
			//FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(HeadTransform.GetLocation(), LookLocation);


			//FRotator ForRemap = FRotator(LookRotation.Pitch * -1.0f, LookRotation.Yaw - OwnerRotation.Yaw, LookRotation.Roll);
			FRotator ForRemap = FRotator(LookRotation.Pitch, LookRotation.Yaw - OwnerRotation.Yaw, LookRotation.Roll);

			RemapLooatRotator(ForRemap);

			// [ 2019-11-25 : magedoga ] Convert to Component Space
			DestAngle = FRotator(LookRotation.Roll, ForRemap.Yaw, ForRemap.Pitch * -1.0f);
		}



		// [ 2019-11-22 : magedoga ] Roll 이용
// 		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(HeadTransform.GetLocation(), LookLocation);
// 		FRotator Gap = LookRotation - OwnerCharacter->GetActorRotation();
// 		Gap.Normalize();
// 		if (FMath::Abs(Gap.Pitch) < WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitPitch) &&
// 			FMath::Abs(Gap.Yaw) < WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitYaw))
// 		{
// 			FVector ToTargetDirection = (LookLocation - HeadTransform.GetLocation()).GetSafeNormal();
// 			FRotator ToTargetRotation = ToTargetDirection.Rotation();
// 
// 			DestAngle = ToTargetRotation - OwnerCharacter->GetActorRotation();
// 
// 			DestAngle.Roll = DestAngle.Pitch;
// 			DestAngle.Pitch = 0.0f;
// 		}


// 		// [ 2019-11-15 : magedoga ] For Clamp Axis
// 		auto RemapLookAtRotator = [&](FRotator& OutRotator) {
// 
// 			auto Remap360To180 = [&](float InAxis) {
// 
// 				float Calculating = InAxis > 180.0f ? InAxis - 360.0f : InAxis;
// 				Calculating = Calculating < -180.0f ? Calculating + 360.0f : Calculating;
// 				return Calculating;
// 			};
// 
// 			OutRotator.Pitch = Remap360To180(OutRotator.Pitch);
// 			OutRotator.Yaw = Remap360To180(OutRotator.Yaw);
// 			OutRotator.Roll = Remap360To180(OutRotator.Roll);
// 		};
// 
// 		// [ 2019-11-15 : magedoga ] Get Look at Rotation
// 		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(HeadTransform.GetLocation(), LookLocation);
// 
// 		// [ 2019-11-15 : magedoga ] Check Look IK Bound
// 		FRotator Gap = LookRotation - OwnerCharacter->GetActorRotation();
// 		Gap.Normalize();
// 		if (FMath::Abs(Gap.Pitch) < WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitPitch) &&
// 			FMath::Abs(Gap.Yaw) < WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitYaw))
// 		{
// 			FRotator MyRotation = OwnerCharacter->GetActorRotation();
// 
// 			FRotator ForRemapRotator = FRotator(LookRotation.Pitch * -1.0f, LookRotation.Yaw - MyRotation.Yaw, LookRotation.Roll);
// 			RemapLookAtRotator(ForRemapRotator);
// 
// 
// 			DestAngle = ForRemapRotator;
// 			DestAngle.Pitch *= -1.0f;
// 		}

		//UKismetSystemLibrary::DrawDebugArrow(this, HeadTransform.GetLocation(), HeadTransform.GetLocation() + LookRotation.Quaternion().GetForwardVector() * 100.0f, 2.0f, FLinearColor::Red, 0.0f, 1.0f);
	}

	// [ 2019-11-15 : magedoga ] Interp To
	LookAngle = FMath::RInterpTo(LookAngle, DestAngle, GetWorld()->GetDeltaSeconds(), WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKBlendSpeed));

	//UKismetSystemLibrary::DrawDebugArrow(this, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 100.0f, 2.0f, FLinearColor::Red, 0.0f, 1.0f);
}

//====================================================================================

void UWRAnimInstance::ProcessLipsync(AWRCharacter* InOwnerCharacter)
{
	if (InOwnerCharacter == nullptr)
		return;

	UWRActorComponentLipsync* LipsyncComponent = InOwnerCharacter->GetComponent<UWRActorComponentLipsync>();
	if (LipsyncComponent == nullptr)
		return;

	if (LipsyncComponent->IsTalking() == false)
		return;

	LipsyncAlpha = LipsyncComponent->GetLipsyncAlpha();
}

//====================================================================================

#if SYNC_HMD_HEAD_INCLUDE_PLAYER
void UWRAnimInstance::ProcessHMDHeadSync(class AWRCharacterPlayer* InOwnerPlayerCharacter)
#else // SYNC_HMD_HEAD_INCLUDE_PLAYER
void UWRAnimInstance::ProcessHMDHeadSync(class AWRCharacter* InOwnerCharacter)
#endif // SYNC_HMD_HEAD_INCLUDE_PLAYER
{


#if WITH_EDITOR
	if (WRCharacterManager::IsValid() == false)
		return;
#endif // WITH_EDITOR

#if SYNC_HMD_HEAD_INCLUDE_PLAYER

	// [ 2020-3-9 : magedoga ]
	// 1. Player도 처리 가능하도록 수정
	// 2. On/Off 가능하도록

	if (InOwnerPlayerCharacter == nullptr)
		return;

	HMDSyncAlpha = 0.0f;

	if (InOwnerPlayerCharacter->IsUsingSyncHMDHead() == false)
		return;

	// [ 2020-3-9 : magedoga ] Showing 3rd Character currently?
	if (InOwnerPlayerCharacter->IsShowing3rdCharacter() == false)
		return;

	FRotator _HMDRotation = InOwnerPlayerCharacter->GetHMDRotationToUpdate();

	// 	if (_HMDRotation.IsZero() == false)
	// 	{
	// 		if (InOwnerPlayerCharacter->IsMe() == true)
	// 		{
	// 			UE_LOG(WRAnimInstance, Display, TEXT("UWRAnimInstance::ProcessHMDHeadSync - Me [%.2f][%.2f][%.2f]"), _HMDRotation.Roll, _HMDRotation.Pitch, _HMDRotation.Yaw);
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(WRAnimInstance, Display, TEXT("UWRAnimInstance::ProcessHMDHeadSync - Ot [%.2f][%.2f][%.2f]"), _HMDRotation.Roll, _HMDRotation.Pitch, _HMDRotation.Yaw);
	// 		}
	// 	}

#else // SYNC_HMD_HEAD_INCLUDE_PLAYER

	if (InOwnerCharacter == nullptr)
		return;

	if (InOwnerCharacter->IsMe() == false)
		return;

	if (MyCharacter == nullptr)
		MyCharacter = Cast<AWRCharacterMine>(InOwnerCharacter);

	if (MyCharacter == nullptr)
		return;

	HMDSyncAlpha = 0.0f;

	if (MyCharacter->IsShowing3rdCharacter() == false)
		return;

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
		return;

	FRotator _HMDRotation = WRInputSystemManager::Get()->GetHMDRotation();
	HMDLocation = WRInputSystemManager::Get()->GetHMDLocation();

#endif // SYNC_HMD_HEAD_INCLUDE_PLAYER

	HMDSyncAlpha = 1.0f;

#if SYNC_HMD_HEAD_INCLUDE_PLAYER

	float ClampYaw = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_SyncHMDHeadClampYaw);
	float ClampPitch = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_SyncHMDHeadClampPitch);
	float ClampRoll = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_SyncHMDHeadClampRoll);

	FRotator ActorRotation = InOwnerPlayerCharacter->GetActorRotation();

	//====================================================================================
// 
// #if WITH_EDITOR
// 
// 	UWorld* World = InOwnerPlayerCharacter->GetWorld();
// 	if (World->WorldType == EWorldType::PIE &&
// 		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false &&
// 		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == false)
// 	{
// 		FTransform HeadWorldTR = InOwnerPlayerCharacter->GetCurrentSkeletalMeshComponent()->GetSocketTransform(FName("head"),ERelativeTransformSpace::RTS_Component);
// 		/*FRotator LookingRotation = UKismetMathLibrary::FindLookAtRotation(HeadWorldTR.GetLocation(), HeadWorldTR.GetLocation() + (InOwnerPlayerCharacter->GetActorForwardVector() * 100.0f));*/
// 		
// 		FRotator HeadRotation = HeadWorldTR.GetRotation().Rotator();
// 		HeadRotation += FRotator(90.0f, -90.0f, 0.0f);
// 
// 		_HMDRotation.Yaw = FMath::Clamp(_HMDRotation.Yaw, ActorRotation.Yaw - ClampYaw, ActorRotation.Yaw + ClampYaw);
// 		_HMDRotation.Pitch = FMath::Clamp(_HMDRotation.Pitch, ActorRotation.Pitch - ClampPitch, ActorRotation.Pitch + ClampPitch);
// 	}
// 	else
// #endif // WITH_EDITOR
// 	{
// 		
// 
// 		_HMDRotation.Yaw = FMath::Clamp(_HMDRotation.Yaw, ActorRotation.Yaw - ClampYaw, ActorRotation.Yaw + ClampYaw);
// 		_HMDRotation.Pitch = FMath::Clamp(_HMDRotation.Pitch, ActorRotation.Pitch - ClampPitch, ActorRotation.Pitch + ClampPitch);
// 	}

	//====================================================================================

	// [ 2020-3-10 : magedoga ] 1. 기존 로직에 Clamp만
// 	_HMDRotation.Yaw = FMath::Clamp(_HMDRotation.Yaw, ActorRotation.Yaw - ClampYaw, ActorRotation.Yaw + ClampYaw);
// 	_HMDRotation.Pitch = FMath::Clamp(_HMDRotation.Pitch, ActorRotation.Pitch - ClampPitch, ActorRotation.Pitch + ClampPitch);

	// [ 2020-3-10 : magedoga ] 2. 변위량만
	// 	float TestValue = WRConsoleManager::Get()->GetFloat(EWRConsole::TestConsoleFloat);
// 	if (TestValue != 0.0f)
// 	{
// 		_HMDRotation.Yaw += TestValue;
// 	}


	_HMDRotation.Yaw = FMath::Clamp(FRotator::NormalizeAxis(FRotator::ClampAxis(_HMDRotation.Yaw) - FRotator::ClampAxis(ActorRotation.Yaw)), -ClampYaw, ClampYaw);
	_HMDRotation.Pitch = FMath::Clamp(_HMDRotation.Pitch - ActorRotation.Pitch, -ClampPitch, ClampPitch);
	_HMDRotation.Roll = FMath::Clamp(_HMDRotation.Roll - ActorRotation.Roll, -ClampRoll, ClampRoll);

#endif // SYNC_HMD_HEAD_INCLUDE_PLAYER

	auto RemapLooatRotator = [&](FRotator& OutRotator) {

		auto Remap360To180 = [&](float InAxis)
		{
			float Result = 0.0f;
			if (InAxis > 180)
			{
				Result = InAxis - 360.0f;
			}
			else
			{
				Result = InAxis;
			}

			if (Result < -180.0f)
			{
				Result += 360.0f;
			}

			return Result;
		};

		OutRotator.Yaw = Remap360To180(OutRotator.Yaw);
		OutRotator.Pitch = Remap360To180(OutRotator.Pitch);
		OutRotator.Roll = Remap360To180(OutRotator.Roll);
	};

#if SYNC_HMD_HEAD_INCLUDE_PLAYER
	HMDRotation.Roll = _HMDRotation.Pitch;
	HMDRotation.Pitch = _HMDRotation.Roll;
	HMDRotation.Yaw = _HMDRotation.Yaw;

	RemapLooatRotator(HMDRotation);

	HMDRotation.Roll = HMDRotation.Roll * -1.0f;
#else // SYNC_HMD_HEAD_INCLUDE_PLAYER
	HMDRotation.Roll = _HMDRotation.Pitch;
	HMDRotation.Pitch = _HMDRotation.Roll;
	HMDRotation.Yaw = _HMDRotation.Yaw + 180.0f;

	RemapLooatRotator(HMDRotation);

	HMDRotation.Roll = HMDRotation.Roll * -1.0f;
#endif // SYNC_HMD_HEAD_INCLUDE_PLAYER
}



//====================================================================================

void UWRAnimInstance::UpdateMoveValue()
{
	if (nullptr == GetOwnerCharacter())
		return;

	if (GetOwnerCharacter()->IsPlayer() == false)
		return;

	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
	if (nullptr == Player)
		return;

	ForwardDelta = Player->MoveForwardValue;
	RightDelta = Player->MoveRightValue;
}


void UWRAnimInstance::SetSkelMeshToFABRIK(USkeletalMeshComponent* InMesh)
{
	SkeletalMeshComponentToFABRIK = InMesh;

	if (nullptr != SkeletalMeshComponentToFABRIK)
	{
		ProcessFABRIK();
	}
	else
	{
		RHandTransform_FABRIK = FTransform::Identity;
		LHandTransform_FABRIK = FTransform::Identity;
	}
}

void UWRAnimInstance::SetLayeredBlendPerBone(bool bFlag)
{
	// [ 2020-8-27 : kyu ] FSM변경시 무조건 해제하는 중이라서 컨텐츠에서 쓰이는 IdelType이 지정된 경우에는 해제하지 못하도록 수정합니다.
	if (bFlag == false && IdleType != EWRIdleType::None)
		return;

	bLayeredBlend = bFlag;
}

void UWRAnimInstance::ResetHMDRotation()
{
	HMDRotation.Yaw = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_SyncHMDHeadClampYaw);
	HMDRotation.Pitch = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_SyncHMDHeadClampPitch);
	HMDRotation.Roll = 0.0f;
}

void UWRAnimInstance::EnableAIMoveFlag()
{
	bIsAIMoveTo = true;
}

void UWRAnimInstance::DisableAIMoveFlag()
{
	bIsAIMoveTo = false;
}

void UWRAnimInstance::SetForwardDeltaAndRightDelta(const float InForwardDelta, const float InRightDelta)
{
	if (FMath::Abs(FinalMaxDelta - MaxDelta) >= 0.01f)
	{
		float TransformSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::Anim_BlendSpaceTransformSpeed);

		MaxDelta = FMath::Lerp(MaxDelta, FinalMaxDelta, TransformSpeed);
	}

	ForwardDelta = FMath::Clamp(InForwardDelta, -MaxDelta, MaxDelta);
	RightDelta = FMath::Clamp(InRightDelta, -MaxDelta, MaxDelta);

}