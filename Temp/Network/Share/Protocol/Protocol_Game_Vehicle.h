#pragma once


namespace pt_Game_Vehicle
{

struct Inf_TransportState_G2U : IMSerialize
{
	AID_t aid; // Vehicle 액터 아이디
	uint8 vehicleState; // 운송 수단 상태
	uint8 curStation; // 현재 지점 (출발지점)
	uint8 nextStation; // 다음 지점 (도착지점)
	std::list<AID_t> lstPassengers; // 승객 리스트
	
	Inf_TransportState_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_TransportState_G2U);
		
		aid = INVALID_AID;
		vehicleState = 0;
		curStation = 0;
		nextStation = 0;
		lstPassengers.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> vehicleState >> curStation >> nextStation >> lstPassengers;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << vehicleState << curStation << nextStation << lstPassengers;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleState : %u", vehicleState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"curStation : %u", curStation );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"nextStation : %u", nextStation );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstPassengers );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_GetTransport_U2G : IMSerialize
{
	AID_t vehicleAID; // 탑승하려는 Vehicle 액터 아이디
	float localPosX; // 탑승체에서의 로컬 좌표
	float localPosY; // 탑승체에서의 로컬 좌표
	float localPosZ; // 탑승체에서의 로컬 좌표
	float localAngle; // 탑승체에서의 각도
	
	Req_GetTransport_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_GetTransport_U2G);
		
		vehicleAID = INVALID_AID;
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> localPosX >> localPosY >> localPosZ >> localAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << localPosX << localPosY << localPosZ << localAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_GetOnTransport_G2U : IMSerialize
{
	RESULT_t result; // 탑승 결과
	
	Ack_GetOnTransport_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_GetOnTransport_G2U);
		
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

struct Inf_GetOnTransport_G2U : IMSerialize
{
	AID_t passengerAID; // 탑승 액터 아이디
	AID_t vehicleAID; // 탑승 Vehicle 액터 아이디
	float localPosX; // 탑승체에서의 로컬 좌표
	float localPosY; // 탑승체에서의 로컬 좌표
	float localPosZ; // 탑승체에서의 로컬 좌표
	float localAngle; // 탑승체에서의 각도
	
	Inf_GetOnTransport_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GetOnTransport_G2U);
		
		passengerAID = INVALID_AID;
		vehicleAID = INVALID_AID;
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> passengerAID >> vehicleAID >> localPosX >> localPosY >> localPosZ >> localAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << passengerAID << vehicleAID << localPosX << localPosY << localPosZ << localAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"passengerAID : %llu", passengerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_GetOffTransport_U2G : IMSerialize
{
	AID_t vehicleAID; // 하차하려는 Vehicle 액터 아이디
	
	Req_GetOffTransport_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_GetOffTransport_U2G);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_GetOffTransport_G2U : IMSerialize
{
	RESULT_t result; // 하차 결과
	
	Ack_GetOffTransport_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_GetOffTransport_G2U);
		
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

struct Inf_GetOffTransport_G2U : IMSerialize
{
	AID_t passengerAID; // 하차하는 액터 아이디
	AID_t vehicleAID; // 하차하는 Vehicle 액터 아이디
	
	Inf_GetOffTransport_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GetOffTransport_G2U);
		
		passengerAID = INVALID_AID;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> passengerAID >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << passengerAID << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"passengerAID : %llu", passengerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_CompleteWarp_G2U : IMSerialize
{
	AID_t vehicleAID; // warp vehicle AID (워프를 진행한 객체)
	std::list<AID_t> lstPassenger; // 승객 리스트
	
	Inf_CompleteWarp_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_CompleteWarp_G2U);
		
