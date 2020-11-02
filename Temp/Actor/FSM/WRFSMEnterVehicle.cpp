
#include "WRFSMEnterVehicle.h"
#include "../Vehicle/WRVehicle.h"
#include "Manager/WRVehicleManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "../Character/WRCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "WheeledVehicleMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Animation/WRAnimInstance.h"
#include "WRScreenFader.h"
#include "ModuleManager.h"
#include "../Character/WRCharacterMine.h"
#include "Component/Character/WRActorComponentVR.h"

//����ġ ������ ��ǥ�� ����ġ ������ �����ǥ�� ��
//AttachToActor(InVehicle, FAttachmentTransformRules::KeepRelativeTransform, *InSocketName); 

//����ġ ���Ŀ� ����ġ �Ǳ� ������ �ִ� ��ġ�� �Ȱ��� ��ġ�� �����ǥ�� ���õ�
//AttachToActor(InVehicle, FAttachmentTransformRules::KeepWorldTransform, *InSocketName);

WRFSMEnterVehicle::WRFSMEnterVehicle()
	:WRFSMBase(EWRFSM::EnterVehicle, EWRFSM::DrivingIdle)
{

}

WRFSMEnterVehicle::~WRFSMEnterVehicle()
{

}

void WRFSMEnterVehicle::Begin(const WRFSMInfo& FSMInfo)
{
	WRFSMInfo CopiedFSMInfo = FSMInfo;
	//ž�� ��ġ�� ���� ��Ÿ�� ���� �̸� ���ϱ�
	CopiedFSMInfo.JumpToSectionName = WREnumUtility::EnumToString("EWRFSM", EWRFSM::EnterVehicle);
	EWRVehicleSeatSide SeatSide = WRVehicleManager::Get()->GetSeatingSide(CopiedFSMInfo.OwnerCharacter->GetActorID());
	if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
	{
		CopiedFSMInfo.JumpToSectionName += "L";
	}
	else if (SeatSide == EWRVehicleSeatSide::RIGHT_SIDE)
	{
		CopiedFSMInfo.JumpToSectionName += "R";
	}

	//ž����ġ�� �����̵�
	AID_t VehicleAID = CopiedFSMInfo.OwnerCharacter->GetBoardingVehicleAID();
	int32 SeatIndex = WRVehicleManager::Get()->GetSeatIndex(VehicleAID, CopiedFSMInfo.OwnerCharacter->GetActorID());
	FString RidePositionSocketName = "RidePosition_" + FString::FromInt(SeatIndex);
	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(VehicleAID);;
	if (pVehicle && pVehicle->GetMesh())
	{
		//���� �ִ� ����
		if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
		{
			pVehicle->OpenLeftDoor(SeatIndex, true);
		}
		else if (SeatSide == EWRVehicleSeatSide::RIGHT_SIDE)
		{
			pVehicle->OpenRightDoor(SeatIndex, true);
		}


		FVector RideLocation;
		FRotator RideRotation;
		pVehicle->GetMesh()->GetSocketWorldLocationAndRotation(FName(*RidePositionSocketName), RideLocation, RideRotation);
		RideRotation.Yaw += 90.0f;

		//�÷��̾� ȸ�� �� ��ġ ����
		if (CopiedFSMInfo.OwnerCharacter->IsMe() == true)
		{
			//UWRActorComponentVR* VRComponent = CopiedFSMInfo.OwnerCharacter->GetComponent<UWRActorComponentVR>();
			//VRComponent->ResetOrientation(RideRotation.Yaw);
			Cast<AWRCharacterMine>(CopiedFSMInfo.OwnerCharacter)->GetRotator()->RotateCharacterWithHMD(RideRotation);	
		}
		else
		{
			CopiedFSMInfo.OwnerCharacter->SetRotation(RideRotation);
		}
		
		CopiedFSMInfo.OwnerCharacter->SetLocation(RideLocation, true);	

#ifdef DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE
		//�浹 �������� ������ ��Ʈ��� ����� �ȵǹǷ� �ӽ÷� ���� ���� �ٲ��ش�.	
		CopiedFSMInfo.OwnerCharacter->SetCollisionProfileNameAndEnabledTemporary(FName("OverlapAll"), ECollisionEnabled::QueryOnly, EWRSetCollisionTo::CapsuleAndMesh);

		//�߷µ� ����
		CopiedFSMInfo.OwnerCharacter->GetCharacterMovement()->GravityScale = 0.f;
		CopiedFSMInfo.OwnerCharacter->GetCapsuleComponent()->SetEnableGravity(false);
#else // DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE
		CopiedFSMInfo.OwnerCharacter->SetCollisionProfileNameAndEnabledTemporary(FName("OverlapAll"), ECollisionEnabled::QueryOnly, EWRSetCollisionTo::MeshOnly);
#endif // DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE


	}
	else
	{
		FString MSG = FString::Format(TEXT("WRFSMEnterVehicle::Begin : {0}"), { VehicleAID });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Invalid Vehicle"));
	}

	if (FSMInfo.OwnerCharacter->IsMe())
	{
		FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
		if (ScreenFader != nullptr)
		{
			ScreenFader->ScreenFadeOut(UWRGameInstance::GetGameInstance(), 0.5f, nullptr, FLinearColor::Black, true, false);
		}
	}

	ProcessAnimation(CopiedFSMInfo);
}

