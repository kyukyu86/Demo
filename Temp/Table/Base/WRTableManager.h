// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"

#include "../../Manager/SingletonClass/WRSingleton.h"
#include <Engine/StreamableManager.h>
#include <Engine/DataTable.h>
#include "Manager/WRPathManager.h"
#include "../../Enum/EWRPath.h"

#define FIND_AND_CAST_FROM_ITEM_TABLE(TableClassType, ItemTID) static_cast<TableClassType*>(WRTableManager::Get()->FindRowInItemTable(ItemTID))
/**
*
*/
class WR_API WRTableManager : public WRSingleton<WRTableManager>
{
	CDECLARE_MANAGER(WRTableManager);
	
private:
	WRTableManager() {};
	virtual ~WRTableManager() {};
	
public:
	//====================================================================================
	// [ 2019-7-12 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-7-12 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-7-12 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-7-12 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-7-12 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================
	
	void LoadTableAsset();

	float GetLoadingTablePercent();
	bool IsLoading();


	struct FWRMapTableBase* FindRowInMapTable(int32 InMapTID, bool bInUsingEnsureMsg = true);
	struct FWRItemTableBase* FindRowInItemTable(const WRTableID IN InItemTID);

	template <class T>
	T* FindRow(int32 Tid, FString TableName = T::GetFileName()) const;

	template <class T> 
	UDataTable* GetTable() const;

	template <class T>
	void AddToTablePath();
private:
	

	void OnLoadTableComplete();
private:

	TMap<FString, UDataTable*>	Tables;
	TArray<FSoftObjectPath> TablesToStream;

	TSharedPtr<FStreamableHandle> StreamHandle = nullptr;

};

template <class T>
T* WRTableManager::FindRow(int32 Tid, FString TableName /*= T::GetFileName()*/) const
{
	if (INVALID_TABLE_TID == Tid)
		return nullptr;

	if (Tables.Contains(TableName) == false)
		return nullptr;

	return Tables[TableName]->FindRow<T>(*FString::Printf(TEXT("%d"), Tid), TEXT("GENERAL"));
}

template <class T>
UDataTable* WRTableManager::GetTable() const
{
	FString TableName = T::GetFileName();
	if (Tables.Contains(TableName) == false)
		return nullptr;

	return Tables[TableName];
}

template <class T>
void WRTableManager::AddToTablePath()
{
	FString TableName = T::GetFileName();
	FString TablePath = WRPathManager::Get()->GetPath(EWRPath::Data_Table) + TableName + "." + TableName;
	if (TablesToStream.Contains(TablePath) == false)
	{
		FSoftObjectPath TableObjPath(TablePath);
		TablesToStream.Add(TableObjPath);
	}
}
