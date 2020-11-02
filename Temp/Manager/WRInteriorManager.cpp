
#include "WRInteriorManager.h"
#include "WRCharacterManager.h"
#include "WRSpawnSimulationManager.h"
#include "WRAsyncLoadUObjectManager.h"

#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterIO.h"

#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Define/WRPathDefine.h"

#include "Network/IMMessageSender.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"

#include "Utility/WRActorUtility.h"
#include "WRNoticeManager.h"

const int32 WRInteriorManager::SpawnInteriorAreaLength(2000);

WRInteriorManager::WRInteriorManager()
{

}

WRInteriorManager::~WRInteriorManager()
{

}

void WRInteriorManager::OnInitialize()
{
	InteriorItemSpawnedByMe.Reset();
}

void WRInteriorManager::OnShutdown()
{
	RemoveSimulationInterior();
}

void WRInteriorManager::OnLoadLevelStart()
{

}

void WRInteriorManager::OnLoadLevelComplete()
{

}

void WRInteriorManager::OnRenderStart()
{

}

void WRInteriorManager::SpawnInteriorToSimulate(ISN_t InISN, WRTableID InTID)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	if (INVALID_ISN == InISN)
	{
		InISN = WRCharacterManager::Get()->GenerateStandaloneAUID();
	}
	FString BpPath = "/Game/Asset/Blueprint/Character/IO/WR_IO_Chair_01.WR_IO_Chair_01";

	FWRItemTableBase* IOBaseTable = WRTableManager::Get()->FindRowInItemTable(InTID);
	FWRInteriorItemTable* InteriorTable = StaticCast<FWRInteriorItemTable*>(IOBaseTable);
	if (InteriorTable)
	{
		FWRInteractionObjectTable* IOTable = WRTableManager::Get()->FindRow<FWRInteractionObjectTable>(InteriorTable->ObjectTID);
		if (IOTable)
		{
			BpPath = IOTable->MainBPPath;
		}
	}

	//IsValidSpawnLocation 함수에서 체크할 영역을 미리 만들어 둔다
	MakeSpawnArea();

	FSimulationDelegateHandles AsyncDeleHandles;
	AsyncDeleHandles.SpawnCompletedDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRInteriorManager::OnSpawnInteriorCompletedWhenSimulating);
	AsyncDeleHandles.OKDele = FWROKDelegate::CreateRaw(this, &WRInteriorManager::RequestSpawnInterior, InISN, InTID);
	AsyncDeleHandles.CancelDele = FWRCancelDelegate::CreateRaw(this, &WRInteriorManager::RemoveSimulationInterior);
	AsyncDeleHandles.ValidLocationDele = FWRValidLocationDelegate::CreateRaw(this, &WRInteriorManager::IsValidSpawnLocation);
	AsyncDeleHandles.MessageDele = FWRMessageDelegate::CreateRaw(this, &WRInteriorManager::Message);
	WRSpawnSimulationManager::Get()->AsyncSpawnSimulationActor(BpPath, pMe->GetActorForwardVector(), pMe->GetActorRightVector(), AsyncDeleHandles);
}

bool WRInteriorManager::InputEvent(EWRInput InInputType, EInputEvent InEventType)
{
	return WRSpawnSimulationManager::Get()->InputEvent(InInputType, InEventType);
}

bool WRInteriorManager::IsSpawnedByMe(ISN_t InISN)
{
	return InteriorItemSpawnedByMe.Contains(InISN);
}

void WRInteriorManager::OnSpawnInteriorCompletedWhenSimulating(UObject* InActor, FString InHashKey)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	AWRCharacterIO* IO = WRSpawnSimulationManager::Get()->GetSimulationActor<AWRCharacterIO>();
	if (IO)
	{
		FVector SpawnLocation = pMe->GetActorForwardVector() * 300.0f + pMe->GetActorLocation();
		SpawnLocation = WRActorUtility::ProjectPointToNavigation(SpawnLocation);
		WRActorUtility::ModifyZHeightForClient(IO, SpawnLocation, SpawnLocation.Z);
		FRotator SpawnRotation = FRotator(0.0f, pMe->GetActorRotation().Yaw - 45.0f, 0.0f);
		IO->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false);

		//시뮬중에 호출받을 오버랩 이벤트 등록
		IO->GetStaticMesh()->SetGenerateOverlapEvents(true);
		IO->GetStaticMesh()->OnComponentBeginOverlap.AddDynamic(IO, &AWRCharacterIO::OnBeginOverlapIOForSimulating);
		IO->GetStaticMesh()->OnComponentEndOverlap.AddDynamic(IO, &AWRCharacterIO::OnEndOverlapIOForSimulating);

		//컬리전 프리셋 변경
		IO->GetStaticMesh()->SetCollisionProfileName(FName("OverlapAll"));
		IO->GetStaticMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		IO->OnPostSpawn();

		UMaterialInterface* MaterialInterface = WRActorSpawnUtility::FindOrLoadObject<UMaterialInterface>(MATERIAL_PATH_4_CONSTRUCTIBLE);
		if (MaterialInterface == nullptr)
		{
			return;
		}

		UWRActorComponentManageMaterial* ActorComponentManageMaterial = IO->GetComponent<UWRActorComponentManageMaterial>();
		if (ActorComponentManageMaterial == nullptr)
		{
			return;
		}

		UMeshComponent* MeshComponent = IO->GetCurrentMesh();
		if (MeshComponent == nullptr)
		{
			return;
		}

		for (int32 i = 0; i < MeshComponent->GetNumMaterials(); ++i)
		{
			ActorComponentManageMaterial->SetMaterial(i, MaterialInterface);
		}
	}
	else
	{
		FString MSG = FString::Format(TEXT("OnSpawnInteriorCompletedWhenSimulating : {0}"), { *InHashKey });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("UObject is nullptr"));
		return;
	}
}

