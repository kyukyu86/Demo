// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAsyncLoadUObjectManager.h"
#include "SecureHash.h"
#include "Engine/AssetManager.h"
#include "Core/WRGameInstance.h"
#include "Utility/WRActorSpawnUtility.h"
#include "UserWidget.h"

DEFINE_LOG_CATEGORY(AsyncLoadUObjectManager)


//====================================================================================

uint64 FWRAsyncLoadTaskParam::IncrementIndex = 0;

FString FWRAsyncLoadTaskParam::GetHashKey(const FString& InHash)
{
	FString _Hash = InHash;
	int32 FoundIndex;
	if (_Hash.FindLastChar('_', FoundIndex) == false)
	{
		return WRASYNCLOADMANAGER_INVALID;
	}

	int32 HashLen = _Hash.Len();
	FString Result = _Hash.Left(FoundIndex);
	return Result;
}

void FWRAsyncLoadTaskParam::LoadComplete()
{
	FStringAssetReference LoadedObjectStringRef = FStringAssetReference(Path);
	UObject* LoadedObject = LoadedObjectStringRef.ResolveObject();
	if (LoadedObject == nullptr)
	{
		UE_LOG(AsyncLoadUObjectManager, Display, TEXT("FWRAsyncLoadTaskParam::LoadComplete - return null value from ResolveObject [%s]"), *Path);
		UE_LOG(AsyncLoadUObjectManager, Display, TEXT("FWRAsyncLoadTaskParam::LoadComplete - return null value from ResolveObject. Call TryLoad."), *Path);
		LoadedObject = LoadedObjectStringRef.TryLoad();
	}

	switch (ObjectType)
	{

	case EWRAsyncLoadObjectType::Actor:
	{
		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
		AActor* SpawnedActor = World->SpawnActor((UClass*)LoadedObject, nullptr, FActorSpawnParameters());
		if (SpawnedActor == nullptr)
		{
			UE_LOG(AsyncLoadUObjectManager, Display, TEXT("FWRAsyncLoadTaskParam::LoadComplete - Null Spawned Actor."));
			return;
		}

		CompleteCallback.ExecuteIfBound(SpawnedActor, Hash);

	} break;
	case EWRAsyncLoadObjectType::Asset:
	{

		CompleteCallback.ExecuteIfBound(LoadedObject, Hash);

	} break;
	case EWRAsyncLoadObjectType::Widget:
	{
		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
		UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(World, (UClass*)LoadedObject);
		if (CreatedWidget == nullptr)
		{
			UE_LOG(AsyncLoadUObjectManager, Display, TEXT("FWRAsyncLoadTaskParam::LoadComplete - Null Created Widget."));
			return;
		}

		CompleteCallback.ExecuteIfBound(CreatedWidget, Hash);

	} break;

	}
}

//====================================================================================



//====================================================================================

WRAsyncLoadUObjectManager::WRAsyncLoadUObjectManager()
{

}

WRAsyncLoadUObjectManager::~WRAsyncLoadUObjectManager()
{

}

void WRAsyncLoadUObjectManager::OnInitialize()
{
	FWRAsyncLoadTaskParam::IncrementIndex = 0;
}

void WRAsyncLoadUObjectManager::OnShutdown()
{
	this->RemoveAll();
}

void WRAsyncLoadUObjectManager::OnLoadLevelStart()
{
	this->RemoveAll();
}

void WRAsyncLoadUObjectManager::OnLoadLevelComplete()
{

}

void WRAsyncLoadUObjectManager::OnRenderStart()
{

}

//====================================================================================



//====================================================================================

