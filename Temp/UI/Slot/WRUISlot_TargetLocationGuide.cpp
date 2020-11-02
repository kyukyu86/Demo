// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_TargetLocationGuide.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WRTargetLocationGuideManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Camera/CameraComponent.h"
#include "Utility/WRActorUtility.h"
#include "Define/WRUIDefine.h"


void UWRUISlot_TargetLocationGuide::NativeConstruct()
{
	Super::NativeConstruct();
	
	BIND_WIDGET(UImage, Image_Arrow);

	SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUISlot_TargetLocationGuide::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ArrowAnimationFlowTime += InDeltaTime;
	Tick_UpdatePosition();
}

void UWRUISlot_TargetLocationGuide::SetTargetLocationInfo(const FWRTargetLocationInfoBase* IN InTargetLocationInfo)
{
	TargetLocationInfoBase = InTargetLocationInfo;
	Tick_UpdatePosition();
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUISlot_TargetLocationGuide::Tick_UpdatePosition()
{
	if (TargetLocationInfoBase == nullptr)
		return;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	UCameraComponent* CamComponet = Cast<UCameraComponent>(MyCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CamComponet == nullptr)
		return ;
	
	//FVector DirVector = TargetLocationInfoBase->GetTargetLocation() - MyCharacter->GetActorLocation();
	FVector DirVector = TargetLocationInfoBase->GetTargetLocation() - WRInputSystemManager::Get()->GetHMDLocation();
	FVector ProjectionVector = FVector::VectorPlaneProject(DirVector, CamComponet->GetForwardVector());
	ProjectionVector.Normalize();

	//float Degree = WRActorUtility::GetDegreesBetween2Vectors(CamComponet->GetUpVector(), ProjectionVector);
	float Degree = GetDegreesBetween2Vectors(CamComponet->GetUpVector(), ProjectionVector, CamComponet->GetForwardVector());

	//if (IsABehindB(TargetLocationInfoBase->GetTargetLocation(), MyCharacter->GetActorLocation(), CamComponet->GetForwardVector()))
	//{
	//	if (0 < Degree)
	//		Degree = 90;
	//	else
	//		Degree = -90;
	//}

	float Radian = FMath::DegreesToRadians(Degree);

	if (Image_Arrow)
	{
		Image_Arrow->SetRenderTransformAngle(180 + Degree);

		if(ArrowAnimationTime != 0 && ArrowRadiusOffset != 0)
		{
			 CurrentArrowRadiusOffset = FMath::Sin(ArrowAnimationFlowTime * 2 * PI / ArrowAnimationTime) * ArrowRadiusOffset * 0.5f + ArrowRadiusOffset * 0.5f;
		}
		
		float CalArrowRadius = ArrowRadius + CurrentArrowRadiusOffset;
		float ArrowPosX = CalArrowRadius * FMath::Sin(Radian);
		float ArrowPosY = -CalArrowRadius * FMath::Cos(Radian);

		Image_Arrow->SetRenderTranslation(FVector2D(ArrowPosX, ArrowPosY));
	}
		
	float PosX = Radius * FMath::Sin(Radian);
	float PosY = -Radius * FMath::Cos(Radian);

	SetRenderTranslation(FVector2D(PosX, PosY)); // Áß¾Ó
}

float UWRUISlot_TargetLocationGuide::GetDegreesBetween2Vectors(const FVector& IN InToVector, const FVector& IN InFromVector, const FVector& IN InBaseVector)
{
	float DotProduct = FVector::DotProduct(InToVector.GetSafeNormal(), InFromVector.GetSafeNormal());
	float Acos = FMath::Acos(DotProduct);	// 0~180 degree
	float ConvertDegrees = FMath::RadiansToDegrees(Acos); //FMath::RadiansToDegrees(acosf(fDotProduct));

	// ÁÂ,¿ì -180~180 ¼³Á¤
	FVector CrossVector = FVector::CrossProduct(InToVector.GetSafeNormal(), InFromVector.GetSafeNormal());

	DotProduct = FVector::DotProduct(InBaseVector.GetSafeNormal(), CrossVector.GetSafeNormal());
	Acos = FMath::Acos(DotProduct);	
	float TempConvertDegrees = FMath::RadiansToDegrees(Acos); 
	if (TempConvertDegrees < 90)
	{
		ConvertDegrees *= -1.0f;
	}

	return ConvertDegrees;
}

bool UWRUISlot_TargetLocationGuide::IsABehindB(const FVector& IN InAPosition, const FVector& IN InBPosition, const FVector& IN InBFowardVector)
{
	float DotProduct = FVector::DotProduct(InBFowardVector.GetSafeNormal(), (InAPosition - InBPosition).GetSafeNormal());
	float Acos = FMath::Acos(DotProduct);	// 0~180 degree
	float ConvertDegrees = FMath::RadiansToDegrees(Acos); //FMath::RadiansToDegrees(acosf(fDotProduct));
	if (ConvertDegrees <= 90)
	{
		return false;
	}

	return true;
}
