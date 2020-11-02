#pragma once


namespace pt_Game_User
{

struct Req_PlayerMove_U2G : IMSerialize
{
	AID_t playerAID; // 주석
	float posX; // 주석
	float posY; // 주석
	float posZ; // 주석
	float forwardScale; // 주석
	float rightScale; // 주석
	float rotate; // 주석
	float angle_for_server; // 서버의 이동을 계산하기 위한 각도
	
	Req_PlayerMove_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_PlayerMove_U2G);
		
		playerAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		forwardScale = 0.0f;
		rightScale = 0.0f;
		rotate = 0.0f;
		angle_for_server = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> posX >> posY >> posZ >> forwardScale >> rightScale >> rotate >> angle_for_server;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << posX << posY << posZ << forwardScale << rightScale << rotate << angle_for_server;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"forwardScale : %f", forwardScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightScale : %f", rightScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle_for_server : %f", angle_for_server );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerMove_G2U : IMSerialize
{
	RESULT_t result; // 주석
	
	Ack_PlayerMove_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_PlayerMove_G2U);
		
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

struct Inf_PlayerMove_G2U : IMSerialize
{
	AID_t playerAID; // 주석
	float posX; // 주석
	float posY; // 주석
	float posZ; // 주석
	float forwardScale; // 주석
	float rightScale; // 주석
	float rotate; // 주석
	
	Inf_PlayerMove_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_PlayerMove_G2U);
		
		playerAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		forwardScale = 0.0f;
		rightScale = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> posX >> posY >> posZ >> forwardScale >> rightScale >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << posX << posY << posZ << forwardScale << rightScale << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"forwardScale : %f", forwardScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightScale : %f", rightScale );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_PlayerStop_U2G : IMSerialize
{
	AID_t playerAID; // 주석
	float posX; // 주석
	float posY; // 주석
	float posZ; // 주석
	float rotate; // 주석
	
	Req_PlayerStop_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_PlayerStop_U2G);
		
		playerAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> posX >> posY >> posZ >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << posX << posY << posZ << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerStop_G2U : IMSerialize
{
	RESULT_t result; // 주석
	
	Ack_PlayerStop_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_PlayerStop_G2U);
		
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

struct Inf_PlayerStop_G2U : IMSerialize
{
	AID_t playerAID; // 주석
	float posX; // 주석
	float posY; // 주석
	float posZ; // 주석
	float rotate; // 주석
	
