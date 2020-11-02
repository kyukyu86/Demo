
#include "WRFSMExitVehicle.h"
#include "../Vehicle/WRVehicle.h"
#include "Manager/WRVehicleManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/WRCharacterMine.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "../Vehicle/WRVehicleMovementComponent4W.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/Engine.h"
#include "IXRTrackingSystem.h"
#include "WRScreenFader.h"
#include "ModuleManager.h"
#include "Component/Character/WRActorComponentVR.h"
#include "../Character/WRPlayerRotator.h"
#include "Manager/WRConsoleManager.h"


WRFSMExitVehicle::WRFSMExitVehicle()
	:WRFSMBase(EWRFSM::ExitVehicle, EWRFSM::Idle)
{

}

WRFSMExitVehicle::~WRFSMExitVehicle()
{

}

void WRFSMExitVehicle::Begin(const WRFSMInfo& FSMInfo)
{
	WRFSMInfo CopiedFSMInfo = FSMInfo;
	CopiedFSMInfo.JumpToSectionName = WREnumUtility::EnumToString("EWRFSM", EWRFSM::ExitVehicle);
	EWRVehicleSeatSide SeatSide = WRVehicleManager::Get()->GetSeatingSide(CopiedFSMInfo.OwnerCharacter->GetActorID());
	if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
	{
		CopiedFSMInfo.JumpToSectionName += "L";
	}
	else if (SeatSide == EWRVehicleSeatSide::RIGHT_SIDE)
	{
		CopiedFSMInfo.JumpToSectionName += "R";
	}

	ProcessAnimation(CopiedFSMInfo);

	//���� �ִ� ����
	FString strAID = CopiedFSMInfo.DataString[0];
	//ž�¹� aid
	AID_t VehicleAID = FCString::Strtoui64(*strAID, nullptr, 10);
	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(VehicleAID);
	if (pVehicle)
	{
		int32 SeatIndex = WRVehicleManager::Get()->GetSeatIndex(pVehicle->GetVehicleAID(), CopiedFSMInfo.OwnerCharacter->GetActorID());
		if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
		{
			pVehicle->OpenLeftDoor(SeatIndex, true);
		}
		else if (SeatSide == EWRVehicleSeatSide::RIGHT_SIDE)
		{
			pVehicle->OpenRightDoor(SeatIndex, true);
		}

		//detach
		CopiedFSMInfo.OwnerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		//��Ʈ �ε���
		FString RidePositionSocketName = "RidePosition_" + CopiedFSMInfo.DataString[2];
		FVector RideLocation;
		FRotator RideRotation;
		pVehicle->GetMesh()->GetSocketWorldLocationAndRotation(FName(*RidePositionSocketName), RideLocation, RideRotation);

		//�÷��̾� ȸ�� �� ��ġ ����
		RideRotation.Yaw += 90.0f;
		RideRotation.Roll = 0.0f;
		RideRotation.Pitch = 0.0f;
		if (CopiedFSMInfo.OwnerCharacter->IsMe() == false)
		{
			CopiedFSMInfo.OwnerCharacter->SetRotation(RideRotation);
		}
// 		else
// 		{
// 			AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(CopiedFSMInfo.OwnerCharacter);
// 			WRPlayerCharacterRotator* Rotator = MyCharacter->GetRotator();
// 			if (Rotator == nullptr)
// 			{
// 				int32 RotationMode = WRConsoleManager::Get()->GetInt(EWRConsole::Character_RotationMode);
// 				MyCharacter->SetRotationMode((EWRCharacterRotationType)RotationMode);
// 				Rotator = MyCharacter->GetRotator();
// 			}
// 
// 			Rotator->RotateCharacterWithHMD(RideRotation);
// 		}
#ifdef DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE
#else // DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE
		CopiedFSMInfo.OwnerCharacter->SetLocation(RideLocation, true);
#endif // DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE
	}
	
	if (CopiedFSMInfo.OwnerCharacter->IsMe())
	{
		FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
		if (ScreenFader != nullptr)
		{
			ScreenFader->ScreenFadeOut(UWRGameInstance::GetGameInstance(), 0.1f, nullptr, FLinearColor::Black, true, false);
		}
	}
}

void WRFSMExitVehicle::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{	

}

