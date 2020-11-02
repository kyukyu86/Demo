#include "WREnvInteractionManager.h"
#include "WRMaterialParameterCollectionManager.h"
#include "WRSFXManager.h"
#include "WRCharacterManager.h"
#include "WRAsyncLoadUObjectManager.h"
#include "WRInputSystemManager.h"

#include <EngineUtils.h>
#include "Engine/EngineTypes.h"

#include "Actor/EnvActor/StaticMesh/WRSpiderWeb.h"
#include "Actor/EnvActor/WREnvVehicleBase.h"
#include "Actor/EnvActor/FloatingBoard/WRFloatingBoard.h"
#include "Actor/EnvActor/Elevator/WRElevator.h"
#include "Actor/EnvActor/Door/WREnvSlideDoor.h"
#include "Actor/EnvActor/Tardis/WRTardis.h"

#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Kismet/GameplayStatics.h"

#include "Foliage/Public/InstancedFoliageActor.h"

#include "Core/WRGameInstance.h"

#include "Utility/WRActorSpawnUtility.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRFoliageTable.h"
#include "Table/WRTransportTable.h"

#include "Component/InstancedFoliage/WRFoliageInstancedStaticMeshComponent.h"
#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Network/IMMessageSender.h"
#include "Actor/EnvActor/MovingWalkway/WREnvMovingWalkway.h"
#include "Component/SplineMesh/WRMovingwalkComponent.h"
#include "Table/WREnvObjectTable.h"
#include "Actor/EnvActor/SecretDialog/WRSecretDialogArea.h"
#include "Actor/EnvActor/SecretDialog/WRSecretDialogPortal.h"
#include "Actor/EnvActor/SecretDialog/WRSecretDialogDome.h"
#include "WRVoiceContentsManager.h"
#include "WRDialogueManager.h"





// For Cpp
DEFINE_LOG_CATEGORY(EnvInteractionManager)


WREnvInteractionManager::WREnvInteractionManager()
{
}

WREnvInteractionManager::~WREnvInteractionManager()
{
}

void WREnvInteractionManager::OnInitialize()
{
	this->InitializeTable();
	this->InitFoliageComponents();
}

void WREnvInteractionManager::OnShutdown()
{
	this->RemoveAll();
}

void WREnvInteractionManager::OnLoadLevelStart()
{
	this->RemoveAll();
}

void WREnvInteractionManager::OnLoadLevelComplete()
{
	this->InitFoliageComponents();
}

void WREnvInteractionManager::OnRenderStart()
{
}


void WREnvInteractionManager::CutStaticFoliage(UWRFoliageInstancedStaticMeshComponent* InFoliageComponent, const FHitResult & SweepResult)
{
	UWorld* InWorld = nullptr;

	InWorld = UWRGameInstance::GetGameInstance()->GetWorld();

	if (InWorld == nullptr)
		return;

	FTransform InTransform;
	InFoliageComponent->GetInstanceTransform(SweepResult.Item, InTransform);
	FWRFoliageTable* InFoliageTable = WRTableManager::Get()->FindRow<FWRFoliageTable>(InFoliageComponent->GetTID());
	if (InFoliageTable == nullptr)
		return;

	if (InFoliageTable->CuttedFoliageTID <= -1)
		return;

	UWRFoliageInstancedStaticMeshComponent* InComponents = nullptr;
	for (UWRFoliageInstancedStaticMeshComponent* TempComponents : InstancedFoliageComponents)
	{
		if (TempComponents->GetTID() == InFoliageTable->CuttedFoliageTID)
		{
			InComponents = TempComponents;
		}
	}

	USoundBase* SoundBase = nullptr;
	UParticleSystem* InParticleSystem = nullptr;

	if (InComponents == nullptr)
		return;

	InParticleSystem = LoadObject<UParticleSystem>(nullptr, *InFoliageTable->CutEffect);//InData->CutEffect.LoadSynchronous();

	if (InParticleSystem == nullptr)
		return;

	SoundBase = LoadObject<USoundBase>(nullptr, *InFoliageTable->CuttingSound); //Cast<USoundBase>(InData->CuttingSound.LoadSynchronous());

	if (SoundBase == nullptr)
		return;

	InComponents->AddInstance(InTransform);
	InFoliageComponent->RemoveInstance(SweepResult.Item);

	/*World Space로 변환*/
	UGameplayStatics::SpawnEmitterAtLocation(InWorld, InParticleSystem, InTransform * InFoliageComponent->GetComponentTransform());

	WRSFXManager::Get()->SpawnAtLocation(SoundBase, InTransform.GetLocation());
}

void WREnvInteractionManager::CutDynamicFoliage(AWRDynamicFoliage* InDynamicFoliage, const FHitResult & SweepResult)
{
	UWorld* InWorld = nullptr;

	InWorld = UWRGameInstance::GetGameInstance()->GetWorld();

	if (InWorld == nullptr)
		return;

	FTransform InTransform = InDynamicFoliage->GetTransform();

	FWRFoliageTable* InFoliageTable = WRTableManager::Get()->FindRow<FWRFoliageTable>(InDynamicFoliage->GetTID());
	if (InFoliageTable == nullptr)
		return;

	if (InFoliageTable->CuttedFoliageTID <= -1)
		return;

	UWRFoliageInstancedStaticMeshComponent* InComponents = nullptr;
	for (UWRFoliageInstancedStaticMeshComponent* TempComponents : InstancedFoliageComponents)
	{
		if (TempComponents->GetTID() == InFoliageTable->CuttedFoliageTID)
		{
			InComponents = TempComponents;
		}
	}


	USoundBase* SoundBase = nullptr;
	UParticleSystem* InParticleSystem = nullptr;

	if (InComponents == nullptr)
		return;

	InParticleSystem = LoadObject<UParticleSystem>(nullptr, *InFoliageTable->CutEffect);//InData->CutEffect.LoadSynchronous();

	if (InParticleSystem == nullptr)
		return;

	SoundBase = LoadObject<USoundBase>(nullptr, *InFoliageTable->CuttingSound);//Cast<USoundBase>(InData->CuttingSound.LoadSynchronous());

	if (SoundBase == nullptr)
		return;

	InComponents->AddInstance(InTransform);
	InDynamicFoliage->Destroy();

	UGameplayStatics::SpawnEmitterAtLocation(InWorld, InParticleSystem, InTransform);

	WRSFXManager::Get()->SpawnAtLocation(SoundBase, InTransform.GetLocation());
}

