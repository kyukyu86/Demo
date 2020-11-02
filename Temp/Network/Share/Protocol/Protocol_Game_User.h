#pragma once


namespace pt_Game_User
{

struct Req_EnterGame_U2G : IMSerialize
{
	ID_t userid; // 주석
	DKEY_t userdkey; // 주석
	PROTOCOL_VERSION_t protocolversion; // 주석
	
	Req_EnterGame_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EnterGame_U2G);
		
		userid = INVALID_ID;
		userdkey = INVALID_DKEY;
		protocolversion = INVALID_PROTOCOL_VERSION;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> userid >> userdkey >> protocolversion;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << userid << userdkey << protocolversion;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"userid : %s", userid.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"userdkey : %u", userdkey );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"protocolversion : %u", protocolversion );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_EnterGame_G2U : IMSerialize
{
	RESULT_t result; // 주석
	USN_t usn;
	
	Ack_EnterGame_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EnterGame_G2U);
		
		result = INVALID_RESULT;
		usn = INVALID_USN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> usn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << usn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"usn : %u", usn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_KickUser_G2U : IMSerialize
{
	RESULT_t result; // 주석
	
	Inf_KickUser_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_KickUser_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_LogoutUser_U2G : IMSerialize
{
	
	Req_LogoutUser_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_LogoutUser_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_LogoutUser_G2U : IMSerialize
{
	RESULT_t result; // 주석
	
	Ack_LogoutUser_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_LogoutUser_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_MoveToLab_U2G : IMSerialize
{
	USN_t usn;
	
	Req_MoveToLab_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_MoveToLab_U2G);
		
		usn = INVALID_USN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> usn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << usn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"usn : %u", usn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_MoveToLab_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_MoveToLab_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_MoveToLab_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_PlayerList_U2G : IMSerialize
{
	
	Req_PlayerList_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_PlayerList_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerList_G2U : IMSerialize
{
	RESULT_t result;
	PSN_t psn;
	NAME_t playername;
	TID_t costumeTID;
	uint8 gender;
	stCustomizeInfo customizeInfo;
	
	Ack_PlayerList_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_PlayerList_G2U);
		
		result = INVALID_RESULT;
		psn = INVALID_PSN;
		playername = INVALID_NAME;
		costumeTID = INVALID_TID;
		gender = (uint8)EWRGender::Male;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> psn >> playername >> costumeTID >> gender >> customizeInfo;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << psn << playername << costumeTID << gender << customizeInfo;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playername : %s", playername.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"costumeTID : %d", costumeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gender : %u", gender );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, customizeInfo, L"stCustomizeInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SelectPlayerName_U2G : IMSerialize
{
	
	Req_SelectPlayerName_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_SelectPlayerName_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SelectPlayerName_G2U : IMSerialize
{
	RESULT_t result;
	NAME_t playername;
	
	Ack_SelectPlayerName_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_SelectPlayerName_G2U);
		
		result = INVALID_RESULT;
		playername = INVALID_NAME;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> playername;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << playername;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playername : %s", playername.c_str() );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_CreatePlayer_U2G : IMSerialize
{
	NAME_t playername;
	TID_t costumeTID;
	uint8 gender;
	stCustomizeInfo customizeInfo;
	std::list<TID_t> listCostumeItem;
	
	Req_CreatePlayer_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_CreatePlayer_U2G);
		
		playername = INVALID_NAME;
		costumeTID = INVALID_TID;
		gender = (uint8)EWRGender::Male;
		listCostumeItem.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playername >> costumeTID >> gender >> customizeInfo >> listCostumeItem;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playername << costumeTID << gender << customizeInfo << listCostumeItem;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playername : %s", playername.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"costumeTID : %d", costumeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gender : %u", gender );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, customizeInfo, L"stCustomizeInfo" );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listCostumeItem );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_CreatePlayer_G2U : IMSerialize
{
	RESULT_t result;
	PSN_t psn;
	NAME_t playername;
	TID_t costumeTID;
	uint8 gender;
	stCustomizeInfo customizeInfo;
	
	Ack_CreatePlayer_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_CreatePlayer_G2U);
		
		result = INVALID_RESULT;
		psn = INVALID_PSN;
		playername = INVALID_NAME;
		costumeTID = INVALID_TID;
		gender = (uint8)EWRGender::Male;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> psn >> playername >> costumeTID >> gender >> customizeInfo;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << psn << playername << costumeTID << gender << customizeInfo;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playername : %s", playername.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"costumeTID : %d", costumeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gender : %u", gender );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, customizeInfo, L"stCustomizeInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_DeletePlayer_U2G : IMSerialize
{
	PSN_t psn;
	
	Req_DeletePlayer_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_DeletePlayer_U2G);
		
		psn = INVALID_PSN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_DeletePlayer_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_DeletePlayer_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_DeletePlayer_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SelectPlayer_U2G : IMSerialize
{
	PSN_t psn;
	
	Req_SelectPlayer_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_SelectPlayer_U2G);
		
		psn = INVALID_PSN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SelectPlayer_G2U : IMSerialize
{
	RESULT_t result;
	stPlayerInfo playerInfo;
	
	Ack_SelectPlayer_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_SelectPlayer_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> playerInfo;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << playerInfo;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, playerInfo, L"stPlayerInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_ReserveMap_U2G : IMSerialize
{
	PSN_t psn;
	TID_t mapTID; // 이동하고자하는 맵 고유 인덱스
	TID_t mapConnectorNodeTID; // Map Connector 트리거 액션 노드
	
	Req_ReserveMap_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_ReserveMap_U2G);
		
		psn = INVALID_PSN;
		mapTID = INVALID_TID;
		mapConnectorNodeTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn >> mapTID >> mapConnectorNodeTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn << mapTID << mapConnectorNodeTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d", mapTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapConnectorNodeTID : %d", mapConnectorNodeTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_ReserveMap_G2U : IMSerialize
{
	RESULT_t result;
	TID_t mapTID;
	MAP_INDEX_t mapIndex; // 진입하게 될 맵 고유 인덱스
	bool bMoveToServer; // true 일경우 서버 이동 발생
	IP_t gameServerIP; // 해당 맵의 서버 아이피
	PORT_t gameServerPort; // 해당 맵의 서버 포트
	TID_t mapConnectorNodeTID; // Map Connector 트리거 액션 노드
	float destPosX; // Map Connector 사용하는 경우 - 이동하는 맵에서의 월드 좌표 (X축)
	float destPosY; // Map Connector 사용하는 경우 - 이동하는 맵에서의 월드 좌표 (Y축)
	float destPosZ; // Map Connector 사용하는 경우 - 이동하는 맵에서의 월드 좌표 (Z축)
	float destAngle; // Map Connector 사용하는 경우 - 이동하는 맵에서의 월드 좌표 (Z축)
	
	Ack_ReserveMap_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_ReserveMap_G2U);
		
		result = INVALID_RESULT;
		mapTID = INVALID_TID;
		mapIndex = INVALID_MAP_INDEX;
		bMoveToServer = false;
		gameServerIP = INVALID_IP;
		gameServerPort = INVALID_PORT;
		mapConnectorNodeTID = INVALID_TID;
		destPosX = 0.0f;
		destPosY = 0.0f;
		destPosZ = 0.0f;
		destAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> mapTID >> mapIndex >> bMoveToServer >> gameServerIP >> gameServerPort >> mapConnectorNodeTID >> destPosX >> destPosY >> destPosZ >> destAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << mapTID << mapIndex << bMoveToServer << gameServerIP << gameServerPort << mapConnectorNodeTID << destPosX << destPosY << destPosZ << destAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d", mapTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapIndex : %u", mapIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bMoveToServer : %d", bMoveToServer );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gameServerIP : %s", gameServerIP.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gameServerPort : %u", gameServerPort );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapConnectorNodeTID : %d", mapConnectorNodeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"destPosX : %f", destPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"destPosY : %f", destPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"destPosZ : %f", destPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"destAngle : %f", destAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_ExitMap_U2G : IMSerialize
{
	PSN_t psn;
	TID_t mapTID;
	MAP_INDEX_t mapIndex; // 나오려는 맵 고유 Index
	
	Req_ExitMap_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_ExitMap_U2G);
		
		psn = INVALID_PSN;
		mapTID = INVALID_TID;
		mapIndex = INVALID_MAP_INDEX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn >> mapTID >> mapIndex;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn << mapTID << mapIndex;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d", mapTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapIndex : %u", mapIndex );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_ExitMap_G2U : IMSerialize
{
	RESULT_t result;
	TID_t mapTID;
	MAP_INDEX_t mapIndex; // 나오려는 맵 고유 Index
	
	Ack_ExitMap_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_ExitMap_G2U);
		
		result = INVALID_RESULT;
		mapTID = INVALID_TID;
		mapIndex = INVALID_MAP_INDEX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> mapTID >> mapIndex;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << mapTID << mapIndex;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d", mapTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapIndex : %u", mapIndex );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_EnterMap_U2G : IMSerialize
{
	PSN_t psn;
	TID_t mapTID;
	MAP_INDEX_t mapIndex; // 이 맵으로 이동. 진입 실패시 에러
	TID_t coordinateTID; // 이 값이 존재하는 경우 CoordinateData 등록된 위치로 이동. 전달하는 mapTID 와 동일한 정보를 가져야 함
	float posX; // 입장하는 좌표 (X축), coordinateTID 및 이 값이 모두 0 인 경우 기본 룰 적용
	float posY; // 입장하는 좌표 (Y축), coordinateTID 및 이 값이 모두 0 인 경우 기본 룰 적용
	float posZ; // 입장하는 좌표 (Z축), coordinateTID 및 이 값이 모두 0 인 경우 기본 룰 적용
	float angle; // 입장하는 위치의 회전
	
	Req_EnterMap_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EnterMap_U2G);
		
		psn = INVALID_PSN;
		mapTID = INVALID_TID;
		mapIndex = INVALID_MAP_INDEX;
		coordinateTID = INVALID_TID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn >> mapTID >> mapIndex >> coordinateTID >> posX >> posY >> posZ >> angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn << mapTID << mapIndex << coordinateTID << posX << posY << posZ << angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d", mapTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapIndex : %u", mapIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"coordinateTID : %d", coordinateTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_EnterMap_G2U : IMSerialize
{
	RESULT_t result;
	TID_t mapTID;
	MAP_INDEX_t mapIndex; // 이 맵으로 이동. 진입 실패시 에러
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	stSmallDateTime mapOpenTime; // 맵이 생성된 시간
	stSmallDateTime curTime; // 현재 시간
	
	Ack_EnterMap_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EnterMap_G2U);
		
		result = INVALID_RESULT;
		mapTID = INVALID_TID;
		mapIndex = INVALID_MAP_INDEX;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> mapTID >> mapIndex >> posX >> posY >> posZ >> angle >> mapOpenTime >> curTime;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << mapTID << mapIndex << posX << posY << posZ << angle << mapOpenTime << curTime;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d", mapTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapIndex : %u", mapIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, mapOpenTime, L"stSmallDateTime" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, curTime, L"stSmallDateTime" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_ReadyToPlay_U2G : IMSerialize
{
	PSN_t psn;
	
	Req_ReadyToPlay_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_ReadyToPlay_U2G);
		
		psn = INVALID_PSN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_ReadyToPlay_G2U : IMSerialize
{
	RESULT_t result; // 현재 맵 안의 액터 정보 Inform 처리 후 마지막으로 전송
	
	Ack_ReadyToPlay_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_ReadyToPlay_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ReserveMapForGroup_G2U : IMSerialize
{
	PSN_t psn; // warp 진행하는 플레이어
	AID_t vehicleAID; // warp vehicle AID (워프를 진행한 객체)
	TID_t mapTID; // 그룹 이동해야하는 맵 고유 인덱스
	MAP_INDEX_t mapIndex; // 그룹 이동해야하는 될 맵 고유 인덱스
	bool bMoveToServer; // true 일경우 서버 이동 발생
	IP_t gameServerIP; // 해당 맵의 서버 아이피
	PORT_t gameServerPort; // 해당 맵의 서버 포트
	
	Inf_ReserveMapForGroup_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_ReserveMapForGroup_G2U);
		
		psn = INVALID_PSN;
		vehicleAID = INVALID_AID;
		mapTID = INVALID_TID;
		mapIndex = INVALID_MAP_INDEX;
		bMoveToServer = false;
		gameServerIP = INVALID_IP;
		gameServerPort = INVALID_PORT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn >> vehicleAID >> mapTID >> mapIndex >> bMoveToServer >> gameServerIP >> gameServerPort;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn << vehicleAID << mapTID << mapIndex << bMoveToServer << gameServerIP << gameServerPort;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d", mapTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapIndex : %u", mapIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bMoveToServer : %d", bMoveToServer );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gameServerIP : %s", gameServerIP.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gameServerPort : %u", gameServerPort );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 인게임 접속 후 클라에서 플레이어 정보 요청
struct Req_EntryInfo_U2G : IMSerialize
{
	
	Req_EntryInfo_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EntryInfo_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 인게임 접속 후 클라에서 플레이어 정보 응답
struct Ack_EntryInfo_G2U : IMSerialize
{
	RESULT_t result;
	stResultEntryInfo resultSub;
	
	Ack_EntryInfo_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EntryInfo_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> resultSub;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << resultSub;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, resultSub, L"stResultEntryInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 아이템 정보 리스트
struct Inf_EntryInfo_Item_G2U : IMSerialize
{
	std::list<stItem> listItem;
	bool bFinish;
	
	Inf_EntryInfo_Item_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_EntryInfo_Item_G2U);
		
		listItem.clear();
		bFinish = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listItem >> bFinish;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listItem << bFinish;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listItem, L"stItem" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bFinish : %d", bFinish );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 세카이 아이템 정보 알림
struct Inf_SekaiItemInfo_G2U : IMSerialize
{
	std::list<stSekaiItemInfo> listSekaiItemInfo;
	bool bFinish;
	
	Inf_SekaiItemInfo_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SekaiItemInfo_G2U);
		
		listSekaiItemInfo.clear();
		bFinish = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listSekaiItemInfo >> bFinish;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listSekaiItemInfo << bFinish;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listSekaiItemInfo, L"stSekaiItemInfo" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bFinish : %d", bFinish );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 세카이 재화 정보 알림
struct Inf_SekaiCurrencyInfo_G2U : IMSerialize
{
	std::list<stSekaiCurrencyInfo> listSekaiCurrencyInfo;
	bool bFinish;
	
	Inf_SekaiCurrencyInfo_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SekaiCurrencyInfo_G2U);
		
		listSekaiCurrencyInfo.clear();
		bFinish = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listSekaiCurrencyInfo >> bFinish;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listSekaiCurrencyInfo << bFinish;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listSekaiCurrencyInfo, L"stSekaiCurrencyInfo" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bFinish : %d", bFinish );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};
}
