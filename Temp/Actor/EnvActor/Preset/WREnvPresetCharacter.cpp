// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WREnvPresetCharacter.h"
#include "Components/BoxComponent.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRUImanager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Camera/CameraComponent.h"
#include "UI/Panel/WRUIPanel_CostumeChange.h"
#include "Table/WRCharacterPresetTable.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Table/Base/WRTableManager.h"
#include "Actor/Character/WRCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Utility/WRActorUtility.h"
#include "Define/WRUIDelegate.h"

#define WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME "00.ClothChangingMain"

void AWREnvPresetCharacter::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));

	if (BoxComponent == nullptr)
		return;

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWREnvPresetCharacter::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AWREnvPresetCharacter::OnOverlapEnd);

	EnvPresetCharacter = SpawnPresetCharacter();
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	// [ 2020-4-23 : vuvle ] : Equip을 다시 해야 연출이 정상적으로 발생되므로 재장착을 하기 위한 함수를 호출한다.
	VerifyCharacterEquip();
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	SetMaterialParameterValue(1.0f);

	if (EnvPresetCharacter != nullptr)
	{
		WRActorUtility::SetComponentCollisionEnabledToAll(EnvPresetCharacter->GetRootComponent(), ECollisionEnabled::NoCollision);
		WRActorUtility::SetGenerateOverlapEventsbyActor(false, EnvPresetCharacter, true);
	}
}

void AWREnvPresetCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EnvPresetCharacter)
	{
		if (WRCharacterManager::Get())
		{
			WRCharacterManager::Get()->Remove(EnvPresetCharacter);
		}
	}

	UnloadEffectCurve();

	Super::EndPlay(EndPlayReason);
}

void AWREnvPresetCharacter::OnPresetCharacterTargeted()
{
	/*TArray<AActor*> ChildActors;
	GetAllChildActors(ChildActors);
	if (ChildActors.Num() != 0)
	{
		for (auto& Character : ChildActors)
		{
			AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(Character);
			if (Character != nullptr)
			{
				EnvPresetCharacter = CastedPlayer;
			}
		}
	}*/

	FTransform WidgetTransform;
	TArray<UActorComponent*> ActorWidgetTransform = GetComponentsByTag(USceneComponent::StaticClass(), "WidgetTransform");
	USceneComponent* CastedActorWidgetTransform = nullptr;
	if (ActorWidgetTransform.IsValidIndex(0))
	{
		CastedActorWidgetTransform = Cast<USceneComponent>(ActorWidgetTransform[0]);
	}
	else
	{
		ensureMsgf(false, TEXT("TopPosition is not set"));
	}

	//FTransform WidgetTransform(GetActorRotation(), GetActorLocation(), FVector(3.0f, 3.0f, 3.0f));
	
	
	if(CastedActorWidgetTransform != nullptr)
		WidgetTransform = CastedActorWidgetTransform->GetRelativeTransform();

 	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
 	{
			UWRUIPanel_CostumeChange* Panel_CostumeChange = Cast<UWRUIPanel_CostumeChange>(InCombinedPanelData.UIPanel);
			if(Panel_CostumeChange)
			{
				Panel_CostumeChange->SetParentEnvPreset(this);
				Panel_CostumeChange->SetPresetCharacter(EnvPresetCharacter);
				Panel_CostumeChange->SetCurrentPresetTID(DefaultPresetTID);
			}
 	});

	WidgetTransform.SetScale3D(FVector(5.0f, 5.0f, 5.0f));

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CostumeChange, EWRUIWidgetCreationBitFlag::Billboard, &Delegate, EnvPresetCharacter);
	WidgetInfo.SetTransform(WidgetTransform);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CostumeChange, EnvPresetCharacter, WidgetTransform, EWRUIWidgetCreationBitFlag::Billboard, &Delegate);

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);
}

void AWREnvPresetCharacter::Tick(float DeltaTime)
{
	//UE_LOG(WRLog, Warning, TEXT("Is producing : %d"), bIsProducing);

	if (IsInInterationArea)
	{
		AActor* LookAtChar = GetLookAtActor();
		if (LookAtChar == this)
		{
			if (EventOccurred == false)
			{
				OnPresetCharacterTargeted();
				EventOccurred = true;
			}
		}
		else
		{
			EventOccurred = false;
		}
	}

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
	if (AIDs.Num())
	{
		for (auto& ID : AIDs)
		{
			WRCharacterManager::Get()->Remove(ID);
		}
		AIDs.Empty();
	}
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
}

