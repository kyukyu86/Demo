

#include "WRSFXManager.h"
//#include "WR.h"

#include "Runtime/Core/Public/Misc/Paths.h"

#include "Core/WRGameInstance.h"

//#include "Components/AudioComponent.h"

#include "Engine/World.h"
#include "UObject/UObjectIterator.h"
#include "Kismet/GameplayStatics.h"

#include "Sound/SoundClass.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundCue.h"


#include "../Enum/EWRConsole.h"
#include "WRConsoleManager.h"


#include "WRPathManager.h"
#include "Utility/WREnumUtility.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Actor/Character/WRCharacter.h"
#include "Actor/EnvActor/SlicedCharacter/WRSlicedActor.h"
#include "Actor/StaticMeshActor/WRStaticMeshProjectile.h"
#include "Components/AudioComponent.h"
#include "Sound/AmbientSound.h"
#include "EngineUtils.h"
#include "Table/WRFootstepsTable.h"
#include "Table/Base/WRTableManager.h"

DEFINE_LOG_CATEGORY(WRLjh)

DECLARE_CYCLE_STAT(TEXT("WRSFXManager_SpawnAtLocation"), STAT_WRSFXManager_SpawnAtLocation, STATGROUP_WRSFXManager);
DECLARE_CYCLE_STAT(TEXT("WRSFXManager_SpawnAttached"), STAT_WRSFXManager_SpawnAttached, STATGROUP_WRSFXManager);
DECLARE_CYCLE_STAT(TEXT("WRSFXManager_Spawn2D"), STAT_WRSFXManager_Spawn2D, STATGROUP_WRSFXManager);
DECLARE_CYCLE_STAT(TEXT("WRSFXManager_SpawnBGM"), STAT_WRSFXManager_SpawnBGM, STATGROUP_WRSFXManager);
DECLARE_CYCLE_STAT(TEXT("WRSFXManager_TryLoad"), STAT_WRSFXManager_TryLoad, STATGROUP_WRSFXManager);

WRSFXManager::WRSFXManager()
{

}

WRSFXManager::~WRSFXManager()
{

}

void WRSFXManager::OnInitialize()
{
}

void WRSFXManager::OnShutdown()
{
	this->RemoveBGM(false);
	this->RemoveAll();

	this->Unregister();
}

void WRSFXManager::OnLoadLevelStart()
{
	this->RemoveBGM(false);
	this->RemoveAll();

	this->Unregister();

	/* Restrict sound playback when LevelLoading */
	IsLock = true;
}

void WRSFXManager::OnLoadLevelComplete()
{
	//this->SpawnBGM("", 1.0f, 1.0f);

	IsLock = false;
}

void WRSFXManager::OnRenderStart()
{
}

UAudioComponent * WRSFXManager::SpawnAtLocation(const FString & InFilePath, const FVector & InLocation, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/, const int32 InIndex /*= -1*/)
{	
	if (SFXs.Contains(InIndex) == true)
	{
		UE_LOG(WRLjh, Warning, TEXT("WRSFXManager::SpawnAtLocation - UID is Already Exist : %d"), InIndex);
		return nullptr;
	}

	//SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_SpawnAtLocation)

	USoundBase* SoundBase = this->TryLoad(InFilePath);

	if (SoundBase == nullptr)
		return nullptr;

	return this->SpawnAtLocation(SoundBase, InLocation, InVolumeMultiplier, InPitchMultiplier, InIndex);
}

UAudioComponent * WRSFXManager::SpawnAtLocation(USoundBase * InGeneratedClass, const FVector & InLocation, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/, const int32 InIndex /*= -1*/)
{
	if (IsLock == true)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Loading Now"), TEXT("WRSFXManager::Spawn()"));
		return nullptr;
	}

	if (InGeneratedClass == nullptr)
		return nullptr;

	SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_SpawnAtLocation)

		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();

	if (World == nullptr)
		return nullptr;

	float tempVolumeMultiplier = 0.0f;
	float tempPitchMultiplier = 0.0f;

	if (InVolumeMultiplier < 0.f)
	{
		tempVolumeMultiplier = InGeneratedClass->GetVolumeMultiplier();
	}
	else
	{
		tempVolumeMultiplier = InVolumeMultiplier;
	}

	if (InPitchMultiplier < 0.0f)
	{
		tempPitchMultiplier = InGeneratedClass->GetPitchMultiplier();
	}
	else
	{
		tempPitchMultiplier = InPitchMultiplier;
	}

	UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAtLocation(World, InGeneratedClass, InLocation, FRotator::ZeroRotator, tempVolumeMultiplier * TempVolume, tempPitchMultiplier, 0.0f, nullptr, nullptr, false);

	if (AudioComponent == nullptr)
		return nullptr;
	
	this->OnComplete(AudioComponent, InIndex);

	return AudioComponent;
}

