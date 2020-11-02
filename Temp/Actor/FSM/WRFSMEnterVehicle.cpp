
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

//어태치 이전의 좌표가 어태치 이후의 상대좌표가 됨
//AttachToActor(InVehicle, FAttachmentTransformRules::KeepRelativeTransform, *InSocketName); 

//어태치 이후에 어태치 되기 직전에 있던 위치와 똑같은 위치의 상대좌표로 세팅됨
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
	//탑승 위치에 따른 몽타주 섹션 이름 구하기
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

	//탑승위치로 순간이동
	AID_t VehicleAID = CopiedFSMInfo.OwnerCharacter->GetBoardingVehicleAID();
	int32 SeatIndex = WRVehicleManager::Get()->GetSeatIndex(VehicleAID, CopiedFSMInfo.OwnerCharacter->GetActorID());
	FString RidePositionSocketName = "RidePosition_" + FString::FromInt(SeatIndex);
	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(VehicleAID);;
	if (pVehicle && pVehicle->GetMesh())
	{
		//차량 애니 시작
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

		//플레이어 회전 및 위치 세팅
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
		//충돌 프리셋이 없으면 루트모션 재생도 안되므로 임시로 범용 으로 바꿔준다.	
		CopiedFSMInfo.OwnerCharacter->SetCollisionProfileNameAndEnabledTemporary(FName("OverlapAll"), ECollisionEnabled::QueryOnly, EWRSetCollisionTo::CapsuleAndMesh);

		//중력도 꺼줌
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

	//attach 및 그라비티 등등 세팅
	int32 SeatIndex = WRVehicleManager::Get()->GetSeatIndex(Vehicle->GetVehicleAID(), InReservedFSM.OwnerCharacter->GetActorID());
	
	//차량 애니 종료
	EWRVehicleSeatSide SeatSide = WRVehicleManager::Get()->GetSeatingSide(InReservedFSM.OwnerCharacter->GetActorID());
	if (SeatSide == EWRVehicleSeatSide::LEFT_SIDE)
	{
		Vehicle->OpenLeftDoor(SeatIndex, false);
	}
	else if (SeatSide == EWRVehicleSeatSide::RIGHT_SIDE)
	{
		Vehicle->OpenRightDoor(SeatIndex, false);
	}

	//내 자신이 운전석에 탑승했을때만 카메라 세팅 및 컨트롤러 세팅 카메라 세팅 및 운전석
	if (InReservedFSM.OwnerCharacter->IsMe())
	{
		UWRActorComponentVR* VRComponent = InReservedFSM.OwnerCharacter->GetComponent<UWRActorComponentVR>();
		FRotator HMDRelativeRotator = VRComponent->GetHMDRelativeRotator();
		//FRotator CameraRotator = FRotator::ZeroRotator;

		//차량 카메라 활성화
		FString CameraName = "Camera_" + FString::FromInt(SeatIndex);
		UCameraComponent* CameraComponent = Cast<UCameraComponent>(Vehicle->GetDefaultSubobjectByName(*CameraName));
		if (CameraComponent)
		{
			CameraComponent->Activate(true);
		}

		//내가 운전자일경우 차량을 possess
		APlayerController* PlayerController = InReservedFSM.OwnerCharacter->GetWorld()->GetFirstPlayerController();
		if (InReservedFSM.OwnerCharacter->IsVehicleDriver())
		{
			PlayerController->Possess(Vehicle);
		}
		else
		{
			PlayerController->SetViewTarget(Vehicle);
		}

		//헤드마운트 초기화
		VRComponent->ResetOrientation(0.0f);

		//내가 운전석에 탑승할때 차량의 물리 켜준다
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

	//탑승이 끝나면
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
	//시동거는 애니를 해야 하기 때문
	//WRFSMVehicleInteraction::Begin 에서 섹션이름을 정해줌
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
