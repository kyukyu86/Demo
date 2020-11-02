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
	// [ 2019-8-14 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

	void OnWorldEntryInitialized();
public:
	GAME_MONEY_t	FindGameMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType);
	CASH_MONEY_t	FindCashMoneyCurrencyValue(const EWRCurrencyType IN InCurrencyType);
	POINT_t			FindPointCurrencyValue(const EWRCurrencyType IN InCurrencyType);
	TICKET_t		FindTicketCurrencyValue(const EWRCurrencyType IN InCurrencyType);

	CURRENCY_t		FindLimitValue(const EWRCurrencyType IN InCurrencyType);		// 재화 제한수치 찾기
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
	TMap<EWRCurrencyType, GAME_MONEY_t> GameMoneyCurrencyValueMap;	// 카테고리가 GameMoney인 재화들 모음
	TMap<EWRCurrencyType, CASH_MONEY_t> CashMoneyCurrencyValueMap;	// 카테고리가 CashMoney인 재화들 모음
	TMap<EWRCurrencyType, POINT_t>		PointCurrencyValueMap;		// 카테고리가 Point인 재화들 모음
	TMap<EWRCurrencyType, TICKET_t>		TicketCurrencyValueMap;		// 카테고리가 Ticket인 재화들 모음
};

