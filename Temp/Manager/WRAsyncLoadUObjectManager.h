// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"
#include "Engine/StreamableManager.h"
#include "List.h"

//DECLARE_DELEGATE_TwoParams(FWRCompleteAsyncSpawnActor, AActor*, FString);
DECLARE_DELEGATE_TwoParams(FWRCompleteAsyncLoad, UObject*, FString);

UENUM()
enum class EWRAsyncLoadObjectType : uint8
{
	Actor,
	Asset,
	Widget,
};

struct FWRAsyncLoadTaskParam
{
public:

	FWRAsyncLoadTaskParam() {}

	FWRAsyncLoadTaskParam(TSharedPtr<FStreamableHandle> InStreamHandle, const FString& InPath, const FString& InHash, EWRAsyncLoadObjectType InObjectType, FWRCompleteAsyncLoad InCallback)
		: StreamableHandle(InStreamHandle)
		, Path(InPath)
		, Hash(InHash)
		, ObjectType(InObjectType)
		, CompleteCallback(InCallback)
	{
		Hash += TEXT("_") + FString::FromInt(IncrementIndex++);
	}

	static uint64 IncrementIndex;

	TSharedPtr<FStreamableHandle>	StreamableHandle;
	FString Path;
	FString Hash;
	EWRAsyncLoadObjectType ObjectType;
	FWRCompleteAsyncLoad CompleteCallback;

	static FString GetHashKey(const FString& InHash);

	void LoadComplete();
	
};


/**
 * 
 */
class WR_API WRAsyncLoadUObjectManager : public WRSingleton<WRAsyncLoadUObjectManager>
{
	CDECLARE_MANAGER(WRAsyncLoadUObjectManager)


private:
	WRAsyncLoadUObjectManager();
	virtual ~WRAsyncLoadUObjectManager();

public:

	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() override;
	//====================================================================================

	FString AsyncSpawnActor(const FString& InFullPath, FWRCompleteAsyncLoad InCompleteDelegate);
	FString AsyncLoadAsset(const FString& InFullPath, FWRCompleteAsyncLoad InCompleteDelegate);
	FString AsyncCreateWidget(const FString& InFullPath, FWRCompleteAsyncLoad InCompleteDelegate);

	bool CancelAsyncLoad(const FString& InHash);

	static bool IsLoadCompleteTasks(TMap<FString, UObject*>& InTasks);
	bool IsEmptyTask() const { return Tasks.Num() == 0; }

private:

	TSharedPtr<FStreamableHandle> RequestAsyncLoad(const FString& InFullPath);

	FString PathToHash(const FString& InValue);

	void OnCompleteAsyncLoad();

	FWRAsyncLoadTaskParam* AddAsyncLoadTaskParam(TSharedRef<FStreamableHandle> InHandle, const FString& InFullPath, EWRAsyncLoadObjectType InObjectType, FWRCompleteAsyncLoad InCallback);
	void RemoveAsyncLoadTaskParam(FWRAsyncLoadTaskParam* InParam);
	void RemoveAll();

private:

	TMap<FString, TDoubleLinkedList<FWRAsyncLoadTaskParam*>*>	Tasks;

};

//====================================================================================

DECLARE_LOG_CATEGORY_EXTERN(AsyncLoadUObjectManager, Display, All);

#define WRASYNCLOADMANAGER_INVALID "Invalid"

//====================================================================================