//******************************************************************************
//	FileName: 	Define_Item.h
//------------------------------------------------------------------------------
//	Created	:	2019/09/17
//------------------------------------------------------------------------------
//	Author	:	gyutaelee
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	아이템 관련 정보
//******************************************************************************

#pragma once

#include "../Protocol/IMMessage.h"
#include "Define_BluePrint.h"

#define DEFAULT_INVENTORY_SIZE		100			// 기본 인벤토리 공간


//-----------------------------------------------------
//	아이템 출처
//-----------------------------------------------------
struct EWRItemSource
{
	enum en
	{
		DBLoad = 0,		// DB 로딩
		Cheat,			// 치트
		Shop,			// 상점
		Discard,		// 버리기
		Quest,			// 퀘스트
		LifeEvent,		// 일상 이벤트
		PickUp,			// 드랍 획득
		Use,			// 아이템 사용
		IO,				// 인터렉션 오브젝트

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(DBLoad,		L"DBLoad");
		InsertMap(Cheat,		L"Cheat");
		InsertMap(Shop,			L"Shop");
		InsertMap(Discard,		L"Discard");
		InsertMap(Quest,		L"Quest");
		InsertMap(LifeEvent,	L"LifeEvent");
		InsertMap(PickUp,		L"PickUp");
		InsertMap(Use,			L"Use");
		InsertMap(IO,			L"IO");
	}
};

struct stItem : IMSerialize
{
	ISN_t				ISN;
	TID_t				TID;
	ITEM_COUNT_t		itemCount;		// 소모품 전용
	uint8				itemType;		// EWRItemMainType::en

	stItem()
		: ISN(INVALID_ISN)
		, TID(INVALID_TID)
		, itemCount(0)
		, itemType((uint8)EWRItemMainType::MAX)
	{
	}
	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> ISN >> TID >> itemCount >> itemType;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << ISN << TID << itemCount << itemType;
	}
#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_STRUCT_NAME_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"stItem");
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN);
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"TID : %d", TID);
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"itemCount : %d", itemCount);
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"itemType : %u", itemType);
	}
#endif	// USE_PROTOCOL_TOSTRING
};