// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <Kismet/BlueprintPlatformLibrary.h>
#include <DelegateCombinations.h>

#include "WRGameInstance.generated.h"

/**
 *
 */
UCLASS()
class WR_API UWRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init();
	virtual void Shutdown();

	static UWRGameInstance* GetGameInstance() { return Instance; }

	virtual class AGameModeBase* CreateGameModeForURL(FURL InURL, UWorld* InWorld) override;
	void ApplyStatbyCustomConfig();



	//====================================================================================
	// [ 2019-6-13 : magedoga ] Core Delegate
private:

	void BindToCoreDelegate();
	void UnbindFromCoreDelegate();

public:

	DECLARE_MULTICAST_DELEGATE(FCCoreDelegate);
	DECLARE_MULTICAST_DELEGATE_OneParam(FCCoreScreenOrientationChangedDelegate, EScreenOrientation::Type);
	DECLARE_MULTICAST_DELEGATE_OneParam(FCCorePostLoadMapDelegate, UWorld*);

	// [ 2019-6-13 : magedoga ] 추후에 필요하면 적용
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCCoreStartupArgumentsDelegate, const TArray<FString>&, StartupArguments);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCCoreRegisteredForRemoteNotificationsDelegate, const TArray<uint8>&, inArray);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCCoreRegisteredForUserNotificationsDelegate, int32, inInt);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCCoreFailedToRegisterForRemoteNotificationsDelegate, FString, inString);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCCoreReceivedRemoteNotificationDelegate, FString, inString, EApplicationState::Type, inAppState);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCCoreReceivedLocalNotificationDelegate, FString, inString, int32, inInt, EApplicationState::Type, inAppState);


	// This is called when the application is about to be deactivated (e.g., due to a phone call or SMS or the sleep button). 
	// The game should be paused if possible, etc... 
	FCCoreDelegate ApplicationDeactivateDelegate;

	// Called when the application has been reactivated (reverse any processing done in the Deactivate delegate)
	FCCoreDelegate ApplicationReactivatedDelegate;

	// This is called when the application is being backgrounded (e.g., due to switching  
	// to another app or closing it via the home button)  
	// The game should release shared resources, save state, etc..., since it can be  
	// terminated from the background state without any further warning.  
	FCCoreDelegate ApplicationEnterBackgroundDelegate; // for instance, hitting the home button

	// Called when the application is returning to the foreground (reverse any processing done in the EnterBackground delegate)
	FCCoreDelegate ApplicationEnterForegroundDelegate;

	// This *may* be called when the application is getting terminated by the OS.  
	// There is no guarantee that this will ever be called on a mobile device,  
	// save state when ApplicationWillEnterBackgroundDelegate is called instead.  
	FCCoreDelegate ApplicationTerminateDelegate;

	// called when the application receives a screen orientation change notification
	FCCoreScreenOrientationChangedDelegate ApplicationReceivedScreenOrientationChangedNotificationDelegate;

	// called when loaded next world begin play
	FCCorePostLoadMapDelegate PostLoadMapWithWorldDelegate;


	// [ 2019-6-14 : magedoga ] Delegate Handle
	FDelegateHandle		HandleDeactivate;
	FDelegateHandle		HandleReactivate;
	FDelegateHandle		HandleEnterBackground;
	FDelegateHandle		HandleEnterForeground;
	FDelegateHandle		HandleTerminate;
	FDelegateHandle		HandleScreenOrientationChange;


	void OnApplicationDeactivateDelegate();
	void OnApplicationReactivateDelegate();
	void OnApplicationEnterBackgroundDelegate();
	void OnApplicationEnterForegroundDelegate();
	void OnApplicationTerminateDelegate();
	void OnApplicationScreenOrientationChangeDelegate(int32 InScreenOrientation);
	//====================================================================================

	void SetEnableCollectSyncLoadInfo(const bool bInEnable);

private:

	UFUNCTION()
	void OnSyncLoadPackage(const FString& InPackageName);

	UFUNCTION()
	void OnAsyncLoadingFlush();

	void RemoveAssetLoadDetectDelegate();

private:

	static UWRGameInstance*	Instance;

	TQueue<FString>	RecentlyLoadedAsset;

	FDelegateHandle SyncLoadPackageHandle;
	FDelegateHandle AsyncLoadingFlushHandle;
};


// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRGameInstance, Display, All);


