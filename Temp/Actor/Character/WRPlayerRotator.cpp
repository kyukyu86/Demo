// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRPlayerRotator.h"
#include "Ticker.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Component/Character/WRActorComponentVR.h"
#include "Manager/WRConsoleManager.h"
#include "WRCharacterPlayer.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRCharacterManager.h"
#include "Engine/Engine.h"
#include "WRCharacterMine.h"
#include "Network/IMMessageSender.h"


//====================================================================================
// [ 2019-8-26 : magedoga ] Rotator

WRPlayerCharacterRotator* WRPlayerCharacterRotator::CreateRotator(EWRCharacterRotationType InRotatorType, class AWRCharacterPlayer* InCharacter)
{
	WRPlayerCharacterRotator* Rotator = nullptr;
	switch (InRotatorType)
	{
	case EWRCharacterRotationType::ThumbStick: { Rotator = new WRRotationbyThumbStick(InCharacter); }break;
	case EWRCharacterRotationType::ThumbStickPerAction: { Rotator = new WRRotationbyThumbStickPerAction(InCharacter); }break;
	case EWRCharacterRotationType::ThumbStickPerAction2: { Rotator = new WRRotationbyThumbStickPerAction2(InCharacter); }break;
	case EWRCharacterRotationType::HeadMoundDisplay: { Rotator = new WRRotationbyRotatorHMD(InCharacter); }break;
	}

	return Rotator;
}

void WRPlayerCharacterRotator::RotateCharacterWithHMD(const FRotator& InRotator)
{
	if (PlayerCharacter->IsValidLowLevel() == false)
		return;

	FRotator CurrentRotator = PlayerCharacter->GetActorRotation();
	CurrentRotator.Pitch = InRotator.Pitch;
	CurrentRotator.Roll = InRotator.Roll;
	PlayerCharacter->SetActorRotation(CurrentRotator);
	
	float DeltaYaw = (InRotator - CurrentRotator).Yaw;
	this->SetRotation(DeltaYaw, 1.0f);
}

void WRPlayerCharacterRotator::SetRotation(const float InDeltaYaw, const float InScale)
{
	this->SetRotationCharacter(InDeltaYaw, InScale);
	this->SetRotationHMD(InDeltaYaw, InScale);
}

void WRPlayerCharacterRotator::SyncRotationbyHMD_Internal()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == false)
		return;

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
		return;

	// [ 2020-2-13 : kyu ] 사다리 타는 도중에는 메쉬 돌리면 안됨
	// [ 2020-3-4 : kyu ] 카메라 회전에 대한 케릭터 Body회전 락
	if (WRInputSystemManager::Get()->IsRotationLock())
		return;

	FVector HMDLocation;
	FRotator HMDRotation;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);

	FRotator CharacterRotation = PlayerCharacter->GetActorRotation();
	CharacterRotation.Yaw = HMDRotation.Yaw;
	PlayerCharacter->SetRotation(CharacterRotation);
}

void WRPlayerCharacterRotator::SyncRotationbyHMDDelta(const float InDeltaTime, const float InSpeed)
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
		return;

	// [ 2020-2-13 : kyu ] 사다리 타는 도중에는 메쉬 돌리면 안됨
	// [ 2020-3-4 : kyu ] 카메라 회전에 대한 케릭터 Body회전 락
	if (WRInputSystemManager::Get()->IsRotationLock())
		return;

	// [ 2019-8-27 : magedoga ] HMD Rotation
	FVector HMDLocation;
	FRotator HMDRotation;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);

	// [ 2019-8-27 : magedoga ] Character Roration
	FRotator CharacterRotation = PlayerCharacter->GetActorRotation();

	// [ 2019-8-27 : magedoga ] RInterp
	FRotator InterpRotator = FMath::RInterpTo(PlayerCharacter->GetActorRotation(), HMDRotation, InDeltaTime, InSpeed);
	CharacterRotation.Yaw = InterpRotator.Yaw;
	PlayerCharacter->SetRotation(CharacterRotation);
}

void WRPlayerCharacterRotator::RequestPlayerRotationToSync()
{
	if (nullptr == PlayerCharacter)
		return;

	if (PlayerCharacter->IsMe() == false)
		return;

	if (PlayerCharacter->GetCurrentFSMType() != EWRFSM::Idle &&
		PlayerCharacter->IsAIMoveTo() == false)
		return;

	AWRCharacterMine* pMe = Cast< AWRCharacterMine>(PlayerCharacter);
	if (pMe)
	{
		pMe->RequestStop();
	}
}

