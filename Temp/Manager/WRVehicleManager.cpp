
#include "WRVehicleManager.h"
#include "WRSFXManager.h"
#include "WRUIManager.h"
#include "WRCharacterManager.h"
#include "WRConsoleManager.h"

#include "Network/IMMessageSender.h"

#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Vehicle/WRVehicle.h"

#include "Utility/WRActorSpawnUtility.h"
#include "Utility/WRActorUtility.h"

#include "Camera/CameraComponent.h"

#include "WheeledVehicleMovementComponent.h"

#include "UnrealString.h"

#include "UI/Panel/WRUIPanel_BoardingRequest.h"

#include "Component/Vehicle/WRVehicleComponentManageMaterial.h"

#include "Define/WRPathDefine.h"
#include "WRSpawnSimulationManager.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRVehicleTable.h"
#include "Actor/Vehicle/WRVehicleWheelFront.h"
#include "Actor/Vehicle/WRVehicleMovementComponent4W.h"
#include "Table/WRCustomTextTable.h"
#include "WRNoticeManager.h"
#include "Utility/WRUIUtility.h"
#include "WRAutoPilotManager.h"
#include "WRInputSystemManager.h"
#include "Utility/WRHMDUtility.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


DEFINE_LOG_CATEGORY(VehicleManager)


const int32 WRVehicleManager::SpawnVehicleAreaLength(2000);
WRVehicleManager::WRVehicleManager()
{

}

WRVehicleManager::~WRVehicleManager()
{

}

void WRVehicleManager::OnInitialize()
{
	for (auto Iter = AsyncSpawnVehicleKey.CreateIterator(); Iter; ++Iter)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(Iter.Key());
	}

	VehicleContainer.Empty();
	AsyncSpawnVehicleKey.Empty();
	VehicleSpawnInfo.Empty();

	SeatName2Boarding = "";
	VehicleAID2Boarding = INVALID_AID;
}

void WRVehicleManager::OnShutdown()
{
	for (auto& AsyncSpawnVehicle : AsyncSpawnVehicleKey)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncSpawnVehicle.Key);
	}
	AsyncSpawnVehicleKey.Empty();

	for (auto& Vehicle : VehicleContainer)
	{
		Remove_Internal(Vehicle.Value);
	}
	VehicleContainer.Empty();

	AWRVehicle* Vehicle = WRSpawnSimulationManager::Get()->GetSimulationActor<AWRVehicle>();
	if (Vehicle)
	{
		Vehicle->OnPreDestroy();
	}
}

void WRVehicleManager::OnLoadLevelStart()
{
	for (auto& AsyncSpawnVehicle : AsyncSpawnVehicleKey)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncSpawnVehicle.Key);
	}
	AsyncSpawnVehicleKey.Empty();

	for (auto& Vehicle : VehicleContainer)
	{
		Remove_Internal(Vehicle.Value);
	}
	VehicleContainer.Empty();
}

void WRVehicleManager::OnLoadLevelComplete()
{

}

void WRVehicleManager::OnRenderStart()
{
	OnInitialize();
}

void WRVehicleManager::EndLoadLevelProcess()
{

}

bool WRVehicleManager::OnTick(float InDeltaTime)
{
#ifdef UE_EDITOR
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return true;

	if (WRSpawnSimulationManager::Get()->IsValidSimulation() &&
		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == false &&
		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		for (int32 Index = 0; Index < 4; ++Index)
		{
			UKismetSystemLibrary::DrawDebugSphere(pMe->GetWorld(), BoxPoint[Index], 50.0f, 12, FLinearColor::Blue);

			int32 NextIndex = (Index + 1) % 4;
			UKismetSystemLibrary::DrawDebugLine(pMe->GetWorld(), BoxPoint[Index], BoxPoint[NextIndex], FLinearColor::Red, 0.0f, 2.0f);
		}
	}
#endif // DEF_YUL_TEST

	return true;
}

void WRVehicleManager::RequestSpawnVehicle(ISN_t InISN, WRTableID InTID)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe == nullptr)
		return;

	//�̹� ��ȯ�� ������ ������� �ش� ������ �����Ѵ�
	if (pMe->GetOwnedVehicle())
	{
		RequestUnspawnVehicle(VehicleItemID);
	}

	FVector Location = WRSpawnSimulationManager::Get()->GetSimulationActorLocation();
	FRotator Rotator = WRSpawnSimulationManager::Get()->GetSimulationActorRotation();
	IMMessageSender::RequestSpawnVehicle(pMe, Location, Rotator.Yaw, InISN, InTID);
}

