// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRMapManager.h"
#include "WRSFXManager.h"
#include "WRCharacterManager.h"
#include "WRQuestManager.h"
#include "ModuleManager.h"
#include "WRMaterialParameterCollectionManager.h"
#include "WRTutorialManager.h"
#include "WRInputSystemManager.h"
#include "Table/Base/WRTableManager.h"

#include "SingletonClass/WRSingletonManager.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"

#include "Actor/Character/WRCharacterMine.h"
#include "Camera/CameraActor.h"
#include "Ticker.h"
#include "Utility/WREnumUtility.h"
#include "CustomAsset/WRLoadLevelDirectionAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PackageName.h"
#include "Core/WRGameInstance.h"
#include "WRUIManager.h"
#include "Sound/SoundCue.h"


#include "Network/IMMessageSender.h"
#include "WRAsyncLoadUObjectManager.h"
#include "Define/WRPathDefine.h"
#if PLATFORM_PS4
#include "../Platforms/PS4/Plugins/Runtime/Morpheus/Source/MorpheusFunctionLibrary/Public/MorpheusFunctionLibrary.h"
#define SPLASH_TEXTURE_PATH "/Game/Asset/UI/Images/TitleLogo_High_1920.TitleLogo_High_1920"
#endif // PLATFORM_PS4
#include "Table/WRCoordinateTable.h"
#include "WRConsoleManager.h"
#include "GameMode/WRGameModeBase.h"
#include "Table/WRMapTable.h"
#include "CustomAsset/WRPreLoadAsset.h"

#define TEMP_FADETIME 1.5f
#define LAP_TID 10001

// For Cpp
DEFINE_LOG_CATEGORY(MapManager)

//====================================================================================

#if !REFACTORING_LOAD_LEVEL

bool FWRLoadLevelState::IsShortFadeTime()
{
	return WRMapManager::Get()->IsShortFadeTime();
}

bool FWRLoadLevelState::IsDirectForDeparture()
{
	return WRMapManager::Get()->IsDirectForDeparture();
}

bool FWRLoadLevelState::IsDirectForArrival()
{
	return WRMapManager::Get()->IsDirectForArrival();
}

//====================================================================================

void FWRLoadLevelStateTransition::OnFadeComplete(EWRScreenFadeEvent InFadeEvent)
{
	checkf(WorldContext->IsValidLowLevel() == true, TEXT("Invalid WorldContext O bject."));
	checkf(MapTable != nullptr, TEXT("Null. MapTable."));

	UWorld* World = WorldContext->GetWorld();

	switch (InFadeEvent)
	{
	case EWRScreenFadeEvent::Out:
	{
		WRSingletonManager::LoadLevelStart();

		FString LevelAssetName;
		FPackageName::TryConvertFilenameToLongPackageName(MapTable->SceneLevelFile.ToString(), LevelAssetName);

		FString LoadLevelCommand = WRMapManager::MakeNextURL(LevelAssetName, MapTable->GetGameModeType());

		World->SeamlessTravel(LoadLevelCommand);
		World->SetSeamlessTravelMidpointPause(true);

	} break;
	case EWRScreenFadeEvent::In:
	{
		// [ 2019-12-5 : magedoga ] Transition 연출 삭제
// 		if (WRMapManager::Get()->bStartLevel == false && MapTable->StreamingLevelEffect.IsNull() == false)
// 		{
// 			FTimerHandle FH;
// 			World->GetTimerManager().SetTimer(FH, FTimerDelegate::CreateLambda([&]() {
// 
// 				WRMapManager::Get()->SetLoadLevelState(EWRLoadLevelState::Destination);
// 				WRMapManager::Get()->FadeForLoadLevel(true, TEMP_FADETIME);
// 
// 			}), 2.0f, false);
// 		}
// 		else
		{
			WRMapManager::Get()->SetLoadLevelState(EWRLoadLevelState::Destination);
			WRMapManager::Get()->FadeForLoadLevel(true, this->IsShortFadeTime() ? 0.1f : TEMP_FADETIME);
		}

	} break;
	}
}

void FWRLoadLevelStateTransition::PostWorldInit(UWorld* InWorld)
{
//	WRMapManager::Get()->FadeForLoadLevel(false, this->IsShortFadeTime() ? 0.1f : TEMP_FADETIME);

	// [ 2020-4-29 : magedoga ] PreLoad
	WRMapManager::Get()->StartPreLoad();
}

void FWRLoadLevelStateTransition::PostLoadMap(UWorld* InWorld)
{
	checkf(WorldContext->IsValidLowLevel() == true, TEXT("Invalid WorldContext Object."));
}

void FWRLoadLevelStateTransition::OnPreLoadComplete()
{
// 	UWorld* TransitionWorld = WorldContext->GetWorld();
// 	TransitionWorld->SetSeamlessTravelMidpointPause(false);

#if PLATFORM_PS4
	FStringAssetReference SplashTextureRef = FStringAssetReference(SPLASH_TEXTURE_PATH);
	UTexture* SplashTexture = Cast<UTexture>(SplashTextureRef.ResolveObject());
	if (SplashTexture != nullptr)
	{
		UMorpheusFunctionLibrary::Show2DVRSplashScreen(SplashTexture);
	}
	else
	{
		UE_LOG(MapManager, Display, TEXT("FWRLoadLevelStateTransition::OnPreLoadComplete - Null SplashTexture"));
	}
#endif // PLATFORM_PS4

	WRMapManager::Get()->FadeForLoadLevel(false, this->IsShortFadeTime() ? 0.1f : TEMP_FADETIME);
}

//====================================================================================

void FWRLoadLevelStateDestination::OnFadeComplete(EWRScreenFadeEvent InFadeEvent)
{
	checkf(WorldContext->IsValidLowLevel() == true, TEXT("Invalid WorldContext Object."))

	UWorld* World = WorldContext->GetWorld();

	switch (InFadeEvent)
	{
	case EWRScreenFadeEvent::Out:
	{

		UWorld* TransitionWorld = WorldContext->GetWorld();
		TransitionWorld->SetSeamlessTravelMidpointPause(false);

	} break;
	case EWRScreenFadeEvent::In:
	{
		if (this->IsDirectForArrival() == true)
		{
			FStringAssetReference ArrivalEffectStringRef = FStringAssetReference(MapTable->Arrivaleffect.ToString());
			UObject* FoundObject = ArrivalEffectStringRef.ResolveObject();
			if (FoundObject == nullptr)
			{
				UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevel_Internal - Not found Arrivaleffect. Call TryLoad"));
				FoundObject = ArrivalEffectStringRef.TryLoad();
			}
			WRMapManager::Get()->LoadLevelDirection(Cast<UWRLoadLevelDirectionAsset>(FoundObject), FWREndLoadLevelDirectionDelegate::CreateRaw(WRMapManager::Get(), &WRMapManager::OnEndDirectionWhenArrival));

// 			UWRLoadLevelDirectionAsset* Asset = Cast<UWRLoadLevelDirectionAsset>(MapTable->Arrivaleffect.LoadSynchronous());
// 			if (Asset->IsValidLowLevel() == true)
// 			{
// 				WRMapManager::Get()->LoadLevelDirection(Asset, FWREndLoadLevelDirectionDelegate::CreateRaw(WRMapManager::Get(), &WRMapManager::OnEndDirectionWhenArrival));
// 			}
		}
		else
		{
			WRMapManager::Get()->CompleteLoadLevelProcess();
		}

		

	} break;
	}
}