void WRFSMEnterVehicle::Tick(AWRCharacter* OwnerCharacter, float DeltaTime)
{

}

void WRFSMEnterVehicle::End(const WRFSMInfo& InReservedFSM)
{
	AWRVehicle* Vehicle = InReservedFSM.OwnerCharacter->GetBoardingVehicle();
	if (nullptr == Vehicle)
	{
		FString MSG = FString::Format(TEXT("WRFSMEnterVehicle::End : {0}"), { InReservedFSM.OwnerCharacter->GetBoardingVehicleAID() });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Not found VehicleAID"));
		return;
	}

	//attach �� �׶��Ƽ ��� ����
	int32 SeatIndex = WRVehicleManager::Get()->GetSeatIndex(Vehicle->GetVehicleAID(), InReservedFSM.OwnerCharacter->GetActorID());
	
	//���� �ִ� ����
	EWRVehicleSeatSide SeatSide = WRVehicleManager::Get()->GetSeatingSide(InReservedFSM.OwnerCharacter->GetActorID());
	if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
	{
		Vehicle->OpenLeftDoor(SeatIndex, false);
	}
	else if (SeatSide == EWRVehicleSeatSide::RIGHT_SIDE)
	{
		Vehicle->OpenRightDoor(SeatIndex, false);
	}

	//�� �ڽ��� �������� ž���������� ī�޶� ���� �� ��Ʈ�ѷ� ���� ī�޶� ���� �� ������
	if (InReservedFSM.OwnerCharacter->IsMe())
	{
		UWRActorComponentVR* VRComponent = InReservedFSM.OwnerCharacter->GetComponent<UWRActorComponentVR>();
		FRotator HMDRelativeRotator = VRComponent->GetHMDRelativeRotator();
		//FRotator CameraRotator = FRotator::ZeroRotator;

		//���� ī�޶� Ȱ��ȭ
		FString CameraName = "Camera_" + FString::FromInt(SeatIndex);
		UCameraComponent* CameraComponent = Cast<UCameraComponent>(Vehicle->GetDefaultSubobjectByName(*CameraName));
		if (CameraComponent)
		{
			CameraComponent->Activate(true);
		}

		//���� �������ϰ�� ������ possess
		APlayerController* PlayerController = InReservedFSM.OwnerCharacter->GetWorld()->GetFirstPlayerController();
		if (InReservedFSM.OwnerCharacter->IsVehicleDriver())
		{
			PlayerController->Possess(Vehicle);
		}
		else
		{
			PlayerController->SetViewTarget(Vehicle);
		}

		//��帶��Ʈ �ʱ�ȭ
		VRComponent->ResetOrientation(0.0f);

		//���� �������� ž���Ҷ� ������ ���� ���ش�
		if (SeatIndex == 0)
		{
			Vehicle->SetVehicleSimulatePhysics(true);
		}
	}
	else
	{
		Vehicle->GetVehicleMovement()->SetOverrideController(InReservedFSM.OwnerCharacter->GetController());
		UWRAnimInstance* AnimInstance = InReservedFSM.OwnerCharacter->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->ResetHMDRotation();
		}
	}

	//ž���� ������
#ifdef DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE
#else // DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE
	InReservedFSM.OwnerCharacter->GetCharacterMovement()->GravityScale = 0.f;
	InReservedFSM.OwnerCharacter->GetCapsuleComponent()->SetEnableGravity(false);
	InReservedFSM.OwnerCharacter->SetCollisionProfileNameAndEnabledTemporary(FName("OverlapAll"), ECollisionEnabled::QueryOnly, EWRSetCollisionTo::CapsuleAndMesh);
#endif // DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE

	FString SeatPositionSocketName = "SeatPosition_" + FString::FromInt(SeatIndex);	
	InReservedFSM.OwnerCharacter->AttachToActor(Vehicle, FAttachmentTransformRules::SnapToTargetNotIncludingScale, *SeatPositionSocketName);

	if (InReservedFSM.OwnerCharacter->IsMe())
	{
		FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
		if (ScreenFader != nullptr)
		{
			ScreenFader->ScreenFadeIn(UWRGameInstance::GetGameInstance(), 0.1f, nullptr);
		}
	}
}

bool WRFSMEnterVehicle::IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo)
{
	if (WRFSMBase::IsHighPriorityThanCurrentFSM(InFSMInfo) == false)
		return false;

	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::DrivingIdle:
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

EWRFSM WRFSMEnterVehicle::GetNextFSMWhenAnimationEnd(AWRCharacter* InChar)
{
	//�õ��Ŵ� �ִϸ� �ؾ� �ϱ� ����
	//WRFSMVehicleInteraction::Begin ���� �����̸��� ������
	if (InChar->IsVehicleDriver())
	{
		return EWRFSM::VehicleInteraction;
	}
	else
	{
		return WRFSMBase::GetNextFSMWhenAnimationEnd(InChar);
	}
}

void WRFSMEnterVehicle::ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter)
{

}

void WRFSMEnterVehicle::ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter)
{

}
