#pragma once

#ifdef USE_CLIENT_ONLY
	#include "IMMessage.h"
	#include "ProtocolHeader.h"
	#include "../Define/Define_Item.h"
#endif

namespace pt_Game_User_Inventory
{

// 장비 장착/교체 요청
struct Req_EquipItem_U2G : IMSerialize
{
	ISN_t ISN;
	
	Req_EquipItem_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_EquipItem_U2G);
		
		ISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장비 장착/교체 응답
struct Ack_EquipItem_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t ISN;
	
	Ack_EquipItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_EquipItem_G2U);
		
		result = INVALID_RESULT;
		ISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> ISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << ISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장비 탈착 요청
struct Req_UnEquipItem_U2G : IMSerialize
{
	ISN_t ISN;
	
	Req_UnEquipItem_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_UnEquipItem_U2G);
		
		ISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장비 탈착 응답
struct Ack_UnEquipItem_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t ISN;
	
	Ack_UnEquipItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_UnEquipItem_G2U);
		
		result = INVALID_RESULT;
		ISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> ISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << ISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장비 슬롯 등록/변경 요청
struct Req_RegisterEquipmentSlot_U2G : IMSerialize
{
	ISN_t ISN;
	uint8 slotType; // EWRRegistrationSlotType::en
	
	Req_RegisterEquipmentSlot_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_RegisterEquipmentSlot_U2G);
		
		ISN = INVALID_ISN;
		slotType = (uint8)EWRRegistrationSlotType::MAX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ISN >> slotType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ISN << slotType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"slotType : %u", slotType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장비 슬롯 등록/변경 응답
struct Ack_RegisterEquipmentSlot_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t ISN;
	uint8 slotType; // EWRRegistrationSlotType::en
	bool bIsAlreadyEquippedSlot; // 해당 슬롯이 이미 장착중인가?
	
	Ack_RegisterEquipmentSlot_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_RegisterEquipmentSlot_G2U);
		
		result = INVALID_RESULT;
		ISN = INVALID_ISN;
		slotType = (uint8)EWRRegistrationSlotType::MAX;
		bIsAlreadyEquippedSlot = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> ISN >> slotType >> bIsAlreadyEquippedSlot;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << ISN << slotType << bIsAlreadyEquippedSlot;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"slotType : %u", slotType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bIsAlreadyEquippedSlot : %d", bIsAlreadyEquippedSlot );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장비 슬롯 해제 요청
struct Req_UnRegisterEquipmentSlot_U2G : IMSerialize
{
	ISN_t ISN;
	
	Req_UnRegisterEquipmentSlot_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_UnRegisterEquipmentSlot_U2G);
		
		ISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장비 슬롯 해제 응답
struct Ack_UnRegisterEquipmentSlot_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t ISN;
	uint8 slotType; // EWRRegistrationSlotType::en
	bool bIsAlreadyEquippedSlot; // 해당 슬롯이 이미 장착중인가?
	
	Ack_UnRegisterEquipmentSlot_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_UnRegisterEquipmentSlot_G2U);
		
		result = INVALID_RESULT;
		ISN = INVALID_ISN;
		slotType = (uint8)EWRRegistrationSlotType::MAX;
		bIsAlreadyEquippedSlot = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> ISN >> slotType >> bIsAlreadyEquippedSlot;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << ISN << slotType << bIsAlreadyEquippedSlot;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"slotType : %u", slotType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bIsAlreadyEquippedSlot : %d", bIsAlreadyEquippedSlot );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장착 장비 정보 알림
struct Inf_ChangeEquipment_G2U : IMSerialize
{
	AID_t AID; // 플레이어 액터 아이디 (서버에서 생성한 키값)
	TID_t leftWeaponTID; // 왼손 무기
	TID_t rightWeaponTID; // 오른손 무기
	TID_t temporaryTID; // 임시 장착류
	
	Inf_ChangeEquipment_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Inf_ChangeEquipment_G2U);
		
		AID = INVALID_AID;
		leftWeaponTID = INVALID_TID;
		rightWeaponTID = INVALID_TID;
		temporaryTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> AID >> leftWeaponTID >> rightWeaponTID >> temporaryTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << AID << leftWeaponTID << rightWeaponTID << temporaryTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AID : %llu", AID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"leftWeaponTID : %d", leftWeaponTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightWeaponTID : %d", rightWeaponTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"temporaryTID : %d", temporaryTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 장착 의상 정보 알림