void FWRLoadLevelStateDestination::PostWorldInit(UWorld* InWorld)
{
	//WRSingletonManager::LoadLevelComplete();
}

void FWRLoadLevelStateDestination::PostLoadMap(UWorld* InWorld)
{
	InWorld->GetFirstPlayerController()->PlayerCameraManager->StartCameraFade(1.0f, 1.0f, 0.0f, FLinearColor::Black, false, true);

	//WRSingletonManager::RenderStart();

	WRMapManager::Get()->FadeForLoadLevel(false, TEMP_FADETIME);

	if (this->IsDirectForArrival() == true)
	{
		WRMaterialParameterCollectionManager::Get()->SetEnableLoadLevelDirection(1.0f);

// 		UWRLoadLevelDirectionAsset* Asset = Cast<UWRLoadLevelDirectionAsset>(MapTable->Arrivaleffect.LoadSynchronous());
// 		if (Asset->IsValidLowLevel() == true)
// 		{
// 			WRMapManager::Get()->LoadLevelDirection(Asset, FWREndLoadLevelDirectionDelegate::CreateRaw(WRMapManager::Get(), &WRMapManager::OnEndDirectionWhenArrival));
// 		}
	}

	WRSingletonManager::RenderStart();
}

#endif // !REFACTORING_LOAD_LEVEL
//====================================================================================




WRMapManager::WRMapManager()
{
}

WRMapManager::~WRMapManager()
{
}


//====================================================================================
	
// bool WRMapManager::OnTick(float InDeltaTime)
// {
// 	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
// 	if (World != nullptr)
// 	{
// 		FRotator CamRotation = World->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation();
// 		FString MSG = FString::Format(TEXT("{0}, {1}, {2}"), { CamRotation.Yaw, CamRotation.Pitch, CamRotation.Roll });
// 		GEngine->AddOnScreenDebugMessage(-1, 0.032f, FColor::Green, MSG, true, FVector2D(5.0f, 5.0f));
// 	}
// 
// 	return true;
// }

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRMapManager::OnInitialize()
{
#if REFACTORING_LOAD_LEVEL
	LevelLoader = FModuleManager::GetModulePtr<FWRLevelLoaderModule>(TEXT("WRLevelLoader"));
#endif // REFACTORING_LOAD_LEVEL
}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRMapManager::OnShutdown()
{
#if REFACTORING_LOAD_LEVEL

	if (TickDirectHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickDirectHandle);
	}

	this->ReleaseDirectData();

	this->ClearAsyncTask(AsyncTaskForDirect, true);
	this->ClearAsyncTask(AsyncTasksForPreLoad, true);

#else // REFACTORING_LOAD_LEVEL
	this->CompleteLoadLevelProcess();

	if (DirectionTickerHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(DirectionTickerHandle);
	}

	CurveDataRelease();

	FModuleManager::Get().LoadModuleChecked<FWRScreenFaderModule>("WRScreenFader").ClearFade(UWRGameInstance::GetGameInstance());

	this->ClearAsyncTask(AsyncTasksForMoveMap, true);
	this->ClearAsyncTask(AsyncTasksForPreLoad, true);
#endif // REFACTORING_LOAD_LEVEL
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRMapManager::OnLoadLevelStart()
{
#if REFACTORING_LOAD_LEVEL

#else // REFACTORING_LOAD_LEVEL
	StartLoadLevelTime = FPlatformTime::Seconds();
	UE_LOG(MapManager, Display, TEXT("WRMapManager::OnRenderStart - Start LoadLevel"));

	ClearAsyncTask(AsyncTasksForPreLoad, true);
#endif // REFACTORING_LOAD_LEVEL
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRMapManager::OnLoadLevelComplete()
{
	
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRMapManager::OnRenderStart()
{
#if REFACTORING_LOAD_LEVEL



#else // REFACTORING_LOAD_LEVEL
	double EndLoadLevelTime = FPlatformTime::Seconds();
	UE_LOG(MapManager, Display, TEXT("WRMapManager::OnRenderStart - LoadLevel Time [%f]"), EndLoadLevelTime - StartLoadLevelTime);
#endif // REFACTORING_LOAD_LEVEL
}

void WRMapManager::EndLoadLevelProcess()
{
	if (WRMapManager::Get()->GetPreviousMapTD() == LAP_TID)
	{
		IMMessageSender::RequestSetLanguageType(EWRLanguageType::en(WRConsoleManager::Get()->GetInt(EWRConsole::LanguageType)));
	}
}

void WRMapManager::OnMapMovementComplete()
{
	UE_LOG(MapManager, Display, TEXT("WRMapManager::OnMapMovementComplete !!!!!!!!!!!"));
	MapMovementCompleteDelegate.Broadcast();
}

#if REFACTORING_LOAD_LEVEL

bool WRMapManager::LoadLevelByTable(const WRTableID InMapTID)
{
	checkf(LevelLoader != nullptr, TEXT("Null LevelLoader. Please Check WRLevelLoader Plugin."));

	FWRMapTableBase* FoundTable = WRTableManager::Get()->FindRowInMapTable(InMapTID);
	if (FoundTable == nullptr)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevelByTable - Not found map table [%d]"), InMapTID);
		return false;
	}

	FString SceneLevelFileName = FoundTable->SceneLevelFile.ToString();
	if (FPackageName::DoesPackageExist(SceneLevelFileName) == false)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevel_Internal - Not exist Level Asset - %s"), *SceneLevelFileName);
		return false;
	}

	FString LevelAssetName;
	FPackageName::TryConvertFilenameToLongPackageName(SceneLevelFileName, LevelAssetName);
	NextURL = WRMapManager::MakeNextURL(LevelAssetName, FoundTable->GetGameModeType());

	MapTable = FoundTable;
	LoadMapTID = InMapTID;
	
	if (this->HasDirectForDeparture() == true)
	{
		this->AddAsyncLoadHashKey(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MapTable->DepartureEffectToMap.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnDirectAssetLoaded)), AsyncTaskForDirect);
	}

	if (this->HasDirectForArrival() == true)
	{
		this->AddAsyncLoadHashKey(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MapTable->Arrivaleffect.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnDirectAssetLoaded)), AsyncTaskForDirect);
	}

