
#include "WRSpawnSimulationManager.h"

#include "Actor/Vehicle/WRVehicle.h"
#include "Actor/Character/WRCharacterIO.h"

#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Component/Vehicle/WRVehicleComponentManageMaterial.h"

#include "Define/WRPathDefine.h"

#include "Table/WRCustomTextTable.h"
#include "Table/Base/WRTableManager.h"

#include "Utility/WRActorUtility.h"

#include "WRNoticeManager.h"
#include "WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "WRInputSystemManager.h"
#include "Utility/WRUIUtility.h"
#include "Kismet/KismetSystemLibrary.h"


WRSpawnSimulationManager::WRSpawnSimulationManager()
{

}

WRSpawnSimulationManager::~WRSpawnSimulationManager()
{

}

void WRSpawnSimulationManager::OnInitialize()
{
	SpawnSimulationParam.Reset();
}

void WRSpawnSimulationManager::OnShutdown()
{
}

void WRSpawnSimulationManager::OnLoadLevelStart()
{
	WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MATERIAL_PATH_4_CONSTRUCTIBLE, FWRCompleteAsyncLoad::CreateRaw(this, &WRSpawnSimulationManager::OnComplete));
}

void WRSpawnSimulationManager::OnLoadLevelComplete()
{

}

void WRSpawnSimulationManager::OnRenderStart()
{

}

bool WRSpawnSimulationManager::OnTick(float InDeltaTime)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return true;

	return true;
}

void WRSpawnSimulationManager::OnBeginOverlapSimulationActor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (SpawnSimulationParam.SimulationState == EWRSpawnSimulationState::ReadyToSimulation)
	{
		SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::InvalidLocation;
	}
}

void WRSpawnSimulationManager::OnEndOverlapSimulationActor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (SpawnSimulationParam.SimulationState == EWRSpawnSimulationState::InvalidLocation)
	{
		SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::ReadyToSimulation;
	}
}

void WRSpawnSimulationManager::AsyncSpawnSimulationActor(const FString& InBpPath, const FVector& InForward, const FVector& InRight, const FSimulationDelegateHandles& InDelegateHandles)
{
	//기존에 세팅된 정보가 있으면 지워야 한다
	InitializeSimulationParam();
		
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe)
	{	
		//로딩 이미지
		FWRWidgetInfo LoadingInfo(EWRUIPanelKind::SpawnLoading, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCameraAll, nullptr, pMe);
		strSimulationUIAsyncKey = WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(LoadingInfo);
	}

	//로드 시작
	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRSpawnSimulationManager::OnSpawnCompleted);
	SpawnSimulationParam.HashKeyToSimulate = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(InBpPath, AsyncLoadDele);
	if (SpawnSimulationParam.HashKeyToSimulate == WRASYNCLOADMANAGER_INVALID)
	{
		//생성이 실패 했으므로 async 걸어둔 ui도 취소 시켜준다		
		WRUIManager::Get()->CancelUIAsync(strSimulationUIAsyncKey);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("AsyncSpawnSimulationActor"), TEXT("Spawn failed to simulate"));
		return;
	}

	SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::AskedSpawn;
	SpawnSimulationParam.ForwardVector = InForward;
	SpawnSimulationParam.RightVector = InRight;
	SpawnSimulationParam.SimulationDelegateHandles = InDelegateHandles;

	if (SpawnSimulationParam.SimulationDelegateHandles.OKDele.IsBound() == false)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("AsyncSpawnSimulationActor"), TEXT("OKDele must be bound"));
	}

	//취소용 함수
	if (SpawnSimulationParam.SimulationDelegateHandles.CancelDele.IsBound() == false)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("AsyncSpawnSimulationActor"), TEXT("CancelDele must be bound"));
	}
}