void WRPlayerCharacterRotator::SetRotationCharacter(const float InDeltaYaw, const float InScale)
{
	PlayerCharacter->AddActorWorldRotation(FRotator(0.0f, InDeltaYaw * InScale, 0.0f));
}

void WRPlayerCharacterRotator::SetRotationHMD(const float InDeltaYaw, const float InScale)
{
	{
		UWRActorComponentVR* VRComponent = PlayerCharacter->GetComponent<UWRActorComponentVR>();
		if (VRComponent != nullptr)
		{
			VRComponent->AddRotationYaw(InDeltaYaw, InScale);
		}
	}
}

void WRPlayerCharacterRotator::Rotation180()
{
	if (bLockRotation180 == true)
		return;

	bLockRotation180 = true;

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		this->SetRotationHMD(180.0f, -1.0f);
		this->SyncRotationbyHMD_Internal();
	}
	else
	{
		this->SetRotation(180.0f, 1.0f);
	}

	//회전값 동기화
	RequestPlayerRotationToSync();
}

//====================================================================================
// [ 2019-8-26 : magedoga ] Mode 1 : ThumbStick
void WRRotationbyThumbStick::Rotation(const float InScale)
{
	if (InScale != 0.0f)
	{
		UCharacterMovementComponent* CharacterMovementComponent = PlayerCharacter->GetCharacterMovement();
		FRotator CurrentRotation = PlayerCharacter->GetActorRotation();
		float DeltaTime = PlayerCharacter->GetWorld()->GetDeltaSeconds();
		FRotator DeltaRotation = CharacterMovementComponent->GetDeltaRotation(DeltaTime);

		SetRotation(DeltaRotation.Yaw, InScale);
	}
}

// void WRRotationbyThumbStick::Rotation180()
// {
// 	this->SetRotationHMD(180.0f, 1.0f);
// 	this->SyncRotationbyHMD();
// }

//====================================================================================
// [ 2019-8-26 : magedoga ] Mode 2 : ThumbStickPerAction
void WRRotationbyThumbStickPerAction::OnEnterThis()
{
	TickRotationPerActionDelegateHandle.Reset();
	TickSyncRotationDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRRotationbyThumbStickPerAction::TickSyncRotation));
}

void WRRotationbyThumbStickPerAction::OnLeaveThis()
{
	if (TickRotationPerActionDelegateHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickRotationPerActionDelegateHandle);
	}

	if (TickSyncRotationDelegateHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickSyncRotationDelegateHandle);
	}
}

void WRRotationbyThumbStickPerAction::Rotation(const float InScale)
{
// 	FString MSG = FString::Format(TEXT("Thumbstick Scale : {0}"), { InScale });
// 	GEngine->AddOnScreenDebugMessage(-1, 0.032f, FColor::Green, MSG);

	// [ 2019-8-26 : magedoga ] Not ready yet
	if (TickRotationPerActionDelegateHandle.IsValid() == true)
		return;

	if (InScale == 0.0f)
	{
		if (false == bAllowAction)
		{
			bAllowAction = true;

			AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
			if (pMe && pMe->GetCurrentFSMType() == EWRFSM::Idle)
			{
				pMe->RequestStop();
			}

		}
		//GEngine->AddOnScreenDebugMessage(1000, 0.032f, FColor::Green, TEXT("Allow Action"), true,FVector2D(5.0f, 5.0f));
		return;
	}

// 	if (InScale == 0.0f)
// 	{
// 		PreScale = InScale;
// 		return;
// 	}
// 
// 	if (PreScale != 0.0f)
// 		return;
// 
// 	if (FMath::Abs(InScale) < 0.95f)
// 		return;
// 
// 	PreScale = InScale;


	// [ 2019-10-16 : magedoga ] 약 회전
	if (FMath::Abs(InScale) < 0.95f)
	{
		float ThumbstickRotationSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationSpeedForThumbStick);
		float DeltaTime = PlayerCharacter->GetWorld()->GetDeltaSeconds();

		float DeltaYaw = ThumbstickRotationSpeed * DeltaTime;
		this->SetRotation(DeltaYaw, InScale < 0.0f ? -1.0f : 1.0f);
// 		FString MSG = FString::Format(TEXT("Thumbstick Rotation - Delta : {0}"), { DeltaYaw });
// 		GEngine->AddOnScreenDebugMessage(1001, 0.032f, FColor::Blue, MSG, false, FVector2D(5.0f, 5.0f));
		return;
	}

	if (bAllowAction == false)
		return;

	// [ 2019-10-16 : magedoga ] 강회전
	bAllowAction = false;

	MoveScale = InScale > 0 ? 1.f : -1.f;
	AccYaw = 0.0f;

	DeltaYawPerAction = WRInputSystemManager::Get()->GetTargetDirAngle(InScale);

	if (DeltaYawPerAction == 0)
	{
		DeltaYawPerAction = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationAnglePerAction);
		if (WRInputSystemManager::Get()->IsPressedLShift())
			DeltaYawPerAction *= 0.1f;
	}

	RotationPerActionTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationAnglePerActionTime);

	TickRotationPerActionDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRRotationbyThumbStickPerAction::TickRotationPerAction));
}