UAudioComponent * WRSFXManager::SpawnAttached(const FString & InFilePath, USceneComponent * InAttachToComponent, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/, const int32 InIndex /*= -1*/)
{
	if (SFXs.Contains(InIndex) == true)
	{
		UE_LOG(WRLjh, Warning, TEXT("WRSFXManager::SpawnAttached - UID is Already Exist : %d"), InIndex);
		return nullptr;
	}

	//SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_SpawnAttached)

	USoundBase* SoundBase = this->TryLoad(InFilePath);

	if (SoundBase == nullptr)
		return nullptr;

	return this->SpawnAttached(SoundBase, InAttachToComponent, InVolumeMultiplier, InPitchMultiplier, InIndex);
}

UAudioComponent * WRSFXManager::SpawnAttached(USoundBase * InGeneratedClass, USceneComponent * InAttachToComponent, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/, const int32 InIndex /*= -1*/)
{
	if (IsLock == true)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Loading Now"), TEXT("WRSFXManager::Spawn()"));
		return nullptr;
	}

	SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_SpawnAttached)

		if (InGeneratedClass == nullptr)
			return nullptr;

	float tempVolumeMultiplier = 0.0f;
	float tempPitchMultiplier = 0.0f;

	if (InVolumeMultiplier < 0.0f)
	{
		tempVolumeMultiplier = InGeneratedClass->GetVolumeMultiplier();
	}
	else
	{
		tempVolumeMultiplier = InVolumeMultiplier;
	}

	if (InPitchMultiplier < 0.0f)
	{
		tempPitchMultiplier = InGeneratedClass->GetPitchMultiplier();
	}
	else
	{
		tempPitchMultiplier = InPitchMultiplier;
	}

	UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAttached(InGeneratedClass, InAttachToComponent, NAME_None, InAttachToComponent->GetComponentLocation(), EAttachLocation::KeepWorldPosition, false, tempVolumeMultiplier * TempVolume, tempPitchMultiplier, 0.0f, nullptr, nullptr, false);

	if (AudioComponent == nullptr)
		return nullptr;
	this->Register(InAttachToComponent->GetOwner(), AudioComponent);
	this->OnComplete(AudioComponent, InIndex);

	return AudioComponent;
}

UAudioComponent * WRSFXManager::Spawn2D(const FString & InFilePath, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/, const int32 InIndex /*= -1*/)
{
	if (SFXs.Contains(InIndex) == true)
	{
		UE_LOG(WRLjh, Warning, TEXT("WRSFXManager::Spawn2D - UID is Already Exist : %d"), InIndex);
		return nullptr;
	}

	//SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_Spawn2D)

	USoundBase* SoundBase = this->TryLoad(InFilePath);

	if (SoundBase == nullptr)
		return nullptr;

	return this->Spawn2D(SoundBase, InVolumeMultiplier, InPitchMultiplier, InIndex);
}

UAudioComponent * WRSFXManager::Spawn2D(USoundBase * InGeneratedClass, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/, const int32 InIndex /*= -1*/)
{
	if (IsLock == true)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Loading Now"), TEXT("WRSFXManager::Spawn()"));
		return nullptr;
	}

	if (InGeneratedClass == nullptr)
		return nullptr;

	SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_Spawn2D)

		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();

	if (World == nullptr)
		return nullptr;

	float tempVolumeMultiplier = 0.0f;
	float tempPitchMultiplier = 0.0f;

	if (InVolumeMultiplier < 0.0f)
	{
		tempVolumeMultiplier = InGeneratedClass->GetVolumeMultiplier();
	}
	else
	{
		tempVolumeMultiplier = InVolumeMultiplier;
	}

	if (InPitchMultiplier < 0.0f)
	{
		tempPitchMultiplier = InGeneratedClass->GetPitchMultiplier();
	}
	else
	{
		tempPitchMultiplier = InPitchMultiplier;
	}
	
	UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(World, InGeneratedClass, tempVolumeMultiplier, tempPitchMultiplier, 0.0f, nullptr, false, false);
	
	if (AudioComponent == nullptr)
		return nullptr;

	this->OnComplete(AudioComponent, InIndex);

	return AudioComponent;
}

