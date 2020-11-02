// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSecretDialogArea.h"
#include "Manager/WREnvInteractionManager.h"
#include "WRSecretDialogPortal.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Table/WREnvObjectTable.h"
#include "Table/Base/WRTableManager.h"

// 현재는 종류가 한개 뿐이라 임시용으로 대응 추후 랜덤으로 공간이 늘어나게되면 패킷에서 제공하는 TID로 대응
#define SecretDialogAreaTID 90009

void AWRSecretDialogArea::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	PortalViewCamera = this->FindComponentByClass<USceneCaptureComponent2D>();
	SpringArmComp = this->FindComponentByClass<USpringArmComponent>();
	SpringArmComp->TargetArmLength = SpringArmComp->TargetArmLength *-1;
}

void AWRSecretDialogArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (this->PortalPtr != nullptr)
	{
		this->CalcSpringArmRotAndCameraRot();
	}
}

void AWRSecretDialogArea::CalcSpringArmRotAndCameraRot()
{
	FVector StartPos = this->PortalPtr->GetActorLocation();
	FVector EndPos = WRCharacterManager::Get()->GetMyCharacter()->GetActorLocation();

	FRotator InLookatRot = FRotator(0.f, UKismetMathLibrary::FindLookAtRotation(StartPos, EndPos).Yaw, 0.f);
	
	SpringArmComp->SetRelativeRotation(InLookatRot);

	FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (pTable == nullptr)
		return;

	float InArmLength = FMath::Clamp(FMath::Abs((EndPos - StartPos).Size()), 0.0f, pTable->LeaveDistance);
	SpringArmComp->TargetArmLength = -InArmLength;
	SpringArmComp->SetWorldRotation(InLookatRot);
	//FRotator LookAtRotation = LookRotation = UKismetMathLibrary::FindLookAtRotation(FVector::z, FVector(0.f, ));;//FRotator(0.f, SpringArmComp->getloca(TargetLocation).Yaw, 0.f);
	//this->SetRotation(LookAtRotation);
	//PortalViewCamera->
	/*APlayerCameraManager* CameraManager = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController()->PlayerCameraManager;

	if (CameraManager == nullptr)
		return;
	FRotator InCameraRotaion = CameraManager->GetTransformComponent()->GetComponentRotation();

	PortalViewCamera->SetWorldRotation(InCameraRotaion);*/
}

void AWRSecretDialogArea::OnSpawnComplete(const FWRSecretDialogInfo& InAreaInfo)
{
	AWRSecretDialogPortal* InPortalPtr = WREnvInteractionManager::Get()->FindPortalByGroupID(InAreaInfo.GroupID);
	if (InPortalPtr == nullptr)
		return;

	FWRSecretDialogAreaTable* pTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (pTable == nullptr)
		return;
	this->SetActorLocation(pTable->AreaLocation);
	this->PortalPtr = InPortalPtr;
	this->GroupID = InAreaInfo.GroupID;
	this->SetActorIDs(InAreaInfo.TalkPlayerList);

	//this->PortalPtr->StartSecretDialog();
}