struct Inf_ChangeCostume_G2U : IMSerialize
{
	AID_t AID; // 플레이어 액터 아이디 (서버에서 생성한 키값)
	TID_t topTID; // 상의류
	TID_t bottomTID; // 하의류
	TID_t shoesTID; // 신발류
	TID_t hatTID; // 모자류
	TID_t glovesTID; // 장갑류
	TID_t glassesTID; // 안경류
	TID_t neckTID; // 목도리 및 스카프류
	TID_t maskTID; // 마스크류
	TID_t backTID; // 망토류
	
	Inf_ChangeCostume_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Inf_ChangeCostume_G2U);
		
		AID = INVALID_AID;
		topTID = INVALID_TID;
		bottomTID = INVALID_TID;
		shoesTID = INVALID_TID;
		hatTID = INVALID_TID;
		glovesTID = INVALID_TID;
		glassesTID = INVALID_TID;
		neckTID = INVALID_TID;
		maskTID = INVALID_TID;
		backTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> AID >> topTID >> bottomTID >> shoesTID >> hatTID >> glovesTID >> glassesTID >> neckTID >> maskTID >> backTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << AID << topTID << bottomTID << shoesTID << hatTID << glovesTID << glassesTID << neckTID << maskTID << backTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AID : %llu", AID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"topTID : %d", topTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bottomTID : %d", bottomTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"shoesTID : %d", shoesTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hatTID : %d", hatTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"glovesTID : %d", glovesTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"glassesTID : %d", glassesTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"neckTID : %d", neckTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"maskTID : %d", maskTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"backTID : %d", backTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 생성 알림
struct Inf_AddItem_G2U : IMSerialize
{
	std::list<stItem> listItem;
	
	Inf_AddItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Inf_AddItem_G2U);
		
		listItem.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listItem;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listItem;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listItem, L"stItem" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 제거 알림
struct Inf_DeleteItem_G2U : IMSerialize
{
	std::list<stItem> listItem;
	
	Inf_DeleteItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Inf_DeleteItem_G2U);
		
		listItem.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listItem;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listItem;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listItem, L"stItem" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 개수 변경 알림
struct Inf_UpdateItemCount_G2U : IMSerialize
{
	std::list<stItem> listItem;
	
	Inf_UpdateItemCount_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Inf_UpdateItemCount_G2U);
		
		listItem.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listItem;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listItem;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listItem, L"stItem" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 버리기 요청
struct Req_DiscardItem_U2G : IMSerialize
{
	ISN_t ISN;
	ITEM_COUNT_t itemCount;
	
	Req_DiscardItem_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_DiscardItem_U2G);
		
		ISN = INVALID_ISN;
		itemCount = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ISN >> itemCount;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ISN << itemCount;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"itemCount : %d", itemCount );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 버리기 응답
struct Ack_DiscardItem_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t ISN;
	
	Ack_DiscardItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_DiscardItem_G2U);
		
		result = INVALID_RESULT;
		ISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> ISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << ISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 사용 요청
struct Req_UseItem_U2G : IMSerialize
{
	ISN_t ISN;
	ITEM_COUNT_t itemCount;
	
	Req_UseItem_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_UseItem_U2G);
		
		ISN = INVALID_ISN;
		itemCount = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ISN >> itemCount;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ISN << itemCount;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"itemCount : %d", itemCount );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 사용 응답
struct Ack_UseItem_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t ISN;
	
	Ack_UseItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_UseItem_G2U);
		
		result = INVALID_RESULT;
		ISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> ISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << ISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ISN : %llu", ISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 인테리어 아이템 배치
struct Req_ArrangeInteriorItem_U2G : IMSerialize
{
	ISN_t isn;
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	
	Req_ArrangeInteriorItem_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_ArrangeInteriorItem_U2G);
		
		isn = INVALID_ISN;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> isn >> posX >> posY >> posZ >> angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << isn << posX << posY << posZ << angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"isn : %llu", isn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 사용 응답
struct Ack_ArrangeInteriorItem_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t isn;
	
	Ack_ArrangeInteriorItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_ArrangeInteriorItem_G2U);
		
		result = INVALID_RESULT;
		isn = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> isn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << isn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"isn : %llu", isn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 인테리어 아이템 배치
struct Req_RemoveInteriorItem_U2G : IMSerialize
{
	ISN_t isn;
	
	Req_RemoveInteriorItem_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Req_RemoveInteriorItem_U2G);
		
		isn = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> isn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << isn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"isn : %llu", isn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 사용 응답
struct Ack_RemoveInteriorItem_G2U : IMSerialize
{
	RESULT_t result;
	ISN_t isn;
	
	Ack_RemoveInteriorItem_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Inventory, EGameUserInventoryType::Ack_RemoveInteriorItem_G2U);
		
		result = INVALID_RESULT;
		isn = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> isn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << isn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"isn : %llu", isn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};
}