FString WRAsyncLoadUObjectManager::AsyncSpawnActor(const FString& InFullPath, FWRCompleteAsyncLoad InCompleteDelegate)
{
	FString ResolvedFullPath = WRActorSpawnUtility::ResolvePath(InFullPath);
	TSharedPtr<FStreamableHandle> HandleSharedPtr = this->RequestAsyncLoad(ResolvedFullPath);
	if (HandleSharedPtr.IsValid() == false)
	{
		return WRASYNCLOADMANAGER_INVALID;
	}

	TSharedRef<FStreamableHandle> HandleSharedRef = HandleSharedPtr.ToSharedRef();
	if (HandleSharedRef->IsActive() == false)
	{
		return WRASYNCLOADMANAGER_INVALID;
	}

	return this->AddAsyncLoadTaskParam(HandleSharedRef, ResolvedFullPath, EWRAsyncLoadObjectType::Actor, InCompleteDelegate)->Hash;
}

FString WRAsyncLoadUObjectManager::AsyncLoadAsset(const FString& InFullPath, FWRCompleteAsyncLoad InCompleteDelegate)
{
	TSharedPtr<FStreamableHandle> HandleSharedPtr = this->RequestAsyncLoad(InFullPath);
	if (HandleSharedPtr.IsValid() == false)
	{
		return WRASYNCLOADMANAGER_INVALID;
	}

	TSharedRef<FStreamableHandle> HandleSharedRef = HandleSharedPtr.ToSharedRef();
	if (HandleSharedRef->IsActive() == false)
	{
		return WRASYNCLOADMANAGER_INVALID;
	}

	return this->AddAsyncLoadTaskParam(HandleSharedRef, InFullPath, EWRAsyncLoadObjectType::Asset, InCompleteDelegate)->Hash;
}

FString WRAsyncLoadUObjectManager::AsyncCreateWidget(const FString& InFullPath, FWRCompleteAsyncLoad InCompleteDelegate)
{
	TSharedPtr<FStreamableHandle> HandleSharedPtr = this->RequestAsyncLoad(InFullPath);
	if (HandleSharedPtr.IsValid() == false)
	{
		return WRASYNCLOADMANAGER_INVALID;
	}

	TSharedRef<FStreamableHandle> HandleSharedRef = HandleSharedPtr.ToSharedRef();
	if (HandleSharedRef->IsActive() == false)
	{
		return WRASYNCLOADMANAGER_INVALID;
	}

	return this->AddAsyncLoadTaskParam(HandleSharedRef, InFullPath, EWRAsyncLoadObjectType::Widget, InCompleteDelegate)->Hash;
}

bool WRAsyncLoadUObjectManager::CancelAsyncLoad(const FString& InHash)
{
	FString HashKey = FWRAsyncLoadTaskParam::GetHashKey(InHash);
	if (Tasks.Contains(HashKey) == false)
		return false;

	FWRAsyncLoadTaskParam* CanceledTask = nullptr;

	auto& List = Tasks[HashKey];
	for (auto& Node : *List)
	{
		if (Node->Hash == InHash)
		{
			Node->StreamableHandle->CancelHandle();
			CanceledTask = Node;
		}
	}

	if (CanceledTask != nullptr)
	{
		this->RemoveAsyncLoadTaskParam(CanceledTask);
	}

	return true;
}

bool WRAsyncLoadUObjectManager::IsLoadCompleteTasks(TMap<FString, UObject*>& InTasks)
{
	if (InTasks.Num() == 0)
		return true;

	bool bLoadComplete = true;
	for (auto It = InTasks.CreateIterator(); It; ++It)
	{
		UObject* Object = It.Value();
		if (Object == nullptr)
		{
			bLoadComplete = false;
			break;
		}
	}

	return bLoadComplete;
}

//====================================================================================



//====================================================================================

TSharedPtr<FStreamableHandle> WRAsyncLoadUObjectManager::RequestAsyncLoad(const FString& InFullPath)
{
	if (FPackageName::DoesPackageExist(InFullPath) == false)
	{
		FString MSG = FString::Format(TEXT("Does Not Package Exist {0}"), { *InFullPath });
		UE_LOG(AsyncLoadUObjectManager, Display, TEXT("WRAsyncLoadUObjectManager::RequestAsyncLoad - %s"), *MSG);
		return nullptr;
	}

	auto& StreamableManager = UAssetManager::GetStreamableManager();
	return StreamableManager.RequestAsyncLoad(FSoftObjectPath(InFullPath), FStreamableDelegate::CreateRaw(this, &WRAsyncLoadUObjectManager::OnCompleteAsyncLoad));
}