// #if PLATFORM_PS4
// 	this->AddAsyncLoadHashKey(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(SPLASH_TEXTURE_PATH, FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnDirectAssetLoaded)), AsyncTaskForDirect);
// #endif // PLATFORM_PS4

	if (AsyncTaskForDirect.Num() > 0)
	{
		return true;
	}

	return this->StartSeamlessTravel(NextURL);
}

//====================================================================================

bool WRMapManager::TeleportInSideWorldComposition(const FVector& InLocation, FSimpleDelegate InCompleteDelegate)
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter->IsValidLowLevel() == false)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::Teleport - Invalid My Character."));
		return false;
	}

	if (this->IsTeleporting() == true)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::TeleportIfPossible - Already Teleporting."));
		return false;
	}

	if (this->IsPossibleTeleport(MyCharacter, InLocation) == false)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::TeleportIfPossible - Impossible Teleport at [%f, %f, %f]"), InLocation.X, InLocation.Y, InLocation.Z);
		return false;
	}

	TeleportCompleteDelegate = InCompleteDelegate;
	bool bSuccess = LevelLoader->TeleportInsideWorldComposition(MyCharacter, InLocation, FSimpleDelegate::CreateRaw(this, &WRMapManager::OnTeleportComplete));

	return bSuccess;
}

bool WRMapManager::IsPossibleTeleport(UObject* InWorldContextObject, const FVector& InLocation) const
{
	return LevelLoader->IsPossibleTeleport(InWorldContextObject, InLocation);
}

void WRMapManager::OnTeleportComplete()
{
	TeleportCompleteDelegate.ExecuteIfBound();
}

//====================================================================================


bool WRMapManager::StartSeamlessTravel(const FString& InURL)
{
	bool bResult = LevelLoader->SeamlessLevelLoad(UWRGameInstance::GetGameInstance(), NextURL, FWRLoadLevelEventDelegate::CreateRaw(this, &WRMapManager::OnSeamlessLoadingEventReceived), true);
	if (bResult == true)
	{
		AWRGameModeBase* GameMode = UWRGameInstance::GetGameInstance()->GetWorld()->GetAuthGameMode<AWRGameModeBase>();
		if (GameMode->IsValidLowLevel() == true)
		{
			GameMode->StartToLeaveMap();
		}
	}

	this->OnSeamlessTravelStarted();

	return bResult;
}

void WRMapManager::StartDirection(const FString& InAssetPath, FSimpleDelegate InCompleteDelegate)
{
	FStringAssetReference DirectAssetRef = FStringAssetReference(InAssetPath);
	UObject* FoundObject = DirectAssetRef.ResolveObject();
	if (FoundObject == nullptr)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevel_Internal - Not found DirectAsset. Call TryLoad"));
		FoundObject = DirectAssetRef.TryLoad();
		if (FoundObject->IsValidLowLevel() == false)
		{
			UE_LOG(MapManager, Display, TEXT("WRMapManager::StartDirection - Direct Asset Can't Load. [%s]"), *InAssetPath);
			return;
		}
	}

	UWRLoadLevelDirectionAsset* DirectAsset = Cast<UWRLoadLevelDirectionAsset>(FoundObject);
	if (DirectAsset->IsValidLowLevel() == false)
		return;

	if (DirectAsset->GridFlow.IsNull() == true)
		return;

	if (DirectAsset->SceneFlow.IsNull() == true)
		return;

	auto FindorLoadCurveAsset = [&](const FString InPath) {

		UCurveFloat* CurveAsset = FindObject<UCurveFloat>(nullptr, *InPath);
		if (CurveAsset == nullptr)
		{
			UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevelDirection - Not found Object : [%s]"), *InPath);
			UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevelDirection - Not found Object. Call LoadObject"));
			CurveAsset = LoadObject<UCurveFloat>(nullptr, *InPath);
		}

		return CurveAsset;
	};

	// [ 2019-11-1 : magedoga ] Setting Curve
	GridFlowCurve = FindorLoadCurveAsset(DirectAsset->GridFlow.ToString()); //FindObject<UCurveFloat>(nullptr, *(InAsset->GridFlow.ToString()));
	if (GridFlowCurve->IsValidLowLevel() == false)
	{
		//CurveDataRelease();
		return;
	}

	//GridFlowCurve->AddToRoot();

	SceneFlowCurve = FindorLoadCurveAsset(DirectAsset->SceneFlow.ToString()); // InAsset->SceneFlow.LoadSynchronous();
	if (SceneFlowCurve->IsValidLowLevel() == false)
	{
		//CurveDataRelease();
		return;
	}

	//SceneFlowCurve->AddToRoot();

	// [ 2019-12-10 : animal14 ] 그리드 연출 시작하면 인풋 잠금
	WRInputSystemManager::Get()->SetLocationLock(true);

	/*EndLoadLevelDirectionDelegate = InDelegate;*/

	// [ 2019-11-1 : magedoga ] Enable
	WRMaterialParameterCollectionManager::Get()->SetEnableLoadLevelDirection(1.0f);

	// [ 2019-11-1 : magedoga ] Setting Global
	WRMaterialParameterCollectionManager::Get()->SetMaxDistance(DirectAsset->MaxDistance);
	WRMaterialParameterCollectionManager::Get()->SetBright(DirectAsset->Brightness);

	// [ 2019-11-1 : magedoga ] Setting Grid
	WRMaterialParameterCollectionManager::Get()->SetGridWidth(DirectAsset->GridWidth);
	WRMaterialParameterCollectionManager::Get()->SetGridColor(DirectAsset->GridColor);
	WRMaterialParameterCollectionManager::Get()->SetGridThickness(DirectAsset->GridThickness);
	WRMaterialParameterCollectionManager::Get()->SetGridBoundThickness(DirectAsset->GridBoundThickness);
	WRMaterialParameterCollectionManager::Get()->SetGridBoundColor(DirectAsset->GridBoundColor);

	// [ 2019-11-1 : magedoga ] Setting Scene
	WRMaterialParameterCollectionManager::Get()->SetSceneBoundThickness(DirectAsset->SceneBoundThickness);
	WRMaterialParameterCollectionManager::Get()->SetSceneBoundColor(DirectAsset->SceneBoundColor);

	EndDirectDelegate = InCompleteDelegate;

	// [ 2019-11-11 : magedoga ] Start Sound
	if (DirectAsset->Sound.IsNull() == false)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter->IsValidLowLevel() == true)
		{
			WRSFXManager::Get()->SpawnAtLocation(Cast<USoundBase>(DirectAsset->Sound.LoadSynchronous()), MyCharacter->GetActorLocation());
		}
	}

	DirectElapsedTime = 0.0f;
	TickDirectHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRMapManager::TickDirect));
}