// void WRRotationbyThumbStickPerAction::Rotation180()
// {
// 	SetRotation(180.0f, 1.0f);
// }

bool WRRotationbyThumbStickPerAction::TickRotationPerAction(float DeltaTime)
{
	float Ratio = DeltaTime / RotationPerActionTime; 
	float FrameDeltaYaw = Ratio * DeltaYawPerAction;
	FrameDeltaYaw = FrameDeltaYaw + AccYaw > DeltaYawPerAction ? DeltaYawPerAction - AccYaw : FrameDeltaYaw;

	AccYaw += FrameDeltaYaw;

	this->SetRotation(FrameDeltaYaw, MoveScale);

	if (AccYaw >= DeltaYawPerAction)
	{
		TickRotationPerActionDelegateHandle.Reset();
		return false;
	}

	return true;
}

bool WRRotationbyThumbStickPerAction::TickSyncRotation(float DeltaTime)
{
	if (PlayerCharacter->IsValidLowLevel() == false)
		return false;

	if (bTrace == false)
	{
		float HMDYaw = WRInputSystemManager::Get()->GetHMDRotation().Yaw;
		float PlayerYaw = PlayerCharacter->GetActorRotation().Yaw;
		float AngleForTrace = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationAngleForTraceHMD);

		float Dot = PlayerCharacter->GetActorForwardVector() | PlayerCharacter->GetCharacterMovement()->Velocity.GetSafeNormal();
		bTrace = Dot > 0.0f && FMath::Abs(HMDYaw - PlayerYaw) > AngleForTrace;
	}

	if (bTrace == true && TickRotationPerActionDelegateHandle.IsValid() == false)
	{
		float RotationSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationSpeedForTraceHMD);
		this->SyncRotationbyHMDDelta(DeltaTime, RotationSpeed);

		if (PlayerCharacter->GetCharacterMovement()->Velocity.Size() == 0.0f)
			bTrace = false;
	}


	//====================================================================================

// 	if(TickRotationPerActionDelegateHandle.IsValid() == false)
// 	{
// 		float RotationSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationSpeedForHMD);
// 		this->SyncRotationbyHMDDelta(DeltaTime, RotationSpeed);
// 	}
		
	return true;
}

//====================================================================================
// [ 2019-8-26 : magedoga ] Mode 3 : Trace HMD
void WRRotationbyRotatorHMD::OnEnterThis()
{
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRRotationbyRotatorHMD::Tick));
}

void WRRotationbyRotatorHMD::OnLeaveThis()
{
	if (TickDelegateHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	}
}

// void WRRotationbyRotatorHMD::Rotation180()
// {
// 	FVector HMDLocation;
// 	FRotator HMDRotation;
// 	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);
// 
// 	HMDRotation.Yaw += 180.0f;
// 	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(HMDRotation.Yaw);
// }

bool WRRotationbyRotatorHMD::Tick(float DeltaTime)
{
	float RotationSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationAngleForTraceHMD);
	this->SyncRotationbyHMDDelta(DeltaTime, RotationSpeed);

	return true;
}

//====================================================================================

void WRRotationbyThumbStickPerAction2::OnEnterThis()
{

}

void WRRotationbyThumbStickPerAction2::OnLeaveThis()
{
	if (TickRotationPerActionHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickRotationPerActionHandle);
		TickRotationPerActionHandle.Reset();
	}

	if (TickSyncRotationByHMDHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickSyncRotationByHMDHandle);
		TickSyncRotationByHMDHandle.Reset();
	}
}

void WRRotationbyThumbStickPerAction2::Rotation(const float InScale)
{
	float ABSScale = FMath::Abs(InScale);
#if WITH_EDITOR
	// [ 2020-7-29 : magedoga ] 에디터에서 LShift로 미세조정
	if (WRInputSystemManager::Get()->IsPressedLShift() == true && ABSScale > 0.0f)
	{
		// [ 2020-7-24 : magedoga ] Continuous Rotation
		this->SetState(EWRThumbStickRotationState::Continuous);
		float ThumbstickRotationSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationSpeedForThumbStick);
		float DeltaTime = PlayerCharacter->GetWorld()->GetDeltaSeconds();

		float DeltaYaw = ThumbstickRotationSpeed * DeltaTime;
		this->SetRotation(DeltaYaw, InScale < 0.0f ? -1.0f : 1.0f);
		return;
	}
