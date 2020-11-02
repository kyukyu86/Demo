// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_CompassMark.h"
#include "TextBlock.h"
#include "Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Manager/WRInputSystemManager.h"
#include "Utility/WRActorUtility.h"
#include "Enum/EWRUIEnum.h"
#include "Actor/Character/WRCharacter.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRUIManager.h"

void UWRUIPanel_CompassMark::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Dist);
	BIND_WIDGET(UImage, Image_Mark);
	if (Image_Mark)
	{
		Mat_Mark = WRUIUtility::GetDynamicMaterial(Image_Mark);
	}

	OwnerActor = nullptr;
}

void UWRUIPanel_CompassMark::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateMarkUI();
}

void UWRUIPanel_CompassMark::SetCompassMarkInfo(const WRCompassMarkHandle IN InCompassMarkHandle, const FWRCompassMarkInfoBase* IN InCompassMarkInfo)
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
	
	SetMarkUI(InCompassMarkInfo->MarkUIType);
}

void UWRUIPanel_CompassMark::SetMarkUI(const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType)
{
	if (Mat_Mark == nullptr)
		return;

	if (InCompassMarkUIType == FWRCompassMarkInfoBase::EWRCompassMarkUIType::Question)
	{
		Mat_Mark->SetTextureParameterValue("IconTexture", QuestionMarkUITexture.LoadSynchronous());
		
	}
	else if (InCompassMarkUIType == FWRCompassMarkInfoBase::EWRCompassMarkUIType::Exclamation)
	{
		Mat_Mark->SetTextureParameterValue("IconTexture", ExclamationMarkUITexture.LoadSynchronous());
	}
	else
	{
		Mat_Mark->SetTextureParameterValue("IconTexture", nullptr);
	}
}

void UWRUIPanel_CompassMark::UpdateMarkUI()
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

void UWRUIPanel_CompassMark::UpdateMarkUIByPosition()
{
	FVector Dir = TargetPosition - WRInputSystemManager::Get()->GetHMDLocation();
	float Dist = Dir.Size();
	int32 MeterDist = Dist * 0.01f;
	
	if (MeterDist == PreviousMeterDist)
	{
		return;
	}

	FString DistStr = FString::Printf(TEXT("%dm"), MeterDist);
	TextBlock_Dist->SetText(FText::FromString(DistStr));
	PreviousMeterDist = MeterDist;
}

void UWRUIPanel_CompassMark::UpdateMarkUIByActor()
{
	if (OwnerActor == nullptr)
		return;

	TargetPosition = OwnerActor->GetActorLocation();
	FVector Dir = TargetPosition - WRInputSystemManager::Get()->GetHMDLocation();
	float Dist = Dir.Size();
	int32 MeterDist = Dist * 0.01f;

	if (MeterDist == PreviousMeterDist)
	{
		return;
	}

	FString DistStr = FString::Printf(TEXT("%dm"), MeterDist);
	TextBlock_Dist->SetText(FText::FromString(DistStr));
	PreviousMeterDist = MeterDist;
}