void AWREnvPresetCharacter::VerifyCharacterEquip()
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(DefaultPresetTID);

	if (PresetTable == nullptr)
		return;

	if (EnvPresetCharacter != nullptr)
	{
		UWRActorComponentEquipmentLook* EquipLook = EnvPresetCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook)
		{
			EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
		}
	}
}

void AWREnvPresetCharacter::LoadEffectCurve()
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(DefaultPresetTID);

	if (PresetTable == nullptr)
		return;

	FWREquipmentLookTable* EquipmentLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(PresetTable->HoloEquipmentLookTID);

	CurveEndEffect = EquipmentLookTable->ChangeCurveSRC.LoadSynchronous();
	if (CurveEndEffect->IsValidLowLevel() == true)
	{
		CurveEndEffect->AddToRoot();
	}

	CurveStartEffect = EquipmentLookTable->ChangeCurveDEST.LoadSynchronous();
	if (CurveStartEffect->IsValidLowLevel() == true)
	{
		CurveStartEffect->AddToRoot();
	}
}

void AWREnvPresetCharacter::UnloadEffectCurve()
{
	if (CurveStartEffect->IsValidLowLevel() == true)
	{
		CurveStartEffect->RemoveFromRoot();
		CurveStartEffect = nullptr;
	}

	if (CurveEndEffect->IsValidLowLevel() == true)
	{
		CurveEndEffect->RemoveFromRoot();
		CurveEndEffect = nullptr;
	}
}

bool AWREnvPresetCharacter::TickProduceStartEffect(float InDeltaTime)
{
	if (CurveStartEffect == nullptr)
		return false;

	ProduceTime += InDeltaTime;

	float StartEffectMinTime, StartEffectMaxTime;
	CurveStartEffect->GetTimeRange(StartEffectMinTime, StartEffectMaxTime);

	if (CurveStartEffect->GetFloatValue(ProduceTime) <= 0.0f)
		return true;

	//EnvPresetCharacter->SetVisibility(true, true);

	if (ProduceTime >= StartEffectMaxTime)
	{
		SetMaterialParameterValue(1.0f);
		ProduceTickHandle.Reset();
		bIsProducing = false;
		//UE_LOG(WRLog, Warning, TEXT("StartEffect Produce Ended"));
		return false;
	}

	if(EnvPresetCharacter->IsVisibility() == false)
		EnvPresetCharacter->SetVisibility(true, true);

	SetMaterialParameterValue(CurveStartEffect->GetFloatValue(ProduceTime));

	return true;
}

AWRCharacter* AWREnvPresetCharacter::SpawnPresetCharacter()
{
	// Spawn Character
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(DefaultPresetTID);

	if (PresetTable == nullptr)
		return nullptr;

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	bIsProducing = true;
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(0, 1, PresetTable->PresetBlueprint, GetActorLocation(), GetActorRotation(), FVector(1.0f, 1.0f, 1.0f));
	AWRCharacter* SpawnedCharacter = WRCharacterManager::Get()->Spawn<AWRCharacter>(SpawnParam, false, true);

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	SpawnedCharacter->SetVisibility(false, true);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	if (SpawnedCharacter != nullptr)
	{
		// [ 2020-4-10 : vuvle ] : Z 를 땅에 붙임
		SpawnedCharacter->SetLocation(GetActorLocation(), true);

		// [ 2020-2-18 : vuvle ] : Spawn 시 반드시 Equip 을 해야 다음 연출이 정상적으로 나옴.
		UWRActorComponentEquipmentLook* EquipLook = SpawnedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook)
		{
			EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
		}

		//// [ 2020-3-23 : vuvle ] : Equip을 다시 해야 연출이 발생된다. 기획 변경으로 첫 캐릭터 부터 연출이 나와야 하는 상황이라 Equip 을 다시 한다.
		//if (EquipLook)
		//{
		//	EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
		//}

		AWRCharacterPlayer* CastedSpawnedCharacter = Cast<AWRCharacterPlayer>(SpawnedCharacter);
		if (CastedSpawnedCharacter->IsValidLowLevel() == true)
		{
			CastedSpawnedCharacter->SetUsingSyncHMDHead(false);
		}
	}

	ProduceTime = 0.0f;
	LoadEffectCurve();

	// [ 2020-4-29 : vuvle ] : BP 비동기 로드 이후 잠깐 보이는 현상이 생겨 안보이게 만드는 구문 추가
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	SetMaterialParameterValue(0.0f);
	ProduceTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AWREnvPresetCharacter::TickProduceStartEffect));
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	
	return SpawnedCharacter;
}