	Inf_PlayerStop_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_PlayerStop_G2U);
		
		playerAID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> posX >> posY >> posZ >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << posX << posY << posZ << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_NpcMove_G2U : IMSerialize
{
	AID_t npcAID; // npc aid
	float sPosX; // start x
	float sPosY; // start y
	float sPosZ; // start z
	float tPosX; // target x
	float tPosY; // target y
	float tPosZ; // target z
	
	Inf_NpcMove_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_NpcMove_G2U);
		
		npcAID = INVALID_AID;
		sPosX = 0.0f;
		sPosY = 0.0f;
		sPosZ = 0.0f;
		tPosX = 0.0f;
		tPosY = 0.0f;
		tPosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> npcAID >> sPosX >> sPosY >> sPosZ >> tPosX >> tPosY >> tPosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << npcAID << sPosX << sPosY << sPosZ << tPosX << tPosY << tPosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAID : %llu", npcAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"sPosX : %f", sPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"sPosY : %f", sPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"sPosZ : %f", sPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tPosX : %f", tPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tPosY : %f", tPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tPosZ : %f", tPosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_UseSkill_U2G : IMSerialize
{
	SKILL_SEQ_t tempSeq;
	TID_t skillTID;
	AID_t casterAID;
	uint8 attackHeight;
	std::list<AID_t> lstTarget;
	int32 collisionType;
	float casterPosX; // skill을 사용한 플레이어의 위치 x
	float casterPosY; // skill을 사용한 플레이어의 위치 y
	float casterPosZ; // skill을 사용한 플레이어의 위치 z
	float rotate; // target z
	
	Req_UseSkill_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_UseSkill_U2G);
		
		tempSeq = INVALID_SKILL_SEQ;
		skillTID = INVALID_TID;
		casterAID = INVALID_AID;
		attackHeight = 0;
		lstTarget.clear();
		collisionType = 0;
		casterPosX = 0.0f;
		casterPosY = 0.0f;
		casterPosZ = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> tempSeq >> skillTID >> casterAID >> attackHeight >> lstTarget >> collisionType >> casterPosX >> casterPosY >> casterPosZ >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << tempSeq << skillTID << casterAID << attackHeight << lstTarget << collisionType << casterPosX << casterPosY << casterPosZ << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tempSeq : %llu", tempSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillTID : %d", skillTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"attackHeight : %u", attackHeight );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstTarget );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosX : %f", casterPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosY : %f", casterPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosZ : %f", casterPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_UseSkill_G2U : IMSerialize
{
	RESULT_t result;
	SKILL_SEQ_t tempSeq;
	SKILL_SEQ_t skillSeq;
	int32 remainCoolTime;
	
	Ack_UseSkill_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_UseSkill_G2U);
		
		result = INVALID_RESULT;
		tempSeq = INVALID_SKILL_SEQ;
		skillSeq = INVALID_SKILL_SEQ;
		remainCoolTime = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> tempSeq >> skillSeq >> remainCoolTime;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << tempSeq << skillSeq << remainCoolTime;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tempSeq : %llu", tempSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"remainCoolTime : %d", remainCoolTime );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_UseSkill_G2U : IMSerialize
{
	AID_t casterAID; // 주석
	SKILL_SEQ_t skillSeq;
	TID_t skillTID;
	uint8 attackHeight;
	std::list<AID_t> lstTarget;
	int32 collisionType;
	float casterPosX;
	float casterPosY;
	float casterPosZ;
	float rotate;
	
	Inf_UseSkill_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_UseSkill_G2U);
		
		casterAID = INVALID_AID;
		skillSeq = INVALID_SKILL_SEQ;
		skillTID = INVALID_TID;
		attackHeight = 0;
		lstTarget.clear();
		collisionType = 0;
		casterPosX = 0.0f;
		casterPosY = 0.0f;
		casterPosZ = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> skillSeq >> skillTID >> attackHeight >> lstTarget >> collisionType >> casterPosX >> casterPosY >> casterPosZ >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << skillSeq << skillTID << attackHeight << lstTarget << collisionType << casterPosX << casterPosY << casterPosZ << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillTID : %d", skillTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"attackHeight : %u", attackHeight );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstTarget );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosX : %f", casterPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosY : %f", casterPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosZ : %f", casterPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_GuardBreak_U2G : IMSerialize
{
	AID_t casterAID; // 방패를 깨트린 스킬을 쓴 엑터의 aid
	AID_t targetAID; // 방패가 깨진 엑터의 aid
	SKILL_SEQ_t skillSeq;
	
	Req_GuardBreak_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_GuardBreak_U2G);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_GuardBreak_G2U : IMSerialize
{
	RESULT_t result;
	SKILL_SEQ_t skillSeq;
	
	Ack_GuardBreak_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_GuardBreak_G2U);
		
		result = INVALID_RESULT;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_GuardBreak_G2U : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID; // 방패가 깨진 엑터의 aid
	SKILL_SEQ_t skillSeq;
	
	Inf_GuardBreak_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_GuardBreak_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SuccessToShield_U2G : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	SKILL_SEQ_t skillSeq;
	TID_t shieldSkillTID;
	
	Req_SuccessToShield_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_SuccessToShield_U2G);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		skillSeq = INVALID_SKILL_SEQ;
		shieldSkillTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> skillSeq >> shieldSkillTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << skillSeq << shieldSkillTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"shieldSkillTID : %d", shieldSkillTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SuccessToShield_G2U : IMSerialize
{
	RESULT_t result;
	SKILL_SEQ_t skillSeq;
	
	Ack_SuccessToShield_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_SuccessToShield_G2U);
		
		result = INVALID_RESULT;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_SuccessToShield_G2U : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	SKILL_SEQ_t skillSeq;
	
	Inf_SuccessToShield_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_SuccessToShield_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_HitResult_U2G : IMSerialize
{
	SKILL_SEQ_t skillSeq;
	AID_t casterAID;
	AID_t targetAID;
	int32 collisionType;
	float hitPosX; // 피격위치 x
	float hitPosY; // 피격위치 y
	float hitPosZ; // 피격위치 z
	
	Req_HitResult_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_HitResult_U2G);
		
		skillSeq = INVALID_SKILL_SEQ;
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		collisionType = 0;
		hitPosX = 0.0f;
		hitPosY = 0.0f;
		hitPosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> skillSeq >> casterAID >> targetAID >> collisionType >> hitPosX >> hitPosY >> hitPosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << skillSeq << casterAID << targetAID << collisionType << hitPosX << hitPosY << hitPosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hitPosX : %f", hitPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hitPosY : %f", hitPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hitPosZ : %f", hitPosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_HitResult_G2U : IMSerialize
{
	RESULT_t result;
	SKILL_SEQ_t skillSeq;
	
	Ack_HitResult_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_HitResult_G2U);
		
		result = INVALID_RESULT;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_HitResult_G2U : IMSerialize
{
	SKILL_SEQ_t skillSeq;
	AID_t casterAID;
	AID_t targetAID;
	int32 collisionType;
	int32 coll_gauge;
	float hitPosX; // 피격위치 x
	float hitPosY; // 피격위치 y
	float hitPosZ; // 피격위치 z
	int64 damage; // 데미지
	int64 currentHP; // 현재 체력
	int8 ccState; // 패링인가
	bool reaction; // 리액션을 하는가
	
	Inf_HitResult_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_HitResult_G2U);
		
		skillSeq = INVALID_SKILL_SEQ;
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		collisionType = 0;
		coll_gauge = 0;
		hitPosX = 0.0f;
		hitPosY = 0.0f;
		hitPosZ = 0.0f;
		damage = 0;
		currentHP = 0;
		ccState = 0;
		reaction = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> skillSeq >> casterAID >> targetAID >> collisionType >> coll_gauge >> hitPosX >> hitPosY >> hitPosZ >> damage >> currentHP >> ccState >> reaction;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << skillSeq << casterAID << targetAID << collisionType << coll_gauge << hitPosX << hitPosY << hitPosZ << damage << currentHP << ccState << reaction;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"coll_gauge : %d", coll_gauge );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hitPosX : %f", hitPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hitPosY : %f", hitPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hitPosZ : %f", hitPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"damage : %lld", damage );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"currentHP : %lld", currentHP );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ccState : %d", ccState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"reaction : %d", reaction );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SuccessToAvoid_U2G : IMSerialize
{
	AID_t casterAID;
	SKILL_SEQ_t skillSeq;
	uint8 avoidDirection;
	
	Req_SuccessToAvoid_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_SuccessToAvoid_U2G);
		
		casterAID = INVALID_AID;
		skillSeq = INVALID_SKILL_SEQ;
		avoidDirection = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> skillSeq >> avoidDirection;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << skillSeq << avoidDirection;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"avoidDirection : %u", avoidDirection );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SuccessToAvoid_G2U : IMSerialize
{
	RESULT_t result;
	SKILL_SEQ_t skillSeq;
	
	Ack_SuccessToAvoid_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_SuccessToAvoid_G2U);
		
		result = INVALID_RESULT;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_SuccessToAvoid_G2U : IMSerialize
{
	AID_t casterAID;
	SKILL_SEQ_t skillSeq;
	uint8 avoidDirection;
	
	Inf_SuccessToAvoid_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_SuccessToAvoid_G2U);
		
		casterAID = INVALID_AID;
		skillSeq = INVALID_SKILL_SEQ;
		avoidDirection = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> skillSeq >> avoidDirection;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << skillSeq << avoidDirection;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"avoidDirection : %u", avoidDirection );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_ActorMoveToPos_U2G : IMSerialize
{
	float angle; // 각도
	float start_posX; // 시작
	float start_posY; // 시작
	float start_posZ; // 시작
	float target_posX; // 목표
	float target_posY; // 목표
	float target_posZ; // 목표
	uint8 actionType; // 이동 타입 참조\Global\Share\Define\Define_Actor.h => EWRActionType
	
	Req_ActorMoveToPos_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_ActorMoveToPos_U2G);
		
		angle = 0.0f;
		start_posX = 0.0f;
		start_posY = 0.0f;
		start_posZ = 0.0f;
		target_posX = 0.0f;
		target_posY = 0.0f;
		target_posZ = 0.0f;
		actionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> angle >> start_posX >> start_posY >> start_posZ >> target_posX >> target_posY >> target_posZ >> actionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << angle << start_posX << start_posY << start_posZ << target_posX << target_posY << target_posZ << actionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"start_posX : %f", start_posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"start_posY : %f", start_posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"start_posZ : %f", start_posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"target_posX : %f", target_posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"target_posY : %f", target_posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"target_posZ : %f", target_posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actionType : %u", actionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_ActorMoveToPos_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_ActorMoveToPos_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_ActorMoveToPos_G2U);
		
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

