// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRGameInstance.h"

#include "GameFramework/PlayerController.h"
#include "ConfigCacheIni.h"

#include "Manager/SingletonClass/WRSingletonManager.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"

#if WITH_EDITOR
//#include "CScreenFader.h"
#endif // WITH_EDITOR

#include <CoreDelegates.h>
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"

// For Cpp
DEFINE_LOG_CATEGORY(WRGameInstance)

UWRGameInstance*	UWRGameInstance::Instance = nullptr;

void UWRGameInstance::Init()
{
	//====================================================================================
	// [ 2019-6-17 : magedoga ] Don't edit this code
	Super::Init();
	Instance = this;
	this->BindToCoreDelegate();
	WRSingletonManager::Start();
	//====================================================================================

	//====================================================================================

	UWRActorComponentConnectbyCable::EmptyCutDirection();

	// [ 2019-6-17 : magedoga ] Create Manager 
	WRSingletonManager::CreateManagerWhen_StartGame();

}

void UWRGameInstance::Shutdown()
{

	//====================================================================================

	this->RemoveAssetLoadDetectDelegate();

#if WITH_EDITOR
	//FCScreenFaderModule::Get().ClearData();
#endif // WITH_EDITOR


	//====================================================================================
	// [ 2019-6-17 : magedoga ] Don't edit this code
	WRSingletonManager::Shutdown();
	this->UnbindFromCoreDelegate();
	Super::Shutdown();
	//====================================================================================
}


class AGameModeBase* UWRGameInstance::CreateGameModeForURL(FURL InURL, UWorld* InWorld)
{
#if !UE_BUILD_SHIPPING
	this->ApplyStatbyCustomConfig();
#endif // UE_BUILD_SHIPPING

	return Super::CreateGameModeForURL(InURL, InWorld);
}

void UWRGameInstance::ApplyStatbyCustomConfig()
{
	UGameViewportClient* GameViewPort = GetWorld()->GetGameViewport();
	if (GameViewPort == nullptr)
		return;

	if (FConfigSection* Section = GConfig->GetSectionPrivate(TEXT("CustomConsoleSetting.Stat"), false, true, *(FPaths::ProjectConfigDir() + TEXT("CustomConsoleSetting.ini"))))
	{
		for (FConfigSectionMap::TConstIterator It(*Section); It; ++It)
		{
			FString KeyString = It.Key().GetPlainNameString();
			const FString& ValueString = It.Value().GetValue();

			if (GameViewPort->IsStatEnabled(ValueString) == false)
			{
				GameViewPort->ConsoleCommand(KeyString + " " + ValueString);
			}
		}
	}
}

//====================================================================================
// [ 2019-6-13 : magedoga ] Core Delegate

void UWRGameInstance::BindToCoreDelegate()
{
	HandleDeactivate = FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &UWRGameInstance::OnApplicationDeactivateDelegate);
	HandleReactivate = FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &UWRGameInstance::OnApplicationReactivateDelegate);
	HandleEnterBackground = FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UWRGameInstance::OnApplicationEnterBackgroundDelegate);
	HandleEnterForeground = FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UWRGameInstance::OnApplicationEnterForegroundDelegate);
	HandleTerminate = FCoreDelegates::ApplicationWillTerminateDelegate.AddUObject(this, &UWRGameInstance::OnApplicationTerminateDelegate);
	HandleScreenOrientationChange = FCoreDelegates::ApplicationReceivedScreenOrientationChangedNotificationDelegate.AddUObject(this, &UWRGameInstance::OnApplicationScreenOrientationChangeDelegate);
}

void UWRGameInstance::UnbindFromCoreDelegate()
{
	FCoreDelegates::ApplicationWillDeactivateDelegate.Remove(HandleDeactivate);
	FCoreDelegates::ApplicationHasReactivatedDelegate.Remove(HandleReactivate);
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.Remove(HandleEnterBackground);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Remove(HandleEnterForeground);
	FCoreDelegates::ApplicationWillTerminateDelegate.Remove(HandleTerminate);
	FCoreDelegates::ApplicationReceivedScreenOrientationChangedNotificationDelegate.Remove(HandleScreenOrientationChange);
}

//====================================================================================

void UWRGameInstance::OnApplicationDeactivateDelegate()
{
	ApplicationDeactivateDelegate.Broadcast();
}

void UWRGameInstance::OnApplicationReactivateDelegate()
{
	ApplicationReactivatedDelegate.Broadcast();
}

void UWRGameInstance::OnApplicationEnterBackgroundDelegate()
{
	ApplicationEnterBackgroundDelegate.Broadcast();
}

void UWRGameInstance::OnApplicationEnterForegroundDelegate()
{
	ApplicationEnterForegroundDelegate.Broadcast();
}

void UWRGameInstance::OnApplicationTerminateDelegate()
{
	ApplicationTerminateDelegate.Broadcast();
}

void UWRGameInstance::OnApplicationScreenOrientationChangeDelegate(int32 InScreenOrientation)
{
	ApplicationReceivedScreenOrientationChangedNotificationDelegate.Broadcast((EScreenOrientation::Type)InScreenOrientation);
}

//====================================================================================


void UWRGameInstance::SetEnableCollectSyncLoadInfo(const bool bInEnable)
{
	if (bInEnable == true)
	{
		if (SyncLoadPackageHandle.IsValid() == false)
		{
			//SyncLoadPackageHandle = FCoreDelegates::OnSyncLoadPackage.AddUFunction(this, "OnSyncLoadPackage");
			SyncLoadPackageHandle = FCoreDelegates::OnSyncLoadPackage.AddUObject(this, &UWRGameInstance::OnSyncLoadPackage);
		}

		if (AsyncLoadingFlushHandle.IsValid() == false)
		{
			//AsyncLoadingFlushHandle = FCoreDelegates::OnAsyncLoadingFlush.AddUFunction(this, "OnAsyncLoadingFlush");
			AsyncLoadingFlushHandle = FCoreDelegates::OnAsyncLoadingFlush.AddUObject(this, &UWRGameInstance::OnAsyncLoadingFlush);
		}

		return;
	}

	this->RemoveAssetLoadDetectDelegate();
}

void UWRGameInstance::OnSyncLoadPackage(const FString& InPackageName)
{
	if (InPackageName.Contains("Script") == true)
		return;

	//UE_LOG(WRGameInstance, Display, TEXT("UWRGameInstance::OnSyncLoadPackage - Add Package [ %s ]"), *InPackageName);
	if(RecentlyLoadedAsset.IsEmpty() == false)
		RecentlyLoadedAsset.Pop();

	RecentlyLoadedAsset.Enqueue(InPackageName);
}

void UWRGameInstance::OnAsyncLoadingFlush()
{
	FString AssetName;
	if (RecentlyLoadedAsset.Peek(AssetName) == true)
	{
		UE_LOG(WRGameInstance, Display, TEXT("UWRGameInstance::OnAsyncLoadingFlush - %s"), *AssetName);
	}
}

void UWRGameInstance::RemoveAssetLoadDetectDelegate()
{
	if (SyncLoadPackageHandle.IsValid() == true)
	{
		FCoreDelegates::OnSyncLoadPackage.Remove(SyncLoadPackageHandle);
		SyncLoadPackageHandle.Reset();
	}

	if (AsyncLoadingFlushHandle.IsValid() == true)
	{
		FCoreDelegates::OnAsyncLoadingFlush.Remove(AsyncLoadingFlushHandle);
		AsyncLoadingFlushHandle.Reset();
	}

	RecentlyLoadedAsset.Empty();
}
