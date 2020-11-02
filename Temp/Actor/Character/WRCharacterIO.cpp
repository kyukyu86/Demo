// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCharacterIO.h"
#include "WRCharacterMine.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRNoticeManager.h"
#include "Manager/WRTriggerManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRInventoryManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRObjectInteractionManager.h"
#include "Manager/WRSpawnSimulationManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRSFXManager.h"

#include "Actor/Character/WRCharacterPlayer.h"

#include "UI/Panel/WRUIPanel_IOPopup.h"
#include "UI/Panel/WRUIPanel_TargetName.h"

#include "Define/WRUIDefine.h"
#include "Define/WRPathDefine.h"

#include <DrawDebugHelpers.h>
#include "Kismet/KismetSystemLibrary.h"

#include "Animation/AnimationAsset.h"
#include "../Animation/WRAnimInstance.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"
#include <Ticker.h>

#include "GameFramework/CharacterMovementComponent.h"

#include "Utility/WRActorUtility.h"

#include "Component/Character/WRActorComponentInteraction.h"
#include "Component/Character/WRActorComponentManageMaterial.h"


//====================================================================================
//
//====================================================================================

AWRCharacterIO::AWRCharacterIO()
{
	PrimaryActorTick.bCanEverTick = false;
	CharacterType = EWRCharacterType::IO;
}

AWRCharacterIO::AWRCharacterIO(const FObjectInitializer& ObjectInitializer)
	: AWRCharacterObjectBase(ObjectInitializer)
{
	WRActorUtility::SetActorTickEnable(this, false);	// Native Tick false
	PrimaryActorTick.bStartWithTickEnabled = false;		// BP Tick false

	CharacterType = EWRCharacterType::IO;

	USceneComponent* pMeshParent = GetMeshParentComponent();
	if (pMeshParent == nullptr)
		return;

	IO_InteractionableArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("IO_InteractionableArea"));
	if (IO_InteractionableArea)
	{
		IO_InteractionableArea->InitCapsuleSize(34.0f, 88.0f);
		IO_InteractionableArea->SetCollisionProfileName(TEXT("WRObjectInteractionableArea"));
		IO_InteractionableArea->AttachToComponent(pMeshParent, FAttachmentTransformRules::KeepRelativeTransform);
		IO_InteractionableArea->SetRelativeLocation(FVector(0, 0, 100));

		IO_InteractionableArea->OnComponentBeginOverlap.AddDynamic(this, &AWRCharacterIO::OnBeginOverlap_InteractionableArea);
		IO_InteractionableArea->OnComponentEndOverlap.AddDynamic(this, &AWRCharacterIO::OnEndOverlap_InteractionableArea);
	}
}

void AWRCharacterIO::UpdateOptimization()
{
	UCharacterMovementComponent* pMoveComp = GetCharacterMovement();
	if (pMoveComp != nullptr)
	{
		pMoveComp->SetComponentTickEnabled(false);
		pMoveComp->GravityScale = (MovableIO ? 1.0f : 0.f);
	}
	TSet<UActorComponent*> CompSet = GetComponents();
	for (UActorComponent* pComp : CompSet)
	{
		if (pComp == nullptr)
			continue;

		USceneComponent* pSceneComp = Cast<USceneComponent>(pComp);
		if (pSceneComp == nullptr)
			continue;

		pSceneComp->SetMobility(MovableIO ? EComponentMobility::Movable : EComponentMobility::Static);
	}
	UCapsuleComponent* pCapsuleComp = GetCapsuleComponent();
	if (pCapsuleComp != nullptr)
	{
		pCapsuleComp->SetEnableGravity(MovableIO ? true : false);
	}
	USkeletalMeshComponent* pSkeletalMeshComp = GetMesh();
	if (pSkeletalMeshComp != nullptr)
	{
		pSkeletalMeshComp->SetEnableGravity(MovableIO ? true : false);

		if (pSkeletalMeshComp->SkeletalMesh == nullptr)
		{
			WRActorUtility::SetComponentTickEnable(pSkeletalMeshComp, false);
		}
	}
	if (StaticMesh != nullptr)
	{
		StaticMesh->SetEnableGravity(MovableIO ? true : false);
	}
}