struct Inf_ActorMoveToPos_G2U : IMSerialize
{
	AID_t actorID; // 주석
	float start_posX; // 시작
	float start_posY; // 시작
	float start_posZ; // 시작
	float target_posX; // 목표
	float target_posY; // 목표
	float target_posZ; // 목표
	uint8 actionType; // 이동 타입 참조\Global\Share\Define\Define_Actor.h => EWRActionType
	
	Inf_ActorMoveToPos_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_ActorMoveToPos_G2U);
		
		actorID = INVALID_AID;
		start_posX = 0.0f;
		start_posY = 0.0f;
		start_posZ = 0.0f;
		target_posX = 0.0f;
		target_posY = 0.0f;
		target_posZ = 0.0f;
		actionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> start_posX >> start_posY >> start_posZ >> target_posX >> target_posY >> target_posZ >> actionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << start_posX << start_posY << start_posZ << target_posX << target_posY << target_posZ << actionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"start_posX : %f", start_posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"start_posY : %f", start_posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"start_posZ : %f", start_posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"target_posX : %f", target_posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"target_posY : %f", target_posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"target_posZ : %f", target_posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actionType : %u", actionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ActorMoveStop_G2U : IMSerialize
{
	AID_t actorID; // 주석
	float stop_posX; // 목표
	float stop_posY; // 목표
	float stop_posZ; // 목표
	float rotate; // 회전값
	
	Inf_ActorMoveStop_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_ActorMoveStop_G2U);
		
		actorID = INVALID_AID;
		stop_posX = 0.0f;
		stop_posY = 0.0f;
		stop_posZ = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> stop_posX >> stop_posY >> stop_posZ >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << stop_posX << stop_posY << stop_posZ << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_posX : %f", stop_posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_posY : %f", stop_posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_posZ : %f", stop_posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SyncDynamicMoveLocation_U2G : IMSerialize
{
	AID_t actorID; // 주석
	float posX; // 주석
	float posY; // 주석
	float posZ; // 주석
	float rotate; // 주석
	uint8 actionType; // 주석
	
	Req_SyncDynamicMoveLocation_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_SyncDynamicMoveLocation_U2G);
		
		actorID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		rotate = 0.0f;
		actionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> posX >> posY >> posZ >> rotate >> actionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << posX << posY << posZ << rotate << actionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actionType : %u", actionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SyncDynamicMoveLocation_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_SyncDynamicMoveLocation_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_SyncDynamicMoveLocation_G2U);
		
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