void WRVehicleManager::RequestUnspawnVehicle(ISN_t InISN)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe == nullptr)
		return;

	IMMessageSender::RequestUnspawnVehicle(pMe->GetOwnedVehicleAID(), InISN);
}

bool WRVehicleManager::RequestGetOnVehicle()
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe == nullptr)
		return false;

	if (GetSeatName2Boarding() == "")
		return false;

	//��Ŷ ó�� �κ�
	FString LeftS;
	FString RightS;
	SeatName2Boarding.Split("_", &LeftS, &RightS);
	int32 SeatNum = FCString::Atoi(*RightS);

	if (0 == SeatNum)	//�������϶�
	{
#ifdef DEF_NOT_CHECK_VEHICLE_OWNER_WHEN_GETON_4_BUILD_TEMPORARY
		IMMessageSender::RequestGetOnVehicle(pMe, SeatNum, VehicleAID2Boarding);
		SeatName2Boarding.Empty();
#else // DEF_NOT_CHECK_VEHICLE_OWNER_WHEN_GETON_4_BUILD_TEMPORARY
		if (pMe->GetOwnedVehicleAID() != VehicleAID2Boarding)	//�� ������ �ƴϸ�
		{
			//�� ���� ������ �ƴѰ�� �������� Ż �� ����
			return false;
		}
		else //�� �����϶��� �׳� ž��
		{
			IMMessageSender::RequestGetOnVehicle(pMe, SeatNum, VehicleAID2Boarding);
			SeatName2Boarding.Empty();
		}
#endif // DEF_NOT_CHECK_VEHICLE_OWNER_WHEN_GETON_4_BUILD_TEMPORARY
	}
	else //�������� �ƴҶ�
	{
		if (pMe->GetOwnedVehicleAID() != VehicleAID2Boarding) //�� ������ �ƴϸ� ž�� ��� ��û
		{
			IMMessageSender::RequestAgreeToBoard(pMe->GetActorID(), SeatNum, VehicleAID2Boarding);

			//UI
			FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
			{
				if (InCombinedPanelData.Is3DWidget())
				{
					InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
					UWRUIPanel_BoardingRequest* Panel = Cast<UWRUIPanel_BoardingRequest>(InCombinedPanelData.UIPanel);
					if (Panel)
					{
						FWROnFinishedBoardingRequestTime BoardingTimeDelegate;
						BoardingTimeDelegate.BindRaw(this, &WRVehicleManager::OnRequestLetMeBoardingTimeOver);
						Panel->SetBoardingType(EWRBoardingRequestType::Passenger, BoardingTimeDelegate, pMe->GetActorID(), VehicleAID2Boarding, (int8)SeatNum);
					}
				}
			});

			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::BoardingRequest, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

			//������ OnRpt_AgreeToBoard_U2G �̰ɷ� �޴´�
		}
		else //�� �����϶��� �׳� ž��
		{
			IMMessageSender::RequestGetOnVehicle(pMe, SeatNum, VehicleAID2Boarding);
			SeatName2Boarding.Empty();
		}
	}

	return true;
}

bool WRVehicleManager::RequestGetOffVehicle()
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe == nullptr)
		return false;

	IMMessageSender::RequestGetOffVehicle(pMe, pMe->GetBoardingVehicleAID());

	return true;
}

int32 WRVehicleManager::GetSeatIndex(AID_t InVehicleAID, AID_t InPassengerAID)
{
	AWRVehicle* pVehicle = VehicleContainer.FindRef(InVehicleAID);
	if (nullptr == pVehicle)
		return INVALID_SEAT_INDEX;

	return pVehicle->GetSeatIndex(InPassengerAID);
}

AWRVehicle* WRVehicleManager::FindVehicleByAID(AID_t InAID)
{
	return VehicleContainer.FindRef(InAID);
}