		vehicleAID = INVALID_AID;
		lstPassenger.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> lstPassenger;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << lstPassenger;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstPassenger );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_GroupWarpTimerCount_G2U : IMSerialize
{
	AID_t vehicleAID; // warp vehicle AID (워프를 진행한 객체)
	uint8 readyCount; // 초단위
	bool bBegin; // true시 처음부터 시작됨을 의미
	
	Inf_GroupWarpTimerCount_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GroupWarpTimerCount_G2U);
		
		vehicleAID = INVALID_AID;
		readyCount = 0;
		bBegin = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> readyCount >> bBegin;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << readyCount << bBegin;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"readyCount : %u", readyCount );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bBegin : %d", bBegin );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_GroupWarpStop_G2U : IMSerialize
{
	AID_t vehicleAID; // warp vehicle AID (워프를 진행한 객체)
	
	Inf_GroupWarpStop_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GroupWarpStop_G2U);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_PlayerMoveInTransport_U2G : IMSerialize
{
	float localPosX; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosY; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosZ; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localAngle; // 탑승체에서의 각도
	float forwardScale; // 주석
	float rightScale; // 주석
	float rotate; // 주석
	
	Req_PlayerMoveInTransport_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_PlayerMoveInTransport_U2G);
		
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
		forwardScale = 0.0f;
		rightScale = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> localPosX >> localPosY >> localPosZ >> localAngle >> forwardScale >> rightScale >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << localPosX << localPosY << localPosZ << localAngle << forwardScale << rightScale << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"forwardScale : %f", forwardScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightScale : %f", rightScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerMoveInTransport_U2G : IMSerialize
{
	RESULT_t result; // 이동 결과
	
	Ack_PlayerMoveInTransport_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_PlayerMoveInTransport_U2G);
		
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

struct Inf_PlayerMoveInTransport_G2U : IMSerialize
{
	AID_t playerAID; // 주석
	float localPosX; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosY; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosZ; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localAngle; // 탑승체에서의 각도
	float forwardScale; // 주석
	float rightScale; // 주석
	float rotate; // 주석
	AID_t vehicleAID; // 현재 탑승하고 있는 탑승체
	
	Inf_PlayerMoveInTransport_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_PlayerMoveInTransport_G2U);
		
		playerAID = INVALID_AID;
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
		forwardScale = 0.0f;
		rightScale = 0.0f;
		rotate = 0.0f;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> localPosX >> localPosY >> localPosZ >> localAngle >> forwardScale >> rightScale >> rotate >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << localPosX << localPosY << localPosZ << localAngle << forwardScale << rightScale << rotate << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"forwardScale : %f", forwardScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightScale : %f", rightScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_PlayerStopInTransport_U2G : IMSerialize
{
	float localPosX; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosY; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosZ; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localAngle; // 탑승체에서의 각도
	
	Req_PlayerStopInTransport_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_PlayerStopInTransport_U2G);
		
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> localPosX >> localPosY >> localPosZ >> localAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << localPosX << localPosY << localPosZ << localAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerStopInTransport_G2U : IMSerialize
{
	RESULT_t result; // 이동 결과
	
	Ack_PlayerStopInTransport_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_PlayerStopInTransport_G2U);
		
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

struct Inf_PlayerStopInTransport_G2U : IMSerialize
{
	AID_t playerAID; // 주석
	float localPosX; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosY; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosZ; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localAngle; // 탑승체에서의 각도
	AID_t vehicleAID; // 현재 탑승하고 있는 탑승체
	
