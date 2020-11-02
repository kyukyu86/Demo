// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_MonitorInLab.h"

#include "Manager/WRSFXManager.h"
#include "Manager/WRMapManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"

#include "Define/WRUIDefine.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/WRGameModeLab.h"
#include "Network/IMMessageSender.h"
#include "Core/WRGameInstance.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"
#include "Interfaces/Interface_PostProcessVolume.h"
#include "UnrealMathUtility.h"
#include "Utility/WRUIUtility.h"
#include "Enum/EWRCharacter.h"
#include "Manager/WRConsoleManager.h"


void UWRUIPanel_MonitorInLab::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Content);
	SetFocusableInputMode(true, this);

	//UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	//for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	//{
	//	APostProcessVolume* PostProcessVolume = *Iter;
	//	if (PostProcessVolume == nullptr)
	//		continue;

	//	FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
	//	PostProcessSettings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);
	//	BlurTime = 1.5f;
	//	break;
	//}

	PlayAnimation(AppearAnimation);

	AppearMonitorMaxTime = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Monitor, "EmissiveBlendTime");
}

void UWRUIPanel_MonitorInLab::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	/*BlurTime -= InDeltaTime;
	if (BlurTime > 0)
	{
		PostProcessSettings->DepthOfFieldMaxBokehSize = BlurTime;
	}
	else if(PostProcessSettings->DepthOfFieldMaxBokehSize != 0)
	{
		PostProcessSettings->DepthOfFieldMaxBokehSize = 0;
	}*/

	AppearMonitorTime += InDeltaTime;
	WRMaterialParameterCollectionManager::Get()->SetScalarParameterValue(EWRMPC::Monitor, "EmissiveValue", FMath::Clamp<float>(AppearMonitorTime, 0, AppearMonitorMaxTime) / AppearMonitorMaxTime);
}

void UWRUIPanel_MonitorInLab::NativeDestruct()
{
	SetFocusableInputMode(false);

	Super::NativeDestruct();
}

FReply UWRUIPanel_MonitorInLab::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	EWRInput WRInput = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (AnyKeys.Contains(WRInput))
	{
		OnAnyButtonClicked();
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UWRUIPanel_MonitorInLab::OnAnyButtonClicked()
{
	if (MonitorLabType == EWRMonitorLabType::Start)
	{
		if (AppearMonitorTime > AppearMonitorMaxTime)
		{
			WRSFXManager::Get()->Spawn2D(TEXT("Start_connect_Cue.Start_connect_Cue"));
			IMMessageSender::RequestReserveMap(DEF_START_MAP_TID);
		}
	}
	else if (MonitorLabType == EWRMonitorLabType::End)
	{
	
	}
}

void UWRUIPanel_MonitorInLab::SetStartGameUI()
{
	WRTextBlock_Content->SetText(StartMonitorText);
	MonitorLabType = EWRMonitorLabType::Start;

	//PlayAnimation(AppearAnimation,0, 0);
}

void UWRUIPanel_MonitorInLab::SetEndGameUI()
{
	WRTextBlock_Content->SetText(EndMonitorText);
	MonitorLabType = EWRMonitorLabType::End;

	PlayAnimation(AppearAnimation);
}