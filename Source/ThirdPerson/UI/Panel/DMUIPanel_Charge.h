// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUIPanel.h"
#include "DMUIPanel_Charge.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUIPanel_Charge : public UDMUIPanel
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UProgressBar* ProgressBar_Charge;

	float GoalTime = 0.f;
	float ElapsedTime = 0.f;
	bool bFullCharge = false;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetGoalTime(const float IN InGoalTime) { GoalTime = InGoalTime; }
};
