#pragma once


namespace pt_Login_User
{

struct Req_LoginUser_U2L : IMSerialize
{
	ID_t userid; // �ּ�
	PROTOCOL_VERSION_t protocolversion; // �ּ�
	
	Req_LoginUser_U2L()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Login_User, ELoginUserType::Req_LoginUser_U2L);
		
		userid = INVALID_ID;
		protocolversion = INVALID_PROTOCOL_VERSION;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> userid >> protocolversion;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << userid << protocolversion;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"userid : %s", userid.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"protocolversion : %u", protocolversion );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_LoginUser_L2U : IMSerialize
{
	RESULT_t result; // �ּ�
	DKEY_t userdkey; // �ּ�
	USER_GRADE_t usergrade; // �ּ�
	
	Ack_LoginUser_L2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Login_User, ELoginUserType::Ack_LoginUser_L2U);
		
		result = INVALID_RESULT;
		userdkey = INVALID_DKEY;
		usergrade = BASE_USER_GRADE;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> userdkey >> usergrade;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << userdkey << usergrade;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"userdkey : %u", userdkey );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"usergrade : %u", usergrade );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_ReserveUser_U2L : IMSerialize
{
	ID_t userid; // �ּ�
	DKEY_t userdkey; // �ּ�
	
	Req_ReserveUser_U2L()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Login_User, ELoginUserType::Req_ReserveUser_U2L);
		
		userid = INVALID_ID;
		userdkey = INVALID_DKEY;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> userid >> userdkey;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << userid << userdkey;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"userid : %s", userid.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"userdkey : %u", userdkey );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_ReserveUser_L2U : IMSerialize
{
	RESULT_t result; // �ּ�
	IP_t connectIP; // �ּ�
	PORT_t connectPort; // �ּ�
	
	Ack_ReserveUser_L2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Login_User, ELoginUserType::Ack_ReserveUser_L2U);
		
		result = INVALID_RESULT;
		connectIP = INVALID_IP;
		connectPort = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> connectIP >> connectPort;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << connectIP << connectPort;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"connectIP : %s", connectIP.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"connectPort : %u", connectPort );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_KickUser_L2U : IMSerialize
{
	RESULT_t result; // �ּ�
	
	Inf_KickUser_L2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Login_User, ELoginUserType::Inf_KickUser_L2U);
		
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
}
