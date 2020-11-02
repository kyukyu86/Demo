// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSecretDialogPortal.h"
#include "Manager/WREnvInteractionManager.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WREnvObjectTable.h"
#include "Table/WRCustomTextTable.h"

#include "Utility/WRUIUtility.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRCharacterManager.h"

#include "Actor/Character/WRCharacterMine.h"

#include "UI/Panel/WRUIPanel_InputIcon.h"
#include "Manager/WRVoiceContentsManager.h"
#include "WidgetComponent/WRWidgetComponentBase.h"

#include "Localization/WRLocalizationProcessor.h"


void AWRSecretDialogPortal::BeginPlay()
{
	Super::BeginPlay();

	this->ParticleSystem = this->FindComponentByClass<UParticleSystemComponent>();

	TArray<UActorComponent*> InTargetArea = this->GetComponentsByTag(USphereComponent::StaticClass(), TEXT("TargetArea"));
	if (InTargetArea.IsValidIndex(0) == true)
	{
		this->TargetArea = Cast<USphereComponent>(InTargetArea[0]);
	}

	TArray<UActorComponent*> InInteractionArea = this->GetComponentsByTag(USphereComponent::StaticClass(), TEXT("InteractionArea"));
	if (InInteractionArea.IsValidIndex(0) == true)
	{
		this->InteractionArea = Cast<USphereComponent>(InInteractionArea[0]);
	}

	TArray<UActorComponent*> InUISocket = this->GetComponentsByTag(USceneComponent::StaticClass(), TEXT("UISocket"));
	if (InUISocket.IsValidIndex(0) == true)
	{
		this->UISocket = Cast<USceneComponent>(InUISocket[0]);
	}
	/*ensure(this->ParticleSystem != nullptr, TEXT("[AWRSecretDialogPortal::InitializePortal] ParticleSystem == nullptr"));
	ensure(this->TargetArea != nullptr, TEXT("[AWRSecretDialogPortal::InitializePortal] TargetArea == nullptr"));
	ensure(this->InteractionArea != nullptr, TEXT("[AWRSecretDialogPortal::InitializePortal] InteractionArea == nullptr"));*/

	if (this->ParticleSystem == nullptr || this->TargetArea == nullptr || this->InteractionArea == nullptr)
	{
		WREnvInteractionManager::Get()->UnSpawnPortal(this->GetAID());
	}

	this->InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &AWRSecretDialogPortal::OnBeginOverlap_InteractionableArea);
	this->InteractionArea->OnComponentEndOverlap.AddDynamic(this, &AWRSecretDialogPortal::OnEndOverlap_InteractionableArea);
	this->CurrentState = EWRSecretDialogPortalStateType::Seed;

	InWidgetComp = Cast<UWRWidgetComponentBase>(this->GetComponentByClass(UWRWidgetComponentBase::StaticClass()));
}

void AWRSecretDialogPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->PortalProcess(DeltaTime);
}

void AWRSecretDialogPortal::BeginDestroy()
{
	Super::BeginDestroy();

}

// 현재는 종류가 한개 뿐이라 임시용으로 대응 추후 랜덤으로 공간이 늘어나게되면 패킷에서 제공하는 TID로 대응
#define SecretDialogAreaTID 90009

void AWRSecretDialogPortal::OnCompleteSpawn(const FWRSecretDialogInfo& InAreaInfo)
{
	FWRSecretDialogAreaTable * InPortalTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (InPortalTable == nullptr)
		return;

	this->SetActorLocation(InAreaInfo.PortalLocation);
	this->LeaveDistance = InPortalTable->LeaveDistance;
	this->CurrentState = EWRSecretDialogPortalStateType::Seed;
	this->GroupID = InAreaInfo.GroupID;
	this->SetAID(InAreaInfo.PortalAID);
}

void AWRSecretDialogPortal::StartSecretDialog()
{
	this->SetPortalState(EWRSecretDialogPortalStateType::WillOpen);
	this->ShowOffTargetUI();
}

void AWRSecretDialogPortal::PortalProcess(float DeltaTime)
{
	switch (this->GetPortalState())
	{
	case EWRSecretDialogPortalStateType::Seed:
		this->SeedProcess(DeltaTime);
		break;
	/*case EWRSecretDialogPortalStateType::WillOpen:
		this->OpeningProcess(DeltaTime);
		break;
	case EWRSecretDialogPortalStateType::Opened:
		this->OpenedProcess(DeltaTime);
		break;*/
	case EWRSecretDialogPortalStateType::WillClose:
		this->ClosingProcess(DeltaTime);
		break;
	}
}

void AWRSecretDialogPortal::SeedProcess(float DeltaTime)
{
	this->ShowUIUpdate();
}

