// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Network/Share/Define/Define_Type.h"

/**
*
*/

// 


class WR_API WRCurrencyManager : public WRSingleton<WRCurrencyManager>
{
	CDECLARE_MANAGER(WRCurrencyManager);
	
private:
	WRCurrencyManager() {}
	virtual ~WRCurrencyManager() {}
	
public:
	//====================================================================================
	// [ 2019-8-14 : hansang88 ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

	void OnWorldEntryInitialized();
public:
	GAME_MONEY_t	FindGameMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType);
	CASH_MONEY_t	FindCashMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType);
	POINT_t			FindPointCurrencyValue(const EWRCurrencyType IN InCurrencyType);
	TICKET_t		FindTicketCurrencyValue(const EWRCurrencyType IN InCurrencyType);

	CURRENCY_t		FindLimitValue(const EWRCurrencyType IN InCurrencyType);		// ��ȭ ���Ѽ�ġ ã��
	struct FWRCurrencyTable* FindCurrencyTable(const EWRCurrencyType IN InCurrencyType);	// 

	bool IsFull(const EWRCurrencyType IN InCurrencyType);

	void SetCurrencyValue(const EWRCurrencyType IN InCurrencyType, CURRENCY_t IN InNewCurrencyValue);	
	EWRCurrencyCategory ConvertTypeToCategory(const EWRCurrencyType IN InCurrencyType);

public:
	// Packet!!
private:
	CURRENCY_t FindCurrencyValue(const EWRCurrencyType IN InCurrencyType);
	void InitCurrencyData();

	void SetGameMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType, const GAME_MONEY_t IN InNewCurrencyValue);
	void SetCashMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType, const CASH_MONEY_t IN InNewCurrencyValue);
	void SetPointCurrencyValue(const EWRCurrencyType IN InCurrencyType, const POINT_t IN InNewCurrencyValue);
	void SetTicketCurrencyValue(const EWRCurrencyType IN InCurrencyType, const TICKET_t IN InNewCurrencyValue);
private:
	TMap<EWRCurrencyType, GAME_MONEY_t> GameMoneyCurrencyValueMap;	// ī�װ��� GameMoney�� ��ȭ�� ����
	TMap<EWRCurrencyType, CASH_MONEY_t> CashMoneyCurrencyValueMap;	// ī�װ��� CashMoney�� ��ȭ�� ����
	TMap<EWRCurrencyType, POINT_t>		PointCurrencyValueMap;		// ī�װ��� Point�� ��ȭ�� ����
	TMap<EWRCurrencyType, TICKET_t>		TicketCurrencyValueMap;		// ī�װ��� Ticket�� ��ȭ�� ����
};

