#include "WRVFXManager.h"
#include "WRPathManager.h"
#include "WRConsoleManager.h"
#include "WRAsyncLoadUObjectManager.h"
#include "WRCharacterManager.h"

#include "Core/WRGameInstance.h"

#include "Enum/EWRPath.h"
#include "Enum/EWRConsole.h"

#include "Actor/Character/WRCharacter.h"

#include "Utility/WRActorSpawnUtility.h"
#include "Utility/WRActorUtility.h"
#include "Utility/WREnumUtility.h"

#include "Actor/EnvActor/SlicedCharacter/WRSlicedActor.h"
#include "Actor/StaticMeshActor/WRStaticMeshProjectile.h"
#include "Actor/Vehicle/WRVehicle.h"

DECLARE_CYCLE_STAT(TEXT("WRVFXManager_SpawnPath"), STAT_WRVFXManager_SpawnPath, STATGROUP_WRVFXManager);
DECLARE_CYCLE_STAT(TEXT("WRVFXManager_SpawnClass"), STAT_WRVFXManager_SpawnClass, STATGROUP_WRVFXManager);

WRVFXManager::WRVFXManager()
{

}

WRVFXManager::~WRVFXManager()
{

}

void WRVFXManager::OnInitialize()
{

}

void WRVFXManager::OnShutdown()
{
	AsyncVFXs.Empty();

	this->Remove();
	this->Unregister();
}

void WRVFXManager::OnLoadLevelStart()
{
	for (auto& AsyncVFX : AsyncVFXs)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncVFX.Key);
	}

	this->Remove();
	this->Unregister();

	IsLock = true;
}

void WRVFXManager::OnLoadLevelComplete()
{
	IsLock = false;
}

void WRVFXManager::OnRenderStart()
{

}

bool WRVFXManager::OnTick(float InDeltaTime)
{
	this->Delay();
	this->Distribute();

	return true;
}

AWRVFX* WRVFXManager::Spawn(const FString& InPath, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent /*= nullptr*/, const EWRVFX InBits /*= EWRVFX::None*/, const FRotator& InAngle /*= FRotator::ZeroRotator*/)
{
	if (IsLock)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Loading Now"), TEXT("WRVFXManager::Spawn()"));
		return nullptr;
	}

	if (InPath.IsEmpty())
	{
		return nullptr;
	}

	if (IsPendingKillPending(InSpawner) || IsPendingKillPending(InTarget))
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(InPath);

		FString Text = FString::Format(TEXT("And VFX {0}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::Spawn()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return nullptr;
	}

	SCOPE_CYCLE_COUNTER(STAT_WRVFXManager_SpawnPath)

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	// [2020-09-10 EZ] : VFX가 휘발성인 경우 && 스포너캐릭터가 현재 비활성화(투명화) 상태이면 스폰 취소	
	if (WREnumUtility::IsBit((uint64)InBits, (uint64)EWRVFX::None))
	{
		AWRCharacter* InSpawnerCharacter = Cast<AWRCharacter>(InSpawner);
		if (InSpawnerCharacter != nullptr)
		{
			if (InSpawnerCharacter->GetIsCharacterActive() == false)
				return nullptr;;
		}
	}

	//FString FullPath = FPackageName::IsValidObjectPath(InPath) ? InPath : WRPathManager::Get()->GetPath(EWRPath::Blueprint_Effect) + InPath;
	FString FullPath = FPackageName::IsValidObjectPath(InPath) ? InPath : WRPathManager::Get()->GetPath(EWRPath::Effect) + InPath;

	if (FPackageName::DoesPackageExist(FullPath) == false)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(FullPath);

		FString Text = FString::Format(TEXT("Does Not Package Exist {0}"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::Spawn()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return nullptr;
	}

	AWRVFX* VFX = WRActorSpawnUtility::Spawn<AWRVFX>(World, FullPath);
	if (VFX == nullptr)
	{
		return nullptr; 
	}

	this->OnComplete(VFX, InSpawner, InTarget, InParent, InBits, InAngle);

	return VFX;
}

AWRVFX* WRVFXManager::Spawn(TSubclassOf<AWRVFX> InGeneratedClass, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent /*= nullptr*/, const EWRVFX InBits /*= EWRVFX::None*/, const FRotator& InAngle /*= FRotator::ZeroRotator*/)
{
	if (IsLock)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Loading Now"), TEXT("WRVFXManager::Spawn()"));
		return nullptr;
	}

	if (IsPendingKillPending(InSpawner) || IsPendingKillPending(InTarget))
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(InGeneratedClass));

		FString Text = FString::Format(TEXT("And VFX {0}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::Spawn()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return nullptr;
	}

	SCOPE_CYCLE_COUNTER(STAT_WRVFXManager_SpawnClass)

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	// [2020-09-10 EZ] : VFX가 휘발성인 경우 && 스포너캐릭터가 현재 비활성화(투명화) 상태이면 스폰 취소
	if (WREnumUtility::IsBit((uint64)InBits, (uint64)EWRVFX::None))
	{
		AWRCharacter* InSpawnerCharacter = Cast<AWRCharacter>(InSpawner);
		if (InSpawnerCharacter != nullptr)
		{
			if (InSpawnerCharacter->GetIsCharacterActive() == false)
				return nullptr;;
		}
	}

	AWRVFX* VFX = WRActorSpawnUtility::Spawn<AWRVFX>(World, InGeneratedClass);
	if (VFX == nullptr)
	{
		return nullptr;
	}

	this->OnComplete(VFX, InSpawner, InTarget, InParent, InBits, InAngle);

	return VFX;
}