bool WRMapManager::StartPreLoad()
{
	auto AddToTask = [&](const FString& InFullPath)
	{
		if (FPackageName::DoesPackageExist(InFullPath) == false)
		{
			return;
		}

		FString HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InFullPath, FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncPreLoadComplete));
		if (HashKey != WRASYNCLOADMANAGER_INVALID)
		{
			AsyncTasksForPreLoad.Add(HashKey, nullptr);
		}
	};

	this->ClearAsyncTask(AsyncTasksForPreLoad, true);
	//====================================================================================
	// [ 2020-4-29 : magedoga ] Add PreLoad

	// [ 2020-4-29 : magedoga ] 내 캐릭터
	AddToTask(WRCharacterManager::Get()->GetMyCharacterBPPath());

	//FWRMapTableBase* MapTable = WRTableManager::Get()->FindRowInMapTable(LoadMapTID);

	if (MapTable != nullptr)
	{
		UWRPreLoadAsset* PreLoadData = MapTable->PreLoadData.LoadSynchronous();
		if (PreLoadData != nullptr)
		{
			for (int i = 0; i < PreLoadData->Characters.Num(); ++i)
			{
				AddToTask(PreLoadData->Characters[i].ToString());
			}

			for (int i = 0; i < PreLoadData->Effects.Num(); ++i)
			{
				//AddToTask(PreLoadData->Effects[i].ToString());
				AddToTask(WRActorSpawnUtility::ResolvePath(PreLoadData->Effects[i].ToString()));
			}

			for (int i = 0; i < PreLoadData->Sounds.Num(); ++i)
			{
				AddToTask(PreLoadData->Sounds[i].ToString());
			}

			if (PreLoadData->Folders.Num() > 0)
			{
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				
				for (int i = 0; i < PreLoadData->Folders.Num(); ++i)
				{
					FARFilter Filter;
					TArray<FAssetData> AssetData;

					if (PreLoadData->Folders[i].Kind != EWRPreLoadKind::None)
					{
						FString Kind = WREnumUtility::EnumToString(TEXT("EWRPreLoadKind"), PreLoadData->Folders[i].Kind);
						Filter.ClassNames.Add(*Kind);
					}
					
					Filter.PackagePaths.Add(*(PreLoadData->Folders[i].FolderPath));
					
					AssetRegistryModule.Get().GetAssets(Filter, AssetData);

					for (auto& Data : AssetData)
					{
						AddToTask(Data.ObjectPath.ToString());
					}
				}
			}
		}
	}

	//====================================================================================

	if (AsyncTasksForPreLoad.Num() == 0)
	{
		this->OnPreLoadComplete();
		return false;
	}

	return true;
}


void WRMapManager::OnSeamlessLoadingEventReceived(EWRSeamlessLevelLoadEventType InType)
{
	switch (InType)
	{
	case EWRSeamlessLevelLoadEventType::TravelFailure:
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::OnReceiveEvent_SeamlessLoading - Seamless Level Load Failed."));
		return;
	} break;
	case EWRSeamlessLevelLoadEventType::TransitionLevelLoaded:
	{
		/* [ 2020-6-18 : magedoga ]

			일단은 바로 넘긴다.
			추후에는 PreLoading이 추가될 수 있음.
		*/

		this->StartPreLoad();

		bReservedMapExist = false;
		CopyCurrentMapInformationToPrevious(LoadMapTID);

	} break;
	case EWRSeamlessLevelLoadEventType::DestinationWorldInitialize:
	{

		FWRScreenFaderModule& ScreenFader = FModuleManager::Get().LoadModuleChecked<FWRScreenFaderModule>("WRScreenFader");
		ScreenFader.SetManualFade(UWRGameInstance::GetGameInstance(), FLinearColor::Black, 1.0f, false);

	} break;
	case EWRSeamlessLevelLoadEventType::DestinationLevelLoaded:
	{
		FWRScreenFaderModule& ScreenFader = FModuleManager::Get().LoadModuleChecked<FWRScreenFaderModule>("WRScreenFader");
		ScreenFader.ScreenFadeIn(UWRGameInstance::GetGameInstance(), 1.0f, FWRDelegateCompleteScreenFade::CreateLambda([=](const EWRScreenFadeEvent InEvent) {
		
			auto OnDestinationLevelLoaded = [=]() {

				WRSingletonManager::RenderStart();
				bProcessLoadLevelEndedFunction = false;

				if (this->HasDirectForArrival() == true)
				{
					//this->SetMidpointPauseWhenDirecting(true);
					this->StartDirection(MapTable->Arrivaleffect.ToString(), FSimpleDelegate::CreateRaw(this, &WRMapManager::OnArrivalDirectEnded));
// 					FTimerManager& TimerManager = UWRGameInstance::GetGameInstance()->GetWorld()->GetTimerManager();
// 					FTimerHandle tt;
// 					TimerManager.SetTimer(tt, FTimerDelegate::CreateLambda([=]() {
// 						
// 						this->SetMidpointPauseWhenDirecting(false);
// 
// 					}), 10.f, false);
				}
				else
				{
					this->OnSeamlessTravelEnded();
				}
			};

			// [ 2020-6-24 : magedoga ] 로딩이 완료된 후 WorldComposition이면 해당 맵 진입 위치로 Teleport 시킨다.
			UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
			if (LevelLoader->IsWorldComposition(World) == true)
			{
				FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(MapTable->StartPosTid);
				if (CoordinateTable != nullptr)
				{
					// [ 2020-6-29 : magedoga ] Teleport
					bool bSuccess = LevelLoader->TeleportInsideWorldComposition(World, CoordinateTable->Position, FSimpleDelegate::CreateLambda([=]() {

						FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(MapTable->StartPosTid);
						FVector Location = CoordinateTable->Position;
						Location.Z += 500.0f;
						WRCharacterManager::Get()->GetMyCharacter()->SetActorLocation(Location);

						// [ 2020-6-29 : magedoga ] Fade In
						FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
						if (ScreenFader != nullptr)
						{
							ScreenFader->ScreenFadeIn(UWRGameInstance::GetGameInstance(), 1.0f, nullptr);
						}

						OnDestinationLevelLoaded();
					}));

					if (bSuccess == true)
					{
						// [ 2020-6-29 : magedoga ] Fade Out
						FWRScreenFaderModule* ScreenFader = FModuleManager::Get().LoadModulePtr<FWRScreenFaderModule>("WRScreenFader");
						if (ScreenFader != nullptr)
						{
							ScreenFader->ScreenFadeOut(UWRGameInstance::GetGameInstance(), 0.001f, nullptr, FLinearColor::Black, true, false);
						}
					}
				}
			}
			else
			{
				OnDestinationLevelLoaded();
			}

		}));

		

	} break;
	}
}