EWRVehicleSeatSide WRVehicleManager::GetSeatingSide(AID_t InPassengerAID)
{
	AWRCharacter* pPassenger = WRCharacterManager::Get()->FindbyAID(InPassengerAID);
	if (nullptr == pPassenger)
		return EWRVehicleSeatSide::None;

	int32 SeatIndex = GetSeatIndex(pPassenger->GetBoardingVehicleAID(), InPassengerAID);
	if (INVALID_SEAT_INDEX == SeatIndex)
		return EWRVehicleSeatSide::None;

	int32 remainder = SeatIndex % 2;
	if (remainder == 0)
	{
		return EWRVehicleSeatSide::LEFT_SIDE;
	}
	else if (remainder == 1)
	{
		return EWRVehicleSeatSide::RIGHT_SIDE;
	}

	return EWRVehicleSeatSide::None;
}

bool WRVehicleManager::IsDriver(AID_t InActorID, AID_t InVehicleAID)
{
	if (GetSeatIndex(InVehicleAID, InActorID) == 0)
		return true;

	return false;
}

void WRVehicleManager::GetAllVehicle(TArray<AActor*>& InoutActor)
{
	for (auto iter = VehicleContainer.CreateIterator(); iter; ++iter)
	{
		InoutActor.Add(iter.Value());
	}
}

void WRVehicleManager::SetMyVehicleInfoToBoarding(const FString& InName, AID_t InAID)
{
	SeatName2Boarding = InName;
	VehicleAID2Boarding = InAID;
}

void WRVehicleManager::SpawnVehicleFromCheat()
{
	SpawnVehiceToSimulate(0, 0);
}

bool WRVehicleManager::IsValidSapwnedVehicleISN(const ISN_t IN InCompareISN)
{
	if (VehicleItemID == INVALID_ISN)
		return false;

	if (VehicleItemID == InCompareISN)
		return true;

	return false;
}

bool WRVehicleManager::IsSpawnedVehicle()
{
	if (VehicleItemID == INVALID_ISN)
		return false;

	return true;
}

void WRVehicleManager::OnInformSpawnVehicle(const pt_Game_Actor::Inf_VehicleAppear_G2U& InPacket)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

#ifdef DEF_YUL_TEST
#else // DEF_YUL_TEST
	if (pMe->GetActorID() == InPacket.masterAID)	//�� ���ϰ�� �̹� �ùķ��̼� ������� �����س��ұ� ������ ���ø� �ϸ� �ȴ�.
	{
		AWRVehicle* Vehicle = WRSpawnSimulationManager::Get()->GetSimulationActor<AWRVehicle>();
		if (Vehicle)
		{
			OnSetVehicleState(Vehicle, InPacket);
		}
		else
		{
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OnInformSpawnVehicle"), TEXT("Simulation actor castin failed"));
		}

 		Vehicle->GetMesh()->OnComponentBeginOverlap.RemoveDynamic(Vehicle, &AWRVehicle::OnBeginOverlapVehicleForSimulating);
 		Vehicle->GetMesh()->OnComponentEndOverlap.RemoveDynamic(Vehicle, &AWRVehicle::OnEndOverlapVehicleForSimulating);

		WRSpawnSimulationManager::Get()->ResetSimulationParam();
	}
	else
#endif // DEF_YUL_TEST
	{
		// Async ��ȯ�Ҷ� ��ǥ�� zeroVector�� ���� ������ �־ �ذ��Ҷ� ���� �Ʒ��� ��� ����
		if (VehicleSpawnInfo.Contains(InPacket.vehicleAID))
			return;

		if (FindVehicleByAID(InPacket.vehicleAID))
			return;

		const FString* AsyncSpawnKey = AsyncSpawnVehicleKey.FindKey(InPacket.vehicleAID);
		if (AsyncSpawnKey)
		{
			WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*AsyncSpawnKey);
		}

		VehicleSpawnInfo.Add(InPacket.vehicleAID, InPacket);

		FString BpPath = "/Game/Asset/BluePrint/Vehicle/BP_Vertebra.BP_Vertebra";
		FWRVehicleTable* VehicleTable = WRTableManager::Get()->FindRow<FWRVehicleTable>(InPacket.vehicleTID);
		if (VehicleTable)
		{
			BpPath = VehicleTable->BlueprinTid;
		}

		FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRVehicleManager::OnSpawnVehicleCompleted);
		FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(BpPath, AsyncLoadDele);
		if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
			return;

		AsyncSpawnVehicleKey.Add(AsyncKey, InPacket.vehicleAID);
	}
}

void WRVehicleManager::OnAckSpawnVehicle(const pt_Game_Vehicle::Ack_SpawnVehicle_G2U& InAck)
{
	VehicleItemID = InAck.itemSN;
}