AWRVFX* WRVFXManager::Spawn(const FWRVFXSpawnDistribution& InParameter, const bool InIsDistributed /*= false*/)
{
	if (InIsDistributed)
	{
		FWRVFXSpawnDistribution* Distribution = new FWRVFXSpawnDistribution;
		Distribution->Path = InParameter.Path;
		Distribution->GeneratedClass = InParameter.GeneratedClass;
		Distribution->Spawner = InParameter.Spawner;
		Distribution->Target = InParameter.Target;
		Distribution->Parent = InParameter.Parent;
		Distribution->Bits = InParameter.Bits;
		Distribution->Angle = InParameter.Angle;

		Distributions.AddTail(Distribution);

		// [ 2020-2-13 : animal14 ] 추후에 생성된 VFX가 알고 싶다면 델리게이트 등록

		return nullptr;
	}

	if (InParameter.Path.IsEmpty())
	{
		return this->Spawn(InParameter.GeneratedClass, InParameter.Spawner, InParameter.Target, InParameter.Parent, InParameter.Bits, InParameter.Angle);
	}
	else
	{
		return this->Spawn(InParameter.Path, InParameter.Spawner, InParameter.Target, InParameter.Parent, InParameter.Bits, InParameter.Angle);
	}
}

FString WRVFXManager::AsyncSpawn(const FString& InPath, uint64 InSpawnerAID, uint64 InTargetAID, uint32 InParentUID /*= 0*/, const EWRVFX InBits /*= EWRVFX::None*/, const FRotator& InAngle /*= FRotator::ZeroRotator*/, const WRAsyncHandle& InHandle /*= nullptr*/)
{
	FString Key = WRASYNCLOADMANAGER_INVALID;

	if (IsLock)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Loading Now"), TEXT("WRVFXManager::AsyncSpawn()"));
		return Key;
	}

	if (InPath.IsEmpty())
	{
		return Key;
	}

	SCOPE_CYCLE_COUNTER(STAT_WRVFXManager_SpawnPath)

	//FString FullPath = FPackageName::IsValidObjectPath(InPath) ? InPath : WRPathManager::Get()->GetPath(EWRPath::Blueprint_Effect) + InPath;
		FString FullPath = FPackageName::IsValidObjectPath(InPath) ? InPath : WRPathManager::Get()->GetPath(EWRPath::Effect) + InPath;

	if (FPackageName::DoesPackageExist(FullPath) == false)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(FullPath);

		FString Text = FString::Format(TEXT("Does Not Package Exist {0}"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::AsyncSpawn()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return Key;
	}

	Key = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(FullPath, FWRCompleteAsyncLoad::CreateRaw(this, &WRVFXManager::AsyncLoadCompleted));

	FWRVFXSpawnAsync Value;
	Value.SpawnerAID = InSpawnerAID;
	Value.TargetAID = InTargetAID;
	Value.ParentUID = InParentUID;
	Value.Bits = InBits;
	Value.Angle = InAngle;
	Value.AsyncHandle = InHandle;

	AsyncVFXs.Add(Key, Value);

	return Key;
}


void WRVFXManager::Remove(const uint32 InUniqueID, const bool InShouldFadeParticle /*= false*/, const bool InIsDelayed /*= false*/)
{
	AWRVFX* VFX = this->Find(InUniqueID);
	if (VFX == nullptr)
	{
		return;
	}

	if (InShouldFadeParticle)
	{
		if (VFX->IsEmpty())
		{
#if UE_EDITOR
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(VFX));

			FString Text = FString::Format(TEXT("VFXParameters {0} is Empty"), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::Remove()"));
#else // UE_EDITOR
#endif // UE_EDITOR

			VFX->OnComplete().Broadcast();
			VFX->Release();
			VFX->RemoveFromRoot();
			VFX->Destroy();
			VFX = nullptr;

			VFXs.Remove(InUniqueID);
		}
		else
		{
			VFX->PreDestroy();
		}
	}
	else
	{
		if (InIsDelayed)
		{
			Delays.AddTail(VFX);
		}
		else
		{
			VFX->OnComplete().Broadcast();
			VFX->Release();
			VFX->RemoveFromRoot();
			VFX->Destroy();
			VFX = nullptr;

			VFXs.Remove(InUniqueID);
		}
	}
}