	Inf_PlayerStopInTransport_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_PlayerStopInTransport_G2U);
		
		playerAID = INVALID_AID;
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> localPosX >> localPosY >> localPosZ >> localAngle >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << localPosX << localPosY << localPosZ << localAngle << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ActorMoveStopInVehicle_G2U : IMSerialize
{
	AID_t actorID; // 주석
	float stop_localPosX; // 탑승체에서의 로컬 좌표, 목표
	float stop_localPosY; // 탑승체에서의 로컬 좌표, 목표
	float stop_localPosZ; // 탑승체에서의 로컬 좌표, 목표
	float stop_localAngle; // 탑승체에서의 각도
	AID_t vehicleAID; // 현재 탑승하고 있는 탑승체
	
	Inf_ActorMoveStopInVehicle_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_ActorMoveStopInVehicle_G2U);
		
		actorID = INVALID_AID;
		stop_localPosX = 0.0f;
		stop_localPosY = 0.0f;
		stop_localPosZ = 0.0f;
		stop_localAngle = 0.0f;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> stop_localPosX >> stop_localPosY >> stop_localPosZ >> stop_localAngle >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << stop_localPosX << stop_localPosY << stop_localPosZ << stop_localAngle << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localPosX : %f", stop_localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localPosY : %f", stop_localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localPosZ : %f", stop_localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localAngle : %f", stop_localAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_GetOnMW_U2G : IMSerialize
{
	float basePosX; // 기준 좌표 (X축)
	float basePosY; // 기준 좌표 (Y축)
	float basePosZ; // 기준 좌표 (Z축)
	
	Req_GetOnMW_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_GetOnMW_U2G);
		
		basePosX = 0.0f;
		basePosY = 0.0f;
		basePosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> basePosX >> basePosY >> basePosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << basePosX << basePosY << basePosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"basePosX : %f", basePosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"basePosY : %f", basePosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"basePosZ : %f", basePosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_GetOnMW_G2U : IMSerialize
{
	RESULT_t result; // 무빙워크 또는 물속 진입 결과
	stSmallDateTime startTime; // 탑승 시간
	
	Ack_GetOnMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_GetOnMW_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> startTime;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << startTime;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, startTime, L"stSmallDateTime" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_GetOnMW_G2U : IMSerialize
{
	AID_t aid; // 무빙워크 또는 물속 진입 액터 아이디
	stSmallDateTime startTime; // 탑승 시간
	float basePosX; // 기준 좌표 (X축)
	float basePosY; // 기준 좌표 (Y축)
	float basePosZ; // 기준 좌표 (Z축)
	
	Inf_GetOnMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GetOnMW_G2U);
		
		aid = INVALID_AID;
		basePosX = 0.0f;
		basePosY = 0.0f;
		basePosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> startTime >> basePosX >> basePosY >> basePosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << startTime << basePosX << basePosY << basePosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, startTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"basePosX : %f", basePosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"basePosY : %f", basePosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"basePosZ : %f", basePosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_GetOffMW_U2G : IMSerialize
{
	float arrivalPosX; // 도착 좌표 (X축)
	float arrivalPosY; // 도착 좌표 (Y축)
	float arrivalPosZ; // 도착 좌표 (Z축)
	float arrivalAngle; // 도착 지점 회전각
	
	Req_GetOffMW_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_GetOffMW_U2G);
		
		arrivalPosX = 0.0f;
		arrivalPosY = 0.0f;
		arrivalPosZ = 0.0f;
		arrivalAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> arrivalPosX >> arrivalPosY >> arrivalPosZ >> arrivalAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << arrivalPosX << arrivalPosY << arrivalPosZ << arrivalAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalPosX : %f", arrivalPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalPosY : %f", arrivalPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalPosZ : %f", arrivalPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalAngle : %f", arrivalAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_GetOffMW_G2U : IMSerialize
{
	RESULT_t result; // 무빙워크 또는 물속 나오기 결과
	
	Ack_GetOffMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_GetOffMW_G2U);
		
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

struct Inf_GetOffMW_G2U : IMSerialize
{
	AID_t aid; // 무빙워크 또는 물속 나오는 액터 아이디
	float arrivalPosX; // 도착 좌표 (X축)
	float arrivalPosY; // 도착 좌표 (Y축)
	float arrivalPosZ; // 도착 좌표 (Z축)
	float arrivalAngle; // 도착 지점 회전각
	
	Inf_GetOffMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GetOffMW_G2U);
		
		aid = INVALID_AID;
		arrivalPosX = 0.0f;
		arrivalPosY = 0.0f;
		arrivalPosZ = 0.0f;
		arrivalAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> arrivalPosX >> arrivalPosY >> arrivalPosZ >> arrivalAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << arrivalPosX << arrivalPosY << arrivalPosZ << arrivalAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalPosX : %f", arrivalPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalPosY : %f", arrivalPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalPosZ : %f", arrivalPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"arrivalAngle : %f", arrivalAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_PlayerMoveInMW_U2G : IMSerialize
{
	float localPosX; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosY; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosZ; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localAngle; // 탑승체에서의 각도
	float forwardScale; // 주석
	float rightScale; // 주석
	float rotate; // 주석
	
	Req_PlayerMoveInMW_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_PlayerMoveInMW_U2G);
		
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
		forwardScale = 0.0f;
		rightScale = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> localPosX >> localPosY >> localPosZ >> localAngle >> forwardScale >> rightScale >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << localPosX << localPosY << localPosZ << localAngle << forwardScale << rightScale << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"forwardScale : %f", forwardScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightScale : %f", rightScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerMoveInMW_G2U : IMSerialize
{
	RESULT_t result; // 이동 결과
	stSmallDateTime moveTime; // 이동한 시점에서의 시간
	
	Ack_PlayerMoveInMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_PlayerMoveInMW_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> moveTime;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << moveTime;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, moveTime, L"stSmallDateTime" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_PlayerMoveInMW_G2U : IMSerialize
{
	AID_t playerAID; // 주석
	stSmallDateTime moveTime; // 이동한 시점에서의 시간
	float localPosX; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosY; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localPosZ; // 탑승체에서의 로컬 좌표 ( 탑승체 좌표 - 내 좌표 )
	float localAngle; // 탑승체에서의 각도
	float forwardScale; // 주석
	float rightScale; // 주석
	float rotate; // 주석
	
	Inf_PlayerMoveInMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_PlayerMoveInMW_G2U);
		
		playerAID = INVALID_AID;
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
		forwardScale = 0.0f;
		rightScale = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> moveTime >> localPosX >> localPosY >> localPosZ >> localAngle >> forwardScale >> rightScale >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << moveTime << localPosX << localPosY << localPosZ << localAngle << forwardScale << rightScale << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, moveTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"forwardScale : %f", forwardScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightScale : %f", rightScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_PlayerStopInMW_U2G : IMSerialize
{
	float localPosX; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosY; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosZ; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localAngle; // 탑승체에서의 각도
	
	Req_PlayerStopInMW_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_PlayerStopInMW_U2G);
		
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> localPosX >> localPosY >> localPosZ >> localAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << localPosX << localPosY << localPosZ << localAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerStopInMW_G2U : IMSerialize
{
	RESULT_t result; // 이동 결과
	stSmallDateTime stopTime; // 정지한 시점에서의 시간
	
	Ack_PlayerStopInMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_PlayerStopInMW_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> stopTime;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << stopTime;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, stopTime, L"stSmallDateTime" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_PlayerStopInMW_G2U : IMSerialize
{
	AID_t playerAID; // 주석
	stSmallDateTime stopTime; // 정지한 시점에서의 시간
	float localPosX; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosY; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localPosZ; // 탑승체에서의 로컬 좌표, Req_PlayerStop_U2G 참조
	float localAngle; // 탑승체에서의 각도
	
	Inf_PlayerStopInMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_PlayerStopInMW_G2U);
		
		playerAID = INVALID_AID;
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> stopTime >> localPosX >> localPosY >> localPosZ >> localAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << stopTime << localPosX << localPosY << localPosZ << localAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, stopTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ActorMoveStopInMW_G2U : IMSerialize
{
	AID_t actorID; // 주석
	stSmallDateTime stopTime; // 정지한 시점에서의 시간
	float stop_localPosX; // 탑승체에서의 로컬 좌표, 목표
	float stop_localPosY; // 탑승체에서의 로컬 좌표, 목표
	float stop_localPosZ; // 탑승체에서의 로컬 좌표, 목표
	float stop_localAngle; // 탑승체에서의 각도
	
	Inf_ActorMoveStopInMW_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_ActorMoveStopInMW_G2U);
		
		actorID = INVALID_AID;
		stop_localPosX = 0.0f;
		stop_localPosY = 0.0f;
		stop_localPosZ = 0.0f;
		stop_localAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> stopTime >> stop_localPosX >> stop_localPosY >> stop_localPosZ >> stop_localAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << stopTime << stop_localPosX << stop_localPosY << stop_localPosZ << stop_localAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, stopTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localPosX : %f", stop_localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localPosY : %f", stop_localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localPosZ : %f", stop_localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_localAngle : %f", stop_localAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SpawnVehicle_U2G : IMSerialize
{
	AID_t actorID; // 탑승체를 생성하는 엑터 아이디(차량 주인)
	TID_t vehicleTID; // 탑승체 TID
	ISN_t itemSN; // 소환 아이템 ISN
	float posX; // 탑승체에서의 좌표
	float posY; // 탑승체에서의 좌표
	float posZ; // 탑승체에서의 좌표
	float Angle; // 탑승체 각도
	
	Req_SpawnVehicle_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_SpawnVehicle_U2G);
		
		actorID = INVALID_AID;
		vehicleTID = INVALID_TID;
		itemSN = INVALID_ISN;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		Angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> vehicleTID >> itemSN >> posX >> posY >> posZ >> Angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << vehicleTID << itemSN << posX << posY << posZ << Angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleTID : %d", vehicleTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"itemSN : %llu", itemSN );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Angle : %f", Angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SpawnVehicle_G2U : IMSerialize
{
	RESULT_t result; // 탑승체 생성 결과 - 세부 내용은 appear packet으로
	ISN_t itemSN; // 소환 아이템 ISN
	
	Ack_SpawnVehicle_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_SpawnVehicle_G2U);
		
		result = INVALID_RESULT;
		itemSN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> itemSN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << itemSN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"itemSN : %llu", itemSN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_UnspawnVehicle_U2G : IMSerialize
{
	AID_t vehicleAID; // 탑승체AID
	ISN_t itemSN; // 소환 아이템 ISN
	
	Req_UnspawnVehicle_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_UnspawnVehicle_U2G);
		
		vehicleAID = INVALID_AID;
		itemSN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> itemSN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << itemSN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"itemSN : %llu", itemSN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_UnspawnVehicle_G2U : IMSerialize
{
	RESULT_t result; // 탑승체 생성 결과 - 세부 내용은 appear packet으로
	ISN_t itemSN; // 소환 아이템 ISN
	
	Ack_UnspawnVehicle_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_UnspawnVehicle_G2U);
		
		result = INVALID_RESULT;
		itemSN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> itemSN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << itemSN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"itemSN : %llu", itemSN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_GetOnVehicle_U2G : IMSerialize
{
	AID_t actorID; // 탑승자
	uint8 seatIndex; // 주석
	AID_t vehicleAID; // 탑승체
	
	Req_GetOnVehicle_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_GetOnVehicle_U2G);
		
		actorID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> seatIndex >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << seatIndex << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_GetOnVehicle_G2U : IMSerialize
{
	RESULT_t result; // 탑승 결과
	
	Ack_GetOnVehicle_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_GetOnVehicle_G2U);
		
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

struct Inf_GetOnVehicle_G2U : IMSerialize
{
	AID_t actorID; // 탑승자
	uint8 seatIndex; // 주석
	AID_t vehicleAID; // 탑승체
	
	Inf_GetOnVehicle_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GetOnVehicle_G2U);
		
		actorID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> seatIndex >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << seatIndex << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 승객 탑승 허락 요청
struct Req_AgreeToBoard_U2G : IMSerialize
{
	AID_t passengerAID; // 승객
	uint8 seatIndex; // 좌석
	AID_t vehicleAID; // 탑승체
	
	Req_AgreeToBoard_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_AgreeToBoard_U2G);
		
		passengerAID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> passengerAID >> seatIndex >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << passengerAID << seatIndex << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"passengerAID : %llu", passengerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 승객 탑승 허락 응답
struct Ack_AgreeToBoard_G2U : IMSerialize
{
	RESULT_t result; // 탑승 결과
	AID_t passengerAID; // 승객
	uint8 seatIndex; // 좌석
	AID_t vehicleAID; // 탑승체
	bool agree; // 동의여부
	
	Ack_AgreeToBoard_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_AgreeToBoard_G2U);
		
		result = INVALID_RESULT;
		passengerAID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
		vehicleAID = INVALID_AID;
		agree = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> passengerAID >> seatIndex >> vehicleAID >> agree;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << passengerAID << seatIndex << vehicleAID << agree;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"passengerAID : %llu", passengerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"agree : %d", agree );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 승객 탑승 허락 요청
struct Cmd_AgreeToBoard_G2U : IMSerialize
{
	AID_t passengerAID; // 승객
	uint8 seatIndex; // 좌석
	AID_t vehicleAID; // 탑승체
	
	Cmd_AgreeToBoard_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Cmd_AgreeToBoard_G2U);
		
		passengerAID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> passengerAID >> seatIndex >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << passengerAID << seatIndex << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"passengerAID : %llu", passengerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 승객 탑승 허락 응답
struct Rpt_AgreeToBoard_U2G : IMSerialize
{
	AID_t passengerAID; // 승객
	uint8 seatIndex; // 좌석번호
	AID_t vehicleAID; // 탑승체
	bool agree; // 동의여부
	
	Rpt_AgreeToBoard_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Rpt_AgreeToBoard_U2G);
		
		passengerAID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
		vehicleAID = INVALID_AID;
		agree = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> passengerAID >> seatIndex >> vehicleAID >> agree;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << passengerAID << seatIndex << vehicleAID << agree;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"passengerAID : %llu", passengerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"agree : %d", agree );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_GetOffVehicle_U2G : IMSerialize
{
	AID_t actorID; // 탑승자
	AID_t vehicleAID; // 탑승체
	
	Req_GetOffVehicle_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_GetOffVehicle_U2G);
		
		actorID = INVALID_AID;
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_GetOffVehicle_G2U : IMSerialize
{
	RESULT_t result; // 탑승 결과
	
	Ack_GetOffVehicle_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_GetOffVehicle_G2U);
		
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

struct Inf_GetOffVehicle_G2U : IMSerialize
{
	AID_t actorID; // 탑승자
	AID_t vehicleAID; // 탑승체
	uint8 seatIndex; // 좌석번호
	
	Inf_GetOffVehicle_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_GetOffVehicle_G2U);
		
		actorID = INVALID_AID;
		vehicleAID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> vehicleAID >> seatIndex;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << vehicleAID << seatIndex;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_BeginBrake_U2G : IMSerialize
{
	AID_t vehicleAID;
	float posX; // 탑승체에서의 좌표
	float posY; // 탑승체에서의 좌표
	float posZ; // 탑승체에서의 좌표
	float Angle; // 탑승체 각도
	
	Req_BeginBrake_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_BeginBrake_U2G);
		
		vehicleAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		Angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> posX >> posY >> posZ >> Angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << posX << posY << posZ << Angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Angle : %f", Angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_BeginBrake_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_BeginBrake_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_BeginBrake_G2U);
		
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

struct Inf_BeginBrake_G2U : IMSerialize
{
	AID_t vehicleAID;
	float posX; // 탑승체에서의 좌표
	float posY; // 탑승체에서의 좌표
	float posZ; // 탑승체에서의 좌표
	float Angle; // 탑승체 각도
	
	Inf_BeginBrake_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_BeginBrake_G2U);
		
		vehicleAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		Angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> posX >> posY >> posZ >> Angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << posX << posY << posZ << Angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Angle : %f", Angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_EndBrake_U2G : IMSerialize
{
	AID_t vehicleAID;
	float posX; // 탑승체에서의 좌표
	float posY; // 탑승체에서의 좌표
	float posZ; // 탑승체에서의 좌표
	float Angle; // 탑승체 각도
	
	Req_EndBrake_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_EndBrake_U2G);
		
		vehicleAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		Angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> posX >> posY >> posZ >> Angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << posX << posY << posZ << Angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Angle : %f", Angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_EndBrake_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_EndBrake_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_EndBrake_G2U);
		
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

struct Inf_EndBrake_G2U : IMSerialize
{
	AID_t vehicleAID;
	float posX; // 탑승체에서의 좌표
	float posY; // 탑승체에서의 좌표
	float posZ; // 탑승체에서의 좌표
	float Angle; // 탑승체 각도
	
	Inf_EndBrake_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_EndBrake_G2U);
		
		vehicleAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		Angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> posX >> posY >> posZ >> Angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << posX << posY << posZ << Angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Angle : %f", Angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_Acceleration_U2G : IMSerialize
{
	AID_t vehicleAID;
	float InputPower; // 가속
	float posX; // 탑승체에서의 좌표
	float posY; // 탑승체에서의 좌표
	float posZ; // 탑승체에서의 좌표
	float Speed; // 탑승체 속도
	float Pitch; // 앞뒤 기우뚱
	float Yaw; // 각도
	float Roll; // 좌우기우둥
	
	Req_Acceleration_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_Acceleration_U2G);
		
		vehicleAID = INVALID_AID;
		InputPower = 0.0f;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		Speed = 0.0f;
		Pitch = 0.0f;
		Yaw = 0.0f;
		Roll = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> InputPower >> posX >> posY >> posZ >> Speed >> Pitch >> Yaw >> Roll;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << InputPower << posX << posY << posZ << Speed << Pitch << Yaw << Roll;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InputPower : %f", InputPower );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Speed : %f", Speed );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Pitch : %f", Pitch );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Yaw : %f", Yaw );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Roll : %f", Roll );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_Acceleration_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_Acceleration_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_Acceleration_G2U);
		
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