void AWRCharacterIO::BeginPlay()
{
	Super::BeginPlay();

	//UKismetSystemLibrary::DrawDebugCircle(this, this->GetActorLocation(), RecognitionDistance, 100, FColor::Yellow, 0.0f, 2.0f, FVector(0.0f, 1.0f, 0.0f), FVector(0.0f, 0.0f, 1.0f), true);	// 세로
	//UKismetSystemLibrary::DrawDebugCircle(this, this->GetActorLocation(), RecognitionDistance, 100, FColor::Green, 1000.0f, 2.0f, FVector(0.0f, 1.0f, 0.0f), FVector(1.0f, 0.0f, 0.0f), true);	// 가로
	//UKismetSystemLibrary::DrawDebugCircle(this, this->GetActorLocation(), RecognitionDistance, 100, FColor::Red, 100.0f, 2.0f, FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 1.0f), true);	// 세로

	// for Debug
	bool bCollisionShow = WRConsoleManager::Get()->GetBool(EWRConsole::IO_CollisionShow);
	SetShowCollisions(bCollisionShow);

	// 생성 액션
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent != nullptr)
	{
		pInteractionComponent->InvokeDirectionGroup(EWRInteractionGroup::Creation);
	}
}

void AWRCharacterIO::MakeDebugString(TArray<FString>& OUT OutString)
{
	AWRCharacter::MakeDebugString(OutString);

	// FSM
	OutString.Add(FString::Printf(TEXT("FSM : %s"), *WREnumUtility::EnumToString("EWRFSM", GetCurrentFSMType())));

	// Occupy
	if (OccupiedPlayer != nullptr)
	{
		OutString.Add(FString::Printf(TEXT("Occupied AID : %d"), OccupiedPlayer->GetActorID()));
	}
}

void AWRCharacterIO::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// for Debug
	FWRDebugUIData data;
	data.Flag = EWRDebugUIBitFlag::IO;
	data.Actor = this;
	MakeDebugString(data.ArrayContents);
	data.Offset = FVector(0, 0, 50);
	WRUIManager::Get()->UpdateDebugUI(data);
}

void AWRCharacterIO::OnPostSpawn()
{
	AWRCharacter::OnPostSpawn();	
}

void AWRCharacterIO::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AWRCharacterIO::OnPreDestroy()
{
	if (IO_InteractionableArea)
	{
		IO_InteractionableArea->OnComponentBeginOverlap.RemoveDynamic(this, &AWRCharacterIO::OnBeginOverlap_InteractionableArea);
		IO_InteractionableArea->OnComponentEndOverlap.RemoveDynamic(this, &AWRCharacterIO::OnEndOverlap_InteractionableArea);
	}

	// [ 2020-3-5 : kyu ] 삭제시 점유 해제
	if (OccupiedPlayer->IsValidLowLevel())
	{
		UWRActorComponentInteraction* pInteractionComp = OccupiedPlayer->GetComponent<UWRActorComponentInteraction>();
		if (pInteractionComp != nullptr && pInteractionComp->IsOccupiedIO(GetActorID()) == true)
		{
			pInteractionComp->SetOccupiedIO(nullptr, true);
		}
	}

	AWRCharacter::OnPreDestroy();
}

void AWRCharacterIO::BuildCustomComponents()
{
	AWRCharacter::BuildCustomComponents();

	CreateComponent<UWRActorComponentInteractionObject>(EWRCharacterComponent::InteractionObject, true);
}

#if WITH_EDITOR
void AWRCharacterIO::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == TEXT("MovableIO"))
	{
		UpdateOptimization();
	}
	else if (PropertyChangedEvent.GetPropertyName() == TEXT("AnimAsset"))
	{
		for (auto& EachDirection : Directions)
		{
			FWRIODirection& DirectionInfo = EachDirection.Value;
			if (DirectionInfo.AnimAsset.IsNull() == true)
				continue;

			EWRIODirectionType eType = EachDirection.Key;
			if (eType == EWRIODirectionType::None
				|| eType == EWRIODirectionType::CharacterGyroStart
				|| eType == EWRIODirectionType::CharacterInteraction
				|| eType == EWRIODirectionType::CharacterProgress)
			{
				continue;
			}

			if (DirectionInfo.AnimAsset->GetSkeleton() != GetMesh()->SkeletalMesh->Skeleton)
			{
				FString Text = FString::Printf(TEXT("Cannot insert an animasset\nSkeleton is diffrent\n <%s>"), *(DirectionInfo.AnimAsset->GetFName().ToString()));
				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("AWRCharacterIO"));
				DirectionInfo.AnimAsset.Reset();
			}
		}
	}
	else
	{
		for (FWRIOGyro& EachStep : GyroStep)
		{
			if (EachStep.GyroType == EWRIOGyroAction::Angle)
				EachStep.bGyroAngle = true;
			else
				EachStep.bGyroAngle = false;
		}
	}	

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#else // WITH_EDITOR
#endif // WITH_EDITOR

