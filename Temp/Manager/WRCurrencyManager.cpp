// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRCurrencyManager.h"
#include "Table/Base/WRTableManager.h"
#include "Engine/DataTable.h"
#include "Table/WRCurrencyTable.h"
#include "AssertionMacros.h"

void WRCurrencyManager::OnInitialize()
{

}

void WRCurrencyManager::OnShutdown()
{

}

void WRCurrencyManager::OnLoadLevelStart()
{

}

void WRCurrencyManager::OnLoadLevelComplete()
{

}

void WRCurrencyManager::OnRenderStart()
{

}

bool WRCurrencyManager::OnTick(float InDeltaTime)
{

	return true;
}

void WRCurrencyManager::OnWorldEntryInitialized()
{
	InitCurrencyData();
}

GAME_MONEY_t WRCurrencyManager::FindGameMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType)
{
	if (GameMoneyCurrencyValueMap.Contains(InCurrencyType) == false)
		return 0;

	return GameMoneyCurrencyValueMap[InCurrencyType];
}

CASH_MONEY_t WRCurrencyManager::FindCashMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType)
{
	if (CashMoneyCurrencyValueMap.Contains(InCurrencyType) == false)
		return 0;

	return CashMoneyCurrencyValueMap[InCurrencyType];
}

POINT_t WRCurrencyManager::FindPointCurrencyValue(const EWRCurrencyType IN InCurrencyType)
{
	if (PointCurrencyValueMap.Contains(InCurrencyType) == false)
		return 0;

	return PointCurrencyValueMap[InCurrencyType];
}

TICKET_t WRCurrencyManager::FindTicketCurrencyValue(const EWRCurrencyType IN InCurrencyType)
{
	if (TicketCurrencyValueMap.Contains(InCurrencyType) == false)
		return 0;

	return TicketCurrencyValueMap[InCurrencyType];
}

CURRENCY_t WRCurrencyManager::FindLimitValue(const EWRCurrencyType IN InCurrencyType)
{
	FWRCurrencyTable* CurrencyTable = FindCurrencyTable(InCurrencyType);
	if (CurrencyTable == nullptr)
		return -1;

	return CurrencyTable->LimitValue;
}

FWRCurrencyTable* WRCurrencyManager::FindCurrencyTable(const EWRCurrencyType IN InCurrencyType)
{
	UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRCurrencyTable>();
	if (DataTable == nullptr)
		return nullptr;

	const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

	for (auto& iter : CurrencyData)
	{
		FWRCurrencyTable* CurrencyTable = reinterpret_cast<FWRCurrencyTable*>(iter.Value);
		if (CurrencyTable == nullptr)
			continue;

		if (CurrencyTable->Type != InCurrencyType)
			continue;

		return CurrencyTable;
	}

	return nullptr;
}

bool WRCurrencyManager::IsFull(const EWRCurrencyType IN InCurrencyType)
{
	FWRCurrencyTable* CurrencyTable = FindCurrencyTable(InCurrencyType);
	if (CurrencyTable == nullptr)
		return false;

	if (FindCurrencyValue(InCurrencyType) >= CurrencyTable->LimitValue)
		return true;

	return false;
}

void WRCurrencyManager::SetCurrencyValue(const EWRCurrencyType IN InCurrencyType, CURRENCY_t IN InNewCurrencyValue)
{
	FWRCurrencyTable* CurrencyTable = FindCurrencyTable(InCurrencyType);
	if (CurrencyTable == nullptr)
		return;

	if (InNewCurrencyValue > CurrencyTable->LimitValue)
	{
		ensureMsgf(false, TEXT("Currency Exceeded the Maximum"));
		InNewCurrencyValue = CurrencyTable->LimitValue;
	}

	if (CurrencyTable->Category == EWRCurrencyCategory::Game_Money)
	{
		SetGameMoneyCurrencyValue(InCurrencyType, InNewCurrencyValue);
	}
	else if (CurrencyTable->Category == EWRCurrencyCategory::Cash_Money)
	{
		SetCashMoneyCurrencyValue(InCurrencyType, InNewCurrencyValue);
	}
	else if (CurrencyTable->Category == EWRCurrencyCategory::Point)
	{
		SetPointCurrencyValue(InCurrencyType, InNewCurrencyValue);
	}
	else if (CurrencyTable->Category == EWRCurrencyCategory::Ticket)
	{
		SetTicketCurrencyValue(InCurrencyType, InNewCurrencyValue);
	}
	else
	{
		ensureMsgf(false, TEXT("Invalid Currency_Categor"));
	}
}