void WRVehicleManager::OnAckUnspawnVehicle(const pt_Game_Vehicle::Ack_UnspawnVehicle_G2U& InAck)
{
	VehicleItemID = INVALID_ISN;
}

void WRVehicleManager::SpawnVehiceToSimulate(ISN_t InISN, WRTableID InTID)
{
	// Async ��ȯ�Ҷ� ��ǥ�� zeroVector�� ���� ������ �־ �ذ��Ҷ� ���� �Ʒ��� ��� ����
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	FString BpPath = "/Game/Asset/BluePrint/Vehicle/BP_Vertebra.BP_Vertebra";;
	FWRVehicleTable* VehicleTable = WRTableManager::Get()->FindRow<FWRVehicleTable>(InTID);
	if (VehicleTable)
	{
		BpPath = VehicleTable->BlueprinTid;
	}

	//IsValidSpawnLocation �Լ����� üũ�� ������ �̸� ����� �д�
	MakeSpawnArea();

	FSimulationDelegateHandles AsyncDeleHandles;
	AsyncDeleHandles.SpawnCompletedDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRVehicleManager::OnSpawnVehicleCompletedWhenSimulating);
	AsyncDeleHandles.OKDele = FWROKDelegate::CreateRaw(this, &WRVehicleManager::RequestSpawnVehicle, InISN, InTID);
	AsyncDeleHandles.CancelDele = FWRCancelDelegate::CreateRaw(this, &WRVehicleManager::RemoveSimulationVehicle);
	AsyncDeleHandles.MessageDele = FWRMessageDelegate::CreateRaw(this, &WRVehicleManager::Message);
	AsyncDeleHandles.ValidLocationDele = FWRValidLocationDelegate::CreateRaw(this, &WRVehicleManager::IsValidSpawnLocation);
	WRSpawnSimulationManager::Get()->AsyncSpawnSimulationActor(BpPath, pMe->GetActorForwardVector(), pMe->GetActorRightVector(), AsyncDeleHandles);
}

void WRVehicleManager::OnSpawnVehicleCompleted(UObject* InActor, FString InHashKey)
{
	AWRVehicle* pVehicle = Cast<AWRVehicle>(InActor);
	if (nullptr == pVehicle)
	{
		FString MSG = FString::Format(TEXT("OnSpawnVehicleCompleted : {0}"), { *InHashKey });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("UObject is nullptr"));
		return;
	}

	AID_t ActorID = AsyncSpawnVehicleKey.FindRef(InHashKey);
	if (VehicleSpawnInfo.Contains(ActorID) == false)
	{
		FString MSG = FString::Format(TEXT("OnSpawnVehicleCompleted : {0}"), { *InHashKey });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Not found actorID"));
		return;
	}

	UE_LOG(VehicleManager, Display, TEXT("WRVehicleManager::OnSpawnVehicleCompleted - Remove Key : %s"), *InHashKey);

	pt_Game_Actor::Inf_VehicleAppear_G2U VehicleInfo = VehicleSpawnInfo.FindRef(ActorID);
	OnSetVehicleState(pVehicle, VehicleInfo);
}

void WRVehicleManager::OnSpawnVehicleCompletedWhenSimulating(UObject* InActor, FString InHashKey)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	AWRVehicle* Vehicle = WRSpawnSimulationManager::Get()->GetSimulationActor<AWRVehicle>();
	if (Vehicle)
	{
		//�ù��߿� ȣ����� ������ �̺�Ʈ ���
		Vehicle->GetMesh()->OnComponentBeginOverlap.AddDynamic(Vehicle, &AWRVehicle::OnBeginOverlapVehicleForSimulating);
		Vehicle->GetMesh()->OnComponentEndOverlap.AddDynamic(Vehicle, &AWRVehicle::OnEndOverlapVehicleForSimulating);

		//�ø��� ������ ����
		Vehicle->GetMesh()->SetCollisionProfileName(FName("OverlapAll"));
		Vehicle->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Vehicle->GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);

		//��ǥ
		FVector SpawnLocation = pMe->GetActorForwardVector() * 500.0f + pMe->GetActorLocation();
		SpawnLocation += pMe->GetActorRightVector() * 150.0f;
		WRActorUtility::ModifyZHeightForServer(pMe, SpawnLocation, SpawnLocation.Z);
		//ȸ��
		FRotator SpawnRotation = FRotator(0.0f, pMe->GetActorRotation().Yaw, 0.0f);