UAudioComponent * WRSFXManager::SpawnBGM(const FString & InFilePath, const float InFadeDuration /*= -1.f*/, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/)
{

	USoundBase* SoundBase = this->TryLoad(InFilePath);

	if (SoundBase == nullptr)
		return nullptr;

	return this->SpawnBGM(SoundBase, InVolumeMultiplier, InPitchMultiplier);
}

UAudioComponent * WRSFXManager::SpawnBGM(USoundBase * InGeneratedClass, const float InFadeDuration /*= 1.f*/, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/)
{
	if (IsLock == true)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Loading Now"), TEXT("WRSFXManager::Spawn()"));
		return nullptr;
	}

	if (InGeneratedClass == nullptr)
		return nullptr;

	SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_SpawnBGM)

		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();

	if (World == nullptr)
		return nullptr;

	if (CurrentBGM != nullptr)
	{
		PreviousBGM = CurrentBGM;

		PreviousBGM->FadeOut(InFadeDuration, 0.f);
		PreviousBGM->RemoveFromRoot();
		PreviousBGM->bAutoDestroy = true;
		PreviousBGM = nullptr;
	}

	float tempVolumeMultiplier = 0.0f;
	float tempPitchMultiplier = 0.0f;

	if (InVolumeMultiplier <= 0.0f)
	{
		tempVolumeMultiplier = InGeneratedClass->GetVolumeMultiplier();
	}
	else
	{
		tempVolumeMultiplier = InVolumeMultiplier;
	}

	if (InPitchMultiplier <= 0.0f)
	{
		tempPitchMultiplier = InGeneratedClass->GetPitchMultiplier();
	}
	else
	{
		tempPitchMultiplier = InPitchMultiplier;
	}

	CurrentBGM = UGameplayStatics::CreateSound2D(World, InGeneratedClass, tempVolumeMultiplier * TempVolume, tempPitchMultiplier, 0.0f, nullptr, false, false);

	if (CurrentBGM == nullptr)
		return nullptr;

	/*float FadeDuration = WRConsoleManager::Get()->GetFloat(EWRConsole::Sound_BGMDuration);

	if (InFadeDuration > 0.f)
	{
		FadeDuration = InFadeDuration;
	}*/

	CurrentBGM->FadeIn(InFadeDuration);
	CurrentBGM->AddToRoot();
	CurrentBGM->bAutoDestroy = false;

	return CurrentBGM;
}


void WRSFXManager::AsyncSpawnSFX(FWRSFXSpawnAsync& IN AsyncInfo)
{
	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRSFXManager::OnCompleteSpawn);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(AsyncInfo.Path, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;
	AsyncSpawns.Add(AsyncKey, AsyncInfo);
}

void WRSFXManager::AsyncSpawn2D(const FString& InFilePath, const float InVolumeMultiplier /*= -1.0F*/, const float InPitchMultiplier /*= -1.0F*/, const int32 InIndex /*= -1*/)
{
	if (InFilePath.IsEmpty() == true || InFilePath.Contains(TEXT("*")))
		return;

	FString TempPath = TEXT("");
	if (InFilePath.Contains(WRPathManager::Get()->GetPath(EWRPath::Sound_SoundCue)))
	{
		TempPath = InFilePath;
	}
	else
	{
		TempPath = WRPathManager::Get()->GetPath(EWRPath::Sound_SoundCue) + InFilePath;
	}

	FWRSFXSpawnAsync WRSFXSpawnAsync;
	WRSFXSpawnAsync.SpawnType = EWRSFXSpawnType::Sound2D;
	WRSFXSpawnAsync.Path = TempPath;
	WRSFXSpawnAsync.Volume = InVolumeMultiplier;
	WRSFXSpawnAsync.Pitch = InPitchMultiplier;
	WRSFXSpawnAsync.Index = InIndex;

	AsyncSpawnSFX(WRSFXSpawnAsync);
}

