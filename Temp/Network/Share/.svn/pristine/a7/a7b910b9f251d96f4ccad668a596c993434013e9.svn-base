//******************************************************************************
//	FileName: 	Define_Sekai.h
//------------------------------------------------------------------------------
//	Created	:	2020/07/27
//------------------------------------------------------------------------------
//	Author	:	GyuTae Lee
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	세카이 관련 정보
//******************************************************************************

#pragma once

#include "../Protocol/IMMessage.h"

#ifdef USE_CLIENT_ONLY
	#include "Define_Time.h"
	#include "Define_Blueprint.h"
#endif


struct stSekaiItemInfo : IMSerialize
{
	TID_t				itemTID;
	float				flowRate;					// 아이템 흐름률 - 계산용
	GAME_MONEY_t		purchasePrice;				// 구매가 - 계산용 (기본가 + 아이템 흐름률)
	GAME_MONEY_t		sellingPrice;				// 판매가 - 계산용 (기본가 + 아이템 흐름률)
	GAME_MONEY_t		commissionPurchasePrice;	// 재화 수수료율 적용 구매가 - 실제 사용 값 (구매가 + 재화 수수료율)
	GAME_MONEY_t		commissionSellingPrice;		// 재화 수수료율 적용 판매가 - 실제 사용 값 (판매가 + 재화 수수료율)
	GAME_MONEY_t		floridaPurchasePrice;		// 구매가 플로리다 지수 - 그래프 표시용 (기본가 + 재화 수수료율)
	GAME_MONEY_t		floridaSellingPrice;		// 판매가 플로리다 지수 - 그래프 표시용 (기본가 + 재화 수수료율)
	stSmallDateTime		date;						// 정산 날짜

	stSekaiItemInfo()
		: itemTID(INVALID_TID)
		, flowRate(0)
		, purchasePrice(0)
		, sellingPrice(0)
		, commissionPurchasePrice(0)
		, commissionSellingPrice(0)
		, floridaPurchasePrice(0)
		, floridaSellingPrice(0)
	{
	}
	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> itemTID >> flowRate >> purchasePrice >> sellingPrice >> commissionPurchasePrice >> commissionSellingPrice >> floridaPurchasePrice >> floridaSellingPrice >> date;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << itemTID << flowRate << purchasePrice << sellingPrice << commissionPurchasePrice << commissionSellingPrice << floridaPurchasePrice << floridaSellingPrice << date;
	}
#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{

	}
#endif	// USE_PROTOCOL_TOSTRING
};

struct stSekaiCurrencyInfo : IMSerialize
{
	uint8				curregncyType;		// EWRCurrencyType::en
	float				commissionRate;		// 재화 수수료율
	stSmallDateTime		date;

	stSekaiCurrencyInfo()
		: curregncyType((uint8)EWRCurrencyType::MAX)
		, commissionRate(0)
	{
	}
	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> curregncyType >> commissionRate >> date;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << curregncyType << commissionRate << date;
	}
#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{

	}
#endif	// USE_PROTOCOL_TOSTRING
};