void WRSpawnSimulationManager::OnSpawnCompleted(UObject* InActor, FString InHashKey)
{
	//상태 바꾸고
	SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::ReadyToSimulation;

	//스폰된 액터 저장
	SpawnSimulationParam.Actor = Cast<AActor>(InActor);

	//스폰 완료 함수 호출
	SpawnSimulationParam.SimulationDelegateHandles.SpawnCompletedDele.ExecuteIfBound(InActor, InHashKey);

	//로딩 이미지 끄기
	CloseUI(EWRUIPanelKind::SpawnLoading);
		
	//스폰 가능 지역인지 체크
	bool bOpenGuideUI = true;
	if (SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.IsBound())
	{
		EWRSimulationResultType Result = SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.Execute(SpawnSimulationParam.Actor);
		if (EWRSimulationResultType::Success == Result)
		{
			//제대로 된 위치에 생성되었을때는 아무것도 안함
		}
		else
		{
			SpawnSimulationParam.SimulationDelegateHandles.MessageDele.Execute(Result);

			bOpenGuideUI = false;
			ExecuteCancelDele();
		}
	}
	else
	{
		//스폰 완료 함수 호출
		SpawnSimulationParam.SimulationDelegateHandles.SpawnCompletedDele.ExecuteIfBound(InActor, InHashKey);
	}
	
	//가이드 ui open
	if (bOpenGuideUI)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::SpawnSimulationGuide, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCameraAll, nullptr, pMe);
			strSimulationUIAsyncKey = WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		}
	}
}

bool WRSpawnSimulationManager::InputEvent(EWRInput InInputType, EInputEvent InEventType, float InScale /*= 0.0f*/)
{
	if (EWRSpawnSimulationState::ReadyToSimulation != WRSpawnSimulationManager::Get()->GetSimulationState() &&
		EWRSpawnSimulationState::InvalidLocation != WRSpawnSimulationManager::Get()->GetSimulationState())
		return false;

	bool bInputPass = false;
	switch (InInputType)
	{
	case EWRInput::Circle:
	{
		if (InEventType == EInputEvent::IE_Pressed)
		{
			if (SpawnSimulationParam.SimulationState == EWRSpawnSimulationState::ReadyToSimulation)
			{
				// 서버에 요청
				SpawnSimulationParam.SimulationDelegateHandles.OKDele.ExecuteIfBound();

				// 서버에 생성요청 햇으므로 삭제
				// 취소는 액터를 삭제시키는 로직이 기본이므로 여기서 바로 삭제				
				ExecuteCancelDele();
			}
			else if (SpawnSimulationParam.SimulationState == EWRSpawnSimulationState::InvalidLocation)
			{
				SpawnSimulationParam.SimulationDelegateHandles.MessageDele.ExecuteIfBound(EWRSimulationResultType::InvalidLocation);
			}
		}
	}
	break;
	case EWRInput::Cross:
	{
		if (InEventType == EInputEvent::IE_Pressed)
		{
			ExecuteCancelDele();
		}
	}
	break;
	case EWRInput::L2:
	{
		//좌회전
		/*if (InEventType == EInputEvent::IE_Pressed ||
			InEventType == EInputEvent::IE_Repeat)
		{
			RotateActor(-2.0f);
		}*/
	}
	break;
	case EWRInput::R2:
	{
		//우회전
		/*if (InEventType == EInputEvent::IE_Pressed ||
			InEventType == EInputEvent::IE_Repeat)
		{
			RotateActor(2.0f);
		}*/
	}
	break;
	case EWRInput::DpadUP:
	{
		//앞으로 이동
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToForward(10.0f);*/
	}
	break;
	case EWRInput::DpadDown:
	{
		//뒤로 이동
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToForward(-10.0f);*/
	}
	break;
	case EWRInput::DpadLeft:
	{
		//좌로 이동
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToRight(-10.0f);*/
	}
	break;
	case EWRInput::DpadRight:
	{
		//우로 이동
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToRight(10.0f);*/
	}
	break;
	case EWRInput::LStickX:
	{
		//앞으로 이동
		if (InEventType == EInputEvent::IE_Repeat)
			MoveActorToForward(InScale * 10.0f);
	}
	break;
	case EWRInput::LStickY:
	{
		//우로 이동
		if (InEventType == EInputEvent::IE_Repeat)
			MoveActorToRight(InScale * 10.0f);
	}
	break;
	case EWRInput::RStickX:
	{
		//아마 나중에 상하조정으로 들어가지 않을까...
	}
	break;
	case EWRInput::RStickY:
	{
		//회전
		if (InEventType == EInputEvent::IE_Repeat)
		{
			RotateActor(InScale * 2.0f);
		}
	}
	break;
	default:
	{
		return false;
	}
	break;
	}

	return true;
}

