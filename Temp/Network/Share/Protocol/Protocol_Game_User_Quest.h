#pragma once


namespace pt_Game_User_Quest
{

// 진행중인 퀘스트 정보 요청
struct Req_ProgressQuestList_U2G : IMSerialize
{
	
	Req_ProgressQuestList_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Req_ProgressQuestList_U2G);
		
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

// 진행중인 퀘스트 정보 응답
struct Ack_ProgressQuestList_G2U : IMSerialize
{
	RESULT_t result;
	std::vector<stQuest> vecQuest;
	
	Ack_ProgressQuestList_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Ack_ProgressQuestList_G2U);
		
		result = INVALID_RESULT;
		vecQuest.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> vecQuest;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << vecQuest;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, vecQuest, L"stQuest" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 완료된 퀘스트 정보 요청
struct Req_CompleteQuestList_U2G : IMSerialize
{
	
	Req_CompleteQuestList_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Req_CompleteQuestList_U2G);
		
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

// 완료된 퀘스트 정보 응답
struct Ack_CompleteQuestList_G2U : IMSerialize
{
	RESULT_t result;
	std::vector<TID_t> vecQuestTID;
	
	Ack_CompleteQuestList_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Ack_CompleteQuestList_G2U);
		
		result = INVALID_RESULT;
		vecQuestTID.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> vecQuestTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << vecQuestTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, vecQuestTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 수락 요청
struct Req_AcceptQuest_U2G : IMSerialize
{
	TID_t TID;
	
	Req_AcceptQuest_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Req_AcceptQuest_U2G);
		
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 수락 응답
struct Ack_AcceptQuest_G2U : IMSerialize
{
	RESULT_t result;
	TID_t TID;
	
	Ack_AcceptQuest_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Ack_AcceptQuest_G2U);
		
		result = INVALID_RESULT;
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 완료가능 확인 요청
struct Req_CompletableQuest_U2G : IMSerialize
{
	TID_t TID;
	
	Req_CompletableQuest_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Req_CompletableQuest_U2G);
		
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 완료가능 확인 응답
struct Ack_CompletableQuest_G2U : IMSerialize
{
	RESULT_t result;
	TID_t TID;
	
	Ack_CompletableQuest_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Ack_CompletableQuest_G2U);
		
		result = INVALID_RESULT;
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 완료/보상 요청
struct Req_CompleteQuest_U2G : IMSerialize
{
	TID_t TID;
	
	Req_CompleteQuest_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Req_CompleteQuest_U2G);
		
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 완료/보상 응답
struct Ack_CompleteQuest_G2U : IMSerialize
{
	RESULT_t result;
	TID_t TID;
	
	Ack_CompleteQuest_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Ack_CompleteQuest_G2U);
		
		result = INVALID_RESULT;
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 태스크 완료 요청
struct Req_CompleteQuestTask_U2G : IMSerialize
{
	TID_t TID;
	TID_t taskTID;
	
	Req_CompleteQuestTask_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Req_CompleteQuestTask_U2G);
		
		TID = INVALID_TID;
		taskTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID >> taskTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID << taskTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"taskTID : %d", taskTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 태스크 완료 응답
struct Ack_CompleteQuestTask_G2U : IMSerialize
{
	RESULT_t result;
	TID_t TID;
	TID_t taskTID;
	
	Ack_CompleteQuestTask_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Ack_CompleteQuestTask_G2U);
		
		result = INVALID_RESULT;
		TID = INVALID_TID;
		taskTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> TID >> taskTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << TID << taskTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"taskTID : %d", taskTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 태스크 완료 알림
struct Inf_CompleteQuestTask_G2U : IMSerialize
{
	TID_t TID;
	TID_t taskTID;
	
	Inf_CompleteQuestTask_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Inf_CompleteQuestTask_G2U);
		
		TID = INVALID_TID;
		taskTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID >> taskTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID << taskTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"taskTID : %d", taskTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 퀘스트 태스크 갱신 알림
struct Inf_UpdateQuestTask_G2U : IMSerialize
{
	TID_t TID;
	TID_t taskTID;
	QUEST_TASK_VALUE_t taskValue;
	
	Inf_UpdateQuestTask_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Inf_UpdateQuestTask_G2U);
		
		TID = INVALID_TID;
		taskTID = INVALID_TID;
		taskValue = INVALID_QUEST_TASK_VALUE;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID >> taskTID >> taskValue;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID << taskTID << taskValue;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"taskTID : %d", taskTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"taskValue : %d", taskValue );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 프로토타입용 - 퀘스트 강제 완료 알림
struct Inf_ForcedQuestComplete_G2U : IMSerialize
{
	TID_t TID;
	
	Inf_ForcedQuestComplete_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Inf_ForcedQuestComplete_G2U);
		
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 프로토타입용 - 퀘스트 강제 포기 알림
struct Inf_ForcedQuestGiveUp_G2U : IMSerialize
{
	TID_t TID;
	
	Inf_ForcedQuestGiveUp_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Quest, EGameUserQuestType::Inf_ForcedQuestGiveUp_G2U);
		
		TID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> TID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << TID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"TID : %d", TID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};
}