EWRCurrencyCategory WRCurrencyManager::ConvertTypeToCategory(const EWRCurrencyType IN InCurrencyType)
{
	if (CATEGORY_GAME_MONEY_BEGIN <= uint8(InCurrencyType) &&  InCurrencyType < EWRCurrencyType::CATEGORY_GAME_MONEY_END)
		return EWRCurrencyCategory::Game_Money;

	if (CATEGORY_CASH_MONEY_BEGIN <= uint8(InCurrencyType) && InCurrencyType < EWRCurrencyType::CATEGORY_CASH_MONEY_END)
		return EWRCurrencyCategory::Cash_Money;

	return EWRCurrencyCategory::None;
}

CURRENCY_t WRCurrencyManager::FindCurrencyValue(const EWRCurrencyType IN InCurrencyType)
{
	EWRCurrencyCategory CurrencyCategory = ConvertTypeToCategory(InCurrencyType);

	if (CurrencyCategory == EWRCurrencyCategory::Game_Money)
	{
		return FindGameMoneyCurrencyValue(InCurrencyType);
	}
	else if (CurrencyCategory == EWRCurrencyCategory::Cash_Money)
	{
		return FindCashMoneyCurrencyValue(InCurrencyType);
	}
	else if (CurrencyCategory == EWRCurrencyCategory::Point)
	{
		return FindPointCurrencyValue(InCurrencyType);
	}
	else if (CurrencyCategory == EWRCurrencyCategory::Ticket)
	{
		return FindTicketCurrencyValue(InCurrencyType);
	}
	else
	{
		ensureMsgf(false, TEXT("Invalid Currency_Categor"));
	}

	return 0;
}

void WRCurrencyManager::InitCurrencyData()
{
	GameMoneyCurrencyValueMap.Empty();
	CashMoneyCurrencyValueMap.Empty();
	PointCurrencyValueMap.Empty();
	TicketCurrencyValueMap.Empty();

	UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRCurrencyTable>();
	if (DataTable == nullptr)
		return;

	const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();
	
	for (auto& iter : CurrencyData)
	{
		FWRCurrencyTable* CurrencyTable = reinterpret_cast<FWRCurrencyTable*>(iter.Value);
		if (CurrencyTable == nullptr)
			continue;

		if (CurrencyTable->Category == EWRCurrencyCategory::Game_Money)
		{
			GameMoneyCurrencyValueMap.Add(CurrencyTable->Type, 0);
		}
		else if (CurrencyTable->Category == EWRCurrencyCategory::Cash_Money)
		{
			CashMoneyCurrencyValueMap.Add(CurrencyTable->Type, 0);
		}
		else if (CurrencyTable->Category == EWRCurrencyCategory::Point)
		{
			PointCurrencyValueMap.Add(CurrencyTable->Type, 0);
		}
		else if (CurrencyTable->Category == EWRCurrencyCategory::Ticket)
		{
			TicketCurrencyValueMap.Add(CurrencyTable->Type, 0);
		}
		else
		{
			ensureMsgf(false, TEXT("Invalid Currency_Categor"));
		}
	}
}

void WRCurrencyManager::SetGameMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType, const GAME_MONEY_t IN InNewCurrencyValue)
{
	if (GameMoneyCurrencyValueMap.Contains(InCurrencyType) == false)
		return;

	GameMoneyCurrencyValueMap[InCurrencyType] = InNewCurrencyValue;
}

void WRCurrencyManager::SetCashMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType, const CASH_MONEY_t IN InNewCurrencyValue)
{
	if (CashMoneyCurrencyValueMap.Contains(InCurrencyType) == false)
		return;

	CashMoneyCurrencyValueMap[InCurrencyType] = InNewCurrencyValue;
}

void WRCurrencyManager::SetPointCurrencyValue(const EWRCurrencyType IN InCurrencyType, const POINT_t IN InNewCurrencyValue)
{
	if (PointCurrencyValueMap.Contains(InCurrencyType) == false)
		return;

	PointCurrencyValueMap[InCurrencyType] = InNewCurrencyValue;
}

void WRCurrencyManager::SetTicketCurrencyValue(const EWRCurrencyType IN InCurrencyType, const TICKET_t IN InNewCurrencyValue)
{
	if (TicketCurrencyValueMap.Contains(InCurrencyType) == false)
		return;

	TicketCurrencyValueMap[InCurrencyType] = InNewCurrencyValue;
}
