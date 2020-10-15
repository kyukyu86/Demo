// Fill out your copyright notice in the Description page of Project Settings.


#include "DMWidgetComponentBase.h"
#include "../../GameInstance/DMGameInstance.h"


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
