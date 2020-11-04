// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMComponentBattle.h"
#include "../Enum/DMActorEnum.h"
#include "../Struct/DMActorStruct.h"
#include "DMComponentMyBattle.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMComponentMyBattle : public UDMComponentBattle
{
	GENERATED_BODY()
	
private:
	bool bChargeState = false;

private:
	bool MouseLButtonEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);
	bool MouseRButtonEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);

	void GetMyFSMData(FDMFSMData& OUT OutData);
	bool IsIdleState();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool InputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);
};