//====================================================================================

void AWRCharacterIO::OnAnimEnded(UAnimSequenceBase* AnimSeq)
{
	// [ 2019-11-21 : kyu ] 일단 애님이든 뭐든 완료되면 끝났다고 보냄	
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent != nullptr)
	{		
		pInteractionComponent->OnAnimationEnded();
	}
}

bool AWRCharacterIO::IsInteracting()
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent != nullptr)
	{
		if (pInteractionComponent->IsInteracting())
			return true;
	}
	return false;
}

void AWRCharacterIO::SetHighLight(EWRObjectHighLight IN InHighLightType)
{
	switch (InHighLightType)
	{
	case EWRObjectHighLight::None:
	{
		SetMaterialParameterValue<float>("05InteractionEmissive", 0.0f);
		SetMaterialParameterValue<float>("05InteractionLine", 0.0f);
	}
	break;
	case EWRObjectHighLight::Recognition:
	{
		// 인지 켬
		SetMaterialParameterValue<float>("05InteractionLine", 1.0f);
		// 타겟 끔
		SetMaterialParameterValue<float>("05InteractionEmissive", 0.0f);
	}
	break;
	case EWRObjectHighLight::Targeted:
	{
		// 인지 끔
		SetMaterialParameterValue<float>("05InteractionLine", 0.0f);
		// 타겟 켬
		SetMaterialParameterValue<float>("05InteractionEmissive", 1.0f);
		float fTargetHighLight = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_NoneTargetHighLightValue);
		SetMaterialParameterValue<float>("05InteractionEmissiveValue", fTargetHighLight);
	}
	break;
	}
}

//====================================================================================
//	Zipline
//====================================================================================

int32 AWRCharacterIO::GetZiplineUniqueKey()
{
// 	if (pTableData == nullptr)
// 		return -1;
// 
// 	return pTableData->ZiplineUniqueKey;
	return -1;
}

void AWRCharacterIO::ArrivalZiplineEndPoint()
{
// 	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
// 	if (pInteractionComponent == nullptr)
// 		return;
// 
// 	pInteractionComponent->OnZiplineEnd();
}

ISN_t AWRCharacterIO::GetISN()
{
	return RelatedISN;
}

bool AWRCharacterIO::CompareISN(ISN_t InISN)
{
	if (InISN == RelatedISN)
		return true;

	return false;
}

void AWRCharacterIO::RemoveDisplay()
{
	UWRActorComponentManageMaterial* ActorComponentManageMaterial = GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::VehicleSpawnDirection);
	ActorComponentManageMaterial->Remove(EWRManageMaterial::Contruct, Duration);

	ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Contruct).AddUObject(this, &AWRCharacterIO::CompleteDisplay);

	WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_VEHICLE_SUMMON, GetActorLocation());
}

void AWRCharacterIO::CompleteDisplay()
{
	IsCompleted = true;

	UWRActorComponentManageMaterial* ActorComponentManageMaterial = GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Contruct).RemoveAll(this);

	WRCharacterManager::Get()->Remove(this);
}

//====================================================================================
//
//====================================================================================

void AWRCharacterIO::OnHookEventToTarget(AWRCharacter* IN InHookCaster, EWRCollisionType IN InConnectedCollisionType)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->OnHookEventToTarget(InHookCaster);
}

bool AWRCharacterIO::IsHookableState()
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return false;

	return pInteractionComponent->IsHookableState();
}

void AWRCharacterIO::OnSpawnComplete(const pt_Game_Actor::Inf_IOAppear_G2U& InInform, FWRInteractionObjectTable* IN InTableData, const bool IN InManualSpawn /* = false */)
{
	pTableData = InTableData;

	if (InManualSpawn == false)
	{
		// Location Set
		SetLocation(FVector(InInform.posX, InInform.posY, InInform.posZ), true);
	}

	// + Optimization
	UpdateOptimization();

	// Interaction Set
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->SetInteractionTable(pTableData);
	pInteractionComponent->SaveDirections(&Directions);
	pInteractionComponent->SaveGyroSteps(&GyroStep);
	pInteractionComponent->UpdateRetainCount(EWRIOInteractionType::DirectInteraction, InInform.directRetainCount);
	pInteractionComponent->UpdateRetainCount(EWRIOInteractionType::HookInteraction, InInform.hookRetainCount);
	pInteractionComponent->SetInteractionState((EWRInteractionState::en)InInform.interactionState);

	if ((EWRInteractionState::en)InInform.interactionState == EWRInteractionState::Using)
	{
		pInteractionComponent->OnInf_InteractionStart(InInform.ownerAID, EWRIOInteractionType::DirectInteraction);
	}

	if (InInform.occupyPlayerAID != INVALID_AID)
	{
		AWRCharacterPlayer* pOccupyPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(InInform.occupyPlayerAID);
		if (pOccupyPlayer != nullptr)
		{
			pInteractionComponent->UpdateToOccupationSync(pOccupyPlayer);
		}
	}

	pInteractionComponent->AfterProcess_SpawnCompleted();

	//누군가가 아이템으로 스폰햇는지 체크
	RelatedISN = InInform.relatedISN;
}

