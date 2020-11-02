#include "WRAnimManager.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRMontageTable.h"
#include "WRPathManager.h"
#include "Enum/EWRPath.h"
#include "Table/WRSkillTable.h"
#include "Table/WRWeaponTable.h"
#include "Table/WRPlayerTable.h"
#include "WRAsyncLoadUObjectManager.h"



WRAnimManager::WRAnimManager()
{
}


WRAnimManager::~WRAnimManager()
{
}

void WRAnimManager::OnInitialize()
{
	for (auto Iter = AsyncAssetsKey.CreateIterator(); Iter; ++Iter)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(Iter.Key());
	}

	for (auto Iter = mapAnimMontage.CreateIterator(); Iter; ++Iter)
	{
		Iter.Value()->RemoveFromRoot();
	}

	mapAnimMontage.Empty();
	AsyncAssetsKey.Empty();
}

void WRAnimManager::OnShutdown()
{
	OnInitialize();
}

void WRAnimManager::OnLoadLevelStart()
{

}

void WRAnimManager::OnLoadLevelComplete()
{

}

void WRAnimManager::OnRenderStart()
{
	OnInitialize();
}

bool WRAnimManager::OnTick(float InDeltaTime)
{
	return true;
}


void WRAnimManager::LoadMontage()
{
	UDataTable* MontageSheet = WRTableManager::Get()->GetTable<FWRMontageTable>();
	if (nullptr == MontageSheet)
	{
		return;
	}

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRAnimManager::OnLoadMontageCompleted);
	TArray<FWRMontageTable*> arrMontageTable;
	MontageSheet->GetAllRows(MontageFileName, arrMontageTable);
	for (int32 Idx = 0; Idx < arrMontageTable.Num(); ++Idx)
	{
		if (mapAnimMontage.Contains(arrMontageTable[Idx]->GetTableID()))
			continue;

		FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(arrMontageTable[Idx]->Path, AsyncLoadDele);
		AsyncAssetsKey.Add(AsyncKey, arrMontageTable[Idx]->GetTableID());
	}
}

void WRAnimManager::LoadMontage(WRTableID InMontageTID)
{
	FWRMontageTable* MontageTable = WRTableManager::Get()->FindRow<FWRMontageTable>(InMontageTID);
	if (nullptr == MontageTable)
		return;

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRAnimManager::OnLoadMontageCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MontageTable->Path, AsyncLoadDele);
	AsyncAssetsKey.Add(AsyncKey, MontageTable->GetTableID());
}

void WRAnimManager::OnLoadMontageCompleted(UObject* InMontage, FString InHashKey)
{
	UAnimMontage* AnimMontage = Cast<UAnimMontage>(InMontage);
	if (nullptr == AnimMontage)
	{
		FString MSG = FString::Format(TEXT("OnLoadMontageCompleted : {0}"), { *InHashKey });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("UObject is nullptr"));
		return;
	}

	WRTableID* MontageTableID = AsyncAssetsKey.Find(InHashKey);
	if (MontageTableID && mapAnimMontage.Contains(*MontageTableID) == false)
	{
		//gc가 수거 못해가게 함
		AnimMontage->AddToRoot();

		mapAnimMontage.Add(*MontageTableID, AnimMontage);
	}

	AsyncAssetsKey.Remove(InHashKey);
}

bool WRAnimManager::Contains(WRTableID InMontageTID)
{
	return mapAnimMontage.Contains(InMontageTID);
}

UAnimMontage* WRAnimManager::FindMontage(WRRecordTID InMontageID)
{
	UAnimMontage* AnimMontage =  mapAnimMontage.FindRef(InMontageID);
	if (nullptr == AnimMontage)	//미리 로드된 몽타주가 없으면 즉시 로드
	{
		FWRMontageTable* MontageTable = WRTableManager::Get()->FindRow<FWRMontageTable>(InMontageID);
		if (nullptr == MontageTable)
			return nullptr;

		//로드된 몽타주를 컨테이너에 넣어둠
		AnimMontage = LoadObject<UAnimMontage>(NULL, *MontageTable->Path);
		if (AnimMontage)
		{
			//gc가 수거 못해가게 함
			AnimMontage->AddToRoot();

			mapAnimMontage.Add(InMontageID, AnimMontage);
		}

		const FString* FoundKey = AsyncAssetsKey.FindKey(InMontageID);
		if (FoundKey)
		{
			WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*FoundKey);
			AsyncAssetsKey.Remove(*FoundKey);
		}
	}

	return AnimMontage;
}


