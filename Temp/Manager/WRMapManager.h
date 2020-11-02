// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"
#include "../../../Plugins/WRScreenFader/Source/WRScreenFader/Public/WRScreenFader.h"
#include "Table/WRMapTable.h"
#include "Engine/World.h"
#include "Curves/CurveFloat.h"
#include "Network/Share/Define/Define_Type.h"
#include "Define/WRTypeDefine.h"

//====================================================================================

#define REFACTORING_LOAD_LEVEL 1

#if REFACTORING_LOAD_LEVEL
#include "WRLevelLoader.h"
#endif // REFACTORING_LOAD_LEVEL

#define DEF_START_MAP_TID 3
UENUM(BlueprintType)
enum class EWRLoadLevelState : uint8
{
	None,
	Transition,
	Destination,
};

//====================================================================================

DECLARE_DELEGATE(FWREndLoadLevelDirectionDelegate);
DECLARE_MULTICAST_DELEGATE(FWRMapMovementComplete)

class WR_API WRMapManager : public WRSingleton<WRMapManager>
{
	CDECLARE_MANAGER(WRMapManager);

#if !REFACTORING_LOAD_LEVEL
	friend struct FWRLoadLevelStateTransition;
	friend struct FWRLoadLevelStateDestination;
#endif // !REFACTORING_LOAD_LEVEL

	enum EWRMapInformationType
	{
		PreviousMap,
		CurrentMap,
		ReservedMap,
		MAX_MAP_INFORMATION_TYPE_COUNT,
	};

	struct FMapInformation
	{
		WRTableID TID = INVALID_TID;
		MAP_INDEX_t ServerMapIndex = INVALID_MAP_INDEX;
	};
	
private:
	WRMapManager();
	virtual ~WRMapManager();

public:
	//virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================
	// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override; 
	//====================================================================================

	virtual void EndLoadLevelProcess() override;

	FWRMapMovementComplete	MapMovementCompleteDelegate;

private:

	// [ 2020-9-4 : magedoga ] ����, �ε巹�� ���� ���� ����
	void OnMapMovementComplete();

	//====================================================================================
	// [ 2020-6-23 : magedoga ] Seamless
public:
	bool LoadLevelByTable(const WRTableID InMapTID);
	bool IsLoading() const { return LevelLoader->IsSeamlessLoading(); }
	bool IsPassedTransition() const { return LevelLoader->IsPassedTransition(); }
	bool HasDirectForDeparture();
	bool HasDirectForArrival();
	
	void SetMidpointPauseWhenDirecting(const bool bInPause);

private:
	bool StartSeamlessTravel(const FString& InURL);
	void StartDirection(const FString& InAssetPath, FSimpleDelegate InCompleteDelegate);
	bool StartPreLoad();

	void OnSeamlessLoadingEventReceived(EWRSeamlessLevelLoadEventType InType);
	void OnSeamlessTravelStarted();
	void OnSeamlessTravelEnded();
	void OnDirectAssetLoaded(UObject* InObject, FString InHashKey);
	void OnDepartureDirectStarted();
	void OnDepartureDirectEnded();
	void OnArrivalDirectEnded();
	void OnAsyncPreLoadComplete(UObject* InObject, FString InHashKey);
	void OnPreLoadComplete();

	void AddAsyncLoadHashKey(const FString& InHashKey, TMap<FString, UObject*>& InMap);
	void ClearAsyncTask(TMap<FString, UObject*>& InContaner, bool bInCancelTask);

	void ReleaseDirectData();

	bool TickDirect(float InDeltaTime);


private:
	FWRLevelLoaderModule* LevelLoader = nullptr;
	FWRMapTableBase* MapTable = nullptr;
	WRTableID LoadMapTID = INVALID_TABLE_TID;
	FString NextURL;
	FDelegateHandle TickDirectHandle;

	// [ 2020-4-28 : magedoga ] for async load
	TMap<FString, UObject*>	AsyncTaskForDirect;
	TMap<FString, UObject*>	AsyncTasksForPreLoad;

	// [ 2019-11-1 : magedoga ] Direction Data
	UCurveFloat*	GridFlowCurve = nullptr;
	UCurveFloat*	SceneFlowCurve = nullptr;
	FSimpleDelegate EndDirectDelegate;

	float DirectElapsedTime = 0.0f;
	bool bMidpointPauseDirecting = false;
	bool bProcessLoadLevelEndedFunction = false;
	//====================================================================================




	//====================================================================================
	// [ 2020-6-23 : magedoga ] Stream
	// World Composition�� �����ϸ� �ұԸ� �ʻ��� ������. �׷��� World Composition�� �ƴ� �ʿ��� SubLevel�� Streaming �� ��Ȳ�� �����
	// ���Ŀ� �ʿ��ϸ� ����.
public:

private:

private:

	//====================================================================================




	//====================================================================================
	// [ 2020-6-23 : magedoga ] Teleport
public:
	bool TeleportInSideWorldComposition(const FVector& InLocation, FSimpleDelegate InCompleteDelegate);
	bool IsTeleporting() const { return LevelLoader->IsTeleporting(); }
	bool IsPossibleTeleport(UObject* InWorldContextObject, const FVector& InLocation) const;

private:
	void OnTeleportComplete();

private:
	// [ 2020-6-23 : magedoga ] Teleport
	FSimpleDelegate	TeleportCompleteDelegate;

	//====================================================================================
	


	//====================================================================================
	// [ 2020-6-23 : magedoga ] Common
public:
	bool			IsMapReserved() const { return bReservedMapExist ; }
	void			SetReservedMapTID(const WRTableID IN InReservedMapTID) { MapInformation[ReservedMap].TID = InReservedMapTID; bReservedMapExist = true; }
	void			SetReservedMapIndex(const MAP_INDEX_t IN InReservedMapIndex) { MapInformation[ReservedMap].ServerMapIndex = InReservedMapIndex; }
	void			SetCurrentMapIndex(const MAP_INDEX_t IN InCurrentMapIndex) { MapInformation[CurrentMap].ServerMapIndex = InCurrentMapIndex; }
	WRTableID		GetReservedMapTID() const { return MapInformation[ReservedMap].TID; }
	MAP_INDEX_t		GetReservedMapIndex() const { return MapInformation[ReservedMap].ServerMapIndex; }
	WRTableID		GetCurrentMapTID() const { return MapInformation[CurrentMap].TID; }
	MAP_INDEX_t		GetCurrentMapIndex() const { return MapInformation[CurrentMap].ServerMapIndex; }
	EWRMapType::en	GetCurrentMapType() const;
	WRTableID		GetPreviousMapTD() const { return MapInformation[PreviousMap].TID; }
	MAP_INDEX_t		GetPreviousMapIndex() const { return MapInformation[PreviousMap].TID; }

	static FString MakeNextURL(const FString& InLongLevelName, EWRGameMode InGameMode);
	
private:
	void CopyCurrentMapInformationToPrevious(const WRTableID IN InCurrentlyLoadedMapTID);

private:
	FMapInformation MapInformation[EWRMapInformationType::MAX_MAP_INFORMATION_TYPE_COUNT];
	bool bReservedMapExist = false;

	//====================================================================================

};


// For Header
DECLARE_LOG_CATEGORY_EXTERN(MapManager, Display, All)	