void WRVFXManager::Remove(AWRVFX* InVFX, const bool InShouldFadeParticle /*= false*/, const bool InIsDelayed /*= false*/)
{
	uint32 UID = InVFX->GetUniqueID();
	if (VFXs.Contains(UID) == false)
	{
		return;
	}

	if (InShouldFadeParticle)
	{
		if (InVFX->IsEmpty())
		{
#if UE_EDITOR
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(InVFX));

			FString Text = FString::Format(TEXT("VFXParameters {0} is Empty"), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::Remove()"));
#else // UE_EDITOR
#endif // UE_EDITOR

			InVFX->OnComplete().Broadcast();
			InVFX->Release();
			InVFX->RemoveFromRoot();
			InVFX->Destroy();
			InVFX = nullptr;

			VFXs.Remove(UID);
		}
		else
		{
			InVFX->PreDestroy();
		}
	}
	else
	{
		if (InIsDelayed)
		{
			Delays.AddTail(InVFX);
		}
		else
		{
			InVFX->OnComplete().Broadcast();
			InVFX->Release();
			InVFX->RemoveFromRoot();
			InVFX->Destroy();
			InVFX = nullptr;

			VFXs.Remove(UID);
		}
	}
}

void WRVFXManager::Remove()
{
	for (auto& VFX : VFXs)
	{
		VFX.Value->OnComplete().Broadcast();
		VFX.Value->Release();
		VFX.Value->RemoveFromRoot();
		VFX.Value->Destroy();
		VFX.Value = nullptr;
	}
	VFXs.Empty();
	Delays.Empty();
	Distributions.Empty();
}

void WRVFXManager::RemoveIncludeChild(AWRVFX* InVFX, const bool InShouldFadeParticle, const bool InIsDelayed)
{
	if (InVFX->Childs.Num() != 0)
	{
		InVFX->RemoveChilds();
	}

	Remove(InVFX, InShouldFadeParticle, InIsDelayed);
}

bool WRVFXManager::AsyncRemove(const FString& InHash)
{
	if (AsyncVFXs.Contains(InHash) == false)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Does Not Hash Exist"), TEXT("WRVFXManager::AsyncRemove()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return false;
	}
	return WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(InHash);
}

AWRVFX* WRVFXManager::Find(const uint32 InUniqueID)
{
	if (VFXs.Contains(InUniqueID) == false)
	{
		return nullptr;
	}
	return VFXs[InUniqueID];
}

void WRVFXManager::Delay()
{
	for (auto& Delay : Delays)
	{
		this->Remove(Delay);
	}
	Delays.Empty();
}

void WRVFXManager::Distribute()
{
	int32 Max = WRConsoleManager::Get()->GetInt(EWRConsole::Spawn_Distribution);
	for (int32 i = 0; i < Max; ++i)
	{
		if (Distributions.Num() == 0)
		{
			break;
		}

		FWRVFXSpawnDistribution* Distribution = Distributions.GetHead()->GetValue();

		this->Spawn(*Distribution);

		Distributions.RemoveNode(Distribution);

		delete Distribution;
		Distribution = nullptr;
	}
}

void WRVFXManager::Register(AActor* InActor, AWRVFX* InVFX)
{
	if (InActor == nullptr)
	{
		return;
	}

	if (Actors.Contains(InActor) == false)
	{
		Actors.Add(InActor, new TDoubleLinkedList<AWRVFX*>());
	}
	Actors[InActor]->AddTail(InVFX);

	if (this->IsBinding(InActor))
	{
	}
	else
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(InActor));

		FString Text = FString::Format(TEXT("Have to bind delegate {0}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::Register()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}
}

void WRVFXManager::Unregister(AActor* InActor)
{
	if (Actors.Contains(InActor) == false)
	{
		return;
	}

	for (auto& Actor : *(Actors[InActor]))
	{
		if (Actor->GetSpawner() == InActor)
		{
			Actor->RemoveSpawner();
		}

		if (Actor->GetTarget() == InActor)
		{
			Actor->RemoveTarget();
		}
	}

	delete Actors[InActor];
	Actors[InActor] = nullptr;

	Actors.Remove(InActor);
}

void WRVFXManager::Unregister(AWRVFX* InVFX)
{
	AActor* Spawner = InVFX->GetSpawner();
	if (Spawner)
	{
		Actors[Spawner]->RemoveNode(InVFX);
	}

	AActor* Target = InVFX->GetTarget();
	if (Target)
	{
		Actors[Target]->RemoveNode(InVFX);
	}
}

void WRVFXManager::Unregister()
{
	for (auto& Actor : Actors)
	{
#if UE_EDITOR
		TDoubleLinkedList<AWRVFX*>* Temps = Actor.Value;
		for (auto& Temp : *Temps)
		{
			if (Temp == nullptr)
				continue;

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("VFX is not nullptr"), TEXT("WRVFXManager::Unregister()"));
		}
#else // UE_EDITOR
#endif // UE_EDITOR

		delete Actor.Value;
		Actor.Value = nullptr;
	}
	Actors.Empty();
}