void AWRSecretDialogPortal::OpeningProcess(float DeltaTime)
{
	if (this->EffectTimeCurve == nullptr || this->ParticleSystem == nullptr)
		return;

	FWRSecretDialogAreaTable * InPortalTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (InPortalTable == nullptr)
		return;

	this->CurrentDeltaTime = this->CurrentDeltaTime + DeltaTime;

	float InCurrentSize = this->EffectTimeCurve->GetFloatValue(this->CurrentDeltaTime);

	this->ParticleSystem->SetFloatParameter(PortalOpenParamName, InCurrentSize);

	if (InCurrentSize >= InPortalTable->MaxPortalSize)
	{
		this->SetPortalState(EWRSecretDialogPortalStateType::Opened);
	}
}

void AWRSecretDialogPortal::OpenedProcess(float DeltaTime)
{
	FVector InVecDistance = this->GetActorLocation() - WRCharacterManager::Get()->GetMyCharacter()->GetActorLocation();

	if (InVecDistance.Size() >= this->LeaveDistance)
	{
		TArray<AID_t> InArr = WRVoiceContentsManager::Get()->GetActorListInGroupChatList(GroupID);

		WRCharacterManager::Get()->SetVisibleByList(InArr, false);
		

		this->SetPortalState(EWRSecretDialogPortalStateType::WillClose);
	}

}

void AWRSecretDialogPortal::ClosingProcess(float DeltaTime)
{
	if (this->EffectTimeCurve == nullptr || this->ParticleSystem == nullptr)
		return;

	FWRSecretDialogAreaTable * InPortalTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (InPortalTable == nullptr)
		return;

	this->CurrentDeltaTime = this->CurrentDeltaTime - DeltaTime;

	float InCurrentSize = this->EffectTimeCurve->GetFloatValue(this->CurrentDeltaTime);

	this->ParticleSystem->SetFloatParameter(PortalOpenParamName, InCurrentSize);

	if (InCurrentSize <= InPortalTable->MinPortalSize)
	{
		WREnvInteractionManager::Get()->UnSpawnSecretDialogArea();
		if (IsReserveDestroy == true)
		{
			WREnvInteractionManager::Get()->UnSpawnPortal(this->GetAID());
		}
		else
		{
			this->SetPortalState(EWRSecretDialogPortalStateType::Seed);
		}
	}
}


void AWRSecretDialogPortal::ShowUIUpdate()
{
	if (this->InWidgetComp != nullptr)
	{
		this->InWidgetComp->SetVisibility(true);
		AWRCharacterMine* InMine = WRCharacterManager::Get()->GetMyCharacter();
		if (InMine != nullptr)
		{
			FVector TargetLocation = InMine->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), TargetLocation);;
			LookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
			this->InWidgetComp->SetWorldRotation(LookAtRotation);
		}
	}

	/*const AActor* pTarget = WRInputSystemManager::Get()->GetLookAtActor();
	if (pTarget == nullptr)
	{
		this->ShowOffTargetUI();
		return;
	}

	if (pTarget->GetUniqueID() == this->GetUniqueID())
	{
		this->ShowOnTargetUI();
	}
	else
	{
		this->ShowOffTargetUI();
	}*/
}

void AWRSecretDialogPortal::ShowOnTargetUI()
{
	if (this->IsUIOpen == true)
		return;

	if (this->GetPortalState() != EWRSecretDialogPortalStateType::Seed)
		return;

	if (WRUIManager::Get()->FindUIData(EWRUIPanelKind::InputIcon) != nullptr)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(*SocketName));
			UWRUIPanel_InputIcon* Panel_InputIcon = Cast<UWRUIPanel_InputIcon>(InCombinedPanelData.UIPanel);
			if (Panel_InputIcon)
			{
				Panel_InputIcon->SetInputKey(EWRInput::Cross);
				FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(45);
				if (CustomTextTable)
				{
					FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
					Panel_InputIcon->SetContent(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey));
				}
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::InputIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, nullptr);
	WidgetInfo.SetTransform(this->UISocket->GetComponentTransform());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	this->IsUIOpen = true;
}

void AWRSecretDialogPortal::ShowOffTargetUI()
{
	if (this->IsUIOpen == false)
		return;

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::InputIcon);

	this->IsUIOpen = false;
}

void AWRSecretDialogPortal::BeginDestroyPortal()
{
	if (this->IsReserveDestroy == true)
		return;

	this->IsReserveDestroy = true;
	this->SetPortalState(EWRSecretDialogPortalStateType::WillClose);
}

void AWRSecretDialogPortal::OnBeginOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWRSecretDialogPortal::OnEndOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWRCharacterMine* InMe = WRCharacterManager::Get()->GetMyCharacter();
	if (InMe == nullptr)
		return;

	if (this->InteractionArea->IsOverlappingActor(InMe) == false)
	{
		if (this->GetPortalState() == EWRSecretDialogPortalStateType::Seed)
			PrimaryActorTick.bCanEverTick = false;
		ShowOffTargetUI();
	}
}