void WRSFXManager::OnCompleteSpawn(UObject* InObject, FString InHashKey)
{
	if (AsyncSpawns.Contains(InHashKey) == false)
		return;

	USoundBase* InSoundBase = Cast<USoundBase>(InObject);
	if (InSoundBase == nullptr)
	{
		AsyncSpawns.Remove(InHashKey);
		return;
	}

	const FWRSFXSpawnAsync* InAsyncInfo = AsyncSpawns.Find(InHashKey);
	if (InAsyncInfo == nullptr)
		return;

	switch (InAsyncInfo->SpawnType)
	{
	case EWRSFXSpawnType::AtLocation:
		this->SpawnAtLocation(InSoundBase, InAsyncInfo->Location, InAsyncInfo->Volume, InAsyncInfo->Pitch, InAsyncInfo->Index);
		break;
	case EWRSFXSpawnType::Attach:
		this->SpawnAttached(InSoundBase, InAsyncInfo->AttachedComponent, InAsyncInfo->Volume, InAsyncInfo->Pitch, InAsyncInfo->Index);
		break;
	case EWRSFXSpawnType::BGM:
		this->SpawnBGM(InSoundBase,InAsyncInfo->FadeDuration,InAsyncInfo->Volume, InAsyncInfo->Pitch);
		break;
	case EWRSFXSpawnType::Sound2D:
		this->Spawn2D(InSoundBase, InAsyncInfo->Volume, InAsyncInfo->Pitch, InAsyncInfo->Index);
		break;
	}
}

TArray<FString> WRSFXManager::GetPreLoadList()
{
	TArray<FString> PreLoadList;

	UDataTable* InFootstepTable = WRTableManager::Get()->GetTable<FWRFootstepsTable>();
	if (InFootstepTable == nullptr)
		return PreLoadList;


	TArray<FWRFootstepsTable*> TempArray;
	InFootstepTable->GetAllRows(FString(""), TempArray);

	for (FWRFootstepsTable* It : TempArray)
	{
		if (nullptr == It)
			continue;

		PreLoadList.Add(It->StepSound);
		PreLoadList.Add(It->StepSoundInRaining);
	}
	FString UI_OKPath = WRPathManager::Get()->GetPath(EWRPath::Sound_SoundCue) + TEXT("UI_OK.UI_OK");
	PreLoadList.Add(UI_OKPath);

	return PreLoadList;
}

void WRSFXManager::Remove(const uint32 InIndex, const float InFadeDuration /*= -1.0f*/)
{
	UAudioComponent* InComponent = this->Find(InIndex);

	if (SFXs.Contains(InIndex) == false)
	{
		UE_LOG(WRLjh, Display, TEXT("[WRSFXManager::Remove] SFXs.Contains(InIndex) == false"));
		return;
	}

	if (InComponent == nullptr || InComponent->IsValidLowLevel() == false)
	{
		UE_LOG(WRLjh, Display, TEXT("[WRSFXManager::Remove] InComponent == nullptr"));
		return;
	}

	this->ReleaseSFX(*InComponent, InFadeDuration);

	SFXs.Remove(InIndex);
	SFXs.Compact();
}

void WRSFXManager::Remove(UAudioComponent * InComponent, const float InFadeDuration /*= -1.0f*/, int32 InUniqueID /*= -1*/)
{
	uint32 InIndex = -1;
	if (InUniqueID < 0)
	{
		InIndex = InComponent->GetUniqueID();
	}
	else
	{
		InIndex = InUniqueID;
	}

	if (SFXs.Contains(InIndex) == false)
	{
		UE_LOG(WRLjh, Display, TEXT("[WRSFXManager::Remove] SFXs.Contains(InIndex) == false"));
		return;
	}

	if (InComponent == nullptr)
	{
		UE_LOG(WRLjh, Display, TEXT("[WRSFXManager::Remove] InComponent == nullptr"));
		return;
	}

	this->ReleaseSFX(*InComponent, InFadeDuration);

	SFXs.Remove(InIndex);
	SFXs.Compact();
}

void WRSFXManager::ReleaseSFX(IN UAudioComponent& InComponent, const float InFadeDuration /*= -1.0f*/)
{
	if (InComponent.OnAudioFinishedNative.IsBound())
	{
		InComponent.OnAudioFinishedNative.Clear();
	}
	else
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(&InComponent));

		FString Text = FString::Format(TEXT("This function is not bound to the given {0}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRSFXManager::ReleaseSFX()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
	}

	if (InFadeDuration > 0.0f)
	{
		InComponent.FadeOut(InFadeDuration, 0.0f);
	}
	else
	{
		InComponent.Stop();
	}
	InComponent.bAutoDestroy = true;
	InComponent.RemoveFromRoot();
}