//====================================================================================



//====================================================================================

FString WRAsyncLoadUObjectManager::PathToHash(const FString& InValue)
{
	return FMD5::HashAnsiString(*InValue);
}

//====================================================================================



//====================================================================================

void WRAsyncLoadUObjectManager::OnCompleteAsyncLoad()
{
	//TDoubleLinkedList<FWRAsyncLoadTaskParam*>	ProcessedTask;
	FWRAsyncLoadTaskParam* ProcessedTask = nullptr;

	for (auto It = Tasks.CreateIterator(); It; ++It)
	{
		auto& List = It.Value();
		for (auto& Node : *List)
		{
			if (Node->StreamableHandle->HasLoadCompleted() == true)
			{
				Node->LoadComplete();
				ProcessedTask = Node;
				break;
				//ProcessedTask.AddTail(Node);

			}
		}

		if (ProcessedTask != nullptr)
			break;
	}

	//if (ProcessedTask.Num() == 0)
	if(ProcessedTask == nullptr)
	{
		UE_LOG(AsyncLoadUObjectManager, Display, TEXT("WRAsyncLoadUObjectManager::OnCompleteAsyncLoad - Not found Completed Task."));
		return;
	}


// 	for (auto& Node : ProcessedTask)
// 	{
// 		this->RemoveAsyncLoadTaskParam(Node);
// 	}

	this->RemoveAsyncLoadTaskParam(ProcessedTask);
}

//====================================================================================



//====================================================================================

FWRAsyncLoadTaskParam* WRAsyncLoadUObjectManager::AddAsyncLoadTaskParam(TSharedRef<FStreamableHandle> InHandle, const FString& InFullPath, EWRAsyncLoadObjectType InObjectType, FWRCompleteAsyncLoad InCallback)
{
	FString PathToHash = this->PathToHash(InFullPath);
	FWRAsyncLoadTaskParam* AsyncLoadTaskParam = new FWRAsyncLoadTaskParam(InHandle, InFullPath, PathToHash, InObjectType, InCallback);
	if (Tasks.Contains(PathToHash) == false)
	{
		Tasks.Add(PathToHash, new TDoubleLinkedList<FWRAsyncLoadTaskParam*>());
	}

	Tasks[PathToHash]->AddTail(AsyncLoadTaskParam);
	return AsyncLoadTaskParam;
}

//====================================================================================คนคง



//====================================================================================

void WRAsyncLoadUObjectManager::RemoveAsyncLoadTaskParam(FWRAsyncLoadTaskParam* InParam)
{
	if (InParam == nullptr)
		return;

	FString HashKey = FWRAsyncLoadTaskParam::GetHashKey(InParam->Hash);
	if (Tasks.Contains(HashKey) == false)
		return;

	bool bEmptyKey = false;

	for (auto It = Tasks.CreateIterator(); It; ++It)
	{
		auto& List = It.Value();
		for (auto& Node : *List)
		{
			if (InParam == Node)
			{
				List->RemoveNode(Node);
				break;
			}
		}

		if (List->Num() == 0)
		{
			bEmptyKey = true;
		}
	}

	if (bEmptyKey == true)
	{
		delete Tasks[HashKey];
		Tasks.Remove(HashKey);
	}
}

void WRAsyncLoadUObjectManager::RemoveAll()
{
	for (auto It = Tasks.CreateIterator(); It; ++It)
	{
		auto& List = It.Value();
		for (auto& Node : *List)
		{
			this->CancelAsyncLoad(Node->Hash);
			//this->RemoveAsyncLoadTaskParam(Node);
		}
	}

	checkf(Tasks.Num() == 0, TEXT("WRAsyncLoadUObjectManager::RemoveAll - Not clean Tasks container. Please check."));
}