void AWRCharacterIO::OnSpawnComplete(FWRInteractionObjectTable* IN InTableData, EWRInteractionState::en IN InState)
{
	pt_Game_Actor::Inf_IOAppear_G2U Inf;
	Inf.interactionState = InState;
	OnSpawnComplete(Inf, InTableData, true);
}

//====================================================================================
//	상호작용 영역
//====================================================================================
void AWRCharacterIO::OnBeginOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->OnBeginOverlap_InteractionableArea(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWRCharacterIO::OnEndOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->OnEndOverlap_InteractionableArea(OverlappedComp, Other, OtherComp, OtherBodyIndex);
}

void AWRCharacterIO::OnBeginOverlapIOForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	WRSpawnSimulationManager::Get()->OnBeginOverlapSimulationActor(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	SetMaterialParameterValue(TEXT("Selected"), 1.0F, GetCurrentMesh());
}

void AWRCharacterIO::OnEndOverlapIOForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	WRSpawnSimulationManager::Get()->OnEndOverlapSimulationActor(OverlappedComp, Other, OtherComp, OtherBodyIndex);

	SetMaterialParameterValue(TEXT("Selected"), 0.0F, GetCurrentMesh());
}

bool AWRCharacterIO::IsOverlappingInteractionableArea()
{
	if (IO_InteractionableArea != nullptr)
	{
		AWRCharacter* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe != nullptr)
		{
			return IO_InteractionableArea->IsOverlappingActor(pMe);
		}
	}
	return false;
}

//====================================================================================
//	타겟팅
//====================================================================================
void AWRCharacterIO::OnTargeted(const bool IN InMulti /* = false */, const bool IN InByMe /* = true */)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->OnTargeted(InMulti, InByMe);
}

void AWRCharacterIO::ReleasedTarget(const bool IN InMulti /* = false */, const bool IN InByMe /* = true */)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->ReleasedTarget(InMulti, InByMe);
}