struct Inf_SyncDynamicMoveLocation_G2U : IMSerialize
{
	AID_t actorID; // 주석
	float posX; // 주석
	float posY; // 주석
	float posZ; // 주석
	float rotate; // 주석
	uint8 actionType; // 주석
	
	Inf_SyncDynamicMoveLocation_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_SyncDynamicMoveLocation_G2U);
		
		actorID = INVALID_AID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		rotate = 0.0f;
		actionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> posX >> posY >> posZ >> rotate >> actionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << posX << posY << posZ << rotate << actionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actionType : %u", actionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_BeginSkill_U2G : IMSerialize
{
	SKILL_SEQ_t tempSeq;
	TID_t skillTID;
	AID_t casterAID;
	std::list<AID_t> lstTarget;
	uint8 attackHeight;
	int32 collisionType;
	float casterPosX; // skill을 사용한 플레이어의 위치 x
	float casterPosY; // skill을 사용한 플레이어의 위치 y
	float casterPosZ; // skill을 사용한 플레이어의 위치 z
	float casterRotate; // skill을 사용한 플레이어의 방향
	
	Req_BeginSkill_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_BeginSkill_U2G);
		
		tempSeq = INVALID_SKILL_SEQ;
		skillTID = INVALID_TID;
		casterAID = INVALID_AID;
		lstTarget.clear();
		attackHeight = 0;
		collisionType = 0;
		casterPosX = 0.0f;
		casterPosY = 0.0f;
		casterPosZ = 0.0f;
		casterRotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> tempSeq >> skillTID >> casterAID >> lstTarget >> attackHeight >> collisionType >> casterPosX >> casterPosY >> casterPosZ >> casterRotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << tempSeq << skillTID << casterAID << lstTarget << attackHeight << collisionType << casterPosX << casterPosY << casterPosZ << casterRotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tempSeq : %llu", tempSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillTID : %d", skillTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstTarget );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"attackHeight : %u", attackHeight );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosX : %f", casterPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosY : %f", casterPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosZ : %f", casterPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterRotate : %f", casterRotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_BeginSkill_G2U : IMSerialize
{
	RESULT_t result;
	SKILL_SEQ_t tempSeq;
	SKILL_SEQ_t skillSeq;
	int32 remainCoolTime; // 남은 쿨타임
	
	Ack_BeginSkill_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_BeginSkill_G2U);
		
		result = INVALID_RESULT;
		tempSeq = INVALID_SKILL_SEQ;
		skillSeq = INVALID_SKILL_SEQ;
		remainCoolTime = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> tempSeq >> skillSeq >> remainCoolTime;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << tempSeq << skillSeq << remainCoolTime;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tempSeq : %llu", tempSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"remainCoolTime : %d", remainCoolTime );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_BeginSkill_G2U : IMSerialize
{
	SKILL_SEQ_t tempSeq;
	SKILL_SEQ_t skillSeq;
	TID_t skillTID;
	AID_t casterAID;
	std::list<AID_t> lstTarget;
	uint8 attackHeight;
	int32 collisionType;
	float casterPosX; // skill을 사용한 플레이어의 위치 x
	float casterPosY; // skill을 사용한 플레이어의 위치 y
	float casterPosZ; // skill을 사용한 플레이어의 위치 z
	float casterRotate; // skill을 사용한 플레이어의 방향
	int32 remainCoolTime; // 남은 쿨타임
	
	Inf_BeginSkill_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_BeginSkill_G2U);
		
		tempSeq = INVALID_SKILL_SEQ;
		skillSeq = INVALID_SKILL_SEQ;
		skillTID = INVALID_TID;
		casterAID = INVALID_AID;
		lstTarget.clear();
		attackHeight = 0;
		collisionType = 0;
		casterPosX = 0.0f;
		casterPosY = 0.0f;
		casterPosZ = 0.0f;
		casterRotate = 0.0f;
		remainCoolTime = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> tempSeq >> skillSeq >> skillTID >> casterAID >> lstTarget >> attackHeight >> collisionType >> casterPosX >> casterPosY >> casterPosZ >> casterRotate >> remainCoolTime;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << tempSeq << skillSeq << skillTID << casterAID << lstTarget << attackHeight << collisionType << casterPosX << casterPosY << casterPosZ << casterRotate << remainCoolTime;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tempSeq : %llu", tempSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillTID : %d", skillTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstTarget );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"attackHeight : %u", attackHeight );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosX : %f", casterPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosY : %f", casterPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosZ : %f", casterPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterRotate : %f", casterRotate );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"remainCoolTime : %d", remainCoolTime );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_EndSkill_U2G : IMSerialize
{
	SKILL_SEQ_t skillSeq;
	AID_t casterAID;
	float casterPosX; // skill을 사용한 플레이어의 위치 x
	float casterPosY; // skill을 사용한 플레이어의 위치 y
	float casterPosZ; // skill을 사용한 플레이어의 위치 z
	
	Req_EndSkill_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EndSkill_U2G);
		
		skillSeq = INVALID_SKILL_SEQ;
		casterAID = INVALID_AID;
		casterPosX = 0.0f;
		casterPosY = 0.0f;
		casterPosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> skillSeq >> casterAID >> casterPosX >> casterPosY >> casterPosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << skillSeq << casterAID << casterPosX << casterPosY << casterPosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosX : %f", casterPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosY : %f", casterPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosZ : %f", casterPosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_EndSkill_G2U : IMSerialize
{
	RESULT_t result;
	SKILL_SEQ_t skillSeq;
	
	Ack_EndSkill_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EndSkill_G2U);
		
		result = INVALID_RESULT;
		skillSeq = INVALID_SKILL_SEQ;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> skillSeq;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << skillSeq;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_EndSkill_G2U : IMSerialize
{
	SKILL_SEQ_t skillSeq;
	AID_t casterAID;
	float casterPosX; // skill을 사용한 플레이어의 위치 x
	float casterPosY; // skill을 사용한 플레이어의 위치 y
	float casterPosZ; // skill을 사용한 플레이어의 위치 z
	
	Inf_EndSkill_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_EndSkill_G2U);
		
		skillSeq = INVALID_SKILL_SEQ;
		casterAID = INVALID_AID;
		casterPosX = 0.0f;
		casterPosY = 0.0f;
		casterPosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> skillSeq >> casterAID >> casterPosX >> casterPosY >> casterPosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << skillSeq << casterAID << casterPosX << casterPosY << casterPosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"skillSeq : %llu", skillSeq );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosX : %f", casterPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosY : %f", casterPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosZ : %f", casterPosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_SetTarget_G2U : IMSerialize
{
	AID_t casterAID; // 주석
	AID_t targetAID; // 주석
	
	Inf_SetTarget_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_SetTarget_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_BeginInteraction_U2G : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	uint8 interactionType;
	
	Req_BeginInteraction_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_BeginInteraction_U2G);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		interactionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> interactionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << interactionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"interactionType : %u", interactionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_BeginInteraction_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_BeginInteraction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_BeginInteraction_G2U);
		
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