void WRVFXManager::OnComplete(AWRVFX* InVFX, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent, const EWRVFX InBits, const FRotator& InAngle)
{
	uint32 UID = InVFX->GetUniqueID();
	if (VFXs.Contains(UID) == false)
	{
		VFXs.Add(UID, InVFX);
	}
	else
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("VFX already exist"), TEXT("WRVFXManager::OnComplete()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
	}

	InVFX->AddToRoot();

	this->Register(InSpawner, InVFX);
	this->Register(InTarget, InVFX);

	InVFX->Adopted(InParent);
	InVFX->Initialize(InSpawner, InTarget, InParent, InBits, InAngle);
}

void WRVFXManager::OnComplete(AWRVFX* InVFX, uint64 InSpawnerAID, uint64 InTargetAID, uint32 InParentUID, const EWRVFX InBits, const FRotator& InAngle)
{
	uint32 UID = InVFX->GetUniqueID();
	if (VFXs.Contains(UID) == false)
	{
		VFXs.Add(UID, InVFX);
	}
	else
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("VFX already exist"), TEXT("WRVFXManager::OnComplete()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
	}

	InVFX->AddToRoot();

	AWRCharacter* Spawner = WRCharacterManager::Get()->FindbyAID(InSpawnerAID);
	if (Spawner == nullptr)
	{
	}

	AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(InTargetAID);
	if (Target == nullptr)
	{
	}

	AWRVFX* Parent = this->Find(InParentUID);
	if (Parent == nullptr)
	{
	}

	this->Register(Spawner, InVFX);
	this->Register(Target, InVFX);

	InVFX->Adopted(Parent);
	InVFX->Initialize(Spawner, Target, Parent, InBits, InAngle);
}

bool WRVFXManager::IsBinding(AActor* InActor)
{
	bool RetVal = false;

	// [ 2019-12-13 : animal14 ] 기본 스포너
	RetVal |= this->Bind<AWRCharacter>(InActor);
	// [ 2019-12-13 : animal14 ] 추가 스포너
	RetVal |= this->Bind<AWRStaticMeshProjectile>(InActor);
	RetVal |= this->Bind<AWRSlicedActor>(InActor);
	RetVal |= this->Bind<AWRVehicle>(InActor);

	return RetVal;
}

void WRVFXManager::AsyncLoadCompleted(UObject* InObject, FString InHash)
{
	if (AsyncVFXs.Contains(InHash) == false)
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Does Not Hash Exist"), TEXT("WRVFXManager::AsyncLoadCompleted()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	AWRVFX* VFX = Cast<AWRVFX>(InObject);
	if (VFX == nullptr)
	{
		return;
	}

	EWRVFX Bits = (EWRVFX)WREnumUtility::AddBit((uint64)AsyncVFXs[InHash].Bits, (uint64)EWRVFX::Async);

	this->OnComplete(VFX, AsyncVFXs[InHash].SpawnerAID, AsyncVFXs[InHash].TargetAID, AsyncVFXs[InHash].ParentUID, Bits, AsyncVFXs[InHash].Angle);

	if (AsyncVFXs[InHash].AsyncHandle.ExecuteIfBound(VFX))
	{
		AsyncVFXs[InHash].AsyncHandle.Unbind();
	}

	AsyncVFXs.Remove(InHash);
}

bool WRVFXManager::IsPendingKillPending(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return false;
	}

	bool RetVal = InActor->IsPendingKillPending();

#if UE_EDITOR
	if (RetVal)
	{
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(InActor));

		FString Text = FString::Format(TEXT("Actor {0} is pending kill."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRVFXManager::IsPendingKillPending()"));
	}
#else // UE_EDITOR
#endif // UE_EDITOR

	return RetVal;
}