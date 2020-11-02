
#include "WRAutoPilotManager.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Utility/WREnumUtility.h"
#include "WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Vehicle/WRVehicle.h"
#include "Actor/Vehicle/WRVehicleMovementComponent4W.h"
#include "Network/IMMessageSender.h"
#include "WRNoticeManager.h"

WRAutoPilotManager::WRAutoPilotManager()
{

}

WRAutoPilotManager::~WRAutoPilotManager()
{

}

void WRAutoPilotManager::OnInitialize()
{

}

void WRAutoPilotManager::OnShutdown()
{

}

void WRAutoPilotManager::OnLoadLevelStart()
{

}

void WRAutoPilotManager::OnLoadLevelComplete()
{

}

void WRAutoPilotManager::OnRenderStart()
{

}

bool WRAutoPilotManager::OnTick(float InDeltaTime)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return false;

	AWRVehicle* MyVehicle = pMe->GetBoardingVehicle();
	if (nullptr == MyVehicle)
		return false;

	ProcessBraking(InDeltaTime, MyVehicle);

	switch (AutoPilotStep)
	{
	case EWRAutoPilotStep::CommonAutoPilot:
	{
		ProcessCommonAutoPilot(InDeltaTime, MyVehicle);
	}
	break;
	case EWRAutoPilotStep::FullBrake:
	{
		ProcessBrake(InDeltaTime, MyVehicle);
	}
	break;
	case EWRAutoPilotStep::AvoidingFront:
	case EWRAutoPilotStep::AvoidingLeft:
	case EWRAutoPilotStep::AvoidingRight:
	{
		ProcessAvoiding(InDeltaTime, MyVehicle);
	}
	break;
	default:
	{
		if (IsValidSteFlag(EWRAutoPilotStep::PauseAuto))
		{
			ProcessPauseAutoPilot(InDeltaTime, MyVehicle);
		}
	}
	break;
	}

	return true;
}

void WRAutoPilotManager::OnBeginOverlapAutoPilot(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString strSensor = OverlappedComp->ComponentTags[1].ToString();
	EWROverlappedAutoPilotSensor OverlappedSensorType = (EWROverlappedAutoPilotSensor)WREnumUtility::StringToEnum("EWROverlappedAutoPilotSensor", strSensor);
	AddOverlappedSensor(OverlappedSensorType);
}

void WRAutoPilotManager::OnEndOverlapAutoPilot(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FString strSensor = OverlappedComp->ComponentTags[1].ToString();
	EWROverlappedAutoPilotSensor OverlappedSensorType = (EWROverlappedAutoPilotSensor)WREnumUtility::StringToEnum("EWROverlappedAutoPilotSensor", strSensor);
	RemoveOverlappedSensor(OverlappedSensorType);	
}

void WRAutoPilotManager::BeginAutoPilot(AWRVehicle* InVehicle)
{
	if (InVehicle == nullptr)
		return;

	if (InVehicle->GetVehicleInSpawnVolume() == false)
	{
		WRNoticeManager::Get()->AddNotice(46);
		return;
	}

	if (WRCharacterManager::Get()->GetMyCharacter()->GetBoardingVehicle())
	{
		FinalDestLocation = GetDestLocation_Test(WRCharacterManager::Get()->GetMyCharacter()->GetBoardingVehicle());
	}
	else
	{
		FinalDestLocation = DestLocation2;
	}

	if (FinalDestLocation == FVector::ZeroVector)
		return;


	InVehicle->ToggleAutoPilot(true);
	AutoPilotStep = EWRAutoPilotStep::CommonAutoPilot;
	ElapsedTime.Reset();
}

void WRAutoPilotManager::EndAutoPilot(AWRVehicle* InVehicle)
{
	if (InVehicle == nullptr)
		return;

	if (IsActiveAutoPilot() == false)
		return;

	InVehicle->ToggleAutoPilot(false);
	InVehicle->RequestAccelator(0.0f);

	AutoPilotStep = EWRAutoPilotStep::None;
	OverlappedSensor = EWROverlappedAutoPilotSensor::None;

	ElapsedTime.Reset();
}

void WRAutoPilotManager::PauseAutoPilot()
{
	if (AutoPilotStep != EWRAutoPilotStep::None)
	{
		AutoPilotStep |= EWRAutoPilotStep::PauseAuto;
	}
}

void WRAutoPilotManager::ResumeAutoPilot()
{
	if (IsValidSteFlag(EWRAutoPilotStep::PauseAuto) == false)
		return;

	AutoPilotStep &= ~EWRAutoPilotStep::PauseAuto;
}