void WREnvInteractionManager::InitFoliageComponents()
{
	UWorld* InWorld = UWRGameInstance::GetGameInstance()->GetWorld();

	if (InWorld == nullptr)
		return;

	for (TActorIterator<AInstancedFoliageActor> It(InWorld); It; ++It)
	{
		AInstancedFoliageActor* foliageActor = *It;

		InstancedFoliageComponents.Empty();
		foliageActor->GetComponents<UWRFoliageInstancedStaticMeshComponent>(InstancedFoliageComponents);
		break;
	}
}

void WREnvInteractionManager::InitializeTable()
{
	UDataTable* FoliageTable = WRTableManager::Get()->GetTable<FWRFoliageTable>();

	if (FoliageTable == nullptr)
		return;

	FoliageTable->GetAllRows(FString(""), FoliageDataArray);
}

bool WREnvInteractionManager::IsFoliage(const FHitResult & SweepResult)
{
	UPrimitiveComponent* HitComponent = SweepResult.GetComponent();
	/*AActor* InActor = SweepResult.GetActor();
	AWRDynamicFoliage* InDynamicFoliage = Cast<AWRDynamicFoliage>(InActor);*/

	if (HitComponent == nullptr)
		return false;

	UWRFoliageInstancedStaticMeshComponent* InstanceStaticFoliageComponent = Cast<UWRFoliageInstancedStaticMeshComponent>(HitComponent);

	if (InstanceStaticFoliageComponent != nullptr)//|| InDynamicFoliage != nullptr)
		return true;

	return false;
}

void WREnvInteractionManager::OverlapFoliage(const FHitResult & SweepResult)
{
	UPrimitiveComponent* HitComponent = SweepResult.GetComponent();

	if (HitComponent == nullptr)
		return;

	UWRFoliageInstancedStaticMeshComponent* InFoliageComponent = Cast<UWRFoliageInstancedStaticMeshComponent>(HitComponent);

	if (InFoliageComponent == nullptr)
		return;

	FTransform InTransform;

	InFoliageComponent->GetInstanceTransform(SweepResult.Item, InTransform);

	FWRFoliageTable* InFoliageTable = WRTableManager::Get()->FindRow<FWRFoliageTable>(InFoliageComponent->GetTID());
	if (InFoliageTable == nullptr)
		return;

	USoundBase* SoundBase = LoadObject<USoundBase>(nullptr, *InFoliageTable->CuttingSound);

	if (SoundBase == nullptr)
		return;

	WRSFXManager::Get()->SpawnAtLocation(SoundBase, InTransform.GetLocation());
}

void WREnvInteractionManager::CutFoliage(const FHitResult & SweepResult)
{
	UPrimitiveComponent* HitComponent = SweepResult.GetComponent();

	if (HitComponent == nullptr)
		return;

	UWRFoliageInstancedStaticMeshComponent* InFoliageComponent = Cast<UWRFoliageInstancedStaticMeshComponent>(HitComponent);

	if (InFoliageComponent != nullptr)
	{
		this->CutStaticFoliage(InFoliageComponent, SweepResult);
	}
	else
	{
		AActor* InActor = SweepResult.GetActor();
		AWRDynamicFoliage* InDynamicFoliage = Cast<AWRDynamicFoliage>(InActor);

		if (InDynamicFoliage != nullptr)
		{
			this->CutDynamicFoliage(InDynamicFoliage, SweepResult);
		}
	}
}

void WREnvInteractionManager::ConvertDynamicFoliage(AWRCharacter* OverlappedActor, const FHitResult & SweepResult)
{
	UPrimitiveComponent* HitComponent = SweepResult.GetComponent();

	if (HitComponent == nullptr)
		return;

	UWRFoliageInstancedStaticMeshComponent* InFoliageComponent = Cast<UWRFoliageInstancedStaticMeshComponent>(HitComponent);

	if (InFoliageComponent == nullptr)
		return;

	FTransform InTransform;
	InFoliageComponent->GetInstanceTransform(SweepResult.Item, InTransform);

	UWorld* InWorld = nullptr;

	InWorld = UWRGameInstance::GetGameInstance()->GetWorld();

	if (InWorld == nullptr)
		return;

	FWRFoliageTable* InFoliageTable = WRTableManager::Get()->FindRow<FWRFoliageTable>(InFoliageComponent->GetTID());
	if (InFoliageTable == nullptr)
		return;

	const FVector InPosition = InTransform.GetLocation();
	const FRotator InRotator = InTransform.Rotator();
	FVector InScale = InTransform.GetScale3D();

	if (InFoliageTable->DynamicFoliage.IsEmpty() == true)
		return;

	FString InFilePath = InFoliageTable->DynamicFoliage;
	AWRDynamicFoliage* InDynamicFoliage = WRActorSpawnUtility::Spawn<AWRDynamicFoliage>(InWorld, InFilePath, InPosition, InRotator, InScale);

	if (InDynamicFoliage == nullptr)
		return;

	// scale 변환이 안되어서 피직스 재정의
	USkeletalMeshComponent* InSkeletalMeshComponent = InDynamicFoliage->FindComponentByClass<USkeletalMeshComponent>();
	InSkeletalMeshComponent->SetWorldScale3D(InScale);
	InSkeletalMeshComponent->RecreatePhysicsState();

	InDynamicFoliage->SetMyOwner(OverlappedActor);
	InDynamicFoliage->SetInstanceFoliageAndIndex(InFoliageComponent, SweepResult.Item);
	InDynamicFoliage->SetTID(InFoliageComponent->GetTID());
	InDynamicFoliage->InitializeFoliage();
	InFoliageComponent->RemoveInstance(SweepResult.Item);
}

bool WREnvInteractionManager::IsSpiderWeb(const FHitResult & SweepResult)
{
	AActor* InActor = SweepResult.GetActor();
	AWRSpiderWeb* InSpiderWeb = Cast<AWRSpiderWeb>(InActor);

	if (InSpiderWeb == nullptr)
		return false;

	return true;
}