void WRInteriorManager::RequestSpawnInterior(ISN_t InISN, WRTableID InTID)
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe == nullptr)
		return;

	FVector Location = WRSpawnSimulationManager::Get()->GetSimulationActorLocation();
	FRotator Rotator = WRSpawnSimulationManager::Get()->GetSimulationActorRotation();
	IMMessageSender::RequestArrangeInteriorItem(InISN, Location, Rotator.Yaw);
}

void WRInteriorManager::RequestRemoveInteriorItem(ISN_t InISN)
{
	IMMessageSender::RequestRemoveInteriorItem(InISN);
}

void WRInteriorManager::RemoveSimulationInterior()
{
	AWRCharacterIO* IO = WRSpawnSimulationManager::Get()->GetSimulationActor<AWRCharacterIO>();
	if (IO)
	{
		IO->RemoveFromRoot();
		IO->OnPreDestroy();

#if UE_EDITOR
		WRActorUtility::IsRooted(IO);
#else // UE_EDITOR
#endif // UE_EDITOR

		IO->Destroy();
	}
}

void WRInteriorManager::Message(EWRSimulationResultType InType)
{
	WRTableID TID = INVALID_TID;
	switch (InType)
	{
	case EWRSimulationResultType::InvalidLocation:
	case EWRSimulationResultType::NotVolumeArea:
	{
		TID = 26;
	}
	break;
	default:
		break;
	}

	WRNoticeManager::Get()->AddNotice(TID);
}

void WRInteriorManager::AckArrangeInteriorItem(const pt_Game_User_Inventory::Ack_ArrangeInteriorItem_G2U& InAck)
{
	InteriorItemSpawnedByMe.Add(InAck.isn);
}

void WRInteriorManager::AckRemoveInteriorItem(const pt_Game_User_Inventory::Ack_RemoveInteriorItem_G2U& InAck)
{
	InteriorItemSpawnedByMe.Remove(InAck.isn);
}

void WRInteriorManager::OnBeginOverlapWhenNotSpawned(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void WRInteriorManager::OnEndOverlapWhenNotSpawned(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

bool WRInteriorManager::SetIOMaterialParameter_deprecated(AActor* InActor, float InValue)
{
	AWRCharacterIO* IO = Cast<AWRCharacterIO>(InActor);
	if (IO)
	{
		IO->SetMaterialParameterValue(TEXT("Selected"), InValue, IO->GetCurrentMesh());
	}

	return true;
}

void WRInteriorManager::MakeSpawnArea()
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	//make box
	BoxPoint[0] = FVector(0.0f, -SpawnInteriorAreaLength / 2, 0.0f);
	BoxPoint[1] = FVector(SpawnInteriorAreaLength, -SpawnInteriorAreaLength / 2, 0.0f);
	BoxPoint[2] = FVector(SpawnInteriorAreaLength, SpawnInteriorAreaLength / 2, 0.0f);
	BoxPoint[3] = FVector(0.0f, SpawnInteriorAreaLength / 2, 0.0f);

	float Yaw = 0.0f;
	float Pitch = 0.0f;
	UKismetMathLibrary::GetYawPitchFromVector(pMe->GetActorForwardVector(), Yaw, Pitch);
	FVector InterpLocation = pMe->GetActorLocation() + pMe->GetActorForwardVector();
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

EWRSimulationResultType WRInteriorManager::IsValidSpawnLocation(AActor* InActor)
{
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

	return EWRSimulationResultType::Success;
}