void WRSFXManager::Register(AActor* InActor, UAudioComponent* InAudioComp)
{
	if (InActor == nullptr)
	{
		return;
	}
	if (this->IsBinding(InActor))
	{
		if (Actors.Contains(InActor) == false)
		{
			Actors.Add(InActor, new TDoubleLinkedList<UAudioComponent*>());
		}
		Actors[InActor]->AddTail(InAudioComp);
	}
}

void WRSFXManager::Unregister()
{
	//for (auto& Actor : Actors)
	//{
	//	//#if UE_EDITOR
	//	//		TDoubleLinkedList<UAudioComponent*>* Temps = Actor.Value;
	//	//		for (auto& Temp : *Temps)
	//	//		{
	//	//			if (Temp == nullptr)
	//	//				continue;
	//	//
	//	//			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("SFX is not nullptr"), TEXT("WRSFXManager::Unregister()"));
	//	//		}
	//	//#else // UE_EDITOR
	//	//#endif // UE_EDITOR

	//	delete Actor.Value;
	//	Actor.Value = nullptr;
	//	Actors.Remove(Actor.Key);
	//}
	TArray< TDoubleLinkedList<UAudioComponent*>*> InLists;
	Actors.GenerateValueArray(InLists);
	for (TDoubleLinkedList<UAudioComponent*>* ItLists : InLists)
	{
		if (ItLists == nullptr)
			continue;

		for (UAudioComponent*It : *ItLists)
		{
			if (It == nullptr)
				continue;

			this->Remove(It);
		}
	}
	Actors.Empty();
}

void WRSFXManager::Unregister(AActor* InActor)
{
	if (Actors.Contains(InActor) == false)
	{
		return;
	}

	TDoubleLinkedList<UAudioComponent*>* InList = Actors[InActor];

	if (InList == nullptr)
		return;

	for (UAudioComponent* It : *InList)
	{
		if (It == nullptr)
			continue;

		this->Remove(It);
	}
	InList->Empty();

	Actors.Remove(InActor);
	Actors.Compact();
}

//void WRSFXManager::Unregister(UAudioComponent* InAudioComponent)
//{
//
//}
//
void WRSFXManager::RemoveAll()
{
	bool isFail = false;

	for (const TPair<uint32, UAudioComponent*>& tmpPair : SFXs)
	{
		if (tmpPair.Value != nullptr)
		{
			UAudioComponent* InComponent = tmpPair.Value;

			this->ReleaseSFX(*InComponent);
		}
	}
	SFXs.Empty();
}

void WRSFXManager::RemoveBGM(const bool InShouldFadeAudio)
{
	if (CurrentBGM == nullptr)
		return;

	ReleaseSFX(*CurrentBGM, InShouldFadeAudio);
	CurrentBGM = nullptr;
}

UAudioComponent * WRSFXManager::Find(const uint32 InIndex)
{
	if (SFXs.Contains(InIndex) == false)
		return nullptr;

	return *SFXs.Find(InIndex);
}

void WRSFXManager::PlayBGM()
{
	UWorld* tmpWorld = UWRGameInstance::GetGameInstance()->GetWorld();

	if (tmpWorld->GetName().Equals(TEXT("Test_Forest")))
	{
		FString tmpPath = FPaths::Combine(WRPathManager::Get()->GetPath(EWRPath::Asset), TEXT("Sound/Sound_Cues/BirdSound.BirdSound"));
		this->SpawnBGM(tmpPath);
	}
}

void WRSFXManager::SFX_TestFunction()
{
	UAudioComponent*tmpSound1 = this->Spawn2D(TEXT("Monster_roar.Monster_roar"));
	UAudioComponent*tmpSound2 = this->Spawn2D(TEXT("Footsteps.Footsteps"));
	UAudioComponent*tmpSound3 = this->Spawn2D(TEXT("Greystone_Effort_Attack.Greystone_Effort_Attack"));
	this->RemoveAll();
}

void WRSFXManager::TestLevelLoad()
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();

	FString WorldName;

	if (World->GetName().Contains(TEXT("SFXTEST1")))
	{
		WorldName = TEXT("Test_Forest_BB");
	}
	else
	{
		WorldName = TEXT("SFXTEST1");
	}

	UGameplayStatics::OpenLevel(World, *WorldName);
	//PlayBGM();
}

