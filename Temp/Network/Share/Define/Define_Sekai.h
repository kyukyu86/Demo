//******************************************************************************
//	FileName: 	Define_Sekai.h
//------------------------------------------------------------------------------
//	Created	:	2020/07/27
//------------------------------------------------------------------------------
//	Author	:	GyuTae Lee
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	��ī�� ���� ����
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
	float				flowRate;					// ������ �帧�� - ����
	GAME_MONEY_t		purchasePrice;				// ���Ű� - ���� (�⺻�� + ������ �帧��)
	GAME_MONEY_t		sellingPrice;				// �ǸŰ� - ���� (�⺻�� + ������ �帧��)
	GAME_MONEY_t		commissionPurchasePrice;	// ��ȭ �������� ���� ���Ű� - ���� ��� �� (���Ű� + ��ȭ ��������)
	GAME_MONEY_t		commissionSellingPrice;		// ��ȭ �������� ���� �ǸŰ� - ���� ��� �� (�ǸŰ� + ��ȭ ��������)
	GAME_MONEY_t		floridaPurchasePrice;		// ���Ű� �÷θ��� ���� - �׷��� ǥ�ÿ� (�⺻�� + ��ȭ ��������)
	GAME_MONEY_t		floridaSellingPrice;		// �ǸŰ� �÷θ��� ���� - �׷��� ǥ�ÿ� (�⺻�� + ��ȭ ��������)
	stSmallDateTime		date;						// ���� ��¥

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
	float				commissionRate;		// ��ȭ ��������
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