#endif //WITH_EDITOR

	bool bFullInputStick = ABSScale > 0.97f;
	if (bFullInputStick == false)
	{
		if (InScale == 0.0f)
		{
			if (RotationState == EWRThumbStickRotationState::EndFixedRotation)
			{
				this->SetState(EWRThumbStickRotationState::Wait);
			}

			if (RotationState == EWRThumbStickRotationState::Continuous)
				this->SetState(EWRThumbStickRotationState::Wait);
		}

		return;
	}

	// [ 2020-7-24 : magedoga ] 더블 스틱 액션으로 고정앵글 회전중이면 아무처리 안함
	if (RotationState == EWRThumbStickRotationState::FixedRotation || RotationState == EWRThumbStickRotationState::EndFixedRotation)
	{
		return;
	}

	if (this->IsPossibleFixedRotation(InScale) == true)
	{
		this->StartFixedRotation(InScale);
	}

	return;

	/*
	float IgnoreRange = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationThumbstickMovingIgnoreValue);
	if (FMath::Abs(InScale) < IgnoreRange)
	{
		if (RotationState == EWRThumbStickRotationState::EndFixedRotation)
		{
			this->SetState(EWRThumbStickRotationState::Wait);
		}

		if (RotationState == EWRThumbStickRotationState::Continuous)
			this->SetState(EWRThumbStickRotationState::Wait);

		return;
	}

	float DetectMinValue = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationThumbstickMovingDetectValue);
	UE_LOG(LogTemp, Display, TEXT("WRRotationbyThumbStickPerAction2::Rotation - %f"), FMath::Abs(FMath::Abs(InScale) - FMath::Abs(PreScale)));
	if (FMath::Abs(FMath::Abs(InScale) - FMath::Abs(PreScale)) > DetectMinValue)
	{
		PreScale = InScale;
		UE_LOG(LogTemp, Display, TEXT("WRRotationbyThumbStickPerAction2::Rotation - Ignore"));
		return;
	}

	PreScale = InScale;

#if WITH_EDITOR
	// [ 2020-7-29 : magedoga ] 에디터에서 LShift로 미세조정
	if (WRInputSystemManager::Get()->IsPressedLShift() == false)
#endif //WITH_EDITOR
	{
		// [ 2020-7-29 : magedoga ] Stick을 빠르게 움직이고 있는중일땐 아무행위 안함.
// 		float MovingDetectValue = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationThumbstickMovingDetectRange);
// 		if (FMath::Abs(InScale) - FMath::Abs(PreScale) < MovingDetectValue)
// 		{
// 			PreScale = InScale;
// 			return;
// 		}
// 
// 		PreScale = InScale;

		//if (FMath::Abs(InScale) < 0.995f)
		{
// 			if (RotationState == EWRThumbStickRotationState::EndFixedRotation)
// 			{
// 				this->SetState(EWRThumbStickRotationState::Wait);
// 			}

// 			if (FMath::Abs(InScale) <= DetectRange)
// 			{
// 				if (RotationState == EWRThumbStickRotationState::EndFixedRotation)
// 				{
// 					this->SetState(EWRThumbStickRotationState::Wait);
// 				}
// 
// 				if(RotationState == EWRThumbStickRotationState::Continuous)
// 					this->SetState(EWRThumbStickRotationState::Wait);
// 
// 				return;
// 			}
		}

		// [ 2020-7-24 : magedoga ] 더블 스틱 액션으로 고정앵글 회전중이면 아무처리 안함
		if (RotationState == EWRThumbStickRotationState::FixedRotation || RotationState == EWRThumbStickRotationState::EndFixedRotation)
		{
			return;
		}

		if (this->IsPossibleFixedRotation(InScale) == true)
		{
			this->StartFixedRotation(InScale);
			return;
		}
	}
// 
// #if WITH_EDITOR
// 	if (FMath::Abs(InScale) == 0.0f)
// 		return;
// #endif // WITH_EDITOR
// 
// 	// [ 2020-7-24 : magedoga ] Continuous Rotation
// 	this->SetState(EWRThumbStickRotationState::Continuous);
// 	float ThumbstickRotationSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationSpeedForThumbStick);
// 	float DeltaTime = PlayerCharacter->GetWorld()->GetDeltaSeconds();
// 
// 	float DeltaYaw = ThumbstickRotationSpeed * DeltaTime;
// 	this->SetRotation(DeltaYaw, InScale < 0.0f ? -1.0f : 1.0f);
// 	//this->SetRotationHMD(DeltaYaw, InScale < 0.0f ? -1.0f : 1.0f);
// 	
// 
// 	UE_LOG(LogTemp, Display, TEXT("WRCharacterRotation : Continuous - DeltaTime:[%f] DeltaYaw:[%f]"), DeltaTime, DeltaYaw);

*/
}