struct Inf_Acceleration_G2U : IMSerialize
{
	AID_t vehicleAID;
	float InputPower; // 가속
	float posX; // 탑승체에서의 좌표
	float posY; // 탑승체에서의 좌표
	float posZ; // 탑승체에서의 좌표
	float Speed; // 탑승체 속도
	float Pitch; // 앞뒤 기우뚱
	float Yaw; // 각도
	float Roll; // 좌우기우둥
	
	Inf_Acceleration_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_Acceleration_G2U);
		
		vehicleAID = INVALID_AID;
		InputPower = 0.0f;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		Speed = 0.0f;
		Pitch = 0.0f;
		Yaw = 0.0f;
		Roll = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> InputPower >> posX >> posY >> posZ >> Speed >> Pitch >> Yaw >> Roll;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << InputPower << posX << posY << posZ << Speed << Pitch << Yaw << Roll;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InputPower : %f", InputPower );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Speed : %f", Speed );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Pitch : %f", Pitch );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Yaw : %f", Yaw );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"Roll : %f", Roll );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_Steering_U2G : IMSerialize
{
	AID_t vehicleAID;
	float steering; // 헨들
	
	Req_Steering_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_Steering_U2G);
		
		vehicleAID = INVALID_AID;
		steering = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> steering;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << steering;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"steering : %f", steering );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_Steering_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_Steering_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_Steering_G2U);
		
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

