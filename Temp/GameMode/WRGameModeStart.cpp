// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRGameModeStart.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRMapTable.h"
#include "Manager/WRMapManager.h"
#include "EngineUtils.h"
#include "Manager/WRCharacterManager.h"
#include "GameFramework/PlayerStart.h"


#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WRUIManager.h"
#include "UI/Panel/WRUIPanel_HMDGuide.h"
#include "Engine/DataTable.h"
#include "Table/Custom/WRWidgetTable.h"
#include "UObjectGlobals.h"
#include "Manager/WRInputSystemManager.h"
#include "HeadMountedDisplayFunctionLibrary.h"

#define LAP_TID 10001

void AWRGameModeStart::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//====================================================================================
}

void AWRGameModeStart::StartPlay()
{
	Super::StartPlay();

	// [ 2020-6-19 : magedoga ] 임시 : 각 매니저에서 기본적으로 필요한 Asset들 비동기로드 Task 기다려줌.
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([&](float InDeltTime){

		if (WRAsyncLoadUObjectManager::Get()->IsEmptyTask() == true)
		{
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == true && WRConsoleManager::Get()->GetInt(EWRConsole::UI_GuideOn) == 1)
			{
				this->InitializeHmdGuide();
			}
			// [ 2020-06-18 EZ 테스트용 분기 ]
			else if (WRConsoleManager::Get()->GetInt(EWRConsole::UI_GuideOn) == 1)
			{
				this->InitializeHmdGuide();
			}
			else
			{
				WRMapManager::Get()->LoadLevelByTable(LAP_TID);
			}
			return false;
		}

		return true;
	}));

	//====================================================================================
}

void AWRGameModeStart::InitializeHmdGuide()
{
	FVector SpawnLocation;
	FRotator SpawnRotation;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		SpawnLocation = It->GetActorLocation();
		SpawnRotation = It->GetActorRotation();
	}

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
		{
			UWRUIPanel_HMDGuide* InPanel = Cast<UWRUIPanel_HMDGuide>(InCombinedPanelData.UIPanel);
			if (InPanel != nullptr)
			{
				InPanel->StartGuideProcess();
			}
		}
	});


	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(0, 0, "TestPlayer", SpawnLocation, SpawnRotation);
	AWRCharacterMine* Player = WRCharacterManager::Get()->Spawn<AWRCharacterMine>(SpawnParam, true, true);
	WRInputSystemManager::Get()->SetLocationLock(true);
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::HmdGuide, EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);

	UDataTable* WidgetTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Asset/Data/Table/Custom/WidgetTable.WidgetTable"));

	if (WidgetTable == nullptr)
		return;

	TArray<FWRWidgetTable*> TempArray;
	WidgetTable->GetAllRows(FString(""), TempArray);

	for (int32 i = 0; i < TempArray.Num(); ++i)
	{
		FWRWidgetTable* pTable = TempArray[i];
		if (pTable == nullptr)
			continue;

		if (pTable->PanelKind == EWRUIPanelKind::HmdGuide)
		{
			WidgetInfo.SetWidgetData(pTable);
			break;
		}
	}
	
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}