// 		if (WRHMDUtility::IsDisplayEnable() == true)
// 			SpawnRotation.Yaw = WRInputSystemManager::Get()->GetHMDRotation().Yaw;
		//��ġ �� ȸ���� ����
		Vehicle->SetLocationAndRotation(SpawnLocation, SpawnRotation, false);

		//���� ��ó��
		Vehicle->OnPostSpawn();

		UMaterialInterface* MaterialInterface = WRActorSpawnUtility::FindOrLoadObject<UMaterialInterface>(MATERIAL_PATH_4_CONSTRUCTIBLE);
		if (MaterialInterface)
		{
			UWRVehicleComponentManageMaterial* ActorComponentManageMaterial = Vehicle->GetComponent<UWRVehicleComponentManageMaterial>();
			if (ActorComponentManageMaterial)
			{
				USkeletalMeshComponent* SkeletalMeshComponent = Vehicle->GetMesh();
				if (SkeletalMeshComponent)
				{
					for (int32 i = 0; i < SkeletalMeshComponent->GetNumMaterials(); ++i)
					{
						ActorComponentManageMaterial->SetMaterial(i, MaterialInterface);
					}
				}
			}
		}
	}
	else
	{
		FString MSG = FString::Format(TEXT("OnSpawnVehicleCompletedWhenSimulating : {0}"), { *InHashKey });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("UObject is nullptr"));
		return;
	}
}

void WRVehicleManager::OnSetVehicleState(AWRVehicle* InVehicle, const pt_Game_Actor::Inf_VehicleAppear_G2U& InPacket)
{
	const FString* HashKey = AsyncSpawnVehicleKey.FindKey(InPacket.masterAID);
	if (HashKey)
	{
		AsyncSpawnVehicleKey.Remove(*HashKey);
	}

	//��ġ ȸ���� ����
	InVehicle->SetLocation(FVector(InPacket.posX, InPacket.posY, InPacket.posZ), true);
	InVehicle->SetRotation(FRotator(0.0f, InPacket.angle, 0.0f));

	//������ ����
// 	InVehicle->SetActorScale3D(FVector::OneVector);
	
	//���;��̵� ����
	if (INVALID_AID == InPacket.vehicleAID)
	{
		InVehicle->SetVehicleAndOwnerAID(WRCharacterManager::Get()->GenerateStandaloneAUID(), InPacket.masterAID);
	}
	else
	{
		InVehicle->SetVehicleAndOwnerAID(InPacket.vehicleAID, InPacket.masterAID);
	}

	//������ ����
	AWRCharacter* pOwnerCharacter = WRCharacterManager::Get()->FindbyAID(InPacket.masterAID);
	if (pOwnerCharacter)
	{
		pOwnerCharacter->SetOwnedVehicleAID(InPacket.vehicleAID);
	}
	else
	{
		WRCharacterManager::Get()->ModifyOwnedVehicleAID(InPacket.masterAID, InPacket.vehicleAID);
	}

	//ž���� ���� �ۼ�
	InVehicle->MakePassengerInfo(InPacket.vehicleTID, InPacket.vecCrew);

	//������ ž������ �ɸ��� ���� ����
	for (int32 Idx = 0; Idx < InPacket.vecCrew.size(); ++Idx)
	{
		WRCharacterManager::Get()->SpawnOnBoardingPlayerWhenVehicleSpawnCompleted(InPacket.vecCrew[Idx]);
	}

	//addtoroot
	if (InVehicle->IsRooted() == false)
	{
		InVehicle->AddToRoot();
	}
	   
	//�����̳� ����
	VehicleContainer.Add(InPacket.vehicleAID, InVehicle);

	//�������� ����
	VehicleSpawnInfo.Remove(InPacket.vehicleAID);

	//���� ��ó��
	InVehicle->OnPostSpawn();

	//���� ����Ʈ ����
	UWRVehicleComponentManageMaterial* VehicleComponentManageMaterial = InVehicle->GetComponent<UWRVehicleComponentManageMaterial>();
	if (VehicleComponentManageMaterial)
	{
		VehicleComponentManageMaterial->RestoreMaterialInterface();

		InVehicle->SetMaterialParameterValue("00.SpawnMain", 0.0F);

		float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::VehicleSpawnDirection);
		VehicleComponentManageMaterial->Spawn(EWRManageMaterial::Spawn, Duration);
	}

	FWRVehicleTable* VehicleTable = WRTableManager::Get()->FindRow<FWRVehicleTable>(InPacket.vehicleTID);
	if (VehicleTable == nullptr)
	{
		return;
	}

	WRVFXManager::Get()->Spawn(VehicleTable->SpawnEffectPath, InVehicle, nullptr);
	WRSFXManager::Get()->SpawnAtLocation(VehicleTable->SpawnSoundPath, InVehicle->GetActorLocation());
}