void WREnvInteractionManager::KickOffSpiderWeb(const FHitResult & SweepResult, AWRCharacter* InAttackedActor)
{
	AActor* InActor = SweepResult.GetActor();
	AWRSpiderWeb* InSpiderWeb = Cast<AWRSpiderWeb>(InActor);

	if (InSpiderWeb == nullptr || InAttackedActor == nullptr)
		return;

	if (InSpiderWeb->IsExceptCharacter(InAttackedActor->GetCharacterTableID()) == true)
		return;

	if (InSpiderWeb->kickOffSound.IsNull() == false)
	{
		USoundBase* KickOffSoundBase = Cast<USoundBase>(InSpiderWeb->kickOffSound.LoadSynchronous());

		WRSFXManager::Get()->SpawnAtLocation(KickOffSoundBase, InSpiderWeb->GetActorLocation());
	}

	InSpiderWeb->KickOffStart();
}

void WREnvInteractionManager::TangledInSpiderWeb(const FHitResult & SweepResult, AWRCharacter* InOverlappedActor)
{
	if (InOverlappedActor == nullptr)
		return;

	UWRActorComponentManageMaterial* InManageMaterialComp = InOverlappedActor->GetComponent<UWRActorComponentManageMaterial>();

	if (InManageMaterialComp == nullptr)
		return;

	AActor* InActor = SweepResult.GetActor();
	AWRSpiderWeb* InSpiderWeb = Cast<AWRSpiderWeb>(InActor);

	if (InSpiderWeb == nullptr)
		return;

	if (InSpiderWeb->TangledSound.IsNull() == false)
	{
		USoundBase* TangledSoundBase = Cast<USoundBase>(InSpiderWeb->TangledSound.LoadSynchronous());

		WRSFXManager::Get()->SpawnAtLocation(TangledSoundBase, InSpiderWeb->GetActorLocation());
	}

	if (InSpiderWeb->IsExceptCharacter(InOverlappedActor->GetCharacterTableID()) == true)
		return;

	if (InSpiderWeb->GetIsTangled() == false && InOverlappedActor->IsMe() == true)
		WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::SpiderWeb, InSpiderWeb->TangledDuration);

	if (InSpiderWeb->GetIsTangled() == true)
		InSpiderWeb->KickOffStart();
}

bool WREnvInteractionManager::IsEnvActor(const FHitResult& SweepResult)
{
	AActor* InActor = SweepResult.GetActor();
	AWREnvActor* InEnvActor = Cast<AWREnvActor>(InActor);

	if (InEnvActor == nullptr)
		return false;

	return true;
}

void WREnvInteractionManager::BeginEnvHittedInteraction(AWRCharacter* InCharacter, const FHitResult& InSweepResult)
{
	if (WREnvInteractionManager::Get()->IsFoliage(InSweepResult) == true)
	{
		WREnvInteractionManager::Get()->CutFoliage(InSweepResult);
		return;
	}
	else if (WREnvInteractionManager::Get()->IsSpiderWeb(InSweepResult))
	{
		if (InCharacter == nullptr)
			return;

		WREnvInteractionManager::Get()->KickOffSpiderWeb(InSweepResult, InCharacter);
		return;
	}
}

void WREnvInteractionManager::BeginEnvOverlapInteraction(AWRCharacter* InCharacter, const FHitResult& InSweepResult)
{
	if (WREnvInteractionManager::Get()->IsFoliage(InSweepResult) == true)
	{
		WREnvInteractionManager::Get()->OverlapFoliage(InSweepResult);

		if (InCharacter == nullptr)
			return;

		WREnvInteractionManager::Get()->ConvertDynamicFoliage(InCharacter, InSweepResult);
		return;
	}
	else if (WREnvInteractionManager::Get()->IsSpiderWeb(InSweepResult))
	{
		WREnvInteractionManager::Get()->TangledInSpiderWeb(InSweepResult, InCharacter);
		return;
	}
}

void WREnvInteractionManager::AddMovingWalkway(AWREnvMovingWalkway* IN InEnvMovingWalkway)
{
	this->Movingwalkways.Add(InEnvMovingWalkway);
}

void WREnvInteractionManager::GetMovingWalkway(TArray<AWREnvMovingWalkway*>& OUT InMovingwalkways)
{
	InMovingwalkways = this->Movingwalkways;
}

//====================================================================================
//	Spawned Env
//====================================================================================

void WREnvInteractionManager::OnPostSpawnCompleted(class AWREnvActor* InEnvActor)
{
	if (nullptr == InEnvActor)
		return;

	switch (InEnvActor->GetEnvType())
	{
	case EWREnvType::SecretDialogArea:
	{

	}
	break;
	case EWREnvType::Elevator:
	case EWREnvType::FloatingBoard:
	case EWREnvType::Tardis:
	{
		AWREnvVehicleBase* CastedVehicleBase = Cast<AWREnvVehicleBase>(InEnvActor);
		if (CastedVehicleBase == nullptr)
			return;

		CastedVehicleBase->OnUpdatePassengersLocation();
		SpawnedEnvVehicle.Add(CastedVehicleBase);
	}
	break;
	}
}