//====================================================================================
// [ 2019-6-17 : magedoga ] Bind
#define BIND_DELEGATE(DelegateName, UseFuncName, UserObject, FuncPtr)	\
	UWRGameInstance::GetGameInstance()->DelegateName.UseFuncName(UserObject,FuncPtr)

// [ 2019-6-17 : magedoga ] Deactivate
#define BIND_DELEGATE_DEACTIVATE_RAW(Object, FuncPtr)							BIND_DELEGATE(ApplicationDeactivateDelegate, AddRaw, Object, FuncPtr)
#define BIND_DELEGATE_DEACTIVATE_UOBJECT(Object, FuncPtr)						BIND_DELEGATE(ApplicationDeactivateDelegate, AddUObject, Object, FuncPtr)

// [ 2019-6-17 : magedoga ] Reactivate
#define BIND_DELEGATE_REACTIVATE_RAW(Object, FuncPtr)							BIND_DELEGATE(ApplicationReactivatedDelegate, AddRaw, Object, FuncPtr)
#define BIND_DELEGATE_REACTIVATE_UOBJECT(Object, FuncPtr)						BIND_DELEGATE(ApplicationReactivatedDelegate, AddUObject, Object, FuncPtr)

// [ 2019-6-17 : magedoga ] Enter bacground
#define BIND_DELEGATE_ENTER_BACKGROUND_RAW(Object, FuncPtr)						BIND_DELEGATE(ApplicationEnterBackgroundDelegate, AddRaw, Object, FuncPtr)
#define BIND_DELEGATE_ENTER_BACKGROUND_UOBJECT(Object, FuncPtr)					BIND_DELEGATE(ApplicationEnterBackgroundDelegate, AddUObject, Object, FuncPtr)

// [ 2019-6-17 : magedoga ] Enter foreground
#define BIND_DELEGATE_ENTER_FOREGROUND_RAW(Object, FuncPtr)						BIND_DELEGATE(ApplicationEnterForegroundDelegate, AddRaw, Object, FuncPtr)
#define BIND_DELEGATE_ENTER_FOREGROUND_UOBJECT(Object, FuncPtr)					BIND_DELEGATE(ApplicationEnterForegroundDelegate, AddUObject, Object, FuncPtr)

// [ 2019-6-17 : magedoga ] Terminate
#define BIND_DELEGATE_TERMINATEAPP_RAW(Object, FuncPtr)							BIND_DELEGATE(ApplicationTerminateDelegate, AddRaw, Object, FuncPtr)
#define BIND_DELEGATE_TERMINATEAPP_UOBJECT(Object, FuncPtr)						BIND_DELEGATE(ApplicationTerminateDelegate, AddUObject, Object, FuncPtr)

// [ 2019-6-17 : magedoga ] Changed Screen Orientation
#define BIND_DELEGATE_SCREEN_ORIENTATION_CHANGED_RAW(Object, FuncPtr)			BIND_DELEGATE(ApplicationReceivedScreenOrientationChangedNotificationDelegate, AddRaw, Object, FuncPtr)
#define BIND_DELEGATE_SCREEN_ORIENTATION_CHANGED_UOBJECT(Object, FuncPtr)		BIND_DELEGATE(ApplicationReceivedScreenOrientationChangedNotificationDelegate, AddUObject, Object, FuncPtr)



//====================================================================================
// [ 2019-6-17 : magedoga ] Remove
#define REMOVE_BINDED_DELEGATE(DelegateName, Handle) \
	UWRGameInstance::GetGameInstance()->DelegateName.Remove(Handle);

#define REMOVE_FROM_DEACTIVATE(Handle)						REMOVE_BINDED_DELEGATE(ApplicationDeactivateDelegate, Handle)
#define REMOVE_FROM_REACTIVATE(Handle)						REMOVE_BINDED_DELEGATE(ApplicationReactivatedDelegate, Handle)
#define REMOVE_FROM_ENTER_BACKFROUND(Handle)				REMOVE_BINDED_DELEGATE(ApplicationEnterBackgroundDelegate, Handle)
#define REMOVE_FROM_ENTER_FOREGROUND(Handle)				REMOVE_BINDED_DELEGATE(ApplicationEnterForegroundDelegate, Handle)
#define REMOVE_FROM_TERMINATEAPP(Handle)					REMOVE_BINDED_DELEGATE(ApplicationDeactivateDelegate, Handle)
#define REMOVE_FROM_SCREEN_ORIENTATION_CHANGED(Handle)		REMOVE_BINDED_DELEGATE(ApplicationReceivedScreenOrientationChangedNotificationDelegate, Handle)