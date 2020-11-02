
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
	//������ ���õ� ������ ������ ������ �Ѵ�
	InitializeSimulationParam();
		
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe)
	{	
		//�ε� �̹���
		FWRWidgetInfo LoadingInfo(EWRUIPanelKind::SpawnLoading, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCameraAll, nullptr, pMe);
		strSimulationUIAsyncKey = WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(LoadingInfo);
	}

	//�ε� ����
	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRSpawnSimulationManager::OnSpawnCompleted);
	SpawnSimulationParam.HashKeyToSimulate = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(InBpPath, AsyncLoadDele);
	if (SpawnSimulationParam.HashKeyToSimulate == WRASYNCLOADMANAGER_INVALID)
	{
		//������ ���� �����Ƿ� async �ɾ�� ui�� ��� �����ش�		
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

	//��ҿ� �Լ�
	if (SpawnSimulationParam.SimulationDelegateHandles.CancelDele.IsBound() == false)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("AsyncSpawnSimulationActor"), TEXT("CancelDele must be bound"));
	}
}

void WRSpawnSimulationManager::OnSpawnCompleted(UObject* InActor, FString InHashKey)
{
	//���� �ٲٰ�
	SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::ReadyToSimulation;

	//������ ���� ����
	SpawnSimulationParam.Actor = Cast<AActor>(InActor);

	//���� �Ϸ� �Լ� ȣ��
	SpawnSimulationParam.SimulationDelegateHandles.SpawnCompletedDele.ExecuteIfBound(InActor, InHashKey);

	//�ε� �̹��� ����
	CloseUI(EWRUIPanelKind::SpawnLoading);
		
	//���� ���� �������� üũ
	bool bOpenGuideUI = true;
	if (SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.IsBound())
	{
		EWRSimulationResultType Result = SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.Execute(SpawnSimulationParam.Actor);
		if (EWRSimulationResultType::Success == Result)
		{
			//����� �� ��ġ�� �����Ǿ������� �ƹ��͵� ����
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
		//���� �Ϸ� �Լ� ȣ��
		SpawnSimulationParam.SimulationDelegateHandles.SpawnCompletedDele.ExecuteIfBound(InActor, InHashKey);
	}
	
	//���̵� ui open
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
				// ������ ��û
				SpawnSimulationParam.SimulationDelegateHandles.OKDele.ExecuteIfBound();

				// ������ ������û �����Ƿ� ����
				// ��Ҵ� ���͸� ������Ű�� ������ �⺻�̹Ƿ� ���⼭ �ٷ� ����				
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
		//��ȸ��
		/*if (InEventType == EInputEvent::IE_Pressed ||
			InEventType == EInputEvent::IE_Repeat)
		{
			RotateActor(-2.0f);
		}*/
	}
	break;
	case EWRInput::R2:
	{
		//��ȸ��
		/*if (InEventType == EInputEvent::IE_Pressed ||
			InEventType == EInputEvent::IE_Repeat)
		{
			RotateActor(2.0f);
		}*/
	}
	break;
	case EWRInput::DpadUP:
	{
		//������ �̵�
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToForward(10.0f);*/
	}
	break;
	case EWRInput::DpadDown:
	{
		//�ڷ� �̵�
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToForward(-10.0f);*/
	}
	break;
	case EWRInput::DpadLeft:
	{
		//�·� �̵�
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToRight(-10.0f);*/
	}
	break;
	case EWRInput::DpadRight:
	{
		//��� �̵�
		/*if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			MoveActorToRight(10.0f);*/
	}
	break;
	case EWRInput::LStickX:
	{
		//������ �̵�
		if (InEventType == EInputEvent::IE_Repeat)
			MoveActorToForward(InScale * 10.0f);
	}
	break;
	case EWRInput::LStickY:
	{
		//��� �̵�
		if (InEventType == EInputEvent::IE_Repeat)
			MoveActorToRight(InScale * 10.0f);
	}
	break;
	case EWRInput::RStickX:
	{
		//�Ƹ� ���߿� ������������ ���� ������...
	}
	break;
	case EWRInput::RStickY:
	{
		//ȸ��
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
	if (SpawnSimulationParam.HashKeyToSimulate != "" && SpawnSimulationParam.Actor == nullptr) //���� ������ �ȵȻ��¸�
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
	if (SpawnSimulationParam.HashKeyToSimulate != "" && SpawnSimulationParam.Actor == nullptr) //���� ������ �ȵȻ��¸�
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*SpawnSimulationParam.HashKeyToSimulate);
	}

	SpawnSimulationParam.Reset();

	//���̵� ui off
	CloseUI(EWRUIPanelKind::SpawnSimulationGuide);
}

void WRSpawnSimulationManager::ExecuteCancelDele()
{
	SpawnSimulationParam.SimulationDelegateHandles.CancelDele.ExecuteIfBound();

	// ���� �ʱ�ȭ
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

	//�̵���ų ����ǥ
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

	//�̵���ų ����ǥ
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

	//�̵���Ų�Ŀ� ���� ���� ��ġ �������� z�� ���ϱ�
	FVector CurrentFloorLocation = WRActorUtility::ProjectPointToNavigation(SpawnSimulationParam.Actor->GetActorLocation());
	if (WRActorUtility::IsOnNavigation(pMe->GetWorld(), CurrentFloorLocation) == false)
	{
		CurrentFloorLocation = SpawnSimulationParam.Actor->GetActorLocation();
	}

	//����ǥ�� �ٴ���ǥ
	float HeightGap = SpawnSimulationParam.Actor->GetActorLocation().Z - CurrentFloorLocation.Z;
	FVector NextFloorLocation = WRActorUtility::ProjectPointToNavigation(InLocation);
	if (WRActorUtility::IsOnNavigation(pMe->GetWorld(), InLocation))
	{
		//�̵��� ��ǥ�� ���̰� : �̵��� ��ġ�� �ٴ���ǥ + ���� ��ǥ������ �ٴڰ� ������ ��������
		InLocation.Z = NextFloorLocation.Z + HeightGap;
	}
	else
	{
		InLocation.Z = CurrentFloorLocation.Z;
	}

	//�ش� ��ǥ�� invalid�ϸ� �ǵ��� prelocation ��ǥ�� �̸� �޾Ƶд�
	FVector PreLocation = SpawnSimulationParam.Actor->GetActorLocation();
	//�̵���ų ��ǥ�� �����ϰ� ��ȿ�ѻ������� üũ
	SpawnSimulationParam.Actor->SetActorLocation(InLocation);

	if (SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.IsBound())
	{
		EWRSimulationResultType Result = SpawnSimulationParam.SimulationDelegateHandles.ValidLocationDele.Execute(SpawnSimulationParam.Actor);
		if (EWRSimulationResultType::Success == Result)
		{
		}
		else
		{
			//��ȿ���� ���� ��ǥ�� ���� ��ǥ�� �ǵ�����
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
// 				//������ �׺� �մ��� ���� ���̸� ���� �Ұ��̱� ������ ���ϰ����� �ѹ� �� üũ
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
// 		//�̹� �Ұ��� ������ ���ϰ� üũ ���� ����
// 		if (SpawnSimulationParam.SimulationDelegateHandles.MaterialParameterDele.IsBound())
// 		{
// 			SpawnSimulationParam.SimulationDelegateHandles.MaterialParameterDele.Execute(SpawnSimulationParam.Actor, 1.0f);
// 		}
// 	}
}