void WRVehicleManager::RemoveVehicle(AWRVehicle* InVehicle, bool IsDirecting /*= false*/)
{
	if (nullptr == InVehicle)
	{
		return;
	}

	if (IsDirecting)
	{
		InVehicle->RemoveVehicle();
	}
	else
	{
		Remove_Internal(InVehicle);
		VehicleContainer.Remove(InVehicle->GetVehicleAID());
	}
}

void WRVehicleManager::RemoveVehicle(AID_t InAID, bool IsDirecting /*= false*/)
{
	AWRVehicle* pVehicle = FindVehicleByAID(InAID);
	if (nullptr == pVehicle)
	{
		return;
	}

	if (IsDirecting)
	{
		pVehicle->RemoveVehicle();
	}
	else
	{
		Remove_Internal(pVehicle);
		VehicleContainer.Remove(InAID);
	}
}

void WRVehicleManager::RemoveSimulationVehicle()
{
	AWRVehicle* pVehicle = WRSpawnSimulationManager::Get()->GetSimulationActor<AWRVehicle>();
	RemoveVehicle(pVehicle);
}

bool WRVehicleManager::InputEvent(EWRInput InInputType, EInputEvent InEventType)
{
	bool bResult = false;
	if (EWRSpawnSimulationState::None != WRSpawnSimulationManager::Get()->GetSimulationState())
	{
		bResult = InputEventToSimulateSpawn(InInputType, InEventType);
	}
	else
	{
		bResult = InputEventToControllVehicle(InInputType, InEventType);
	}

	return bResult;
}

