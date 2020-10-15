
#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Containers/List.h"
#include "Singleton/DMSingleton.h"

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
class THIRDPERSON_API DMAsyncLoadManager : public DMSingleton<DMAsyncLoadManager>
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
	virtual ~DMAsyncLoadManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;

	FString AsyncSpawnActor(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate);
	FString AsyncLoadAsset(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate);
	FString AsyncCreateWidget(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate);

	bool CancelAsyncLoad(const FString& InHash);

	static bool IsLoadCompleteTasks(TMap<FString, UObject*>& InTasks);

};

//====================================================================================

#define ASYNCLOADMANAGER_INVALID "Invalid"

//====================================================================================