struct Inf_BeginInteraction_G2U : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	uint8 interactionType;
	
	Inf_BeginInteraction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_BeginInteraction_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		interactionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> interactionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << interactionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"interactionType : %u", interactionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_StopInteraction_U2G : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	
	Req_StopInteraction_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_StopInteraction_U2G);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_StopInteraction_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_StopInteraction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_StopInteraction_G2U);
		
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

struct Inf_StopInteraction_G2U : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	
	Inf_StopInteraction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_StopInteraction_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_EndInteraction_U2G : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	uint8 retainCount;
	uint8 interactionType;
	
	Req_EndInteraction_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EndInteraction_U2G);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		retainCount = 0;
		interactionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> retainCount >> interactionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << retainCount << interactionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"retainCount : %u", retainCount );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"interactionType : %u", interactionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_EndInteraction_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_EndInteraction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EndInteraction_G2U);
		
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

struct Inf_EndInteraction_G2U : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	uint16 retainCount;
	uint8 interactionType;
	
	Inf_EndInteraction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_EndInteraction_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		retainCount = 0;
		interactionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> retainCount >> interactionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << retainCount << interactionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"retainCount : %u", retainCount );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"interactionType : %u", interactionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_NpcDialog_G2U : IMSerialize
{
	AID_t playerAID; // master player aid
	AID_t npcAID; // npc aid
	TID_t dialogTID;
	
	Inf_NpcDialog_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_NpcDialog_G2U);
		
		playerAID = INVALID_AID;
		npcAID = INVALID_AID;
		dialogTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> npcAID >> dialogTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << npcAID << dialogTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAID : %llu", npcAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dialogTID : %d", dialogTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ActorCCState_G2U : IMSerialize
{
	AID_t actorAID; // master player aid
	uint16 ccState;
	bool enabled;
	
	Inf_ActorCCState_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_ActorCCState_G2U);
		
		actorAID = INVALID_AID;
		ccState = INVALID_TID;
		enabled = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorAID >> ccState >> enabled;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorAID << ccState << enabled;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorAID : %llu", actorAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ccState : %u", ccState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"enabled : %d", enabled );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ActorWarp_G2U : IMSerialize
{
	AID_t actorID; // 주석
	float stop_posX; // 목표
	float stop_posY; // 목표
	float stop_posZ; // 목표
	float rotate; // 회전값
	
	Inf_ActorWarp_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_ActorWarp_G2U);
		
		actorID = INVALID_AID;
		stop_posX = 0.0f;
		stop_posY = 0.0f;
		stop_posZ = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> stop_posX >> stop_posY >> stop_posZ >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << stop_posX << stop_posY << stop_posZ << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_posX : %f", stop_posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_posY : %f", stop_posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stop_posZ : %f", stop_posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_BeginBuff_G2U : IMSerialize
{
	AID_t ownerAID; // 버프에 걸린 액터
	stBuffInfo buffInfo; // 버프 정보
	
	Inf_BeginBuff_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_BeginBuff_G2U);
		
		ownerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ownerAID >> buffInfo;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ownerAID << buffInfo;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ownerAID : %llu", ownerAID );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, buffInfo, L"stBuffInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_EndBuff_G2U : IMSerialize
{
	AID_t ownerAID; // 버프에 걸렸던 액터
	TID_t buffTID; // 버프
	int32 stack; // 중첩개수
	
	Inf_EndBuff_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_EndBuff_G2U);
		
		ownerAID = INVALID_AID;
		buffTID = INVALID_TID;
		stack = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ownerAID >> buffTID >> stack;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ownerAID << buffTID << stack;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ownerAID : %llu", ownerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"buffTID : %d", buffTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"stack : %d", stack );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_DotBuff_G2U : IMSerialize
{
	AID_t ownerAID; // 버프에 걸린 액터
	AID_t casterAID; // 버프를 건 액터
	TID_t buffTID; // 버프
	int64 value; // 데미지
	
	Inf_DotBuff_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_DotBuff_G2U);
		
		ownerAID = INVALID_AID;
		casterAID = INVALID_AID;
		buffTID = INVALID_TID;
		value = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ownerAID >> casterAID >> buffTID >> value;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ownerAID << casterAID << buffTID << value;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ownerAID : %llu", ownerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"buffTID : %d", buffTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"value : %lld", value );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_CallADA_U2G : IMSerialize
{
	AID_t adaAID;
	float casterPosX; // 플레이어의 위치 x
	float casterPosY; // 플레이어의 위치 y
	float casterPosZ; // 플레이어의 위치 z
	
	Req_CallADA_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_CallADA_U2G);
		
		casterPosX = 0.0f;
		casterPosY = 0.0f;
		casterPosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> adaAID >> casterPosX >> casterPosY >> casterPosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << adaAID << casterPosX << casterPosY << casterPosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"adaAID : %llu", adaAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosX : %f", casterPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosY : %f", casterPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterPosZ : %f", casterPosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_CallADA_G2U : IMSerialize
{
	RESULT_t result;
	AID_t adaAID;
	float adaPosX; // 에이다 위치 x
	float adaPosY; // 에이다 위치 y
	float adaPosZ; // 에이다 위치 z
	float rotate;
	
	Ack_CallADA_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_CallADA_G2U);
		
		result = INVALID_RESULT;
		adaPosX = 0.0f;
		adaPosY = 0.0f;
		adaPosZ = 0.0f;
		rotate = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> adaAID >> adaPosX >> adaPosY >> adaPosZ >> rotate;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << adaAID << adaPosX << adaPosY << adaPosZ << rotate;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"adaAID : %llu", adaAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"adaPosX : %f", adaPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"adaPosY : %f", adaPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"adaPosZ : %f", adaPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rotate : %f", rotate );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_PlayerWarp_U2G : IMSerialize
{
	TID_t coordinateTID;
	
	Req_PlayerWarp_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_PlayerWarp_U2G);
		
		coordinateTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> coordinateTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << coordinateTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"coordinateTID : %d", coordinateTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_PlayerWarp_G2U : IMSerialize
{
	RESULT_t result;
	AID_t casterAID; // 버프를 건 액터
	TID_t coordinateTID;
	
	Ack_PlayerWarp_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_PlayerWarp_G2U);
		
		result = INVALID_RESULT;
		casterAID = INVALID_AID;
		coordinateTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> casterAID >> coordinateTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << casterAID << coordinateTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"coordinateTID : %d", coordinateTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_BeginNpcHook_G2U : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	int32 collisionType;
	
	Inf_BeginNpcHook_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_BeginNpcHook_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		collisionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> collisionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << collisionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_EndNpcHook_G2U : IMSerialize
{
	AID_t casterAID;
	AID_t targetAID;
	int32 collisionType;
	
	Inf_EndNpcHook_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_EndNpcHook_G2U);
		
		casterAID = INVALID_AID;
		targetAID = INVALID_AID;
		collisionType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> targetAID >> collisionType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << targetAID << collisionType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"collisionType : %d", collisionType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_DynamicMove_U2G : IMSerialize
{
	AID_t casterAID;
	TID_t dynamicMoveTableTID;
	float DestX;
	float DestY;
	float DestZ;
	
	Req_DynamicMove_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_DynamicMove_U2G);
		
		casterAID = INVALID_AID;
		dynamicMoveTableTID = INVALID_TID;
		DestX = 0.0;
		DestY = 0.0;
		DestZ = 0.0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> dynamicMoveTableTID >> DestX >> DestY >> DestZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << dynamicMoveTableTID << DestX << DestY << DestZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dynamicMoveTableTID : %d", dynamicMoveTableTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"DestX : %f", DestX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"DestY : %f", DestY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"DestZ : %f", DestZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_DynamicMove_G2U : IMSerialize
{
	RESULT_t result;
	AID_t casterAID;
	TID_t dynamicMoveTableTID;
	
	Ack_DynamicMove_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_DynamicMove_G2U);
		
		result = INVALID_RESULT;
		casterAID = INVALID_AID;
		dynamicMoveTableTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> casterAID >> dynamicMoveTableTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << casterAID << dynamicMoveTableTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dynamicMoveTableTID : %d", dynamicMoveTableTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_DynamicMove_G2U : IMSerialize
{
	AID_t casterAID;
	TID_t dynamicMoveTableTID;
	float DestX;
	float DestY;
	float DestZ;
	
	Inf_DynamicMove_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_DynamicMove_G2U);
		
		casterAID = INVALID_AID;
		dynamicMoveTableTID = INVALID_TID;
		DestX = 0.0;
		DestY = 0.0;
		DestZ = 0.0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> casterAID >> dynamicMoveTableTID >> DestX >> DestY >> DestZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << casterAID << dynamicMoveTableTID << DestX << DestY << DestZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"casterAID : %llu", casterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dynamicMoveTableTID : %d", dynamicMoveTableTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"DestX : %f", DestX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"DestY : %f", DestY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"DestZ : %f", DestZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_BeginLadder_U2G : IMSerialize
{
	int32 InHandleIndex;
	int32 InHand;
	
	Req_BeginLadder_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_BeginLadder_U2G);
		
		InHandleIndex = -1;
		InHand = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> InHandleIndex >> InHand;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << InHandleIndex << InHand;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHandleIndex : %d", InHandleIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHand : %d", InHand );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_BeginLadder_G2U : IMSerialize
{
	RESULT_t result;
	AID_t InCharID;
	int32 InHandleIndex;
	int32 InHand;
	
	Ack_BeginLadder_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_BeginLadder_G2U);
		
		result = INVALID_RESULT;
		InCharID = INVALID_AID;
		InHandleIndex = -1;
		InHand = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> InCharID >> InHandleIndex >> InHand;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << InCharID << InHandleIndex << InHand;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InCharID : %llu", InCharID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHandleIndex : %d", InHandleIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHand : %d", InHand );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_MoveLadder_U2G : IMSerialize
{
	int32 InHandleIndex;
	int32 InHand;
	int32 InDirection;
	
	Req_MoveLadder_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_MoveLadder_U2G);
		
		InHandleIndex = -1;
		InHand = 0;
		InDirection = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> InHandleIndex >> InHand >> InDirection;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << InHandleIndex << InHand << InDirection;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHandleIndex : %d", InHandleIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHand : %d", InHand );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InDirection : %d", InDirection );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_MoveLadder_G2U : IMSerialize
{
	RESULT_t result;
	AID_t InCharID;
	int32 InHandleIndex;
	int32 InHand;
	int32 InDirection;
	
	Ack_MoveLadder_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_MoveLadder_G2U);
		
		result = INVALID_RESULT;
		InCharID = INVALID_AID;
		InHandleIndex = -1;
		InHand = 0;
		InDirection = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> InCharID >> InHandleIndex >> InHand >> InDirection;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << InCharID << InHandleIndex << InHand << InDirection;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InCharID : %llu", InCharID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHandleIndex : %d", InHandleIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHand : %d", InHand );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InDirection : %d", InDirection );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_EndLadder_U2G : IMSerialize
{
	int32 InHandleIndex;
	
	Req_EndLadder_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EndLadder_U2G);
		
		InHandleIndex = -1;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> InHandleIndex;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << InHandleIndex;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHandleIndex : %d", InHandleIndex );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_EndLadder_G2U : IMSerialize
{
	RESULT_t result;
	AID_t InCharID;
	int32 InHandleIndex;
	
	Ack_EndLadder_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EndLadder_G2U);
		
		result = INVALID_RESULT;
		InCharID = INVALID_AID;
		InHandleIndex = -1;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> InCharID >> InHandleIndex;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << InCharID << InHandleIndex;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InCharID : %llu", InCharID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHandleIndex : %d", InHandleIndex );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_CancelLadder_U2G : IMSerialize
{
	
	Req_CancelLadder_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_CancelLadder_U2G);
		
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