void WRFSMExitVehicle::End(const WRFSMInfo& InReservedFSM)
{
	UWRActorComponentFSM* FSMComponent = InReservedFSM.OwnerCharacter->GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return;

	FString strAID = FSMComponent->GetCurrentFSMInfo()->DataString[0];
	//aid ����
	AID_t VehicleAID = FCString::Strtoui64(*strAID, nullptr, 10);
	AWRVehicle* Vehicle = WRVehicleManager::Get()->FindVehicleByAID(VehicleAID);

	if (nullptr == Vehicle)
	{
		FString MSG = FString::Format(TEXT("WRFSMExitVehicle::End : {0}"), { InReservedFSM.OwnerCharacter->GetBoardingVehicleAID() });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Not found VehicleAID"));
		return;
	}

	//�ɸ��� ȸ�� ����
	InReservedFSM.OwnerCharacter->SetRotation(FRotator(0.0f, Vehicle->GetActorRotation().Yaw, 0.0f));

	//��Ʈ ���̵� ����
	FString strSeatSide = FSMComponent->GetCurrentFSMInfo()->DataString[1];
	EWRVehicleSeatSide SeatSide = (EWRVehicleSeatSide)(FCString::Atoi(*strSeatSide));

	//��Ʈ �ε���
	FString strSeatIndex = FSMComponent->GetCurrentFSMInfo()->DataString[2];
	int32 SeatIndex = FCString::Atoi(*strSeatIndex);

	//���� �ִ� ����
	if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
	{
		Vehicle->OpenLeftDoor(SeatIndex, false);
	}
	else if (SeatSide == EWRVehicleSeatSide::RIGHT_SIDE)
	{
		Vehicle->OpenRightDoor(SeatIndex, false);
	}

	//ī�޶� ����
	if (InReservedFSM.OwnerCharacter->IsMe())
	{
		//���� ���ͷ��� ���� �ڵ����� aid�� ã�ƿ� �� �־�� �Ѵ�..��� ã��...
		APlayerController* PlayerController = Vehicle->GetWorld()->GetFirstPlayerController();
		PlayerController->Possess(InReservedFSM.OwnerCharacter);

/*		Cast<AWRCharacterMine>(InReservedFSM.OwnerCharacter)->GetRotator()->RotateCharacterWithHMD(FRotator(0.0f, Vehicle->GetActorRotation().Yaw, 0.0f));*/

		//���� ī�޶� �� Ȱ��ȭ
		FString CameraName = "Camera_" + FString::FromInt(SeatIndex);
		UCameraComponent* CameraComponent = Cast<UCameraComponent>(Vehicle->GetDefaultSubobjectByName(*CameraName));
		if (CameraComponent)
		{
			CameraComponent->Deactivate();
		}

		//���� ���������� �����Ҷ� ���� �ٽ� ����
		if (SeatIndex == 0)
		{
			Vehicle->SetVehicleSimulatePhysics(false);
		}

// 		if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->IsHeadTrackingAllowed())
// 			GEngine->XRSystem->SetBaseRotation(InReservedFSM.OwnerCharacter->GetActorRotation());
		UWRActorComponentVR* VRComponent = InReservedFSM.OwnerCharacter->GetComponent<UWRActorComponentVR>();
		VRComponent->ResetOrientation(InReservedFSM.OwnerCharacter->GetActorRotation().Yaw, true);

		FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
		if (ScreenFader != nullptr)
		{
			ScreenFader->ScreenFadeIn(UWRGameInstance::GetGameInstance(), 0.5f, nullptr);
		}
	}

	//��Ʈ�ѷ� �ʱ�ȭ
	if (Vehicle->GetVehicleMovement())
	{
		Vehicle->GetVehicleMovement()->SetOverrideController(nullptr);
	}

	//������ ��ġ ������Ʈ �� �浹�ɼ� �ʱ�ȭ
	if (Vehicle->GetMesh())
	{
		InReservedFSM.OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		InReservedFSM.OwnerCharacter->GetCapsuleComponent()->SetEnableGravity(true);
		InReservedFSM.OwnerCharacter->RevertCollision();
	}
	else
	{
		FString MSG = FString::Format(TEXT("WRFSMExitVehicle::Begin : {0}"), { InReservedFSM.OwnerCharacter->GetBoardingVehicleAID() });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Invalid Vehicle"));
	}

	if (SeatIndex == 0)
	{
		Vehicle->InitVehicleState();
	}
}


bool WRFSMExitVehicle::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Idle:
	{
		return true;
	}
	break;
	default:
	{
		return false;
	}
	break;
	}
}

EWRFSM WRFSMExitVehicle::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	return WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);
}

void WRFSMExitVehicle::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMExitVehicle::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}

