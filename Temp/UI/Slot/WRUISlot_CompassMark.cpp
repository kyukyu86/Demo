// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_CompassMark.h"
#include "Manager/WRCompassManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Utility/WRActorUtility.h"
#include "TextBlock.h"
#include "Define/WRUIDefine.h"
#include "Enum/EWRUIEnum.h"
#include "Actor/Character/WRCharacter.h"

void UWRUISlot_CompassMark::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Dist);
}

void UWRUISlot_CompassMark::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateMarkUI();
}

void UWRUISlot_CompassMark::SetCompassMarkInfo(const WRCompassMarkHandle IN InCompassMarkHandle, const FWRCompassMarkInfoBase* IN InCompassMarkInfo)
{
	if (InCompassMarkInfo == nullptr)
		return;

	CompassMarkHandle = InCompassMarkHandle;
	CompassMarkType = InCompassMarkInfo->GetCompassMarkType();
	if (CompassMarkType == FWRCompassMarkInfoBase::None)
	{
		return;
	}
	else if (CompassMarkType == FWRCompassMarkInfoBase::Position)
	{
		const FWRCompassMarkInfoForPosition* CompassMarkInfoForPosition = static_cast<const FWRCompassMarkInfoForPosition*>(InCompassMarkInfo);
		if (CompassMarkInfoForPosition)
		{
			TargetPosition = CompassMarkInfoForPosition->TargetPosition;
		}
	}
	else if (CompassMarkType == FWRCompassMarkInfoBase::Actor)
	{
		const FWRCompassMarkInfoForActor* CompassMarkInfoForActor = static_cast<const FWRCompassMarkInfoForActor*>(InCompassMarkInfo);
		if (CompassMarkInfoForActor)
		{
			OwnerActor = CompassMarkInfoForActor->TargetActor;
		}
	}
	else
	{
		return;
	}
}

void UWRUISlot_CompassMark::UpdateMarkUI()
{
	if (CompassMarkType == FWRCompassMarkInfoBase::None)
	{
		return;
	}
	else if (CompassMarkType == FWRCompassMarkInfoBase::Position)
	{
		UpdateMarkUIByPosition();
	}
	else if (CompassMarkType == FWRCompassMarkInfoBase::Actor)
	{
		UpdateMarkUIByActor();
	}
	else
	{
		return;
	}
}

void UWRUISlot_CompassMark::UpdateMarkUIByPosition()
{
	FVector FowardVector = WRInputSystemManager::Get()->GetHMDRotation().Vector();
	FowardVector.Z = 0;
	FVector Dir = TargetPosition - WRInputSystemManager::Get()->GetHMDLocation();
	float Dist = Dir.Size();
	Dir.Z = 0;
	float Degree = WRActorUtility::GetDegreesBetween2Vectors(FowardVector, Dir);
	float MoveWidth = DEF_COMPASS_WIDTH * Degree / 360;
	float Base = DEF_COMPASS_WIDTH * DEF_DISPLAY_COMPASS_DEGREE / 360 * 0.5f;
	MoveWidth = FMath::Clamp(MoveWidth, -Base, Base);
	
	SetRenderTranslation(FVector2D(MoveWidth, 0));

	FString DistStr = FString::Printf(TEXT("%d m"), int32(Dist * 0.01f));
	TextBlock_Dist->SetText(FText::FromString(DistStr));
}

void UWRUISlot_CompassMark::UpdateMarkUIByActor()
{
	if (OwnerActor== nullptr)
		return;

	TargetPosition = OwnerActor->GetActorLocation();

	FVector FowardVector = WRInputSystemManager::Get()->GetHMDRotation().Vector();
	FowardVector.Z = 0;
	FVector Dir = TargetPosition - WRInputSystemManager::Get()->GetHMDLocation();
	float Dist = Dir.Size();
	Dir.Z = 0;
	float Degree = WRActorUtility::GetDegreesBetween2Vectors(FowardVector, Dir);
	float MoveWidth = DEF_COMPASS_WIDTH * Degree / 360;
	float Base = DEF_COMPASS_WIDTH * DEF_DISPLAY_COMPASS_DEGREE / 360 * 0.5f;
	MoveWidth = FMath::Clamp(MoveWidth, -Base, Base);

	SetRenderTranslation(FVector2D(MoveWidth, 0));

	FString DistStr = FString::Printf(TEXT("%d m"), int32(Dist * 0.01f));
	TextBlock_Dist->SetText(FText::FromString(DistStr));
}
