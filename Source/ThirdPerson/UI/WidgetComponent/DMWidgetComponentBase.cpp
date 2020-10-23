// Fill out your copyright notice in the Description page of Project Settings.


#include "DMWidgetComponentBase.h"
#include "../../GameInstance/DMGameInstance.h"
#include "../../Manager/DMUIManager.h"
#include <Blueprint/UserWidget.h>
#include "../../Util/DMActorUtil.h"
#include <Engine/AssetManager.h>


void UDMWidgetComponentBase::BeginPlay()
{
	Super::BeginPlay();

	if (bUseBGWidgetComp && BGWidgetComponent != nullptr)
	{
		//UUserWidget* CreatedBGWidget = DMUIManager::Get()->CreateUISyncFullPath("/Game/Blueprints/UI/WidgetActor/WidgetActorBG.WidgetActorBG");

		//const FString strPath = "/Game/Blueprints/Actor/WidgetActor/WidgetActorBG.WidgetActorBG_C";
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		UObject* pNewObject = StreamableManager.LoadSynchronous(BGWidgetComponent.Get());
		if (pNewObject == nullptr)
			return;

		BackGroundWidgetComponent = NewObject<UWidgetComponent>(this, (UClass*)pNewObject);
		if (BackGroundWidgetComponent)
		{
			BackGroundWidgetComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			BackGroundWidgetComponent->RegisterComponent();
			BackGroundWidgetComponent->SetRelativeTransform(FTransform::Identity);
			BackGroundWidgetComponent->SetRelativeLocation(BGOffset);
			FVector2D vThisSize = this->GetDrawSize();
			BackGroundWidgetComponent->SetDrawSize(vThisSize);
			
			BackGroundWidgetComponent->SetGeometryMode(this->GetGeometryMode());
			BackGroundWidgetComponent->SetCylinderArcAngle(this->GetCylinderArcAngle());
		}
	}
}

void UDMWidgetComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BackGroundWidgetComponent)
	{
		BackGroundWidgetComponent->UnregisterComponent();
		BackGroundWidgetComponent->RemoveFromRoot();
	}
	Super::EndPlay(EndPlayReason);
}

void UDMWidgetComponentBase::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if ((Flags & EDMWidgetComponentFlag::Billboarrd) != EDMWidgetComponentFlag::None)
	{
		UpdateBillboard();
	}
}

void UDMWidgetComponentBase::SetFlag(EDMWidgetComponentFlag& IN InFlags)
{
	Flags = InFlags;

	if ((Flags & EDMWidgetComponentFlag::Focusable) != EDMWidgetComponentFlag::None)
	{
		
	}
}

FVector UDMWidgetComponentBase::GetCameraLocation()
{
	APlayerController* PlayerController = UDMGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	return PlayerController->PlayerCameraManager->GetCameraLocation();
}

FRotator UDMWidgetComponentBase::GetCameraRotation()
{
	APlayerController* PlayerController = UDMGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	return PlayerController->PlayerCameraManager->GetCameraRotation();
}

void UDMWidgetComponentBase::UpdateBillboard()
{
	if (GetWorld()->WorldType != EWorldType::PIE && GetWorld()->WorldType != EWorldType::Game)
		return;

	FVector Dir = GetCameraLocation() - GetRelativeLocation();
	//Dir.Z = 0.f;
	FRotator WidgetRotation = Dir.Rotation();

	SetRelativeRotation(FQuat(WidgetRotation));
}
