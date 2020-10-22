

#include "DMAsyncLoadManager.h"
//#include "SecureHash.h"
#include "Engine/AssetManager.h"
#include "../GameInstance/DMGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "../Util/DMActorUtil.h"
#include "../GameInstance/DMGameInstance.h"


uint64 FDMAsyncLoadTaskParam::IncrementIndex = 0;

FString FDMAsyncLoadTaskParam::GetHashKey(const FString& InHash)
{
	FString _Hash = InHash;
	int32 FoundIndex;
	if (_Hash.FindLastChar('_', FoundIndex) == false)
	{
		return ASYNCLOADMANAGER_INVALID;
	}

	int32 HashLen = _Hash.Len();
	FString Result = _Hash.Left(FoundIndex);
	return Result;
}

void FDMAsyncLoadTaskParam::LoadComplete()
{
	FStringAssetReference LoadedObjectStringRef = FStringAssetReference(Path);
	UObject* LoadedObject = LoadedObjectStringRef.ResolveObject();
	if (LoadedObject == nullptr)
	{
		LoadedObject = LoadedObjectStringRef.TryLoad();
	}

	switch (ObjectType)
	{

	case EDMAsyncLoadObjectType::Actor:
	{
		UWorld* World = UDMGameInstance::GetGameInstance()->GetWorld();
		AActor* SpawnedActor = World->SpawnActor((UClass*)LoadedObject, nullptr, FActorSpawnParameters());
		if (SpawnedActor == nullptr)
		{
			ensureMsgf(false, TEXT("SpawnedActor is Invalid"));
			return;
		}

		CompleteCallback.ExecuteIfBound(SpawnedActor, Hash);

	} break;
	case EDMAsyncLoadObjectType::Asset:
	{

		CompleteCallback.ExecuteIfBound(LoadedObject, Hash);

	} break;
	case EDMAsyncLoadObjectType::Widget:
	{
		UWorld* World = UDMGameInstance::GetGameInstance()->GetWorld();
		UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(World, (UClass*)LoadedObject);
		if (CreatedWidget == nullptr)
		{
			ensureMsgf(false, TEXT("CreatedWidget is Invalid"));
			return;
		}

		CompleteCallback.ExecuteIfBound(CreatedWidget, Hash);

	} break;

	}
}

//====================================================================================



//====================================================================================

DMAsyncLoadManager::DMAsyncLoadManager()
{

}

DMAsyncLoadManager::~DMAsyncLoadManager()
{

}

void DMAsyncLoadManager::OnInit()
{

}

void DMAsyncLoadManager::OnShutdown()
{

}

FString DMAsyncLoadManager::AsyncSpawnActor(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate)
{
	FString ResolvedFullPath = DMActorUtil::ResolvePath(InFullPath);
	TSharedPtr<FStreamableHandle> HandleSharedPtr = this->RequestAsyncLoad(ResolvedFullPath);
	if (HandleSharedPtr.IsValid() == false)
	{
		return ASYNCLOADMANAGER_INVALID;
	}

	TSharedRef<FStreamableHandle> HandleSharedRef = HandleSharedPtr.ToSharedRef();
	if (HandleSharedRef->IsActive() == false)
	{
		return ASYNCLOADMANAGER_INVALID;
	}

	return this->AddAsyncLoadTaskParam(HandleSharedRef, ResolvedFullPath, EDMAsyncLoadObjectType::Actor, InCompleteDelegate)->Hash;
}

FString DMAsyncLoadManager::AsyncLoadAsset(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate)
{
	TSharedPtr<FStreamableHandle> HandleSharedPtr = this->RequestAsyncLoad(InFullPath);
	if (HandleSharedPtr.IsValid() == false)
	{
		return ASYNCLOADMANAGER_INVALID;
	}

	TSharedRef<FStreamableHandle> HandleSharedRef = HandleSharedPtr.ToSharedRef();
	if (HandleSharedRef->IsActive() == false)
	{
		return ASYNCLOADMANAGER_INVALID;
	}

	return this->AddAsyncLoadTaskParam(HandleSharedRef, InFullPath, EDMAsyncLoadObjectType::Asset, InCompleteDelegate)->Hash;
}