void WRMapManager::OnSeamlessTravelStarted()
{
	WRSingletonManager::LoadLevelStart();
}

void WRMapManager::OnSeamlessTravelEnded()
{
	if (bProcessLoadLevelEndedFunction == true)
		return;

	bProcessLoadLevelEndedFunction = true;

	this->ClearAsyncTask(AsyncTaskForDirect, true);

	MapTable = nullptr;
	LoadMapTID = INVALID_TABLE_TID;

	WRSingletonManager::EndLoadLevelProcess();
	IMMessageSender::RequestReadyToPlay();

	// [ 2020-9-4 : magedoga ] 중간에 연출이 멈춰있지 않다면 끝난것.
	if (bMidpointPauseDirecting == false)
	{
		this->OnMapMovementComplete();
	}
}

void WRMapManager::OnDirectAssetLoaded(UObject* InObject, FString InHashKey)
{
	if (AsyncTaskForDirect.Contains(InHashKey) == false)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::OnAsyncLoadCompleteForMoveMap - Not requested hash key."));
		return;
	}

	InObject->AddToRoot();
	AsyncTaskForDirect[InHashKey] = InObject;

	// [ 2020-4-28 : magedoga ] LoadLevelDirectionAsset이면 그 안의 Asest들 다시 Async Load 요청
	UWRLoadLevelDirectionAsset* DirectionAsset = Cast<UWRLoadLevelDirectionAsset>(InObject);
	if (DirectionAsset != nullptr)
	{
		this->AddAsyncLoadHashKey(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(DirectionAsset->GridFlow.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnDirectAssetLoaded)), AsyncTaskForDirect);
		this->AddAsyncLoadHashKey(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(DirectionAsset->SceneFlow.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnDirectAssetLoaded)), AsyncTaskForDirect);
		this->AddAsyncLoadHashKey(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(DirectionAsset->Sound.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnDirectAssetLoaded)), AsyncTaskForDirect);
		return;
	}

	if (WRAsyncLoadUObjectManager::IsLoadCompleteTasks(AsyncTaskForDirect) == true)
	{
		// [ 2020-6-22 : magedoga ] Start Departure Direct
		this->StartDirection(MapTable->DepartureEffectToMap.ToString(), FSimpleDelegate::CreateRaw(this, &WRMapManager::OnDepartureDirectEnded));
		this->OnDepartureDirectStarted();
	}
}

void WRMapManager::OnDepartureDirectStarted()
{
	WRUIManager::Get()->OnDepartureEffectToMapCalled();
}

void WRMapManager::OnDepartureDirectEnded()
{
	FWRScreenFaderModule& ScreenFader = FModuleManager::Get().LoadModuleChecked<FWRScreenFaderModule>("WRScreenFader");
	ScreenFader.ScreenFadeOut(UWRGameInstance::GetGameInstance(), 1.0f, FWRDelegateCompleteScreenFade::CreateLambda([=](const EWRScreenFadeEvent InEvent) {
	
		this->StartSeamlessTravel(NextURL);

	}));
}

void WRMapManager::OnArrivalDirectEnded()
{
	// [ 2020-9-4 : magedoga ] 이미 호출됐다면, OnMapMovementComplete를 호출해야함.
	bool bCopyProcessLoadLevelEndedFunction = bProcessLoadLevelEndedFunction;
	this->OnSeamlessTravelEnded();

	if (bCopyProcessLoadLevelEndedFunction == true)
	{
		this->OnMapMovementComplete();
	}
}

void WRMapManager::OnAsyncPreLoadComplete(UObject* InObject, FString InHashKey)
{
	if (AsyncTasksForPreLoad.Contains(InHashKey) == false)
		return;
	if (InObject == nullptr)
		return;

	InObject->AddToRoot();
	AsyncTasksForPreLoad[InHashKey] = InObject;

	if (WRAsyncLoadUObjectManager::IsLoadCompleteTasks(AsyncTasksForPreLoad) == true)
	{
		this->OnPreLoadComplete();
	}
}

void WRMapManager::OnPreLoadComplete()
{
	LevelLoader->SetSeamlessTravelMidpointPause(false);
}

void WRMapManager::AddAsyncLoadHashKey(const FString& InHashKey, TMap<FString, UObject*>& InMap)
{
	if (InHashKey == WRASYNCLOADMANAGER_INVALID)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::AddAsyncLoadHashKey - Invalid HashKey"));
		return;
	}

	if (InMap.Contains(InHashKey) == true)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::AddAsyncLoadHashKey - Already Exist Key."));
		return;
	}

	InMap.Add(InHashKey, nullptr);
}

void WRMapManager::ClearAsyncTask(TMap<FString, UObject*>& InContaner, bool bInCancelTask)
{
	for (auto It = InContaner.CreateIterator(); It; ++It)
	{
		UObject* Value = It.Value();
		if (Value->IsValidLowLevel() == false && bInCancelTask == true)
		{
			WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(It.Key());
		}
		else
		{
			Value->RemoveFromRoot();
		}
	}

	InContaner.Empty();
}


void WRMapManager::ReleaseDirectData()
{
	if (GridFlowCurve->IsValidLowLevel() == true)
	{
		GridFlowCurve->RemoveFromRoot();
		GridFlowCurve = nullptr;
	}

	if (SceneFlowCurve->IsValidLowLevel() == true)
	{
		SceneFlowCurve->RemoveFromRoot();
		SceneFlowCurve = nullptr;
	}

	EndDirectDelegate.Unbind();
	DirectElapsedTime = 0.0f;
}

bool WRMapManager::TickDirect(float InDeltaTime)
{
	if (bMidpointPauseDirecting == true)
	{
		float GridMinTime, GridMaxTime = 0.0f;
		GridFlowCurve->GetTimeRange(GridMinTime, GridMaxTime);
// 		float SceneMinTime, SceneMaxTime = 0.0f;
// 		SceneFlowCurve->GetTimeRange(SceneMinTime, SceneMaxTime);
// 
// 		float GridRatio = FMath::Clamp(DirectElapsedTime / GridMaxTime, 0.0f, 1.0f);
// 		float SceneRatio = FMath::Clamp(DirectElapsedTime / SceneMaxTime, 0.0f, 1.0f);
// 		UE_LOG(MapManager, Display, TEXT("WRMapManager::TickDirect - Grid[%f] - Scene[%f]"), GridRatio, SceneRatio);
		if (GridMaxTime-0.00001f < DirectElapsedTime)
		{
			if (bProcessLoadLevelEndedFunction == false)
			{
				this->OnSeamlessTravelEnded();
				bProcessLoadLevelEndedFunction = true;
			}
			return true;
		}
	}

	DirectElapsedTime += InDeltaTime;

	float GridFlow = GridFlowCurve->GetFloatValue(DirectElapsedTime);
	WRMaterialParameterCollectionManager::Get()->SetGridFlow(GridFlow);

	float SceneFlow = SceneFlowCurve->GetFloatValue(DirectElapsedTime);
	WRMaterialParameterCollectionManager::Get()->SetSceneFlow(SceneFlow);

	float GridMinTime, GridMaxTime = 0.0f;
	GridFlowCurve->GetTimeRange(GridMinTime, GridMaxTime);
	float SceneMinTime, SceneMaxTime = 0.0f;
	SceneFlowCurve->GetTimeRange(SceneMinTime, SceneMaxTime);

	// [ 2019-11-1 : magedoga ] 연출종료
	float MaxTime = GridMaxTime > SceneMaxTime ? GridMaxTime : SceneMaxTime;
	if (MaxTime <= DirectElapsedTime)
	{
		if (EndDirectDelegate.IsBound() == true)
		{
			EndDirectDelegate.Execute();
			EndDirectDelegate.Unbind();
		}

		this->ReleaseDirectData();
		return false;
	}

	return true;
}