bool WRAutoPilotManager::IsActiveAutoPilot()
{
	if (AutoPilotStep == EWRAutoPilotStep::None)
	{
		return false;
	}

	return true;
}

bool WRAutoPilotManager::IsValidSteFlag(EWRAutoPilotStep InFlag)
{
	if ((AutoPilotStep & InFlag) != EWRAutoPilotStep::None)
		return true;

	return false;
}

bool WRAutoPilotManager::IsvalidSensorFlag(EWROverlappedAutoPilotSensor InFlag)
{
	if ((OverlappedSensor & InFlag) != EWROverlappedAutoPilotSensor::None)
		return true;

	return false;
}

void WRAutoPilotManager::AddOverlappedSensor(EWROverlappedAutoPilotSensor InState)
{
	OverlappedSensor |= InState;

	if (AutoPilotStep != EWRAutoPilotStep::CommonAutoPilot &&
		AutoPilotStep != EWRAutoPilotStep::None)
	{
		if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FLSensor) && 
			IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FRSensor))
		{
			//정면에 장애물
			AutoPilotStep = EWRAutoPilotStep::AvoidingFront;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FLSensor))
		{
			//왼쪽에 장애물
			AutoPilotStep = EWRAutoPilotStep::AvoidingLeft;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FRSensor))
		{
			//오른쪽에 장애물
			AutoPilotStep = EWRAutoPilotStep::AvoidingRight;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::RSensor))
		{
		}
		else
		{
			AutoPilotStep = EWRAutoPilotStep::CommonAutoPilot;
		}
	}	
}

void WRAutoPilotManager::RemoveOverlappedSensor(EWROverlappedAutoPilotSensor InState)
{
	OverlappedSensor &= ~InState;

	if (AutoPilotStep != EWRAutoPilotStep::CommonAutoPilot &&
		AutoPilotStep != EWRAutoPilotStep::None)
	{
		if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FLSensor) &&
			IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FRSensor))
		{
			AutoPilotStep = EWRAutoPilotStep::AvoidingFront;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FLSensor))
		{
			AutoPilotStep = EWRAutoPilotStep::AvoidingLeft;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FRSensor))
		{
			AutoPilotStep = EWRAutoPilotStep::AvoidingRight;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::RSensor))
		{
			//AutoPilotStep = EWRAutoPilotStep::AvoidingFront;
		}
		else
		{
			AutoPilotStep = EWRAutoPilotStep::CommonAutoPilot;
		}
	}
}

void WRAutoPilotManager::ProcessBraking(float InDeltaTime, AWRVehicle* InVehicle)
{
	UWRVehicleMovementComponent4W* VehicleMovement = InVehicle->GetWRVehicleMovement();
	if (nullptr == VehicleMovement)
		return;

	if (AutoPilotStep == EWRAutoPilotStep::None)
		return;

	int32 CurrentSpeed = VehicleMovement->GetSpeedByKM();
	float Distance2Dest = (FinalDestLocation - InVehicle->GetActorLocation()).Size();
	bool bBraking = false;
	if (CurrentSpeed < 30)
	{
		if (Distance2Dest < 500.0f)
		{
			bBraking = true;
		}
	}
	else if (CurrentSpeed < 50)
	{
		if (Distance2Dest < 700.f)
		{
			bBraking = true;
		}
	}
	else if (CurrentSpeed < 70)
	{
		if (Distance2Dest < 1000.0f)
		{
			bBraking = true;
		}
	}
	else if (CurrentSpeed < 100)
	{
		if (Distance2Dest < 1200.0f)
		{
			bBraking = true;
		}
	}
	else if (CurrentSpeed < 120)
	{
		if (Distance2Dest < 1500.0f)
		{
			bBraking = true;
		}
	}
	else
	{
		if (Distance2Dest < 2000.0f)
		{
			bBraking = true;
		}
	}

	if (bBraking)
	{
		AutoPilotStep = EWRAutoPilotStep::FullBrake;
	}
}

void WRAutoPilotManager::ProcessPauseAutoPilot(float InDeltaTime, AWRVehicle* InVehicle)
{
	UWRVehicleMovementComponent4W* VehicleMovement = InVehicle->GetWRVehicleMovement();
	if (nullptr == VehicleMovement)
		return;

	if (VehicleMovement->GetCurrentGear() <= -1)
	{
		InVehicle->RequestAccelator(-1.0f);
	}
	else if (1 <= VehicleMovement->GetCurrentGear())
	{
		InVehicle->RequestAccelator(1.0f);
	}
}