FTransform AWRCharacterIO::GetWidgetTransform()
{
	FTransform tf;
	TArray<UActorComponent*> WidgetTransformComponents = GetComponentsByTag(USceneComponent::StaticClass(), "WidgetTransform");
	if (WidgetTransformComponents.IsValidIndex(0))
	{
		USceneComponent* pCastedComponent = Cast<USceneComponent>(WidgetTransformComponents[0]);
		tf = pCastedComponent->GetRelativeTransform();
	}
	else
	{
		tf.SetLocation(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	}
	return tf;
}

EWRUIWidgetCreationBitFlag AWRCharacterIO::GetWidgetComponentShowType()
{
	EWRUIWidgetCreationBitFlag WidgetCompFlag = EWRUIWidgetCreationBitFlag::None;

	switch (ObjectUIType)
	{
	case EWRObjectUIType::FixedLocation:
	{
		
	}
	break;
	case EWRObjectUIType::FixedLocationAndBillboard:
	{
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::Billboard;
	}
	break;
	case EWRObjectUIType::AttachChar:
	{
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::AttachToCharacter;
	}
	break;
	case EWRObjectUIType::AttachCharAndBillboard:
	{
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::AttachToCharacter;
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::Billboard;
	}
	break;
	case EWRObjectUIType::RotateToMeAndBillboard:
	{
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::AttachToCharacterAndRotateToMe;
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::Billboard;
	}
	break;
	case EWRObjectUIType::RotateAxisZToMeAndBillboard:
	{
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::AttachToCharacterAndRotateAxisZToMe;
		WidgetCompFlag |= EWRUIWidgetCreationBitFlag::Billboard;
	}
	break;
	}
	return WidgetCompFlag;
}

FTransform AWRCharacterIO::GetInteractionTransform()
{
	FTransform tf;
	TArray<UActorComponent*> InteractionTransformComponents = GetComponentsByTag(USceneComponent::StaticClass(), "InteractionTransform");
	if (InteractionTransformComponents.IsValidIndex(0))
	{
		USceneComponent* pCastedComponent = Cast<USceneComponent>(InteractionTransformComponents[0]);
		tf = pCastedComponent->GetComponentToWorld();
	}
	else
	{
		return FTransform::Identity;
	}
	return tf;
}

USceneComponent* AWRCharacterIO::GetInteractionComponent()
{
	TArray<UActorComponent*> InteractionTransformComponents = GetComponentsByTag(USceneComponent::StaticClass(), "InteractionTransform");
	if (InteractionTransformComponents.IsValidIndex(0))
	{
		USceneComponent* pCastedComponent = Cast<USceneComponent>(InteractionTransformComponents[0]);
		return pCastedComponent;
	}
	return nullptr;
}

USceneComponent* AWRCharacterIO::GetOccupationEndComponent()
{
	TArray<UActorComponent*> OccupationEndTransforms = GetComponentsByTag(USceneComponent::StaticClass(), "OccupationEndTransform");
	if (OccupationEndTransforms.IsValidIndex(0))
	{
		USceneComponent* pCastedComponent = Cast<USceneComponent>(OccupationEndTransforms[0]);
		return pCastedComponent;
	}
	return nullptr;
}

void AWRCharacterIO::SetTargetAreaEnable(const bool IN InEnable)
{
	UCapsuleComponent* GettedComponent = Cast<UCapsuleComponent>(GetDefaultSubobjectByName("TargetArea"));
	if (GettedComponent)
	{
		if (InEnable)
		{
			GettedComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			GettedComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
//	TArray<UActorComponent*> ComponentList = GetComponentsByTag(USceneComponent::StaticClass(), "TargetArea");
// 	if (ComponentList.IsValidIndex(0))
// 	{
// 		UCapsuleComponent* pCastedComponent = Cast<UCapsuleComponent>(ComponentList[0]);
// 		if (pCastedComponent)
// 		{
// 			if (InEnable)
// 			{
// 				pCastedComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 			}
// 			else
// 			{
// 				pCastedComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 			}
// 		}
// 	}
}

bool AWRCharacterIO::IsDirectOccupiable()
{
	if (pTableData == nullptr)
		return false;
	
	return pTableData->DirectOccupiable;
}

bool AWRCharacterIO::IsDirectOccupiedState()
{
	if (pTableData == nullptr)
		return false;

	if (pTableData->DirectOccupiable && OccupiedPlayer != nullptr)
		return true;

	return false;
}

const EWRHookType AWRCharacterIO::GetHookType()
{
	if (pTableData == nullptr)
		return EWRHookType::None;

	return pTableData->HookType;
}

void AWRCharacterIO::OnHookArrival(const EWRConnectCableEvent IN InEvent, const FVector IN InImpactLocation, AWRCharacter* IN InHookCaster)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->OnHookArrival(InEvent, InImpactLocation, InHookCaster);
}

void AWRCharacterIO::OnHookCutted()
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->OnHookCutted();
}

//====================================================================================
//	클릭 이벤트
//====================================================================================

bool AWRCharacterIO::OnPressedEvent(const EWRInput InInputType)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return false;

	return pInteractionComponent->OnPressedEvent(InInputType);
}

bool AWRCharacterIO::OnReleasedEvent(const EWRInput InInputType)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return false;

	return pInteractionComponent->OnReleasedEvent(InInputType);
}

void AWRCharacterIO::OnPadEvent(const EWRInput IN InEvent)
{
	UWRActorComponentInteractionObject* pInteractionComponent = GetComponent<UWRActorComponentInteractionObject>();
	if (pInteractionComponent == nullptr)
		return;

	pInteractionComponent->OnPadEvent(InEvent);
}

//====================================================================================
//	기타
//====================================================================================

void AWRCharacterIO::SetShowCollisions(const bool IN InShow)
{
	if (IO_InteractionableArea != nullptr)
	{
		IO_InteractionableArea->SetHiddenInGame(!InShow);

		if (InShow)
		{
			UKismetSystemLibrary::DrawDebugCapsule(IO_InteractionableArea, IO_InteractionableArea->GetComponentLocation(), IO_InteractionableArea->GetScaledCapsuleHalfHeight(), IO_InteractionableArea->GetScaledCapsuleRadius(), IO_InteractionableArea->GetComponentRotation(), FColor::Green, 1000.0f, 1.0f);
		}
	}
}