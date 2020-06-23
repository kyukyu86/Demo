
#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Containers/List.h"
#include "Base/DMBaseManager.h"

DECLARE_DELEGATE_TwoParams(FDMCompleteAsyncLoad, UObject*, FString);

UENUM()
enum class EDMAsyncLoadObjectType : uint8
{
	Actor,
	Asset,
	Widget,
};

struct FDMAsyncLoadTaskParam
{
public:

	FDMAsyncLoadTaskParam() {}

	FDMAsyncLoadTaskParam(TSharedPtr<FStreamableHandle> InStreamHandle, const FString& InPath, const FString& InHash, EDMAsyncLoadObjectType InObjectType, FDMCompleteAsyncLoad InCallback)
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
	EDMAsyncLoadObjectType ObjectType;
	FDMCompleteAsyncLoad CompleteCallback;

	static FString GetHashKey(const FString& InHash);

	void LoadComplete();
};


/**
 *
 */
class THIRDPERSON_API DMAsyncLoadManager : public DMBaseManager<DMAsyncLoadManager>
{
private:
	TMap<FString, TDoubleLinkedList<FDMAsyncLoadTaskParam*>*>	Tasks;

private:
	TSharedPtr<FStreamableHandle> RequestAsyncLoad(const FString& InFullPath);
	FString PathToHash(const FString& InValue);
	void OnCompleteAsyncLoad();
	FDMAsyncLoadTaskParam* AddAsyncLoadTaskParam(TSharedRef<FStreamableHandle> InHandle, const FString& InFullPath, EDMAsyncLoadObjectType InObjectType, FDMCompleteAsyncLoad InCallback);
	void RemoveAsyncLoadTaskParam(FDMAsyncLoadTaskParam* InParam);
	void RemoveAll();

public:
	DMAsyncLoadManager();
	~DMAsyncLoadManager();

// 	virtual void OnInitialize()			override;
// 	virtual void OnShutdown()			override;
// 	virtual void OnLoadLevelStart()		override;
// 	virtual void OnLoadLevelComplete()	override;
// 	virtual void OnRenderStart()		override;

	FString AsyncSpawnActor(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate);
	FString AsyncLoadAsset(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate);
	FString AsyncCreateWidget(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate);

	bool CancelAsyncLoad(const FString& InHash);

	static bool IsLoadCompleteTasks(TMap<FString, UObject*>& InTasks);

};

//====================================================================================

#define WRASYNCLOADMANAGER_INVALID "Invalid"

//====================================================================================