bool WRSpawnSimulationManager::IsValidSimulation()
{
	return SpawnSimulationParam.IsValid();
}

void WRSpawnSimulationManager::InitializeSimulationParam()
{
	if (SpawnSimulationParam.HashKeyToSimulate != "" && SpawnSimulationParam.Actor == nullptr) //아직 스폰이 안된상태면
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*SpawnSimulationParam.HashKeyToSimulate);
	}
	else
	{
		SpawnSimulationParam.SimulationDelegateHandles.CancelDele.ExecuteIfBound();
	}

	SpawnSimulationParam.Reset();
}

void WRSpawnSimulationManager::ResetSimulationParam()
{
	if (SpawnSimulationParam.HashKeyToSimulate != "" && SpawnSimulationParam.Actor == nullptr) //아직 스폰이 안된상태면
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*SpawnSimulationParam.HashKeyToSimulate);
	}

	SpawnSimulationParam.Reset();

	//가이드 ui off
	CloseUI(EWRUIPanelKind::SpawnSimulationGuide);
}

void WRSpawnSimulationManager::ExecuteCancelDele()
{
	SpawnSimulationParam.SimulationDelegateHandles.CancelDele.ExecuteIfBound();

	// 상태 초기화
	SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::None;
	ResetSimulationParam();
}

FSpawnSimulationParam WRSpawnSimulationManager::GetSimulationParam()
{
	return SpawnSimulationParam;
}

FVector WRSpawnSimulationManager::GetSimulationActorLocation()
{
	return SpawnSimulationParam.Actor->GetActorLocation();
}

FRotator WRSpawnSimulationManager::GetSimulationActorRotation()
{
	return SpawnSimulationParam.Actor->GetActorRotation();
}

bool WRSpawnSimulationManager::IsMySpawnSimulationActor(const FString& InHashKey)
{
	if (InHashKey == SpawnSimulationParam.HashKeyToSimulate)
	{
		return true;
	}

	return false;
}

EWRSpawnSimulationState WRSpawnSimulationManager::GetSimulationState()
{
	return SpawnSimulationParam.SimulationState;
}

void WRSpawnSimulationManager::MoveActorToForward(float InDistance)
{
	if (IsValidSimulation() == false)
		return;

	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	//이동시킬 새좌표
	FVector NewLocation = SpawnSimulationParam.Actor->GetActorLocation();
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		NewLocation += WRInputSystemManager::Get()->GetHMDForwardVector() * InDistance;
	}
	else
	{
		NewLocation += pMe->GetActorForwardVector() * InDistance;
	}

	DetermineValidLocation(NewLocation);
}

void WRSpawnSimulationManager::MoveActorToRight(float InDistance)
{
	if (IsValidSimulation() == false)
		return;

	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	//이동시킬 새좌표
	FVector NewLocation = SpawnSimulationParam.Actor->GetActorLocation();
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		NewLocation += WRInputSystemManager::Get()->GetHMDRightVector() * InDistance;
	}
	else
	{
		NewLocation += pMe->GetActorRightVector() * InDistance;
	}

	DetermineValidLocation(NewLocation);
}