bool WRVehicleManager::InputEventToControllVehicle(EWRInput InInputType, EInputEvent InEventType)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return false;

	if (pMe->GetCurrentFSMType() == EWRFSM::EnterVehicle ||
		pMe->GetCurrentFSMType() == EWRFSM::ExitVehicle)
	{
		return true;
	}

	switch (pMe->GetCurrentFSMType())
	{
	case EWRFSM::EnterVehicle:
	case EWRFSM::ExitVehicle:
	case EWRFSM::VehicleInteraction:
	{
		//ž�����̰ų� ������ ���߿��� Ű �Է��� ���� ���ؼ� true�� ����
		return true;
	}
	break;
	default:
		break;
	}

	bool bInputPass = false;
	switch (InInputType)
	{
	case EWRInput::Triangle://ž�� or ����
	{
		if (InEventType == EInputEvent::IE_Pressed)
		{
			if (pMe->GetCurrentFSMType() == EWRFSM::DrivingIdle)	//����
			{
				AWRVehicle* pVehicle = pMe->GetOwnedVehicle();
				if (nullptr == pVehicle || nullptr == pVehicle->GetVehicleMovementComponent())
					return false;

				if (pMe->IsVehicleDriver() == false)
					return false;
				
				if (pVehicle->GetWRVehicleMovement()->GetSpeedByKM() != 0)
				{
					WRNoticeManager::Get()->AddNotice(27);
					return true;
				}

				return RequestGetOffVehicle();
			}
			else //ž��
			{
				return RequestGetOnVehicle();
			}
		}

		return false;
	}
	break;
	case EWRInput::Cross:
	{
		if (InEventType == EInputEvent::IE_Pressed)
		{
			//������ž�� ����
			UWRUIPanel_BoardingRequest* BoardingRequestPanel = WRUIManager::Get()->FindUI<UWRUIPanel_BoardingRequest>(EWRUIPanelKind::BoardingRequest);
			if (BoardingRequestPanel)
			{
				AID_t PassengerAID = BoardingRequestPanel->GetPassengerAID();
				AID_t VehicleAID = BoardingRequestPanel->GetVehicleAID();
				AID_t SeatIndex = BoardingRequestPanel->GetSeatIndex();

				if (INVALID_AID == PassengerAID || INVALID_AID == VehicleAID)
				{
					return true;
				}

				IMMessageSender::ReportAgreeToBoard(PassengerAID, SeatIndex, VehicleAID, false);

				WRUIManager::Get()->CloseUI(EWRUIPanelKind::BoardingRequest);
			}
			else
			{
				//���� ž������ ���� ����
				if (pMe->GetBoardingVehicle() == nullptr)
				{
					if (SeatName2Boarding.IsEmpty() == false)
					{
						RequestUnspawnVehicle(VehicleItemID);

						return true;
					}
				}
				else //���� ž���� ����
				{

				}
			}

			return false;
		}
	}
	break;
	default:
	{
		AWRVehicle* pVehicle = pMe->GetOwnedVehicle();
		if (nullptr == pVehicle || nullptr == pVehicle->GetVehicleMovementComponent())
			return false;

		if (pMe->IsVehicleDriver() == false)
			return false;

		if (pMe->GetCurrentFSMType() != EWRFSM::DrivingIdle)
			return true;

		switch (InInputType)
		{
		case EWRInput::Square:
		{
			//ũ����
			if (InEventType == EInputEvent::IE_Pressed)
			{
				IMMessageSender::RequestBeHorning(pVehicle);
			}
		}
		break;
		case EWRInput::Circle:
		{
			if (InEventType == EInputEvent::IE_Pressed)
			{
				//������ž�¼���
				UWRUIPanel_BoardingRequest* BoardingRequestPanel = WRUIManager::Get()->FindUI<UWRUIPanel_BoardingRequest>(EWRUIPanelKind::BoardingRequest);
				if (BoardingRequestPanel)
				{
					AID_t PassengerAID = BoardingRequestPanel->GetPassengerAID();
					AID_t VehicleAID = BoardingRequestPanel->GetVehicleAID();
					AID_t SeatIndex = BoardingRequestPanel->GetSeatIndex();
					if (INVALID_AID == PassengerAID || INVALID_AID == VehicleAID)
					{
						return true;
					}

					IMMessageSender::ReportAgreeToBoard(PassengerAID, SeatIndex, VehicleAID, true);
					WRUIManager::Get()->CloseUI(EWRUIPanelKind::BoardingRequest);
				}
				else
				{
					return false;
				}
			}
		}
		break;
		case EWRInput::DpadUP:
		{
			//������Ʈ on off
			stVehicleExteriorInfo ExInfo;
			IMMessageSender::RequestUpdateExterior(pVehicle, ExInfo);
		}
		break;
		case EWRInput::DpadLeft:
		{
			//����������
		}
		break;
		case EWRInput::DpadRight:
		{
			//����������
		}
		break;
		case EWRInput::L1:
		{
			//�극��ũ
			if (InEventType == EInputEvent::IE_Pressed)
			{
				pVehicle->RequestBeginBrake();
				WRAutoPilotManager::Get()->EndAutoPilot(pVehicle);
			}
			else if (InEventType == EInputEvent::IE_Released)
			{
				pVehicle->RequestEndBrake();
			}
		}
		break;
		case EWRInput::L2:
		{
			//����			
			if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			{
				pVehicle->RequestAccelator(-1.0f);

				WRAutoPilotManager::Get()->EndAutoPilot(pVehicle);
			}
			else if (InEventType == EInputEvent::IE_Released)
			{
				pVehicle->RequestAccelator(0.0f);
			}
		}
		break;
		case EWRInput::L3:
		{
			//ī�޶� ����Ī
			if (InEventType == EInputEvent::IE_Pressed)
			{
				//pVehicle->SwitchCamera();
			}
		}
		break;
		case EWRInput::R1:
		{
			//�� �Ѳ�����
			if (InEventType == EInputEvent::IE_Pressed)
			{
				pVehicle->RequestToggleRoof();
			}
		}
		break;
		case EWRInput::R2:
		{
			//����
			if (InEventType == EInputEvent::IE_Pressed || InEventType == EInputEvent::IE_Repeat)
			{
				pVehicle->RequestAccelator(1.0f);

				pVehicle->StartSmoke(1.0f);

				//PlayEngineSound();

				WRAutoPilotManager::Get()->EndAutoPilot(pVehicle);
			}
			else if (InEventType == EInputEvent::IE_Released)
			{
				pVehicle->RequestAccelator(0.0f);
			}
		}
		break;
		case EWRInput::R3:
		{
			//���� ���� on off	
			if (InEventType == EInputEvent::IE_Pressed)
			{
				if (WRAutoPilotManager::Get()->IsActiveAutoPilot())
				{
					WRAutoPilotManager::Get()->EndAutoPilot(pVehicle);
				}
				else
				{
					WRAutoPilotManager::Get()->BeginAutoPilot(pVehicle);
				}
			}
		}
		break;
		default:
		{

		}
		break;
		}

		return true;
	}
	break;
	}

	return false;
}