#else // REFACTORING_LOAD_LEVEL


EWRLoadLevelState WRMapManager::GetLoadLevelState() const
{
	if (LoadLevelState == nullptr)
		return EWRLoadLevelState::None;

	return LoadLevelState->GetState();
}

bool WRMapManager::LoadLevelByTable(const WRTableID IN InMapTID)
{
	FWRMapTableBase* FoundTable = WRTableManager::Get()->FindRowInMapTable(InMapTID);
	
	if (FoundTable == nullptr || FoundTable == INVALID_TABLE_TID)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevelByTable - Null Table."));
		return false;
	}

	MapTable = FoundTable;
	LoadMapTID = InMapTID;


	// [ 2020-4-28 : magedoga ] Async
	auto AddToAsyncTask = [&](const FString& InHashKey)
	{
		if (InHashKey != WRASYNCLOADMANAGER_INVALID)
		{
			AsyncTasksForMoveMap.Add(InHashKey, nullptr);
		}
	};

	
	if (this->IsDirectForDeparture() == true)
	{
		AddToAsyncTask(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MapTable->DepartureEffectToMap.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncLoadCompleteForMoveMap)));
	}

	if (this->IsDirectForArrival() == true)
	{
		AddToAsyncTask(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MapTable->Arrivaleffect.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncLoadCompleteForMoveMap)));
	}

// #if PLATFORM_PS4
// 	AddToAsyncTask(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(SPLASH_TEXTURE_PATH, FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncLoadCompleteForMoveMap)));
// #endif // PLATFORM_PS4

	if (AsyncTasksForMoveMap.Num() > 0)
	{
		return true;
	}


	return this->LoadLevel_Internal(FoundTable->SceneLevelFile.ToString());
}

void WRMapManager::LoadLevelDirection(class UWRLoadLevelDirectionAsset* InAsset, FWREndLoadLevelDirectionDelegate InDelegate)
{
	if (InAsset->IsValidLowLevel() == false)
		return;

	if (InAsset->GridFlow.IsNull() == true)
		return;

	if (InAsset->SceneFlow.IsNull() == true)
		return;

	auto FindorLoadCurveAsset = [&](const FString InPath) {

		UCurveFloat* CurveAsset = FindObject<UCurveFloat>(nullptr, *InPath);
		if (CurveAsset == nullptr)
		{
			UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevelDirection - Not found Object : [%s]"), *InPath);
			UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevelDirection - Not found Object. Call LoadObject"));
			CurveAsset = LoadObject<UCurveFloat>(nullptr, *InPath);
		}

		return CurveAsset;
	};

	// [ 2019-11-1 : magedoga ] Setting Curve
	GridFlowCurve = FindorLoadCurveAsset(InAsset->GridFlow.ToString()); //FindObject<UCurveFloat>(nullptr, *(InAsset->GridFlow.ToString()));
	if (GridFlowCurve->IsValidLowLevel() == false)
	{
		CurveDataRelease();
		return;
	}
	
	//GridFlowCurve->AddToRoot();

	SceneFlowCurve = FindorLoadCurveAsset(InAsset->SceneFlow.ToString()); // InAsset->SceneFlow.LoadSynchronous();
	if (SceneFlowCurve->IsValidLowLevel() == false)
	{
		CurveDataRelease();
		return;
	}

	//SceneFlowCurve->AddToRoot();

	EndLoadLevelDirectionDelegate = InDelegate;

	// [ 2019-12-10 : animal14 ] 그리드 연출 시작하면 인풋 잠금
	WRInputSystemManager::Get()->SetLocationLock(true);

	/*EndLoadLevelDirectionDelegate = InDelegate;*/

	// [ 2019-11-1 : magedoga ] Enable
	WRMaterialParameterCollectionManager::Get()->SetEnableLoadLevelDirection(1.0f);

	// [ 2019-11-1 : magedoga ] Setting Global
	WRMaterialParameterCollectionManager::Get()->SetMaxDistance(InAsset->MaxDistance);
	WRMaterialParameterCollectionManager::Get()->SetBright(InAsset->Brightness);

	// [ 2019-11-1 : magedoga ] Setting Grid
	WRMaterialParameterCollectionManager::Get()->SetGridWidth(InAsset->GridWidth);
	WRMaterialParameterCollectionManager::Get()->SetGridColor(InAsset->GridColor);
	WRMaterialParameterCollectionManager::Get()->SetGridThickness(InAsset->GridThickness);
	WRMaterialParameterCollectionManager::Get()->SetGridBoundThickness(InAsset->GridBoundThickness);
	WRMaterialParameterCollectionManager::Get()->SetGridBoundColor(InAsset->GridBoundColor);

	// [ 2019-11-1 : magedoga ] Setting Scene
	WRMaterialParameterCollectionManager::Get()->SetSceneBoundThickness(InAsset->SceneBoundThickness);
	WRMaterialParameterCollectionManager::Get()->SetSceneBoundColor(InAsset->SceneBoundColor);

	// [ 2019-11-11 : magedoga ] Start Sound
	if (InAsset->Sound.IsNull() == false)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if(MyCharacter->IsValidLowLevel() == true)
		{
			WRSFXManager::Get()->SpawnAtLocation(Cast<USoundBase>(InAsset->Sound.LoadSynchronous()), MyCharacter->GetActorLocation());
		}
	}

	DirectionElapsedTime = 0.0f;
	

	DirectionTickerHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRMapManager::TickDirection));
}
#endif // REFACTORING_LOAD_LEVEL

EWRMapType::en WRMapManager::GetCurrentMapType() const
{
	return FWRMapTableBase::GetMapType(GetCurrentMapTID());
}

#if !REFACTORING_LOAD_LEVEL

bool WRMapManager::IsShortFadeTime()
{
	return bStartLevel == true || MapTable->DepartureEffectToMap.IsNull() == false;
}