void WRSpawnSimulationManager::DetermineValidLocation(FVector InLocation)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	//이동시킨후에 위로 액터 위치 보정해줄 z값 구하기
	FVector CurrentFloorLocation = WRActorUtility::ProjectPointToNavigation(SpawnSimulationParam.Actor->GetActorLocation());
	if (WRActorUtility::IsOnNavigation(pMe->GetWorld(), CurrentFloorLocation) == false)
	{
		CurrentFloorLocation = SpawnSimulationParam.Actor->GetActorLocation();
	}

	//새좌표의 바닥좌표
	float HeightGap = SpawnSimulationParam.Actor->GetActorLocation().Z - CurrentFloorLocation.Z;
	FVector NextFloorLocation = WRActorUtility::ProjectPointToNavigation(InLocation);
	if (WRActorUtility::IsOnNavigation(pMe->GetWorld(), InLocation))
	{
		//이동한 좌표의 높이값 : 이동한 위치의 바닥좌표 + 이전 좌표에서의 바닥과 액터의 높이차이
		InLocation.Z = NextFloorLocation.Z + HeightGap;
	}
	else
	{
		InLocation.Z = CurrentFloorLocation.Z;
	}

	//해당 좌표가 invalid하면 되돌릴 prelocation 좌표를 미리 받아둔다
	FVector PreLocation = SpawnSimulationParam.Actor->GetActorLocation();
	//이동시킬 좌표를 세팅하고 유효한상태인지 체크
	SpawnSimulationParam.Actor->SetActorLocation(InLocation);

	if (SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.IsBound())
	{
		EWRSimulationResultType Result = SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.Execute(SpawnSimulationParam.Actor);
		if (EWRSimulationResultType::Success == Result)
		{
		}
		else
		{
			//유효하지 못한 좌표면 이전 좌표로 되돌린다
			SpawnSimulationParam.Actor->SetActorLocation(PreLocation);
		}
	}
}

void WRSpawnSimulationManager::RotateActor(float InAngle)
{
	if (IsValidSimulation() == false)
		return;

	FRotator NewRot = SpawnSimulationParam.Actor->GetActorRotation();
	NewRot.Yaw += InAngle;
	SpawnSimulationParam.Actor->SetActorRotation(NewRot);
}

void WRSpawnSimulationManager::CloseUI(const EWRUIPanelKind IN InUIKind)
{
	if (WRUIManager::Get()->IsWidgetOpened(InUIKind))
	{
		WRUIManager::Get()->CloseUI(InUIKind);
	}
	else
	{
		WRUIManager::Get()->CancelUIAsync(strSimulationUIAsyncKey);
	}
}

void WRSpawnSimulationManager::OnComplete(UObject* InObject, FString InHash)
{
	UE_LOG(WRLog, Display, TEXT("[WRSpawnSimulationManager::OnComplete] Material was loaded."));
}


void WRSpawnSimulationManager::UpdateActorLocationInVolume_deprecated(AActor* InActor, bool bInVolume)
{
	if (SpawnSimulationParam.Actor != InActor)
		return;

// 	if (WRActorUtility::IsOnNavigation(UWRGameInstance::GetGameInstance()->GetWorld(), InActor->GetActorLocation()))
// 	{
// 		if (bInVolume)
// 		{
// 			SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::ReadyToSimulation;
// 			if (SpawnSimulationParam.SimulationDelegateHandles.MaterialParameterDele.IsBound())
// 			{
// 				//가능한 네비가 잇더라도 볼륨 밖이면 스폰 불가이기 때문에 리턴값으로 한번 더 체크
// 				SpawnSimulationParam.SimulationDelegateHandles.MaterialParameterDele.Execute(SpawnSimulationParam.Actor, 0.0f);
// 			}
// 		}
// 		else
// 		{
// 			SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::InvalidLocation;
// 			SpawnSimulationParam.SimulationDelegateHandles.MaterialParameterDele.Execute(SpawnSimulationParam.Actor, 1.0f);
// 		}
// 	}
// 	else
// 	{
// 		SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::InvalidLocation;
// 		//이미 불가기 때문에 리턴값 체크 이유 없음
// 		if (SpawnSimulationParam.SimulationDelegateHandles.MaterialParameterDele.IsBound())
// 		{
// 			SpawnSimulationParam.SimulationDelegateHandles.MaterialParameterDele.Execute(SpawnSimulationParam.Actor, 1.0f);
// 		}
// 	}
}