bool WRVehicleManager::InputEventToSimulateSpawn(EWRInput InInputType, EInputEvent InEventType)
{
	return WRSpawnSimulationManager::Get()->InputEvent(InInputType, InEventType);
}

void WRVehicleManager::OnRequestLetMeBoardingTimeOver(AID_t InAID, AID_t InVehicleAID, int8 InSeatIndex)
{

}

void WRVehicleManager::MakeSpawnArea()
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	//make box
	BoxPoint[0] = FVector(0.0f, -SpawnVehicleAreaLength / 2, 0.0f);
	BoxPoint[1] = FVector(SpawnVehicleAreaLength, -SpawnVehicleAreaLength / 2, 0.0f);
	BoxPoint[2] = FVector(SpawnVehicleAreaLength, SpawnVehicleAreaLength / 2, 0.0f);
	BoxPoint[3] = FVector(0.0f, SpawnVehicleAreaLength / 2, 0.0f);

	float Yaw = 0.0f;
	float Pitch = 0.0f;
	UKismetMathLibrary::GetYawPitchFromVector(pMe->GetActorForwardVector(), Yaw, Pitch);
	FVector InterpLocation = pMe->GetActorLocation() + (pMe->GetActorForwardVector() * 200.0f);
	for (int32 Index = 0; Index < 4; ++Index)
	{
		BoxPoint[Index] = UKismetMathLibrary::RotateAngleAxis(BoxPoint[Index], Yaw, FVector::UpVector) + InterpLocation;
	}

	//make EdgeLines
	for (int32 Index = 0; Index < 4; ++Index)
	{
		int32 NextIndex = (Index + 1) % 4;

		EdgeLines[Index] = BoxPoint[NextIndex] - BoxPoint[Index];
	}
}

EWRSimulationResultType WRVehicleManager::IsValidSpawnLocation(AActor* InActor)
{
	AWRVehicle* pVehicle = Cast<AWRVehicle>(InActor);
	if (nullptr == pVehicle)
		return EWRSimulationResultType::None;

	FVector ActorLocation = InActor->GetActorLocation();
	for (int32 index = 0; index < 4; ++index)
	{
		FVector Dir = ActorLocation - BoxPoint[index];
		float dot = UKismetMathLibrary::Dot_VectorVector(EdgeLines[index], Dir);
		if (dot < 0.0f)
		{
			return EWRSimulationResultType::NotSpawnArea;
		}
	}

	if (pVehicle->GetVehicleInSpawnVolume() == false)
	{
		return EWRSimulationResultType::NotVolumeArea;
	}

	return EWRSimulationResultType::Success;
}

void WRVehicleManager::Message(EWRSimulationResultType InType)
{
	WRTableID TID = INVALID_TID;
	switch (InType)
	{
	case EWRSimulationResultType::NotVolumeArea:
	{
		TID = 34;
	}
	break;
	case EWRSimulationResultType::NotSpawnArea:
	case EWRSimulationResultType::InvalidLocation:
	{
		TID = 26;
	}
	break;
	default:
		break;
	}

	WRNoticeManager::Get()->AddNotice(TID);
}

void WRVehicleManager::Remove_Internal(AWRVehicle* InVehicle)
{
	InVehicle->RemoveFromRoot();
	InVehicle->OnPreDestroy();

#if UE_EDITOR
	WRActorUtility::IsRooted(InVehicle);
#else // UE_EDITOR
#endif // UE_EDITOR
	
	InVehicle->Destroy();
}

bool WRVehicleManager::SetMaterialParameter_deprecated(AActor* InActor, float InValue)
{
	bool bResult = true;
	AWRVehicle* Vehicle = Cast<AWRVehicle>(InActor);
	if (Vehicle)
	{
		Vehicle->SetMaterialParameterValue(TEXT("Selected"), InValue);
	}

	if (1.0f == InValue)
	{
		WRNoticeManager::Get()->AddNotice(34);
	}

	return bResult;
}