#endif // !REFACTORING_LOAD_LEVEL

bool WRMapManager::HasDirectForDeparture()
{
	return MapTable->DepartureEffectToMap.IsNull() == false;
}

bool WRMapManager::HasDirectForArrival()
{
	return MapTable->Arrivaleffect.IsNull() == false;
}


void WRMapManager::SetMidpointPauseWhenDirecting(const bool bInPause)
{
	bMidpointPauseDirecting = bInPause;
	UE_LOG(MapManager, Display, TEXT("WRMapManager::SetMidpointPauseWhenDirecting - %s"), bMidpointPauseDirecting == true ? TEXT("Pause") : TEXT("Unpause"));
}

void WRMapManager::CopyCurrentMapInformationToPrevious(const WRTableID IN InCurrentlyLoadedMapTID)
{
	MapInformation[PreviousMap].TID = MapInformation[CurrentMap].TID;
	MapInformation[PreviousMap].ServerMapIndex = MapInformation[CurrentMap].ServerMapIndex;
	MapInformation[CurrentMap].TID = InCurrentlyLoadedMapTID;
}

//====================================================================================

#if !REFACTORING_LOAD_LEVEL

bool WRMapManager::LoadLevel_Internal(const FString& InLevelPath)
{
	if (FPackageName::DoesPackageExist(InLevelPath) == false)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevel_Internal - Not exist Level Asset - %s"), *InLevelPath);
		return false;
	}

	// [ 2019-10-24 : magedoga ] 이벤트 바인딩
	PostWorldInitHandle	=		FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &WRMapManager::OnPostWorldInitialize);
	PostLoadWithWorldHandle	=	FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &WRMapManager::OnPostLoadMapWithWorld);

	// [ 2019-10-24 : magedoga ] 상태 초기화
	this->SetLoadLevelState(EWRLoadLevelState::None);
	this->SetLoadLevelState(EWRLoadLevelState::Transition);

	// [ 2019-10-23 : magedoga ] Transition을 위한 Fade
	UWorld* CurrentWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	bStartLevel = (CurrentWorld->IsValidLowLevel() == true && CurrentWorld->GetName().ToLower().Contains("startlevel") == true);

	if (this->IsDirectForDeparture() == true)
	{
		WRUIManager::Get()->OnDepartureEffectToMapCalled();

		FStringAssetReference DepartureEffectStringRef = FStringAssetReference(MapTable->DepartureEffectToMap.ToString());
		UObject* FoundObject = DepartureEffectStringRef.ResolveObject();
		if (FoundObject == nullptr)
		{
			UE_LOG(MapManager, Display, TEXT("WRMapManager::LoadLevel_Internal - Not found DepartureEffectToMap. Call TryLoad"));
			FoundObject = DepartureEffectStringRef.TryLoad();
		}
		this->LoadLevelDirection(Cast<UWRLoadLevelDirectionAsset>(FoundObject), FWREndLoadLevelDirectionDelegate::CreateRaw(this, &WRMapManager::OnEndDirectionWhenDepatureToNext));
	}
	else
	{
		this->FadeForLoadLevel(true, this->IsShortFadeTime() ? 0.1f : TEMP_FADETIME);
	}


	return true;
}



//====================================================================================
// [ 2019-10-21 : magedoga ] World Event

void WRMapManager::OnPostWorldInitialize(UWorld* InWorld, const UWorld::InitializationValues InIVS)
{
	checkf(LoadLevelState != nullptr, TEXT("Null LoadLevelState"));
	LoadLevelState->PostWorldInit(InWorld);
}

void WRMapManager::OnPostLoadMapWithWorld(UWorld* InWorld)
{
	checkf(LoadLevelState != nullptr, TEXT("Null LoadLevelState"));
	bReservedMapExist = false;
	CopyCurrentMapInformationToPrevious(LoadMapTID);
	LoadLevelState->PostLoadMap(InWorld);
	
}

//====================================================================================


void WRMapManager::OnCompleteScreenFadeWhenLevelLoading(const EWRScreenFadeEvent InEvent)
{
	checkf(LoadLevelState != nullptr, TEXT("Null LoadLevelState"));
	LoadLevelState->OnFadeComplete(InEvent);
}

void WRMapManager::SetLoadLevelState(EWRLoadLevelState InState)
{
	if (LoadLevelState != nullptr)
	{
		delete LoadLevelState;
		LoadLevelState = nullptr;
	}
	
	switch (InState)
	{
	case EWRLoadLevelState::Transition:
	{
		LoadLevelState = new FWRLoadLevelStateTransition(UWRGameInstance::GetGameInstance(), MapTable);
	} break;
	case EWRLoadLevelState::Destination:
	{
		LoadLevelState = new FWRLoadLevelStateDestination(UWRGameInstance::GetGameInstance(), MapTable);
	} break;
	case EWRLoadLevelState::None:
	{
		delete LoadLevelState;
		LoadLevelState = nullptr;
	} break;
	}
}


bool WRMapManager::TickDirection(float InDeltaTime)
{
	DirectionElapsedTime += InDeltaTime;

	float GridFlow = GridFlowCurve->GetFloatValue(DirectionElapsedTime);
	WRMaterialParameterCollectionManager::Get()->SetGridFlow(GridFlow);

	float SceneFlow = SceneFlowCurve->GetFloatValue(DirectionElapsedTime);
	WRMaterialParameterCollectionManager::Get()->SetSceneFlow(SceneFlow);

	float GridMinTime, GridMaxTime = 0.0f;
	GridFlowCurve->GetTimeRange(GridMinTime, GridMaxTime);
	float SceneMinTime, SceneMaxTime = 0.0f;
	SceneFlowCurve->GetTimeRange(SceneMinTime, SceneMaxTime);

	// [ 2019-11-1 : magedoga ] 연출종료
	float MaxTime = GridMaxTime > SceneMaxTime ? GridMaxTime : SceneMaxTime;
	if (MaxTime <= DirectionElapsedTime)
	{
		CurveDataRelease();
		//this->OnEndDirection();
		if (EndLoadLevelDirectionDelegate.IsBound() == true)
		{
			EndLoadLevelDirectionDelegate.Execute();
			EndLoadLevelDirectionDelegate.Unbind();
		}

		return false;
	}

	return true;
}

void WRMapManager::OnEndDirectionWhenArrival()
{
	WRMaterialParameterCollectionManager::Get()->SetEnableLoadLevelDirection(0.0f);
	this->CompleteLoadLevelProcess();
}

void WRMapManager::OnEndDirectionWhenDepatureToNext()
{
	this->FadeForLoadLevel(true, 0.1f);
}

