// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRSingletonObject.h"

class WRSingletonManager
{
public:

	static void Start();

	static void Shutdown();


	static void LoadLevelStart();

	static void LoadLevelComplete();

	static void RenderStart();

	static void EndLoadLevelProcess();


	static void Add(class WRSingletonObject* InSingleton);

	static bool Tick(float InDeltaTime);


	// [ 2019-6-18 : magedoga ] Create Manager
	static void CreateManagerWhen_StartGame();

private:

	static void DestroySingletonInstance();


private:

	static TDoubleLinkedList<class WRSingletonObject*> SingletonList;

	//static TMap<EWRManagerTickGroup, class WRSingletonObject*> SingletonsForTick;

	static FDelegateHandle TickHandle;

};

// For Header
DECLARE_STATS_GROUP(TEXT("WRSingletonManager"), STATGROUP_WRSingletonManager, STATCAT_Advanced);