void WRRotationbyThumbStickPerAction2::SetSyncRotationByHMD(const bool bInEnable)
{
	// [ 2020-7-27 : magedoga ] On
	if (bInEnable == true && TickSyncRotationByHMDHandle.IsValid() == false)
	{
		TickSyncRotationByHMDHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRRotationbyThumbStickPerAction2::TickSyncRotationByHMD));
		UE_LOG(LogTemp, Display, TEXT("WRCharacterRotation : SyncRotationHMD - On"));
	}

	// [ 2020-7-27 : magedoga ] Off
	if (bInEnable == false && TickSyncRotationByHMDHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickSyncRotationByHMDHandle);
		TickSyncRotationByHMDHandle.Reset();
		UE_LOG(LogTemp, Display, TEXT("WRCharacterRotation : SyncRotationHMD - Off"));
	}
}

void WRRotationbyThumbStickPerAction2::SetSyncRotationByHMD_Once()
{
	WRPlayerCharacterRotator::SyncRotationbyHMD_Internal();
}

bool WRRotationbyThumbStickPerAction2::TickRotationPerAction(float InDeltaTime)
{
	float Ratio = InDeltaTime / RotationPerActionTime;
	float FrameDeltaYaw = Ratio * DeltaYawPerAction;
	FrameDeltaYaw = FrameDeltaYaw + AccYaw > DeltaYawPerAction ? DeltaYawPerAction - AccYaw : FrameDeltaYaw;

	AccYaw += FrameDeltaYaw;

	this->SetRotation(FrameDeltaYaw, MoveScale);

	if (MoveScale != 0.0f)
	{
		//회전 동기화
		RequestPlayerRotationToSync();
	}

	if (AccYaw >= DeltaYawPerAction)
	{
		UE_LOG(LogTemp, Display, TEXT("WRCharacterRotation : FixedRotationEnded"));
		this->SetState(EWRThumbStickRotationState::EndFixedRotation);
		return false;
	}

	return true;
}

bool WRRotationbyThumbStickPerAction2::TickSyncRotationByHMD(float InDeltaTime)
{
	if (TickSyncRotationByHMDHandle.IsValid() == false)
	{
		UE_LOG(LogTemp, Display, TEXT("WRCharacterRotation : TickSyncRotationByHMD FALSE"));
		return false;
	}

	WRPlayerCharacterRotator::SyncRotationbyHMD_Internal();
	return true;
}

void WRRotationbyThumbStickPerAction2::SetState(EWRThumbStickRotationState InNewState)
{
	if (RotationState == InNewState)
	{
		return;
	}

	EWRThumbStickRotationState OldState = RotationState;
	RotationState = InNewState;
	this->OnChangeState(OldState, RotationState);
}

void WRRotationbyThumbStickPerAction2::OnChangeState(EWRThumbStickRotationState InOldState, EWRThumbStickRotationState InNewState)
{

	if (InNewState == EWRThumbStickRotationState::EndFixedRotation)
	{
		TickRotationPerActionHandle.Reset();
	}
}

void WRRotationbyThumbStickPerAction2::StartFixedRotation(float InMoveScale)
{
	UE_LOG(LogTemp, Display, TEXT("WRCharacterRotation : StartFixedRotation"));

	RotationPerActionTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationAnglePerActionTime);
	DeltaYawPerAction = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_RotationAnglePerAction);
	MoveScale = InMoveScale;
	AccYaw = 0.0f;

	this->SetState(EWRThumbStickRotationState::FixedRotation);
	TickRotationPerActionHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRRotationbyThumbStickPerAction2::TickRotationPerAction));
}

bool WRRotationbyThumbStickPerAction2::IsPossibleFixedRotation(const float InScale) const
{
	bool bFull = FMath::Abs(InScale) > 0.995f;
	if (RotationState == EWRThumbStickRotationState::Continuous && bFull == true)
	{
		return true;
	}

	if (RotationState == EWRThumbStickRotationState::Wait && bFull == true)
	{
		return true;
	}

	//if (TickRotationPerActionHandle.IsValid() == true)
	if(RotationState == EWRThumbStickRotationState::FixedRotation)
	{
		return false;
	}

	return bFull == true;
}
