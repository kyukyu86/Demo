#pragma once


namespace pt_Game_Vehicle
{

struct Inf_TransportState_G2U : IMSerialize
{
	AID_t aid; // Vehicle ���� ���̵�
	uint8 vehicleState; // ��� ���� ����
	uint8 curStation; // ���� ���� (�������)
	uint8 nextStation; // ���� ���� (��������)
	std::list<AID_t> lstPassengers; // �°� ����Ʈ
	
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
	AID_t vehicleAID; // ž���Ϸ��� Vehicle ���� ���̵�
	float localPosX; // ž��ü������ ���� ��ǥ
	float localPosY; // ž��ü������ ���� ��ǥ
	float localPosZ; // ž��ü������ ���� ��ǥ
	float localAngle; // ž��ü������ ����
	
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
	RESULT_t result; // ž�� ���
	
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
	AID_t passengerAID; // ž�� ���� ���̵�
	AID_t vehicleAID; // ž�� Vehicle ���� ���̵�
	float localPosX; // ž��ü������ ���� ��ǥ
	float localPosY; // ž��ü������ ���� ��ǥ
	float localPosZ; // ž��ü������ ���� ��ǥ
	float localAngle; // ž��ü������ ����
	
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
	AID_t vehicleAID; // �����Ϸ��� Vehicle ���� ���̵�
	
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
	RESULT_t result; // ���� ���
	
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
	AID_t passengerAID; // �����ϴ� ���� ���̵�
	AID_t vehicleAID; // �����ϴ� Vehicle ���� ���̵�
	
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
	AID_t vehicleAID; // warp vehicle AID (������ ������ ��ü)
	std::list<AID_t> lstPassenger; // �°� ����Ʈ
	
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
	AID_t vehicleAID; // warp vehicle AID (������ ������ ��ü)
	uint8 readyCount; // �ʴ���
	bool bBegin; // true�� ó������ ���۵��� �ǹ�
	
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
	AID_t vehicleAID; // warp vehicle AID (������ ������ ��ü)
	
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
	float localPosX; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosY; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosZ; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localAngle; // ž��ü������ ����
	float forwardScale; // �ּ�
	float rightScale; // �ּ�
	float rotate; // �ּ�
	
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
	RESULT_t result; // �̵� ���
	
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
	AID_t playerAID; // �ּ�
	float localPosX; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosY; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosZ; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localAngle; // ž��ü������ ����
	float forwardScale; // �ּ�
	float rightScale; // �ּ�
	float rotate; // �ּ�
	AID_t vehicleAID; // ���� ž���ϰ� �ִ� ž��ü
	
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
	float localPosX; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosY; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosZ; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localAngle; // ž��ü������ ����
	
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
	RESULT_t result; // �̵� ���
	
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
	AID_t playerAID; // �ּ�
	float localPosX; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosY; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosZ; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localAngle; // ž��ü������ ����
	AID_t vehicleAID; // ���� ž���ϰ� �ִ� ž��ü
	
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
	AID_t actorID; // �ּ�
	float stop_localPosX; // ž��ü������ ���� ��ǥ, ��ǥ
	float stop_localPosY; // ž��ü������ ���� ��ǥ, ��ǥ
	float stop_localPosZ; // ž��ü������ ���� ��ǥ, ��ǥ
	float stop_localAngle; // ž��ü������ ����
	AID_t vehicleAID; // ���� ž���ϰ� �ִ� ž��ü
	
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
	float basePosX; // ���� ��ǥ (X��)
	float basePosY; // ���� ��ǥ (Y��)
	float basePosZ; // ���� ��ǥ (Z��)
	
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
	RESULT_t result; // ������ũ �Ǵ� ���� ���� ���
	stSmallDateTime startTime; // ž�� �ð�
	
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
	AID_t aid; // ������ũ �Ǵ� ���� ���� ���� ���̵�
	stSmallDateTime startTime; // ž�� �ð�
	float basePosX; // ���� ��ǥ (X��)
	float basePosY; // ���� ��ǥ (Y��)
	float basePosZ; // ���� ��ǥ (Z��)
	
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
	float arrivalPosX; // ���� ��ǥ (X��)
	float arrivalPosY; // ���� ��ǥ (Y��)
	float arrivalPosZ; // ���� ��ǥ (Z��)
	float arrivalAngle; // ���� ���� ȸ����
	
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
	RESULT_t result; // ������ũ �Ǵ� ���� ������ ���
	
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
	AID_t aid; // ������ũ �Ǵ� ���� ������ ���� ���̵�
	float arrivalPosX; // ���� ��ǥ (X��)
	float arrivalPosY; // ���� ��ǥ (Y��)
	float arrivalPosZ; // ���� ��ǥ (Z��)
	float arrivalAngle; // ���� ���� ȸ����
	
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
	float localPosX; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosY; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosZ; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localAngle; // ž��ü������ ����
	float forwardScale; // �ּ�
	float rightScale; // �ּ�
	float rotate; // �ּ�
	
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
	RESULT_t result; // �̵� ���
	stSmallDateTime moveTime; // �̵��� ���������� �ð�
	
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
	AID_t playerAID; // �ּ�
	stSmallDateTime moveTime; // �̵��� ���������� �ð�
	float localPosX; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosY; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localPosZ; // ž��ü������ ���� ��ǥ ( ž��ü ��ǥ - �� ��ǥ )
	float localAngle; // ž��ü������ ����
	float forwardScale; // �ּ�
	float rightScale; // �ּ�
	float rotate; // �ּ�
	
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
	float localPosX; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosY; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosZ; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localAngle; // ž��ü������ ����
	
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
	RESULT_t result; // �̵� ���
	stSmallDateTime stopTime; // ������ ���������� �ð�
	
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
	AID_t playerAID; // �ּ�
	stSmallDateTime stopTime; // ������ ���������� �ð�
	float localPosX; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosY; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localPosZ; // ž��ü������ ���� ��ǥ, Req_PlayerStop_U2G ����
	float localAngle; // ž��ü������ ����
	
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
	AID_t actorID; // �ּ�
	stSmallDateTime stopTime; // ������ ���������� �ð�
	float stop_localPosX; // ž��ü������ ���� ��ǥ, ��ǥ
	float stop_localPosY; // ž��ü������ ���� ��ǥ, ��ǥ
	float stop_localPosZ; // ž��ü������ ���� ��ǥ, ��ǥ
	float stop_localAngle; // ž��ü������ ����
	
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
	AID_t actorID; // ž��ü�� �����ϴ� ���� ���̵�(���� ����)
	TID_t vehicleTID; // ž��ü TID
	ISN_t itemSN; // ��ȯ ������ ISN
	float posX; // ž��ü������ ��ǥ
	float posY; // ž��ü������ ��ǥ
	float posZ; // ž��ü������ ��ǥ
	float Angle; // ž��ü ����
	
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
	RESULT_t result; // ž��ü ���� ��� - ���� ������ appear packet����
	ISN_t itemSN; // ��ȯ ������ ISN
	
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
	AID_t vehicleAID; // ž��üAID
	ISN_t itemSN; // ��ȯ ������ ISN
	
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
	RESULT_t result; // ž��ü ���� ��� - ���� ������ appear packet����
	ISN_t itemSN; // ��ȯ ������ ISN
	
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
	AID_t actorID; // ž����
	uint8 seatIndex; // �ּ�
	AID_t vehicleAID; // ž��ü
	
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
	RESULT_t result; // ž�� ���
	
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
	AID_t actorID; // ž����
	uint8 seatIndex; // �ּ�
	AID_t vehicleAID; // ž��ü
	
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

// �°� ž�� ��� ��û
struct Req_AgreeToBoard_U2G : IMSerialize
{
	AID_t passengerAID; // �°�
	uint8 seatIndex; // �¼�
	AID_t vehicleAID; // ž��ü
	
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

// �°� ž�� ��� ����
struct Ack_AgreeToBoard_G2U : IMSerialize
{
	RESULT_t result; // ž�� ���
	AID_t passengerAID; // �°�
	uint8 seatIndex; // �¼�
	AID_t vehicleAID; // ž��ü
	bool agree; // ���ǿ���
	
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

// �°� ž�� ��� ��û
struct Cmd_AgreeToBoard_G2U : IMSerialize
{
	AID_t passengerAID; // �°�
	uint8 seatIndex; // �¼�
	AID_t vehicleAID; // ž��ü
	
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

// �°� ž�� ��� ����
struct Rpt_AgreeToBoard_U2G : IMSerialize
{
	AID_t passengerAID; // �°�
	uint8 seatIndex; // �¼���ȣ
	AID_t vehicleAID; // ž��ü
	bool agree; // ���ǿ���
	
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
	AID_t actorID; // ž����
	AID_t vehicleAID; // ž��ü
	
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
	RESULT_t result; // ž�� ���
	
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
	AID_t actorID; // ž����
	AID_t vehicleAID; // ž��ü
	uint8 seatIndex; // �¼���ȣ
	
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
	float posX; // ž��ü������ ��ǥ
	float posY; // ž��ü������ ��ǥ
	float posZ; // ž��ü������ ��ǥ
	float Angle; // ž��ü ����
	
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
	RESULT_t result; // ���
	
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
	float posX; // ž��ü������ ��ǥ
	float posY; // ž��ü������ ��ǥ
	float posZ; // ž��ü������ ��ǥ
	float Angle; // ž��ü ����
	
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
	float posX; // ž��ü������ ��ǥ
	float posY; // ž��ü������ ��ǥ
	float posZ; // ž��ü������ ��ǥ
	float Angle; // ž��ü ����
	
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
	RESULT_t result; // ���
	
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
	float posX; // ž��ü������ ��ǥ
	float posY; // ž��ü������ ��ǥ
	float posZ; // ž��ü������ ��ǥ
	float Angle; // ž��ü ����
	
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
	float InputPower; // ����
	float posX; // ž��ü������ ��ǥ
	float posY; // ž��ü������ ��ǥ
	float posZ; // ž��ü������ ��ǥ
	float Speed; // ž��ü �ӵ�
	float Pitch; // �յ� ����
	float Yaw; // ����
	float Roll; // �¿����
	
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
	RESULT_t result; // ���
	
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
	float InputPower; // ����
	float posX; // ž��ü������ ��ǥ
	float posY; // ž��ü������ ��ǥ
	float posZ; // ž��ü������ ��ǥ
	float Speed; // ž��ü �ӵ�
	float Pitch; // �յ� ����
	float Yaw; // ����
	float Roll; // �¿����
	
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
	float steering; // ���
	
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
	RESULT_t result; // ���
	
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
	float steering; // ���
	
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
	RESULT_t result; // ���
	
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
	stVehicleExteriorInfo Exterior; // ž�� �ð�
	
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
	RESULT_t result; // ���
	
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
	stVehicleExteriorInfo Exterior; // ž�� �ð�
	
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
	AID_t actorID; // �¼� ž�� ��û ���
	uint8 seatIndex; // �¼���ȣ
	
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
	stVector3 force; // ������
	stVector3 location; // ��ġ
	
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
	RESULT_t result; // ���
	
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
	stVector3 force; // ������
	stVector3 location; // ��ġ
	
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
