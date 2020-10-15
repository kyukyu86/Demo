// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "../../Enum/DMUIEnum.h"
#include "DMWidgetComponentBase.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMWidgetComponentBase : public UWidgetComponent
{
	GENERATED_BODY()
	
private:
	EDMWidgetComponentFlag Flags = EDMWidgetComponentFlag::None;

private:
	FVector GetCameraLocation();
	FRotator GetCameraRotation();

	void UpdateBillboard();

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetFlag(EDMWidgetComponentFlag& IN InFlags);
};