float WRSFXManager::GetSoundVolume(IN const EWRSoundClassKind & InKind)
{
	if (InKind < EWRSoundClassKind::Max)
	{
		FString tmpSoundClassName = WREnumUtility::EnumToString("EWRSoundClassKind", InKind);

		for (TObjectIterator<USoundClass> it; it; ++it)
		{
			USoundClass* tmpSoundClass = *it;
			if (tmpSoundClass->GetName().Equals(tmpSoundClassName))
			{
				return tmpSoundClass->Properties.Volume;
			}
		}
	}
	return 1.0f;
}

void WRSFXManager::SetSoundVolume(IN const EWRSoundClassKind& InKind, IN const float& InVolume)
{
	if (IsLock == true)
		return;

	float fVol = FMath::Max(InVolume, 0.0f);

	FString tmpSoundClassName = WREnumUtility::EnumToString("EWRSoundClassKind", InKind);

	for (TObjectIterator<USoundClass> it; it; ++it)
	{
		USoundClass* tmpSoundClass = *it;
		if (tmpSoundClass->GetName().Equals(tmpSoundClassName))
		{
			tmpSoundClass->Properties.Volume = InVolume;
			return;
		}
	}
}

void WRSFXManager::OnComplete(UAudioComponent * InComponent, const int32 InIndex)
{
	uint32 FinalIndex = ConvertIndex(*InComponent, InIndex);

	if (SFXs.Contains(FinalIndex) == false)
	{
		SFXs.Add(FinalIndex, InComponent);
	}
	else
	{
#if UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("SFX already exist"), TEXT("WRSFXManager::OnComplete()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
	}

	if (InComponent->IsSafeForRootSet() == true)
		InComponent->AddToRoot();

	InComponent->OnAudioFinishedNative.AddRaw(this, &WRSFXManager::OnAudioFinishedNative);
	if (InComponent->OnAudioFinishedNative.IsBoundToObject(this) == false)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(InComponent));

		FString Text = FString::Format(TEXT("This function is not bound to the given {0}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRSFXManager::OnComplete()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
	}
}

USoundBase* WRSFXManager::TryLoad(const FString & InFilePath)
{
	if (InFilePath.IsEmpty() == true || InFilePath.Contains(TEXT("*")))
		return nullptr;

	FString TempPath = TEXT("");
	if (InFilePath.Contains(WRPathManager::Get()->GetPath(EWRPath::Sound_SoundCue)))
	{
		TempPath = InFilePath;
	}
	else
	{
		TempPath = WRPathManager::Get()->GetPath(EWRPath::Sound_SoundCue) + InFilePath;
	}

	if (FPackageName::DoesPackageExist(TempPath) == false)
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(TempPath);

		FString Text = FString::Format(TEXT("Does Not Package Exist {0}"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRSFXManager::TryLoad"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return nullptr;
	}

	SCOPE_CYCLE_COUNTER(STAT_WRSFXManager_TryLoad)

	USoundBase* ResultSound = FindObject<USoundBase>(nullptr, *TempPath);
	if (ResultSound == nullptr)
	{
		ResultSound = LoadObject<USoundBase>(nullptr, *TempPath);
	}
	return ResultSound;
}

bool WRSFXManager::IsBinding(AActor* InActor)
{
	bool RetVal = this->Bind<AWRCharacter>(InActor);

	RetVal |= this->Bind<AWRStaticMeshProjectile>(InActor);
	RetVal |= this->Bind<AWRSlicedActor>(InActor);

	return RetVal;
}

void WRSFXManager::SetTempVolume(const float& InVolume)
{
	TempVolume = InVolume;
	UWorld* InWorld = UWRGameInstance::GetGameInstance()->GetWorld();

	if (InWorld == nullptr)
		return;

	for (TActorIterator<AAmbientSound> It(InWorld); It; ++It)
	{
		AAmbientSound* AnbientSoundActor = *It;

		UAudioComponent* InComp = AnbientSoundActor->GetAudioComponent();
		if (InComp != nullptr)
		{
			InComp->SetVolumeMultiplier(InVolume);
		}
	}
}

void WRSFXManager::OnAudioFinishedNative(UAudioComponent * InComponent)
{
	this->Remove(InComponent);
}