void WRMapManager::CurveDataRelease()
{
	if (GridFlowCurve->IsValidLowLevel() == true)
	{
		GridFlowCurve->RemoveFromRoot();
		GridFlowCurve = nullptr;
	}
	if (SceneFlowCurve->IsValidLowLevel() == true)
	{
		SceneFlowCurve->RemoveFromRoot();
		SceneFlowCurve = nullptr;
	}
}


void WRMapManager::FadeForLoadLevel(const bool bInFadeOut, const float InDuration)
{
	// [ 2019-12-5 : magedoga ] 일반은 검은색, 연출은 흰색
	FLinearColor FadeColor = /*this->IsDirectForDeparture() == true ? FLinearColor::White : */FLinearColor::Black;

	FWRScreenFaderModule& ScreenFader = FModuleManager::Get().LoadModuleChecked<FWRScreenFaderModule>("WRScreenFader");
	bInFadeOut == true ? ScreenFader.ScreenFadeOut(UWRGameInstance::GetGameInstance(), InDuration, FWRDelegateCompleteScreenFade::CreateRaw(this, &WRMapManager::OnCompleteScreenFadeWhenLevelLoading), FadeColor)
		: ScreenFader.ScreenFadeIn(UWRGameInstance::GetGameInstance(), InDuration, FWRDelegateCompleteScreenFade::CreateRaw(this, &WRMapManager::OnCompleteScreenFadeWhenLevelLoading), FadeColor);
}

void WRMapManager::CompleteLoadLevelProcess()
{
	if (PostWorldInitHandle.IsValid() == true)
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(PostWorldInitHandle);
	}

	if (PostLoadWithWorldHandle.IsValid() == true)
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadWithWorldHandle);
	}

	this->SetLoadLevelState(EWRLoadLevelState::None);
	MapTable = nullptr;
	LoadMapTID = INVALID_TABLE_TID;

	WRSingletonManager::EndLoadLevelProcess();

	IMMessageSender::RequestReadyToPlay();

	this->ClearAsyncTask(AsyncTasksForMoveMap, false);
}

//====================================================================================



//====================================================================================

void WRMapManager::OnAsyncLoadCompleteForMoveMap(UObject* InObject, FString InHashKey)
{
	if (AsyncTasksForMoveMap.Contains(InHashKey) == false)
	{
		UE_LOG(MapManager, Display, TEXT("WRMapManager::OnAsyncLoadCompleteForMoveMap - Not requested hash key."));
		return;
	}

	InObject->AddToRoot();
	AsyncTasksForMoveMap[InHashKey] = InObject;

	auto AddToAsyncTask = [&](const FString& InHashKey)
	{
		if (InHashKey != WRASYNCLOADMANAGER_INVALID)
		{
			AsyncTasksForMoveMap.Add(InHashKey, nullptr);
		}
	};

	// [ 2020-4-28 : magedoga ] LoadLevelDirectionAsset이면 그 안의 Asest들 다시 Async Load 요청
	UWRLoadLevelDirectionAsset* DirectionAsset = Cast<UWRLoadLevelDirectionAsset>(InObject);
	if (DirectionAsset != nullptr)
	{
		AddToAsyncTask(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(DirectionAsset->GridFlow.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncLoadCompleteForMoveMap)));
		AddToAsyncTask(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(DirectionAsset->SceneFlow.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncLoadCompleteForMoveMap)));
		AddToAsyncTask(WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(DirectionAsset->Sound.ToString(), FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncLoadCompleteForMoveMap)));
		return;
	}
	if(WRAsyncLoadUObjectManager::IsLoadCompleteTasks(AsyncTasksForMoveMap) == true)
	{
		this->LoadLevel_Internal(MapTable->SceneLevelFile.ToString());
	}
}

void WRMapManager::ClearAsyncTask(TMap<FString, UObject*>& InContaner, bool bInCancelTask)
{
	for (auto It = InContaner.CreateIterator(); It; ++It)
	{
		UObject* Value = It.Value();
		if (Value->IsValidLowLevel() == false && bInCancelTask == true)
		{
			WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(It.Key());
		}
		else
		{
			Value->RemoveFromRoot();
		}
	}

	InContaner.Empty();
}


//====================================================================================
// [ 2020-4-29 : magedoga ] 임시 프리로드 
bool WRMapManager::StartPreLoad()
{
	auto AddToTask = [&](const FString& InFullPath)
	{
		if (FPackageName::DoesPackageExist(InFullPath) == false)
		{
			return;
		}

		FString HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InFullPath, FWRCompleteAsyncLoad::CreateRaw(this, &WRMapManager::OnAsyncLoadCompleteForPreLoad));
		if (HashKey != WRASYNCLOADMANAGER_INVALID)
		{
			AsyncTasksForPreLoad.Add(HashKey, nullptr);
		}
	};


	//====================================================================================
	// [ 2020-4-29 : magedoga ] Add PreLoad

	// [ 2020-4-29 : magedoga ] 내 캐릭터
	AddToTask(WRCharacterManager::Get()->GetMyCharacterBPPath());

	// [ 2020-4-29 : magedoga ] 퀘스트 fx
	AddToTask(WRActorSpawnUtility::ResolvePath(WRPathManager::Get()->GetPath(EWRPath::Blueprint_Effect) + EFFECT_PATH_4_QUEST_POINT));
	AddToTask(WRActorSpawnUtility::ResolvePath(WRPathManager::Get()->GetPath(EWRPath::Blueprint_Effect) + EFFECT_PATH_4_COMPLETE_QUEST));




	//====================================================================================



	if (AsyncTasksForPreLoad.Num() == 0)
	{
		FWRLoadLevelStateTransition* TransitionState = static_cast<FWRLoadLevelStateTransition*>(LoadLevelState);
		if (TransitionState != nullptr)
		{
			TransitionState->OnPreLoadComplete();
		}
		
		return false;
	}
	
	return true;
}

void WRMapManager::OnAsyncLoadCompleteForPreLoad(UObject* InObject, FString InHashKey)
{
	if (AsyncTasksForPreLoad.Contains(InHashKey) == false)
		return;

	InObject->AddToRoot();
	AsyncTasksForPreLoad[InHashKey] = InObject;

	if (WRAsyncLoadUObjectManager::IsLoadCompleteTasks(AsyncTasksForPreLoad) == true)
	{
		FWRLoadLevelStateTransition* TransitionState = static_cast<FWRLoadLevelStateTransition*>(LoadLevelState);
		if (TransitionState != nullptr)
		{
			TransitionState->OnPreLoadComplete();
		}
	}
}

#endif // !REFACTORING_LOAD_LEVEL

//====================================================================================


FString WRMapManager::MakeNextURL(const FString& InLongLevelName, EWRGameMode InGameMode)
{
	FString GameModeName = WREnumUtility::EnumToString<EWRGameMode>("EWRGameMode", InGameMode);
	FString Command = InLongLevelName + "?GAME=/Script/WR.WRGameMode" + GameModeName;

	return Command;
}