struct Inf_Steering_G2U : IMSerialize
{
	AID_t vehicleAID;
	float steering; // 헨들
	
	Inf_Steering_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_Steering_G2U);
		
		vehicleAID = INVALID_AID;
		steering = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> steering;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << steering;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"steering : %f", steering );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_BeHorning_U2G : IMSerialize
{
	AID_t vehicleAID;
	
	Req_BeHorning_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_BeHorning_U2G);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_BeHorning_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_BeHorning_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_BeHorning_G2U);
		
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

struct Inf_BeHorning_G2U : IMSerialize
{
	AID_t vehicleAID;
	
	Inf_BeHorning_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_BeHorning_G2U);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_UpdateExterior_U2G : IMSerialize
{
	AID_t vehicleAID;
	stVehicleExteriorInfo Exterior; // 탑승 시간
	
	Req_UpdateExterior_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_UpdateExterior_U2G);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> Exterior;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << Exterior;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Exterior, L"stVehicleExteriorInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_UpdateExterior_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_UpdateExterior_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_UpdateExterior_G2U);
		
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

struct Inf_UpdateExterior_G2U : IMSerialize
{
	AID_t vehicleAID;
	stVehicleExteriorInfo Exterior; // 탑승 시간
	
	Inf_UpdateExterior_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_UpdateExterior_G2U);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> Exterior;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << Exterior;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Exterior, L"stVehicleExteriorInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_CancelOnBoard_G2U : IMSerialize
{
	AID_t vehicleAID;
	AID_t actorID; // 좌석 탑승 요청 취소
	uint8 seatIndex; // 좌석번호
	
	Inf_CancelOnBoard_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_CancelOnBoard_G2U);
		
		vehicleAID = INVALID_AID;
		actorID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> actorID >> seatIndex;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << actorID << seatIndex;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SyncPhysicsforce_U2G : IMSerialize
{
	AID_t vehicleAID;
	stVector3 force; // 힘방향
	stVector3 location; // 위치
	
	Req_SyncPhysicsforce_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Req_SyncPhysicsforce_U2G);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> force >> location;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << force << location;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, force, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, location, L"stVector3" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SyncPhysicsforce_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_SyncPhysicsforce_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Ack_SyncPhysicsforce_G2U);
		
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

struct Inf_SyncPhysicsforce_G2U : IMSerialize
{
	AID_t vehicleAID;
	stVector3 force; // 힘방향
	stVector3 location; // 위치
	
	Inf_SyncPhysicsforce_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Vehicle, EGameVehicleType::Inf_SyncPhysicsforce_G2U);
		
		vehicleAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> force >> location;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << force << location;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, force, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, location, L"stVector3" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};
}