void WRAutoPilotManager::ProcessCommonAutoPilot(float InDeltaTime, AWRVehicle* InVehicle)
{
	UWRVehicleMovementComponent4W* VehicleMovement = InVehicle->GetWRVehicleMovement();
	if (nullptr == VehicleMovement)
		return;

	if (ElapsedTime.TickElapsedTime(InDeltaTime) == false)
		return;
	   	   
	////////////////////////////////////////////////////////////////////
	//속도 체크
	FVector DirToDest = (FinalDestLocation - InVehicle->GetActorLocation()).GetSafeNormal();
	FVector ForwardDir = InVehicle->GetActorForwardVector();

	//브레이크 입력 상태면 브레이크를 풀어준다
	if (VehicleMovement->GetHandBrakeInput())
	{
		InVehicle->RequestEndBrake();
	}

	////////////////////////////////////////////////////////////////////
	//회전 체크
	FVector RightDir = InVehicle->GetActorRightVector();
	RightDir.Z = 0.0f;
	DirToDest.Z = 0.0f;
	float fRightDot = UKismetMathLibrary::Dot_VectorVector(RightDir, DirToDest);

	//각 충돌 센서에 맞게 스티어링과 엑셀입력을 변경
	int32 CurrentSpeed = VehicleMovement->GetSpeedByKM();
	if (CurrentSpeed < 15.0f)
	{
		bool bAdjustSteeringValue = true;
		float SteeringValue = fRightDot;
		float ThrottleValue = 0.0f;
		if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FLSensor) &&
			IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FRSensor))
		{
			SteeringValue = fRightDot * -1.0f;
			ThrottleValue = -0.5f;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FLSensor))
		{
			SteeringValue = fRightDot * -1.0f;
			ThrottleValue = -0.5f;
		}
		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::FRSensor))
		{
			SteeringValue = fRightDot * -1.0f;
			ThrottleValue = -0.5f;
		}

		else if (IsvalidSensorFlag(EWROverlappedAutoPilotSensor::RSensor))
		{
			SteeringValue = fRightDot;
			ThrottleValue = 0.5f;
		}
		else
		{
			SteeringValue = fRightDot;
			ThrottleValue = 1.0f;
			bAdjustSteeringValue = false;
		}

		if (bAdjustSteeringValue)
		{
			if (0 <= SteeringValue)
			{
				SteeringValue = 1.0f;
			}
			else
			{
				SteeringValue = -1.0f;
			}
		}

		InVehicle->RequestSteering(SteeringValue);
		InVehicle->RequestAccelator(ThrottleValue);
	}
	else
	{
		static float testSteering = 2.0f;
		InVehicle->RequestSteering(fRightDot * testSteering);
		InVehicle->RequestAccelator(1.0f);
	}
}

void WRAutoPilotManager::ProcessBrake(float InDeltaTime, AWRVehicle* InVehicle)
{
	UWRVehicleMovementComponent4W* VehicleMovement = InVehicle->GetWRVehicleMovement();
	if (nullptr == VehicleMovement)
		return;

	int32 CurrentSpeed = VehicleMovement->GetSpeedByKM();
	if (1 < CurrentSpeed)
	{
		//감속
		InVehicle->RequestBeginBrake();
		InVehicle->RequestAccelator(-1.0f, false);
	}
	else
	{
		//감속
		//InVehicle->RequestEndBrake();
		InVehicle->RequestAccelator(0.0f, false);

		EndAutoPilot(InVehicle);
	}
}

void WRAutoPilotManager::ProcessAvoiding(float InDeltaTime, AWRVehicle* InVehicle)
{

}

FVector WRAutoPilotManager::GetDestLocation_Test(AWRVehicle* InVehicle)
{
	FVector ForwardDir = InVehicle->GetActorForwardVector();

	FVector DirToDest1 = (DestLocation1 - InVehicle->GetActorLocation()).GetSafeNormal();
	float fDot = UKismetMathLibrary::Dot_VectorVector(ForwardDir, DirToDest1);
	if (0 <= fDot)
	{
		return DestLocation1;
	}
	else
	{
		FVector DirToDest2 = (DestLocation2 - InVehicle->GetActorLocation()).GetSafeNormal();
		fDot = UKismetMathLibrary::Dot_VectorVector(ForwardDir, DirToDest2);
		if (0 <= fDot)
		{
			return DestLocation2;
		}
	}

	return FVector::ZeroVector;
}