FString DMAsyncLoadManager::AsyncCreateWidget(const FString& InFullPath, FDMCompleteAsyncLoad InCompleteDelegate)
{
	TSharedPtr<FStreamableHandle> HandleSharedPtr = this->RequestAsyncLoad(InFullPath);
	if (HandleSharedPtr.IsValid() == false)
	{
		return ASYNCLOADMANAGER_INVALID;
	}

	TSharedRef<FStreamableHandle> HandleSharedRef = HandleSharedPtr.ToSharedRef();
	if (HandleSharedRef->IsActive() == false)
	{
		return ASYNCLOADMANAGER_INVALID;
	}

	return this->AddAsyncLoadTaskParam(HandleSharedRef, InFullPath, EDMAsyncLoadObjectType::Widget, InCompleteDelegate)->Hash;
}

bool DMAsyncLoadManager::CancelAsyncLoad(const FString& InHash)
{
	FString HashKey = FDMAsyncLoadTaskParam::GetHashKey(InHash);
	if (Tasks.Contains(HashKey) == false)
		return false;

	FDMAsyncLoadTaskParam* CanceledTask = nullptr;

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

bool DMAsyncLoadManager::IsLoadCompleteTasks(TMap<FString, UObject*>& InTasks)
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

TSharedPtr<FStreamableHandle> DMAsyncLoadManager::RequestAsyncLoad(const FString& InFullPath)
{
	if (FPackageName::DoesPackageExist(InFullPath) == false)
	{
		FString MSG = FString::Format(TEXT("Does Not Package Exist {0}"), { *InFullPath });
		return nullptr;
	}

	auto& StreamableManager = UAssetManager::GetStreamableManager();
	return StreamableManager.RequestAsyncLoad(FSoftObjectPath(InFullPath), FStreamableDelegate::CreateRaw(this, &DMAsyncLoadManager::OnCompleteAsyncLoad));
}

//====================================================================================

FString DMAsyncLoadManager::PathToHash(const FString& InValue)
{
	return FMD5::HashAnsiString(*InValue);
}

//====================================================================================

void DMAsyncLoadManager::OnCompleteAsyncLoad()
{
	FDMAsyncLoadTaskParam* ProcessedTask = nullptr;

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
			}
		}

		if (ProcessedTask != nullptr)
			break;
	}

	if (ProcessedTask == nullptr)
	{
		return;
	}

	this->RemoveAsyncLoadTaskParam(ProcessedTask);
}

//====================================================================================

FDMAsyncLoadTaskParam* DMAsyncLoadManager::AddAsyncLoadTaskParam(TSharedRef<FStreamableHandle> InHandle, const FString& InFullPath, EDMAsyncLoadObjectType InObjectType, FDMCompleteAsyncLoad InCallback)
{
	FString PathToHash = this->PathToHash(InFullPath);
	FDMAsyncLoadTaskParam* AsyncLoadTaskParam = new FDMAsyncLoadTaskParam(InHandle, InFullPath, PathToHash, InObjectType, InCallback);
	if (Tasks.Contains(PathToHash) == false)
	{
		Tasks.Add(PathToHash, new TDoubleLinkedList<FDMAsyncLoadTaskParam*>());
	}

	Tasks[PathToHash]->AddTail(AsyncLoadTaskParam);
	return AsyncLoadTaskParam;
}

//====================================================================================

void DMAsyncLoadManager::RemoveAsyncLoadTaskParam(FDMAsyncLoadTaskParam* InParam)
{
	if (InParam == nullptr)
		return;

	FString HashKey = FDMAsyncLoadTaskParam::GetHashKey(InParam->Hash);
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

void DMAsyncLoadManager::RemoveAll()
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
