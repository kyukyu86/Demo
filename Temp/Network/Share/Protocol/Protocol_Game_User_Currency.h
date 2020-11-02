#pragma once


namespace pt_Game_User_Currency
{

// 재화 정보 갱신
struct Inf_UpdateCurrency_G2U : IMSerialize
{
	uint8 type; // EWRCurrencyType::en
	CURRENCY_t value; // 재화 값
	
	Inf_UpdateCurrency_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_User_Currency, EGameUserCurrencyType::Inf_UpdateCurrency_G2U);
		
		type = (uint8)EWRCurrencyType::MAX;
		value = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> type >> value;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << type << value;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"type : %u", type );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"value : %lld", value );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};
}