void WREnvInteractionManager::SpawnFloatingBoard(const pt_Game_Actor::Inf_FloatingBoardAppear_G2U& IN InInform)
{
	if (FindEnvByAID(InInform.aid) != nullptr)
		return;

	FWRFloatingBoardTable* pTable = WRTableManager::Get()->FindRow<FWRFloatingBoardTable>(InInform.floatingBoardTID);
	if (pTable == nullptr)
		return;

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WREnvInteractionManager::OnSpawnFloatingBoardCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(pTable->MainBPPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	UE_LOG(EnvInteractionManager, Display, TEXT("WREnvInteractionManager::SpawnFloatingBoard - Add Key : %s"), *AsyncKey);
	AsyncSpawnkey.Add(AsyncKey, InInform.aid);
	FloatingBoardSpawnInfo.Add(AsyncKey, InInform);
}

void WREnvInteractionManager::OnSpawnFloatingBoardCompleted(UObject* InObject, FString InHashKey)
{
	if (FloatingBoardSpawnInfo.Contains(InHashKey) == false)
		return;

	AWRFloatingBoard* pSpawnedEnv = Cast<AWRFloatingBoard>(InObject);
	if (nullptr == pSpawnedEnv)
		return;

	pt_Game_Actor::Inf_FloatingBoardAppear_G2U InInform = FloatingBoardSpawnInfo.FindRef(InHashKey);
	FWRFloatingBoardTable* pTable = WRTableManager::Get()->FindRow<FWRFloatingBoardTable>(InInform.floatingBoardTID);
	if (pTable == nullptr)
		return;

	OnPostSpawnCompleted(pSpawnedEnv);

	pSpawnedEnv->OnSpawnComplete(InInform);

	// Remove Async Info
	FloatingBoardSpawnInfo.Remove(InHashKey);
	AsyncSpawnkey.Remove(InHashKey);
}

void WREnvInteractionManager::SpawnElevator(const pt_Game_Actor::Inf_ElevatorAppear_G2U& IN InInform)
{
	if (FindEnvByAID(InInform.aid) != nullptr)
		return;

	FWRElevatorTable* pTable = WRTableManager::Get()->FindRow<FWRElevatorTable>(InInform.elevatorTID);
	if (pTable == nullptr)
		return;

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WREnvInteractionManager::OnSpawnElevatorCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(pTable->MainBPPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	UE_LOG(EnvInteractionManager, Display, TEXT("WREnvInteractionManager::SpawnElevator - Add Key : %s"), *AsyncKey);
	AsyncSpawnkey.Add(AsyncKey, InInform.aid);
	ElevatorSpawnInfo.Add(AsyncKey, InInform);
}

void WREnvInteractionManager::OnSpawnElevatorCompleted(UObject* InObject, FString InHashKey)
{
	if (ElevatorSpawnInfo.Contains(InHashKey) == false)
		return;

	AWRElevator* pSpawnedEnv = Cast<AWRElevator>(InObject);
	if (nullptr == pSpawnedEnv)
		return;

	pt_Game_Actor::Inf_ElevatorAppear_G2U InInform = ElevatorSpawnInfo.FindRef(InHashKey);
	FWRElevatorTable* pTable = WRTableManager::Get()->FindRow<FWRElevatorTable>(InInform.elevatorTID);
	if (pTable == nullptr)
		return;

	OnPostSpawnCompleted(pSpawnedEnv);

	pSpawnedEnv->OnSpawnComplete(InInform);

	// Remove Async Info
	ElevatorSpawnInfo.Remove(InHashKey);
	AsyncSpawnkey.Remove(InHashKey);
}

void WREnvInteractionManager::SpawnTardis(const pt_Game_Actor::Inf_TardisAppear_G2U& IN InInform)
{
	if (FindEnvByAID(InInform.aid) != nullptr)
		return;

	FWRTardisTable* pTable = WRTableManager::Get()->FindRow<FWRTardisTable>(InInform.tardisTID);
	if (pTable == nullptr)
		return;

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WREnvInteractionManager::OnSpawnTardisCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(pTable->MainBPPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	UE_LOG(EnvInteractionManager, Display, TEXT("WREnvInteractionManager::SpawnElevator - Add Key : %s"), *AsyncKey);
	AsyncSpawnkey.Add(AsyncKey, InInform.aid);
	TardisSpawnInfo.Add(AsyncKey, InInform);
}

void WREnvInteractionManager::OnSpawnTardisCompleted(UObject* InObject, FString InHashKey)
{
	if (TardisSpawnInfo.Contains(InHashKey) == false)
		return;

	AWRTardis* pSpawnedEnv = Cast<AWRTardis>(InObject);
	if (nullptr == pSpawnedEnv)
		return;

	pt_Game_Actor::Inf_TardisAppear_G2U InInform = TardisSpawnInfo.FindRef(InHashKey);
	FWRTardisTable* pTable = WRTableManager::Get()->FindRow<FWRTardisTable>(InInform.tardisTID);
	if (pTable == nullptr)
		return;

	OnPostSpawnCompleted(pSpawnedEnv);

	pSpawnedEnv->OnSpawnComplete(InInform);

	// Remove Async Info
	TardisSpawnInfo.Remove(InHashKey);
	AsyncSpawnkey.Remove(InHashKey);
}

void WREnvInteractionManager::ReserveDestroyDome()
{
	if (this->SpawnedSecretDialogDome != nullptr)
		this->SpawnedSecretDialogDome->ReserveDestroy(true);

	WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, 3156);
}

// 현재는 종류가 한개 뿐이라 임시용으로 대응 추후 랜덤으로 공간이 늘어나게되면 패킷에서 제공하는 TID로 대응
#define SecretDialogAreaTID 90009

void WREnvInteractionManager::SpawnSecretDialogDome(const CHAT_GROUP_ID_t IN InGroupID, const TArray<AID_t> InAIDs)
{
	if (IsReserveSpawnDome == true)
		return;

	if (this->SpawnedSecretDialogDome != nullptr)
		return;

	FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (pTable == nullptr)
		return;

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WREnvInteractionManager::OnSpawnSecretDialogDomeCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(pTable->SecretDialogDomeBlueprintPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	UE_LOG(EnvInteractionManager, Display, TEXT("WREnvInteractionManager::SpawnSecretDialogDome - Add Key : %s"), *AsyncKey);
	FWRSecretDialogInfo InInfo;

	InInfo.GroupID = InGroupID;
	InInfo.TalkPlayerList = InAIDs;

	SecretDialogSpawnInfo.Add(AsyncKey, InInfo);
	IsReserveSpawnDome = true;
}

void WREnvInteractionManager::OnSpawnSecretDialogDomeCompleted(UObject* InObject, FString InHashKey)
{
	if (SecretDialogSpawnInfo.Contains(InHashKey) == false)
		return;

	FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (pTable == nullptr)
		return;


	AWRSecretDialogDome* pSpawnedEnv = Cast<AWRSecretDialogDome>(InObject);
	if (nullptr == pSpawnedEnv)
	{
		SecretDialogSpawnInfo.Remove(InHashKey);
		return;
	}

	this->SpawnedSecretDialogDome = pSpawnedEnv;
	IsReserveSpawnDome = false;


	FWRSecretDialogInfo InInfo = SecretDialogSpawnInfo.FindRef(InHashKey);

	this->SpawnedSecretDialogDome->SetGroupIDAndRadius(InInfo.GroupID, pTable->LeaveDistance);
	// Set Dome Location
	{
		if (InInfo.TalkPlayerList.IsValidIndex(0) == false || InInfo.TalkPlayerList.IsValidIndex(1) == false)
		{
			this->UnSpawnSecretDialogDome();
			SecretDialogSpawnInfo.Remove(InHashKey);
			return;
		}

		AWRCharacter* InFirst = WRCharacterManager::Get()->FindbyAID(InInfo.TalkPlayerList[0]);
		AWRCharacter* InSecond = WRCharacterManager::Get()->FindbyAID(InInfo.TalkPlayerList[1]);

		if (InFirst == nullptr || InSecond == nullptr)
		{
			this->UnSpawnSecretDialogDome();
			SecretDialogSpawnInfo.Remove(InHashKey);
			return;
		}

		FVector MiddlePointBetweenTwoVec = FVector::ZeroVector;

		FVector TempVec = (InSecond->GetActorLocation() - InFirst->GetActorLocation()) / 2.f;
		MiddlePointBetweenTwoVec = InFirst->GetActorLocation() + TempVec;

		pSpawnedEnv->SetActorLocation(MiddlePointBetweenTwoVec);
	}
	SecretDialogSpawnInfo.Remove(InHashKey);
}

void WREnvInteractionManager::UnSpawnSecretDialogDome()
{
	if (this->SpawnedSecretDialogDome == nullptr)
		return;

	this->SpawnedSecretDialogDome->RemoveFromRoot();
	this->SpawnedSecretDialogDome->Destroy();
	this->SpawnedSecretDialogDome = nullptr;
}

void WREnvInteractionManager::SpawnSecretDialogPortal(const FWRSecretDialogInfo& InInfo)
{
	AID_t MyAID = WRCharacterManager::Get()->GetMyAID();
	AWRCharacter* InMe = WRCharacterManager::Get()->GetMyCharacter();
	if (InMe == nullptr)
		return;

	if (InInfo.GroupID == InMe->GetDialogGroupID())
		return;

	FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (pTable == nullptr)
		return;

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WREnvInteractionManager::OnSpawnSecretDialogPortalCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(pTable->SecretDialogBlueprintPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	UE_LOG(EnvInteractionManager, Display, TEXT("WREnvInteractionManager::SpawnSecretDialogArea - Add Key : %s"), *AsyncKey);

	SecretDialogSpawnInfo.Add(AsyncKey, InInfo);
}

void WREnvInteractionManager::OnSpawnSecretDialogPortalCompleted(UObject* InObject, FString InHashKey)
{
	if (SecretDialogSpawnInfo.Contains(InHashKey) == false)
		return;

	AWRSecretDialogPortal* pSpawnedEnv = Cast<AWRSecretDialogPortal>(InObject);
	if (nullptr == pSpawnedEnv)
		return;

	pSpawnedEnv->OnCompleteSpawn(SecretDialogSpawnInfo.FindRef(InHashKey));

	SpawnedPortals.Add(pSpawnedEnv);
	SecretDialogSpawnInfo.Remove(InHashKey);
}

void WREnvInteractionManager::UnSpawnPortal(const AID_t& InPortalAID)
{
	AWRSecretDialogPortal* InPortalPtr = Cast<AWRSecretDialogPortal>(this->FindEnvByAID(InPortalAID));
	if (InPortalPtr == nullptr)
		return;
	SpawnedPortals.Remove(InPortalPtr);
	InPortalPtr->RemoveFromRoot();
	InPortalPtr->Destroy();
}

void WREnvInteractionManager::SpawnSecretDialogArea(const FWRSecretDialogInfo& InAreaInfo)
{
	if (IsReserveSpawnEnvArea == true || SpawnedSecretDialogArea != nullptr)
		return;

	FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (pTable == nullptr)
		return;

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WREnvInteractionManager::OnSpawnSecretDialogAreaCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(pTable->SecretDialogAreaBlueprintPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	IsReserveSpawnEnvArea = true;
	SecretDialogSpawnInfo.Add(AsyncKey, InAreaInfo);
}

void WREnvInteractionManager::OnSpawnSecretDialogAreaCompleted(UObject* InObject, FString InHashKey)
{
	if (SecretDialogSpawnInfo.Contains(InHashKey) == false)
		return;

	AWRSecretDialogArea* pSpawnedEnv = Cast<AWRSecretDialogArea>(InObject);
	if (nullptr == pSpawnedEnv)
		return;

	FWRSecretDialogInfo InInfo = SecretDialogSpawnInfo.FindRef(InHashKey);

	pSpawnedEnv->OnSpawnComplete(InInfo);
	
	SpawnedSecretDialogArea = pSpawnedEnv;
	
	/*TArray<AID_t> AIDList = pSpawnedEnv->GetActorIDs();
	for (AID_t InID : AIDList)
	{
		AWRCharacterPlayer* InPlayer = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(InID));
		if (InPlayer == nullptr)
			continue;

		FVector LastPosition = InPlayer->CalcLocationForSecretDialogArea(InPlayer->GetActorLocation());

		InPlayer->SetActorLocation(LastPosition);
		InPlayer->SetPeekedOnPortal(true);
	}*/
	//WRCharacterManager::Get()->SetVisibleByList(AIDList, true);

	IsReserveSpawnEnvArea = false;
	SecretDialogSpawnInfo.Remove(InHashKey);
}

void WREnvInteractionManager::UnSpawnSecretDialogArea()
{
	if (SpawnedSecretDialogArea == nullptr)
		return;

	/*AWRSecretDialogPortal* InPortal = this->FindPortalByGroupID(SpawnedSecretDialogArea->GetChatGroupID());

	TArray<AID_t> AIDList = SpawnedSecretDialogArea->GetActorIDs();

	for (AID_t InID : AIDList)
	{
		AWRCharacterPlayer* InPlayer = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(InID));
		if (InPlayer == nullptr)
			continue;

		FVector LastPosition = InPlayer->CalcLocationForSecretDialogPortal(InPlayer->GetActorLocation());

		InPlayer->SetActorLocation(LastPosition);
		InPlayer->SetPeekedOnPortal(false);
	}	
	*/
	SpawnedSecretDialogArea->RemoveFromRoot();
	SpawnedSecretDialogArea->Destroy();
	SpawnedSecretDialogArea = nullptr;
}

void WREnvInteractionManager::StartSecretDialogProcess()
{
	AActor* InTartgetedActor = WRInputSystemManager::Get()->GetLookAtActor();
	if (InTartgetedActor == nullptr)
		return;

	AWRSecretDialogPortal* InPortalPtr = Cast<AWRSecretDialogPortal>(InTartgetedActor);
	if (InPortalPtr == nullptr)
		return;

	if (SpawnedPortals.Contains(InPortalPtr) == true)
	{
		IMMessageSender::RequestSecretDialogInfo(InPortalPtr->GetGroupID());
	}

}

class AWRSecretDialogPortal* WREnvInteractionManager::FindPortalByGroupID(const CHAT_GROUP_ID_t IN InGroupID)
{
	for (AWRSecretDialogPortal* It : SpawnedPortals)
	{
		if (It->GetGroupID() == InGroupID)
		{
			return It;
		}
		break;
	}
	return nullptr;
}

void WREnvInteractionManager::BeginClosePortalProcess(const AID_t& InPortalAID)
{
	AWREnvActor* InEnvPtr = WREnvInteractionManager::Get()->FindEnvByAID(InPortalAID);
	if (InEnvPtr != nullptr)
	{
		AWRSecretDialogPortal* InPortalPtr = Cast<AWRSecretDialogPortal>(InEnvPtr);
		if (InPortalPtr != nullptr)
			InPortalPtr->BeginDestroyPortal();
	}
}

class AWREnvActor* WREnvInteractionManager::FindEnvByAID(const AID_t IN InAID)
{
	if (InAID == INVALID_AID)
		return nullptr;

	for (auto& Env : SpawnedEnvActors)
	{
		if (Env->GetAID() == InAID)
		{
			return Env;
		}
	}

	for (auto& Env : SpawnedEnvVehicle)
	{
		if (Env->GetAID() == InAID)
		{
			return Env;
		}
	}

	for (auto& Env : SpawnedPortals)
	{
		if (Env->GetAID() == InAID)
		{
			return Env;
		}
	}

	return nullptr;
}

void WREnvInteractionManager::RemoveAll()
{
	for (auto& Env : SpawnedEnvActors)
	{
		if (Env->IsValidLowLevel() == false)
			continue;;

		Env->RemoveFromRoot();
		Env->Destroy();
	}
	SpawnedEnvActors.Empty();

	for (auto& Env : SpawnedEnvVehicle)
	{
		if (Env->IsValidLowLevel() == false)
			continue;;

		Env->RemoveFromRoot();
		Env->Destroy();
	}
	SpawnedEnvVehicle.Empty();

	for (auto& Env : SpawnedPortals)
	{
		if (Env->IsValidLowLevel() == false)
			continue;;

		Env->RemoveFromRoot();
		Env->Destroy();
	}
	SpawnedPortals.Empty();

	this->UnSpawnSecretDialogDome();
	this->UnSpawnSecretDialogArea();
	// [ 2020-4-6 : kyu ] None Spawn이라 수동삭제 필요없음
// 	for (auto& Env : VehicleOutterDoor)
// 	{
// 		if (Env->IsValidLowLevel() == false)
// 			continue;;
// 
// 		Env->RemoveFromRoot();
// 		Env->Destroy();
// 	}
	VehicleOutterDoor.Empty();
	Movingwalkways.Empty();

	AsyncSpawnkey.Empty();
}

void WREnvInteractionManager::UpdatePassengerLocation()
{
	for (auto& EnvVehicle : SpawnedEnvVehicle)
	{
		if (EnvVehicle == nullptr)
			continue;

		EnvVehicle->OnUpdatePassengersLocation();
	}
}

bool WREnvInteractionManager::IsEnvVehiclePassenger(const AID_t IN InPlayerAID, const EWREnvType IN InEnvType /*= EWREnvType::None*/)
{
	for (auto& EnvVehicle : SpawnedEnvVehicle)
	{
		if (EnvVehicle == nullptr)
			continue;

		switch (InEnvType)
		{
		case EWREnvType::None:
		{
			if (EnvVehicle->IsAlreadyPassenger(InPlayerAID))
				return true;
		}
		break;
		default:
		{
			if (EnvVehicle->GetEnvType() != InEnvType)
				continue;

			if (EnvVehicle->IsAlreadyPassenger(InPlayerAID))
				return true;
		}
		break;
		}
	}

	// + Async List
	auto FindAsyncListLambda = ([](std::list<AID_t>& InPassengerList, AID_t InFinidAID)
	{
		std::list<AID_t>::const_iterator iter = InPassengerList.begin();
		std::list<AID_t>::const_iterator iter_end = InPassengerList.end();
		for (; iter != iter_end; ++iter)
		{
			if ((AID_t)*iter == InFinidAID)
				return true;
		}
		return false;
	});

	switch (InEnvType)
	{
	case EWREnvType::None:
	{
		for (auto& AsyncInfo : FloatingBoardSpawnInfo)
		{
			pt_Game_Actor::Inf_FloatingBoardAppear_G2U Inf = AsyncInfo.Value;
			if (FindAsyncListLambda(Inf.lstPassengers, InPlayerAID))
				return true;
		}
		for (auto& AsyncInfo : ElevatorSpawnInfo)
		{
			pt_Game_Actor::Inf_ElevatorAppear_G2U Inf = AsyncInfo.Value;
			if (FindAsyncListLambda(Inf.lstPassengers, InPlayerAID))
				return true;
		}
		for (auto& AsyncInfo : TardisSpawnInfo)
		{
			pt_Game_Actor::Inf_TardisAppear_G2U Inf = AsyncInfo.Value;
			if (FindAsyncListLambda(Inf.lstPassengers, InPlayerAID))
				return true;
		}
	}
	break;
	case EWREnvType::FloatingBoard:
	{
		for (auto& AsyncInfo : FloatingBoardSpawnInfo)
		{
			pt_Game_Actor::Inf_FloatingBoardAppear_G2U Inf = AsyncInfo.Value;
			if (FindAsyncListLambda(Inf.lstPassengers, InPlayerAID))
				return true;
		}
	}
	break;
	case EWREnvType::Elevator:
	{
		for (auto& AsyncInfo : ElevatorSpawnInfo)
		{
			pt_Game_Actor::Inf_ElevatorAppear_G2U Inf = AsyncInfo.Value;
			if (FindAsyncListLambda(Inf.lstPassengers, InPlayerAID))
				return true;
		}
	}
	break;
	case EWREnvType::Tardis:
	{
		for (auto& AsyncInfo : TardisSpawnInfo)
		{
			pt_Game_Actor::Inf_TardisAppear_G2U Inf = AsyncInfo.Value;
			if (FindAsyncListLambda(Inf.lstPassengers, InPlayerAID))
				return true;
		}
	}
	break;
	}

	return false;
}

class AWREnvVehicleBase* WREnvInteractionManager::FindEnvVehicleByPassengerAID(const AID_t IN InPassengerAID)
{
	if (InPassengerAID == INVALID_AID)
		return nullptr;

	for (auto& EnvVehicle : SpawnedEnvVehicle)
	{
		if (EnvVehicle == nullptr)
			continue;

		if (EnvVehicle->IsAlreadyPassenger(InPassengerAID))
			return EnvVehicle;
	}
	return nullptr;
}

AWREnvMovingWalkway* WREnvInteractionManager::FindOverlapingMWByCharacterAID(const AID_t IN InCharacterAID)
{
	for (AWREnvMovingWalkway*It : Movingwalkways)
	{
		AWRCharacter* InCharacter = WRCharacterManager::Get()->FindbyAID(InCharacterAID);
		if (InCharacter == nullptr)
			continue;

		if (It->IsOverlappingActor(InCharacter) == true)
			return It;
	}
	return nullptr;
}
AWREnvMovingWalkway* WREnvInteractionManager::FindOverlappedMWByCharacterAID(const AID_t IN InCharacterAID)
{
	for (AWREnvMovingWalkway*It : Movingwalkways)
	{
		if (It->IsOverlappedActor(InCharacterAID) == true)
			return It;
	}
	return nullptr;
}

void WREnvInteractionManager::Send_GetOnEnvVehicle(class AWRCharacterMine* IN InMine, class AWREnvVehicleBase* IN InEnvVehicle)
{
	if (InEnvVehicle == nullptr)
		return;

	AID_t PlayerAID = InMine->GetActorID();
	AID_t EnvVehicleAID = InEnvVehicle->GetAID();

	InMine->SetEnvVehicleAID(EnvVehicleAID);
	InMine->SetMoveState(EWRMoveStateType::OnTransport);
	InEnvVehicle->AddPassenger(PlayerAID);
	IMMessageSender::RequestGetOnTransportation(EnvVehicleAID, InMine->GetEnvVehicleOffset(), InMine->GetEnvVehicleOffsetAngle());

	if (InEnvVehicle->GetEnvType() == EWREnvType::Tardis && WRInputSystemManager::Get()->IsVoiceRecording())
	{
		WRInputSystemManager::Get()->CancelRecording();
	}
}

void WREnvInteractionManager::Send_GetOffEnvVehicle(class AWRCharacterPlayer* IN InPlayer)
{
	AID_t PlayerAID = InPlayer->GetActorID();
	AID_t EnvVehicleAID = InPlayer->GetEnvVehicleAID();

	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(EnvVehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	InPlayer->SetEnvVehicleAID(INVALID_AID);
	if (InPlayer->IsMe() == true)
	{
		AWRCharacterMine* InMe = Cast<AWRCharacterMine>(InPlayer);
		if (InMe != nullptr)
			InMe->SetMoveState(EWRMoveStateType::OnFoot);
	}
	FoundEnvVehicle->RemovePassenger(PlayerAID);
	IMMessageSender::RequestGetOffTransportation(EnvVehicleAID);
}

void WREnvInteractionManager::OnInf_VehicleState(const pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform)
{
	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(InInform.aid);
	if (FoundEnvVehicle == nullptr)
		return;

	FoundEnvVehicle->OnChangeState(InInform);
}

void WREnvInteractionManager::OnInf_GetOn(const pt_Game_Vehicle::Inf_GetOnTransport_G2U& IN InInform)
{
	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(InInform.vehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	FoundEnvVehicle->AddPassenger(InInform.passengerAID);

	AWRCharacterPlayer* FoundPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InInform.passengerAID);
	if (FoundPlayer != nullptr)
	{
		FoundPlayer->SetEnvVehicleAID(InInform.vehicleAID);
	}
}

void WREnvInteractionManager::OnInf_GetOff(const pt_Game_Vehicle::Inf_GetOffTransport_G2U& IN InInform)
{
	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(InInform.vehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	FoundEnvVehicle->RemovePassenger(InInform.passengerAID);

	AWRCharacterPlayer* FoundPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InInform.passengerAID);
	if (FoundPlayer != nullptr)
	{
		FoundPlayer->SetEnvVehicleAID(INVALID_AID);
		FoundPlayer->SetEnvVehicleTransformOffset(false);
	}
}

bool WREnvInteractionManager::OnInf_ActorMoveToPos_G2U(const pt_Game_User::Inf_ActorMoveToPos_G2U& IN InInform)
{
	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(InInform.actorID);
	if (FoundEnvVehicle != nullptr)
	{
		FoundEnvVehicle->OnMoveToPos(InInform);
		return true;
	}
	else
	{

	}
	return false;
}

bool WREnvInteractionManager::OnInf_ActorMoveStop_G2U(const pt_Game_User::Inf_ActorMoveStop_G2U& IN InInform)
{
	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(InInform.actorID);
	if (FoundEnvVehicle != nullptr)
	{
		FoundEnvVehicle->OnMoveStop(InInform);
		return true;
	}
	else
	{
		// 		for (auto Key : AsyncSpawnkey)
		// 		{
		// 			if (Key.Value == InInform.actorID)
		// 			{
		// 				{
		// 					pt_Game_Actor::Inf_FloatingBoardAppear_G2U* pInfo = FloatingBoardInfo.Find(Key.Key);
		// 					if (pInfo)
		// 					{
		// 						pInfo->posX = InInform.stop_posX;
		// 						pInfo->posY = InInform.stop_posY;
		// 						pInfo->posZ = InInform.stop_posZ;
		// 						pInfo->angle = InInform.rotate;
		// 						return true;
		// 					}
		// 				}				
		// 				{
		// 					pt_Game_Actor::Inf_ElevatorAppear_G2U* pInfo = ElevatorInfo.Find(Key.Key);
		// 					if (pInfo)
		// 					{
		// 						pInfo->posX = InInform.stop_posX;
		// 						pInfo->posY = InInform.stop_posY;
		// 						pInfo->posZ = InInform.stop_posZ;
		// 						pInfo->angle = InInform.rotate;
		// 						return true;
		// 					}
		// 				}
		// 				{
		// 					pt_Game_Actor::Inf_TardisAppear_G2U* pInfo = TardisInfo.Find(Key.Key);
		// 					if (pInfo)
		// 					{
		// 						pInfo->posX = InInform.stop_posX;
		// 						pInfo->posY = InInform.stop_posY;
		// 						pInfo->posZ = InInform.stop_posZ;
		// 						pInfo->angle = InInform.rotate;
		// 						return true;
		// 					}
		// 				}
		// 			}
		// 		}
	}
	return false;
}

void WREnvInteractionManager::OnInf_GroupWarpTimerCount_G2U(const pt_Game_Vehicle::Inf_GroupWarpTimerCount_G2U& IN InInform)
{
	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(InInform.vehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	AWRTardis* CastedTardis = Cast<AWRTardis>(FoundEnvVehicle);
	if (CastedTardis == nullptr)
		return;

	CastedTardis->OnGroupWarpTimerCount(InInform.readyCount, InInform.bBegin);
}

void WREnvInteractionManager::OnInf_GroupWarpStop_G2U(const pt_Game_Vehicle::Inf_GroupWarpStop_G2U& IN InInform)
{
	AWREnvVehicleBase* FoundEnvVehicle = FindEnvVehicle<AWREnvVehicleBase>(InInform.vehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	AWRTardis* CastedTardis = Cast<AWRTardis>(FoundEnvVehicle);
	if (CastedTardis == nullptr)
		return;

	CastedTardis->OnGroupWarpStop();
}

void WREnvInteractionManager::OnReq_GetOnMW_U2G()
{
	AWRCharacterMine* InMine = WRCharacterManager::Get()->GetMyCharacter();
	if (InMine == nullptr)
		return;

	IMMessageSender::RequestBeginOnMW(InMine->GetActorLocation());
	UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnReq_GetOnMW_U2G"));
}

void WREnvInteractionManager::OnAck_GetOnMW_G2U(const stSmallDateTime& IN InStartTime)
{
	AWRCharacterMine* InMine = WRCharacterManager::Get()->GetMyCharacter();
	if (InMine == nullptr)
		return;

	AWREnvMovingWalkway* InMovingwalkway = WREnvInteractionManager::Get()->FindOverlapingMWByCharacterAID(InMine->GetActorID());
	if (InMovingwalkway != nullptr)
	{
		if (InMovingwalkway->IsOverlappedActor(InMine->GetActorID()) == false)
		{
			InMovingwalkway->AddOverlappedActors(InMine->GetActorID(), InStartTime);
			InMine->SetIsBoardingEnv(true);
			InMine->SetMoveState(EWRMoveStateType::OnMovingwalk);

			//UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnAck_GetOnMW_G2U"));
		}
	}
}

void WREnvInteractionManager::OnReq_GetOffMW_U2G()
{
	AWRCharacterMine* InMine = WRCharacterManager::Get()->GetMyCharacter();
	if (InMine == nullptr)
		return;

	IMMessageSender::RequestEndOnMW(InMine->GetActorLocation(), InMine->GetActorRotation().Yaw);
	UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnReq_GetOffMW_U2G"));
}

void WREnvInteractionManager::OnAck_GetOffMW_G2U()
{
	AWRCharacterMine* InMine = WRCharacterManager::Get()->GetMyCharacter();
	if (InMine == nullptr)
		return;

	AWREnvMovingWalkway* InMovingwalkway = WREnvInteractionManager::Get()->FindOverlappedMWByCharacterAID(InMine->GetActorID());
	if (InMovingwalkway != nullptr)
	{
		InMovingwalkway->RemoveOverlappedActors(InMine->GetActorID());
		InMine->SetIsBoardingEnv(false);
		InMine->SetMoveState(EWRMoveStateType::OnFoot);
		//UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnAck_GetOffMW_G2U"));
	}
}

void WREnvInteractionManager::OnInf_GetOnMW_G2U(const pt_Game_Vehicle::Inf_GetOnMW_G2U& IN InPacket)
{
	AID_t InActorID = InPacket.aid;
	stSmallDateTime InStartTime = InPacket.startTime;
	AWRCharacterPlayer* InCharacter = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(InActorID));
	if (InCharacter == nullptr)
	{
		UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnInf_GetOnMW_G2U, Character is null, AID : %lld"), InPacket.aid);
		return;
	}
	AWREnvMovingWalkway* InMovingwalkway = WREnvInteractionManager::Get()->FindOverlapingMWByCharacterAID(InActorID);
	if (InMovingwalkway != nullptr)
	{
		if (InMovingwalkway->IsOverlappedActor(InCharacter->GetActorID()) == false)
		{
			InMovingwalkway->AddOverlappedActors(InCharacter->GetActorID(), InStartTime);
			InCharacter->SetIsBoardingEnv(true);
			//UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnInf_GetOnMW_G2U"));
		}
	}
}

void WREnvInteractionManager::OnInf_GetOffMW_G2U(const pt_Game_Vehicle::Inf_GetOffMW_G2U& IN InPacket)
{
	AID_t InActorID = InPacket.aid;
	AWRCharacterPlayer* InCharacter = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(InActorID));
	if (InCharacter == nullptr)
	{
		UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnInf_GetOffMW_G2U, Character is null, AID : %lld"), InActorID);
		return;
	}

	AWREnvMovingWalkway* InMovingwalkway = WREnvInteractionManager::Get()->FindOverlappedMWByCharacterAID(InActorID);
	if (InMovingwalkway != nullptr)
	{
		InMovingwalkway->RemoveOverlappedActors(InCharacter->GetActorID());
		InCharacter->SetIsBoardingEnv(false);
		FVector InLocation = FVector(InPacket.arrivalPosX, InPacket.arrivalPosY, InPacket.arrivalPosZ);
		FRotator InRotator = InCharacter->GetActorRotation();
		InRotator.Yaw = InPacket.arrivalAngle;
		InCharacter->SetLocationAndRotation(InLocation, InRotator);
		//UE_LOG(CharacterManager, Display, TEXT("WREnvInteractionManager::OnInf_GetOffMW_G2U"));
	}
}

void WREnvInteractionManager::AddVehicleOutterDoor(class AWREnvSlideDoor* IN InOutterDoor)
{
	VehicleOutterDoor.Add(InOutterDoor);
}

//====================================================================================
//
//====================================================================================
