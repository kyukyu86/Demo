// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_Charge.h"
#include "Components/ProgressBar.h"


void UDMUIPanel_Charge::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDMUIPanel_Charge::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUIPanel_Charge::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ElapsedTime += InDeltaTime;

	float fRate = FMath::Min(ElapsedTime / GoalTime, 1.f);
	ProgressBar_Charge->SetPercent(fRate);

	if (fRate == 1.f && bFullCharge == false)
	{
		bFullCharge = true;
		PlayAnimationByName("FullCharge");
	}
}