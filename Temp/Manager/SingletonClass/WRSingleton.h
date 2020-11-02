// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRSingletonObject.h"
#include "WRSingletonManager.h"

#define CDECLARE_MANAGER(ClassType)		friend class WRSingleton<ClassType>;

/**
 *
 */
template <class TManager>
class WR_API WRSingleton : public WRSingletonObject
{
protected:
	FORCEINLINE WRSingleton() {}
	FORCEINLINE virtual ~WRSingleton() {}

public:

	// [ 2019-6-17 : magedoga ] Core Delegate 연결이 필요 없을 때 사용
	static TManager* CreateInstance(const FName InName, const bool bInEnableTick = false, EWRManagerTickGroup InTickGroupType = EWRManagerTickGroup::TG_Normal)
	{
		checkf(Instance == nullptr, TEXT("%s - Already created instance."), *(InName.ToString()));

		Instance = new TManager();
		Instance->Initialize(InName, bInEnableTick, InTickGroupType);
		WRSingletonManager::Add(Instance);

		return Instance;
	}

	// [ 2019-6-17 : magedoga ] Core Delegate 연결이 필요할 때 사용
	static TManager* CreateInstance(const FName InName, FWRBindDelegateFlag InDelegateFlag, const bool bInEnableTick = false, EWRManagerTickGroup InTickGroupType = EWRManagerTickGroup::TG_Normal)
	{
		checkf(Instance == nullptr, TEXT("%s - Already created instance."), *(InName.ToString()));

		Instance = new TManager();
		Instance->Initialize(InName, InDelegateFlag, bInEnableTick, InTickGroupType);
		WRSingletonManager::Add(Instance);

		return Instance;
	}

	static TManager* Get()
	{
		checkf(Instance != nullptr, TEXT("Not created instance. You have to create this manager instance first."));
		return Instance;
	}

	static bool IsValid()
	{
		return Instance != nullptr;
	}

protected:

	virtual void OnDestroyInstance() override
	{
		checkf(Instance != nullptr, TEXT("Null this manager instance"));
		// 		WRSingleton<TManager>* BaseInstance = static_cast<WRSingleton<TManager>*>(Instance);
		// 		delete BaseInstance;
		delete Instance;
		Instance = nullptr;
	}

private:

	static TManager* Instance;

};


template <class TManager> TManager* WRSingleton<TManager>::Instance = nullptr;