struct Ack_CancelLadder_G2U : IMSerialize
{
	RESULT_t result;
	AID_t InCharID;
	
	Ack_CancelLadder_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_CancelLadder_G2U);
		
		result = INVALID_RESULT;
		InCharID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> InCharID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << InCharID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InCharID : %llu", InCharID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 낙하 시작 요청
struct Req_BeginFall_U2G : IMSerialize
{
	float PosX;
	float PosY;
	float PosZ;
	
	Req_BeginFall_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_BeginFall_U2G);
		
		PosX = 0.0f;
		PosY = 0.0f;
		PosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> PosX >> PosY >> PosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << PosX << PosY << PosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosX : %f", PosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosY : %f", PosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosZ : %f", PosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 낙하 시작 응답
struct Ack_BeginFall_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_BeginFall_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_BeginFall_G2U);
		
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

// 낙하 시작 알림
struct Inf_BeginFall_G2U : IMSerialize
{
	AID_t fallAID;
	float PosX;
	float PosY;
	float PosZ;
	
	Inf_BeginFall_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_BeginFall_G2U);
		
		fallAID = INVALID_AID;
		PosX = 0.0f;
		PosY = 0.0f;
		PosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> fallAID >> PosX >> PosY >> PosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << fallAID << PosX << PosY << PosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"fallAID : %llu", fallAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosX : %f", PosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosY : %f", PosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosZ : %f", PosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 낙하 종료 요청