AActor* AWREnvPresetCharacter::GetLookAtActor()
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();

	if (MyChar->IsValidLowLevel() == true)
	{
		FHitResult Hit(1.0f);

		// [ 2019-8-14 : hansang88 ] 콘솔 변수로 변경
		float RayLength = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingRayLength);

		UCameraComponent* CamComponet = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
		if (CamComponet == nullptr)
			return nullptr;

		FVector HMDForward = CamComponet->GetForwardVector();
		FVector StartLocation = CamComponet->GetComponentLocation();
		FVector EndLocation = StartLocation + (HMDForward * RayLength);
		//float	SphereRadius = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingSphereRadius);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(MyChar);

		TArray<AWRCharacter*> DeadCharacters = WRCharacterManager::Get()->GetDeadCharacters();
		for (int i = 0; i < DeadCharacters.Num(); ++i)
			CollisionParams.AddIgnoredActor(DeadCharacters[i]);


		if (MyChar->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_GameTraceChannel14, CollisionParams) == true)
			//if (MyChar->GetWorld()->SweepSingleByObjectType(Hit, StartLocation, EndLocation, FQuat::Identity, ObjectParams, FCollisionShape::MakeSphere(SphereRadius), Params))
		{
			// 			FString ActorName = Hit.Actor->GetName();
			// 			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, ActorName);
			return Hit.Actor.Get();
		}
	}

	return nullptr;
}

void AWREnvPresetCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWRCharacter* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();

	if (MyCharacter == nullptr)
		return;

	if (MyCharacter->GetName() == Other->GetName())
	{
		IsInInterationArea = true;
	}
}

void AWREnvPresetCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWRCharacter* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();

	if (MyCharacter == nullptr)
		return;

	if (MyCharacter->GetName() == Other->GetName())
	{
		IsInInterationArea = false;
		EventOccurred = false;
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::CostumeChange);
	}
}

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
void AWREnvPresetCharacter::OnCompleteDirection(AActor* InActor, EWREquipPart InPart)
{
	AWRCharacter* Character = Cast<AWRCharacter>(InActor);
	if (Character == nullptr)
	{
		return;
	}

	AIDs.Add(Character->GetActorID());
	//WRCharacterManager::Get()->Remove(Character);
}
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

void AWREnvPresetCharacter::SwitchPresetCharacter()
{
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
	//WRCharacterManager::Get()->Remove(EnvPresetCharacter);
	UWRActorComponentEquipmentLook* EquipLook = EnvPresetCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLook)
	{
		//EquipLook->Unequip(EWREquipPart::Body);
		EquipLook->Unequip(EWREquipPart::Body, FWREquipmentComplete::CreateUObject(this, &AWREnvPresetCharacter::OnCompleteDirection));
	}

	EnvPresetCharacter = SpawnPresetCharacter();

	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::CostumeChange);
	if (CombinedPanelData && CombinedPanelData->WidgetComponentBase)
	{
		CombinedPanelData->WidgetComponentBase->SetOwnerActor(EnvPresetCharacter);
	}

	OnPresetCharacterTargeted();
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	bIsProducing = true;
	ProduceTime = 0.0f;
	ProduceTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AWREnvPresetCharacter::TickProduceEndEffect));
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
}

bool AWREnvPresetCharacter::TickProduceEndEffect(float InDeltaTime)
{
	if (CurveEndEffect == nullptr)
		return false;

	ProduceTime += InDeltaTime;

	float EndEffectMinTime, EndEffectMaxTime;
	CurveEndEffect->GetTimeRange(EndEffectMinTime, EndEffectMaxTime);

	if (ProduceTime >= EndEffectMaxTime)
	{
		SetMaterialParameterValue(0.0f);
		UnloadEffectCurve();
		WRCharacterManager::Get()->Remove(EnvPresetCharacter);
		ProduceTickHandle.Reset();
		EnvPresetCharacter = SpawnPresetCharacter();
		OnPresetCharacterTargeted();
		//UE_LOG(WRLog, Warning, TEXT("EndEffect Produce Ended"));
		//bIsProducing = false;
		return false;
	}

// 	if (CurveEndEffect->GetFloatValue(ProduceTime) >= 1.0f)
// 		return true;
	SetMaterialParameterValue(CurveEndEffect->GetFloatValue(ProduceTime));

	return true;
}

void AWREnvPresetCharacter::SetMaterialParameterValue(const float InValue)
{
	if (EnvPresetCharacter == nullptr)
		return;

	// UE_LOG(WRLog, Warning, TEXT("Param : %f"), InValue);

	UWRActorComponentEquipmentLook* EquipmentLookComponent = EnvPresetCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent)
	{
		EquipmentLookComponent->SetMaterialParameterValue(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, InValue);
	}
}

