#pragma once
#include "Base/WRTableRow.h"
#include "Base/WRTableManager.h"
#include "WRVideoChatTable.generated.h"



USTRUCT(BlueprintType)
struct FWRVideoChatDialogTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VideoChatData_VideoChatDialog"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = VideoChatData)
		int32 EndingCallDialogueTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = VideoChatData)
		int32 ConnectingDialogueTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = VideoChatData)
		int32 ReceivingDialogueTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = VideoChatData)
		int32 RejectCallDialogueTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = VideoChatData)
		int32 CallingDialogueTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = VideoChatData)
		int32 CancelCallDialogueTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = VideoChatData)
		int32 UnableCallDialogueTID;

	static WRTableID GetEndingCallDialogueTID()
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatDialogTable>();
		if (DataTable == nullptr)
			return INVALID_TABLE_TID;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatDialogTable* VideoChatDialogueTable = reinterpret_cast<FWRVideoChatDialogTable*>(iter.Value);
			if (VideoChatDialogueTable == nullptr)
				continue;

			return VideoChatDialogueTable->EndingCallDialogueTID;
		}

		return INVALID_TABLE_TID;
	}

	static WRTableID GetConnectingDialogueTID()
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatDialogTable>();
		if (DataTable == nullptr)
			return INVALID_TABLE_TID;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatDialogTable* VideoChatDialogueTable = reinterpret_cast<FWRVideoChatDialogTable*>(iter.Value);
			if (VideoChatDialogueTable == nullptr)
				continue;

			return VideoChatDialogueTable->ConnectingDialogueTID;
		}

		return INVALID_TABLE_TID;
	}

	static WRTableID GetReceivingDialogueTID()
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatDialogTable>();
		if (DataTable == nullptr)
			return INVALID_TABLE_TID;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatDialogTable* VideoChatDialogueTable = reinterpret_cast<FWRVideoChatDialogTable*>(iter.Value);
			if (VideoChatDialogueTable == nullptr)
				continue;

			return VideoChatDialogueTable->ReceivingDialogueTID;
		}

		return INVALID_TABLE_TID;
	}

	static WRTableID GetRejectCallDialogueTID()
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatDialogTable>();
		if (DataTable == nullptr)
			return INVALID_TABLE_TID;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatDialogTable* VideoChatDialogueTable = reinterpret_cast<FWRVideoChatDialogTable*>(iter.Value);
			if (VideoChatDialogueTable == nullptr)
				continue;

			return VideoChatDialogueTable->RejectCallDialogueTID;
		}

		return INVALID_TABLE_TID;
	}

	static WRTableID GetCallingDialogueTID()
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatDialogTable>();
		if (DataTable == nullptr)
			return INVALID_TABLE_TID;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatDialogTable* VideoChatDialogueTable = reinterpret_cast<FWRVideoChatDialogTable*>(iter.Value);
			if (VideoChatDialogueTable == nullptr)
				continue;

			return VideoChatDialogueTable->CallingDialogueTID;
		}

		return INVALID_TABLE_TID;
	}

	static WRTableID GetCancelCallDialogueTID()
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatDialogTable>();
		if (DataTable == nullptr)
			return INVALID_TABLE_TID;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatDialogTable* VideoChatDialogueTable = reinterpret_cast<FWRVideoChatDialogTable*>(iter.Value);
			if (VideoChatDialogueTable == nullptr)
				continue;

			return VideoChatDialogueTable->CancelCallDialogueTID;
		}

		return INVALID_TABLE_TID;
	}

	static WRTableID GetUnableCallDialogueTID()
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatDialogTable>();
		if (DataTable == nullptr)
			return INVALID_TABLE_TID;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatDialogTable* VideoChatDialogueTable = reinterpret_cast<FWRVideoChatDialogTable*>(iter.Value);
			if (VideoChatDialogueTable == nullptr)
				continue;

			return VideoChatDialogueTable->UnableCallDialogueTID;
		}

		return INVALID_TABLE_TID;
	}
};