struct Req_EndFallen_U2G : IMSerialize
{
	float PosX;
	float PosY;
	float PosZ;
	
	Req_EndFallen_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EndFallen_U2G);
		
		PosX = 0.0f;
		PosY = 0.0f;
		PosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> PosX >> PosY >> PosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << PosX << PosY << PosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosX : %f", PosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosY : %f", PosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosZ : %f", PosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 낙하 종료 응답
struct Ack_EndFallen_G2U : IMSerialize
{
	RESULT_t result;
	HP_t curHP;
	
	Ack_EndFallen_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EndFallen_G2U);
		
		result = INVALID_RESULT;
		curHP = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> curHP;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << curHP;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"curHP : %d", curHP );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 낙하 종료 알림
struct Inf_EndFallen_G2U : IMSerialize
{
	AID_t fallAID;
	HP_t curHP;
	float PosX;
	float PosY;
	float PosZ;
	
	Inf_EndFallen_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_EndFallen_G2U);
		
		fallAID = INVALID_AID;
		curHP = 0;
		PosX = 0.0f;
		PosY = 0.0f;
		PosZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> fallAID >> curHP >> PosX >> PosY >> PosZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << fallAID << curHP << PosX << PosY << PosZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"fallAID : %llu", fallAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"curHP : %d", curHP );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosX : %f", PosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosY : %f", PosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"PosZ : %f", PosZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// HeadRotation 동기화 요청
struct Req_SyncHMD_U2G : IMSerialize
{
	AID_t ActorID;
	float RotationX;
	float RotationY;
	float RotationZ;
	
	Req_SyncHMD_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_SyncHMD_U2G);
		
		ActorID = INVALID_AID;
		RotationX = 0.0f;
		RotationY = 0.0f;
		RotationZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ActorID >> RotationX >> RotationY >> RotationZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ActorID << RotationX << RotationY << RotationZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ActorID : %llu", ActorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationX : %f", RotationX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationY : %f", RotationY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationZ : %f", RotationZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// HeadRotation 동기화 응답
struct Ack_SyncHMD_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_SyncHMD_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_SyncHMD_G2U);
		
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

// HeadRotation 동기화 알림
struct Inf_SyncHMD_G2U : IMSerialize
{
	AID_t ActorID;
	float RotationX;
	float RotationY;
	float RotationZ;
	
	Inf_SyncHMD_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_SyncHMD_G2U);
		
		ActorID = INVALID_AID;
		RotationX = 0.0f;
		RotationY = 0.0f;
		RotationZ = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ActorID >> RotationX >> RotationY >> RotationZ;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ActorID << RotationX << RotationY << RotationZ;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ActorID : %llu", ActorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationX : %f", RotationX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationY : %f", RotationY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationZ : %f", RotationZ );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_BattleState_G2U : IMSerialize
{
	AID_t ActorID;
	bool BattleState;
	
	Inf_BattleState_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Inf_BattleState_G2U);
		
		ActorID = INVALID_AID;
		BattleState = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ActorID >> BattleState;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ActorID << BattleState;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ActorID : %llu", ActorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"BattleState : %d", BattleState );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성인식 시작 요청
struct Req_BeginVoiceCommand_U2G : IMSerialize
{
	float currPosX;
	float currPosY;
	float currPosZ;
	float currAngle;
	bool enableStartDlg;
	
	Req_BeginVoiceCommand_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_BeginVoiceCommand_U2G);
		
		currPosX = 0.0f;
		currPosY = 0.0f;
		currPosZ = 0.0f;
		currAngle = 0.0f;
		enableStartDlg = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> currPosX >> currPosY >> currPosZ >> currAngle >> enableStartDlg;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << currPosX << currPosY << currPosZ << currAngle << enableStartDlg;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"currPosX : %f", currPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"currPosY : %f", currPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"currPosZ : %f", currPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"currAngle : %f", currAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"enableStartDlg : %d", enableStartDlg );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성인식 시작 응답
struct Ack_BeginVoiceCommand_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_BeginVoiceCommand_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_BeginVoiceCommand_G2U);
		
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

// 음성인식 종료 요청
struct Req_EndVoiceCommand_U2G : IMSerialize
{
	
	Req_EndVoiceCommand_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Req_EndVoiceCommand_U2G);
		
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

// 음성인식 종료 응답
struct Ack_EndVoiceCommand_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_EndVoiceCommand_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User, EGameUserType::Ack_EndVoiceCommand_G2U);
		
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
