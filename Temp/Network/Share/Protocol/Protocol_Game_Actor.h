#pragma once


namespace pt_Game_Actor
{

struct Inf_PlayerAppear_G2U : IMSerialize
{
	AID_t aid; // 플레이어 액터 아이디 (서버에서 생성한 키값)
	NAME_t nickName; // 플레이어 닉네임
	HP_t maxhp; // 최대 HP
	HP_t hp; // 현재 HP
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	float dPosX; // 자동이동중 목표지점 좌표 (X축)
	float dPosY; // 자동이동중 목표지점 좌표 (Y축)
	float dPosZ; // 자동이동중 목표지점 좌표 (Z축)
	TID_t leftWeaponTID; // 왼손 무기
	TID_t rightWeaponTID; // 오른손 무기
	TID_t temporaryTID; // 임시 장착류
	std::list<TID_t> listCostumeTID; // 의상 정보
	TID_t presetTID; // 프리셋 번호
	uint8 gender;
	stCustomizeInfo customizeInfo;
	std::list<stBuffInfo> lstBuffInfo; // 버프 리스트
	int32 InHandleIndex;
	int32 InHand;
	AID_t vehicleAID; // 현재 타고 있는 Vehicle 액터 아이디
	AID_t myVehicleAID; // 내가 소환한 Vehicle 액터 아이디
	uint8 seatIndex; // 현재 타고 있는 차량의 시트 인덱스
	float localPosX; // 탑승체에서의 로컬 좌표 (X축)
	float localPosY; // 탑승체에서의 로컬 좌표 (Y축)
	float localPosZ; // 탑승체에서의 로컬 좌표 (Z축)
	float localAngle; // 탑승체에서의 각도
	TID_t danceGroupTID; // 댄스 종류
	TID_t danceTID; // 댄스 단계
	AID_t occupyIOAID; // 점유 IO AID
	uint8 cheersCondition; // 건배상태 - EWRCheersConditionType::en
	bool BattleState;
	bool bInMW;
	stSmallDateTime mwRenewalTime; // 무빙워크 좌표 갱신 시간
	float mwBasePosX; // 무빙워크 기준 좌표 (X축)
	float mwBasePosY; // 무빙워크 기준 좌표 (Y축)
	float mwBasePosZ; // 무빙워크 기준 좌표 (Z축)
	uint8 chatType; // 채팅타입 - Normal, Secret, Group
	CHAT_GROUP_ID_t chatGroupID; // 채팅그룹아이디
	uint8 language; // 대화 언어
	
	Inf_PlayerAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_PlayerAppear_G2U);
		
		aid = INVALID_AID;
		nickName = INVALID_NAME;
		maxhp = 0;
		hp = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		dPosX = 0.0f;
		dPosY = 0.0f;
		dPosZ = 0.0f;
		leftWeaponTID = INVALID_TID;
		rightWeaponTID = INVALID_TID;
		temporaryTID = INVALID_TID;
		listCostumeTID.clear();
		presetTID = INVALID_TID;
		gender = (uint8)EWRGender::Male;
		lstBuffInfo.clear();
		InHandleIndex = -1;
		InHand = 0;
		vehicleAID = INVALID_AID;
		myVehicleAID = INVALID_AID;
		seatIndex = INVALID_SEAT_INDEX;
		localPosX = 0.0f;
		localPosY = 0.0f;
		localPosZ = 0.0f;
		localAngle = 0.0f;
		danceGroupTID = INVALID_TID;
		danceTID = INVALID_TID;
		occupyIOAID = INVALID_AID;
		cheersCondition = 0;
		BattleState = false;
		bInMW = false;
		mwBasePosX = 0.0f;
		mwBasePosY = 0.0f;
		mwBasePosZ = 0.0f;
		chatType = 0;
		chatGroupID = INVALID_CHAT_GROUP_ID;
		language = (uint8)EWRLanguageType::MAX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> nickName >> maxhp >> hp >> posX >> posY >> posZ >> angle >> dPosX >> dPosY >> dPosZ >> leftWeaponTID >> rightWeaponTID >> temporaryTID >> listCostumeTID >> presetTID >> gender >> customizeInfo >> lstBuffInfo >> InHandleIndex >> InHand >> vehicleAID >> myVehicleAID >> seatIndex >> localPosX >> localPosY >> localPosZ >> localAngle >> danceGroupTID >> danceTID >> occupyIOAID >> cheersCondition >> BattleState >> bInMW >> mwRenewalTime >> mwBasePosX >> mwBasePosY >> mwBasePosZ >> chatType >> chatGroupID >> language;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << nickName << maxhp << hp << posX << posY << posZ << angle << dPosX << dPosY << dPosZ << leftWeaponTID << rightWeaponTID << temporaryTID << listCostumeTID << presetTID << gender << customizeInfo << lstBuffInfo << InHandleIndex << InHand << vehicleAID << myVehicleAID << seatIndex << localPosX << localPosY << localPosZ << localAngle << danceGroupTID << danceTID << occupyIOAID << cheersCondition << BattleState << bInMW << mwRenewalTime << mwBasePosX << mwBasePosY << mwBasePosZ << chatType << chatGroupID << language;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"nickName : %s", nickName.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"maxhp : %d", maxhp );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hp : %d", hp );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosX : %f", dPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosY : %f", dPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosZ : %f", dPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"leftWeaponTID : %d", leftWeaponTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"rightWeaponTID : %d", rightWeaponTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"temporaryTID : %d", temporaryTID );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listCostumeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"presetTID : %d", presetTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"gender : %u", gender );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, customizeInfo, L"stCustomizeInfo" );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstBuffInfo, L"stBuffInfo" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHandleIndex : %d", InHandleIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"InHand : %d", InHand );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"myVehicleAID : %llu", myVehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"seatIndex : %u", seatIndex );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosX : %f", localPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosY : %f", localPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localPosZ : %f", localPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"localAngle : %f", localAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceGroupTID : %d", danceGroupTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceTID : %d", danceTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"occupyIOAID : %llu", occupyIOAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cheersCondition : %u", cheersCondition );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"BattleState : %d", BattleState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bInMW : %d", bInMW );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, mwRenewalTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mwBasePosX : %f", mwBasePosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mwBasePosY : %f", mwBasePosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mwBasePosZ : %f", mwBasePosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"chatType : %u", chatType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"chatGroupID : %u", chatGroupID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"language : %u", language );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_NpcAppear_G2U : IMSerialize
{
	AID_t npcAID; // 플레이어 액터 아이디 (서버에서 생성한 키값)
	TID_t npcTID; // NPC 테이블의 TID
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	HP_t hp; // 현재 HP
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	float dPosX; // 자동이동중 목표지점 좌표 (X축)
	float dPosY; // 자동이동중 목표지점 좌표 (Y축)
	float dPosZ; // 자동이동중 목표지점 좌표 (Z축)
	bool bBattleState; // 전투 상태 여부
	bool bSpawn; // 스폰으로 인한 패킷 전송인가
	std::list<stBuffInfo> lstBuffInfo; // 버프 리스트
	std::list<stHookInfo> lstHookInfo; // 갈고리 리스트
	uint8 cheersCondition; // 건배상태 - EWRCheersConditionType::en
	
	Inf_NpcAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_NpcAppear_G2U);
		
		npcAID = INVALID_AID;
		npcTID = INVALID_TID;
		spawnTID = INVALID_TID;
		hp = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		dPosX = 0.0f;
		dPosY = 0.0f;
		dPosZ = 0.0f;
		bBattleState = false;
		bSpawn = false;
		lstBuffInfo.clear();
		lstHookInfo.clear();
		cheersCondition = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> npcAID >> npcTID >> spawnTID >> hp >> posX >> posY >> posZ >> angle >> dPosX >> dPosY >> dPosZ >> bBattleState >> bSpawn >> lstBuffInfo >> lstHookInfo >> cheersCondition;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << npcAID << npcTID << spawnTID << hp << posX << posY << posZ << angle << dPosX << dPosY << dPosZ << bBattleState << bSpawn << lstBuffInfo << lstHookInfo << cheersCondition;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAID : %llu", npcAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcTID : %d", npcTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hp : %d", hp );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosX : %f", dPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosY : %f", dPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosZ : %f", dPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bBattleState : %d", bBattleState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bSpawn : %d", bSpawn );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstBuffInfo, L"stBuffInfo" );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstHookInfo, L"stHookInfo" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cheersCondition : %u", cheersCondition );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_IOAppear_G2U : IMSerialize
{
	AID_t ioAID; // 인터렉션 오브젝트 AID (서버에서 생성한 키값)
	TID_t ioTID; // 인터렉션 오브젝트 TID (ObjectData_InteractionObject.xml의 tid 참조)
	AID_t ownerAID; // 인터렉션 하고 있는 주인
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	uint16 directRetainCount;
	uint16 hookRetainCount;
	uint8 interactionState; // 상호작용 상태 - EWRInteractionState::en
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	AID_t occupyPlayerAID; // 점유 Player AID
	ISN_t relatedISN; // io 와 연결되어 있는 아이템 고유 번호
	
	Inf_IOAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_IOAppear_G2U);
		
		ioAID = INVALID_AID;
		ioTID = INVALID_TID;
		ownerAID = INVALID_AID;
		spawnTID = INVALID_TID;
		directRetainCount = 0;
		hookRetainCount = 0;
		interactionState = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		occupyPlayerAID = INVALID_AID;
		relatedISN = INVALID_ISN;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> ioAID >> ioTID >> ownerAID >> spawnTID >> directRetainCount >> hookRetainCount >> interactionState >> posX >> posY >> posZ >> angle >> occupyPlayerAID >> relatedISN;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << ioAID << ioTID << ownerAID << spawnTID << directRetainCount << hookRetainCount << interactionState << posX << posY << posZ << angle << occupyPlayerAID << relatedISN;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ioAID : %llu", ioAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ioTID : %d", ioTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"ownerAID : %llu", ownerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"directRetainCount : %u", directRetainCount );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hookRetainCount : %u", hookRetainCount );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"interactionState : %u", interactionState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"occupyPlayerAID : %llu", occupyPlayerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"relatedISN : %llu", relatedISN );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_BrickAppear_G2U : IMSerialize
{
	AID_t brickAID; // 인터렉션 오브젝트 AID (서버에서 생성한 키값)
	TID_t brickTID; // 인터렉션 오브젝트 TID (ObjectData_Brick.xml의 tid 참조)
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	
	Inf_BrickAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_BrickAppear_G2U);
		
		brickAID = INVALID_AID;
		brickTID = INVALID_TID;
		spawnTID = INVALID_TID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> brickAID >> brickTID >> spawnTID >> posX >> posY >> posZ >> angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << brickAID << brickTID << spawnTID << posX << posY << posZ << angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"brickAID : %llu", brickAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"brickTID : %d", brickTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_APAppear_G2U : IMSerialize
{
	AID_t apAID; // ActionProp AID (서버에서 생성한 키값)
	TID_t apTID; // ActionProp TID (ObjectData_ActionProp.xml의 tid 참조)
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	uint8 apState; // 상태 - EWRAPState::en
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	
	Inf_APAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_APAppear_G2U);
		
		apAID = INVALID_AID;
		apTID = INVALID_TID;
		spawnTID = INVALID_TID;
		apState = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> apAID >> apTID >> spawnTID >> apState >> posX >> posY >> posZ >> angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << apAID << apTID << spawnTID << apState << posX << posY << posZ << angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"apAID : %llu", apAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"apTID : %d", apTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"apState : %u", apState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_SOAppear_G2U : IMSerialize
{
	AID_t apAID; // SO AID (서버에서 생성한 키값)
	TID_t apTID; // SO TID (ObjectData_ActionProp.xml의 tid 참조)
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	
	Inf_SOAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SOAppear_G2U);
		
		apAID = INVALID_AID;
		apTID = INVALID_TID;
		spawnTID = INVALID_TID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> apAID >> apTID >> spawnTID >> posX >> posY >> posZ >> angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << apAID << apTID << spawnTID << posX << posY << posZ << angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"apAID : %llu", apAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"apTID : %d", apTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ElevatorAppear_G2U : IMSerialize
{
	AID_t aid; // Elevator AID (서버에서 생성한 키값)
	TID_t elevatorTID; // Elevator TID (TransportData_Elevator.xml의 tid 참조)
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	uint8 vehicleState; // 상호작용 상태 - EWRVehicleState::en
	uint8 curStation; // 현재 지점 (출발지점)
	uint8 nextStation; // 다음 지점 (도착지점)
	stSmallDateTime startTime; // 출발 시간
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	std::list<AID_t> lstPassengers; // 승객 리스트
	
	Inf_ElevatorAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ElevatorAppear_G2U);
		
		aid = INVALID_AID;
		elevatorTID = INVALID_TID;
		spawnTID = INVALID_TID;
		vehicleState = 0;
		curStation = 0;
		nextStation = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		lstPassengers.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> elevatorTID >> spawnTID >> vehicleState >> curStation >> nextStation >> startTime >> posX >> posY >> posZ >> angle >> lstPassengers;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << elevatorTID << spawnTID << vehicleState << curStation << nextStation << startTime << posX << posY << posZ << angle << lstPassengers;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"elevatorTID : %d", elevatorTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleState : %u", vehicleState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"curStation : %u", curStation );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"nextStation : %u", nextStation );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, startTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstPassengers );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_MonorailAppear_G2U : IMSerialize
{
	AID_t aid; // Monorail AID (서버에서 생성한 키값)
	TID_t monorailTID; // Monorail TID (TransportData_Monorail.xml의 tid 참조)
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	uint8 vehicleState; // 상호작용 상태 - EWRVehicleState::en
	uint8 curStation; // 현재 지점 (출발지점)
	uint8 nextStation; // 다음 지점 (도착지점)
	stSmallDateTime startTime; // 출발 시간
	SPEED_t speed; // run/walk 공통
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	std::list<AID_t> lstPassengers; // 승객 리스트
	
	Inf_MonorailAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_MonorailAppear_G2U);
		
		aid = INVALID_AID;
		monorailTID = INVALID_TID;
		spawnTID = INVALID_TID;
		vehicleState = 0;
		curStation = 0;
		nextStation = 0;
		speed = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		lstPassengers.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> monorailTID >> spawnTID >> vehicleState >> curStation >> nextStation >> startTime >> speed >> posX >> posY >> posZ >> angle >> lstPassengers;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << monorailTID << spawnTID << vehicleState << curStation << nextStation << startTime << speed << posX << posY << posZ << angle << lstPassengers;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"monorailTID : %d", monorailTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleState : %u", vehicleState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"curStation : %u", curStation );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"nextStation : %u", nextStation );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, startTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speed : %d", speed );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstPassengers );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_TardisAppear_G2U : IMSerialize
{
	AID_t aid; // Tardis AID (서버에서 생성한 키값)
	TID_t tardisTID; // Tardis TID (TransportData_Tardis.xml의 tid 참조)
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	uint8 vehicleState; // 상호작용 상태 - EWRVehicleState::en
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	std::list<AID_t> lstPassengers; // 승객 리스트
	
	Inf_TardisAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_TardisAppear_G2U);
		
		aid = INVALID_AID;
		tardisTID = INVALID_TID;
		spawnTID = INVALID_TID;
		vehicleState = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		lstPassengers.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> tardisTID >> spawnTID >> vehicleState >> posX >> posY >> posZ >> angle >> lstPassengers;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << tardisTID << spawnTID << vehicleState << posX << posY << posZ << angle << lstPassengers;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"tardisTID : %d", tardisTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleState : %u", vehicleState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstPassengers );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_FloatingBoardAppear_G2U : IMSerialize
{
	AID_t aid; // FloatingBoard AID (서버에서 생성한 키값)
	TID_t floatingBoardTID; // FloatingBoard TID (TransportData_FloatingBoard.xml의 tid 참조)
	TID_t spawnTID; // 스폰 유형인 경우 값 존재
	uint8 vehicleState; // 상호작용 상태 - EWRVehicleState::en
	uint8 curStation; // 현재 지점 (출발지점)
	uint8 nextStation; // 다음 지점 (도착지점)
	stSmallDateTime startTime; // 출발 시간
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	std::list<AID_t> lstPassengers; // 승객 리스트
	
	Inf_FloatingBoardAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_FloatingBoardAppear_G2U);
		
		aid = INVALID_AID;
		floatingBoardTID = INVALID_TID;
		spawnTID = INVALID_TID;
		vehicleState = 0;
		curStation = 0;
		nextStation = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		lstPassengers.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> floatingBoardTID >> spawnTID >> vehicleState >> curStation >> nextStation >> startTime >> posX >> posY >> posZ >> angle >> lstPassengers;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << floatingBoardTID << spawnTID << vehicleState << curStation << nextStation << startTime << posX << posY << posZ << angle << lstPassengers;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"floatingBoardTID : %d", floatingBoardTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"spawnTID : %d", spawnTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleState : %u", vehicleState );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"curStation : %u", curStation );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"nextStation : %u", nextStation );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, startTime, L"stSmallDateTime" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstPassengers );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_VehicleAppear_G2U : IMSerialize
{
	AID_t vehicleAID; // 액터 아이디 (서버에서 생성한 키값)
	AID_t masterAID; // 마스터 액터 아이디 (주인 액터)
	TID_t vehicleTID; // NPC 테이블의 TID
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	float dPosX; // 자동이동중 목표지점 좌표 (X축)
	float dPosY; // 자동이동중 목표지점 좌표 (Y축)
	float dPosZ; // 자동이동중 목표지점 좌표 (Z축)
	float speed;
	std::vector<AID_t> vecCrew; // 탑승자 리스트
	stVehicleExteriorInfo ExteriorInfo; // 외형정보
	
	Inf_VehicleAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_VehicleAppear_G2U);
		
		vehicleAID = INVALID_AID;
		masterAID = INVALID_AID;
		vehicleTID = INVALID_TID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		dPosX = 0.0f;
		dPosY = 0.0f;
		dPosZ = 0.0f;
		speed = 0.0f;
		vecCrew.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> vehicleAID >> masterAID >> vehicleTID >> posX >> posY >> posZ >> angle >> dPosX >> dPosY >> dPosZ >> speed >> vecCrew >> ExteriorInfo;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << vehicleAID << masterAID << vehicleTID << posX << posY << posZ << angle << dPosX << dPosY << dPosZ << speed << vecCrew << ExteriorInfo;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleAID : %llu", vehicleAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"masterAID : %llu", masterAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"vehicleTID : %d", vehicleTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosX : %f", dPosX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosY : %f", dPosY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dPosZ : %f", dPosZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speed : %f", speed );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, vecCrew );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, ExteriorInfo, L"stVehicleExteriorInfo" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_SecretDialogAppear_G2U : IMSerialize
{
	AID_t SecretDialogAID; // 액터 아이디 (서버에서 생성한 키값)
	TID_t SecretDialogTID; // NPC 테이블의 TID
	CHAT_GROUP_ID_t groupID; // 채팅 그룹 아이디
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	std::vector<AID_t> vecPlayer; // 탑승자 리스트
	
	Inf_SecretDialogAppear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SecretDialogAppear_G2U);
		
		SecretDialogAID = INVALID_AID;
		SecretDialogTID = INVALID_TID;
		groupID = INVALID_CHAT_GROUP_ID;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
		vecPlayer.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> SecretDialogAID >> SecretDialogTID >> groupID >> posX >> posY >> posZ >> angle >> vecPlayer;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << SecretDialogAID << SecretDialogTID << groupID << posX << posY << posZ << angle << vecPlayer;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"SecretDialogAID : %llu", SecretDialogAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"SecretDialogTID : %d", SecretDialogTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, vecPlayer );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ActorDisappear_G2U : IMSerialize
{
	AID_t aid; // 사라지는 객체 AID
	
	Inf_ActorDisappear_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ActorDisappear_G2U);
		
		aid = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ActorDied_G2U : IMSerialize
{
	AID_t killerAID; // 공격한 Actor ID (서버에서 생성한 키값)
	AID_t deadAID; // 사망한 Actor ID (서버에서 생성한 키값)
	
	Inf_ActorDied_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ActorDied_G2U);
		
		killerAID = INVALID_AID;
		deadAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> killerAID >> deadAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << killerAID << deadAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"killerAID : %llu", killerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"deadAID : %llu", deadAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ChangeHP_G2U : IMSerialize
{
	AID_t aid; // HP 변경된 Actor ID (서버에서 생성한 키값)
	HP_t maxHP; // 최대 HP
	HP_t currentHP; // 현재 HP
	
	Inf_ChangeHP_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ChangeHP_G2U);
		
		aid = INVALID_AID;
		maxHP = 0;
		currentHP = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> maxHP >> currentHP;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << maxHP << currentHP;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"maxHP : %d", maxHP );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"currentHP : %d", currentHP );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ChangeWalkSpeed_G2U : IMSerialize
{
	AID_t aid; // 속도가 변경되는 Actor ID (서버에서 생성한 키값)
	SPEED_t walkSpeed; // 변경되는 값
	
	Inf_ChangeWalkSpeed_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ChangeWalkSpeed_G2U);
		
		aid = INVALID_AID;
		walkSpeed = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> walkSpeed;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << walkSpeed;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"walkSpeed : %d", walkSpeed );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ChangeSpeed_G2U : IMSerialize
{
	AID_t aid; // 속도가 변경되는 Actor ID (서버에서 생성한 키값)
	SPEED_t walkSpeed; // 변경되는 값
	SPEED_t runSpeed; // 변경되는 값
	
	Inf_ChangeSpeed_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ChangeSpeed_G2U);
		
		aid = INVALID_AID;
		walkSpeed = 0;
		runSpeed = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> walkSpeed >> runSpeed;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << walkSpeed << runSpeed;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"walkSpeed : %d", walkSpeed );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"runSpeed : %d", runSpeed );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_Revive_U2G : IMSerialize
{
	PSN_t psn; // 부활하려는 PSN
	bool bResurrection; // 제자리 부활인가
	
	Req_Revive_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_Revive_U2G);
		
		psn = INVALID_PSN;
		bResurrection = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> psn >> bResurrection;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << psn << bResurrection;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u", psn );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bResurrection : %d", bResurrection );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_Revive_G2U : IMSerialize
{
	RESULT_t result; // 현재 맵 안의 액터 정보 Inform 처리 후 마지막으로 전송
	
	Ack_Revive_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_Revive_G2U);
		
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

struct Inf_Revive_G2U : IMSerialize
{
	AID_t aid; // 부활 Actor ID (서버에서 생성한 키값)
	NAME_t nickName; // 플레이어 닉네임
	HP_t maxhp; // 최대 HP
	HP_t hp; // 현재 HP
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	
	Inf_Revive_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_Revive_G2U);
		
		aid = INVALID_AID;
		nickName = INVALID_NAME;
		maxhp = 0;
		hp = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> nickName >> maxhp >> hp >> posX >> posY >> posZ >> angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << nickName << maxhp << hp << posX << posY << posZ << angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"nickName : %s", nickName.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"maxhp : %d", maxhp );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hp : %d", hp );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ClientDirectSync_G2U : IMSerialize
{
	TID_t triggerNodeTID; // 트리거 노드 json 파일의 tid
	
	Inf_ClientDirectSync_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ClientDirectSync_G2U);
		
		triggerNodeTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> triggerNodeTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << triggerNodeTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"triggerNodeTID : %d", triggerNodeTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_TriggerEvent_U2G : IMSerialize
{
	TID_t triggerNodeTID; // 트리거 노드 json 파일의 tid
	uint32 triggerNodeType; // 트리거 노드 유형 EWRTriggerNodeType::en
	float posX; // 좌표 (X축)
	float posY; // 좌표 (Y축)
	float posZ; // 좌표 (Z축)
	float angle; // 회전
	
	Req_TriggerEvent_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_TriggerEvent_U2G);
		
		triggerNodeTID = INVALID_TID;
		triggerNodeType = 0;
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
		angle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> triggerNodeTID >> triggerNodeType >> posX >> posY >> posZ >> angle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << triggerNodeTID << triggerNodeType << posX << posY << posZ << angle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"triggerNodeTID : %d", triggerNodeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"triggerNodeType : %u", triggerNodeType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posX : %f", posX );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posY : %f", posY );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"posZ : %f", posZ );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"angle : %f", angle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_TriggerEvent_G2U : IMSerialize
{
	RESULT_t result; // 현재 맵 안의 액터 정보 Inform 처리 후 마지막으로 전송
	TID_t triggerNodeTID; // 트리거 노드 json 파일의 tid
	uint32 triggerNodeType; // 트리거 노드 유형 EWRTriggerNodeType::en
	
	Ack_TriggerEvent_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_TriggerEvent_G2U);
		
		result = INVALID_RESULT;
		triggerNodeTID = INVALID_TID;
		triggerNodeType = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> triggerNodeTID >> triggerNodeType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << triggerNodeTID << triggerNodeType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"triggerNodeTID : %d", triggerNodeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"triggerNodeType : %u", triggerNodeType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SkipSequence_U2G : IMSerialize
{
	
	Req_SkipSequence_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_SkipSequence_U2G);
		
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

struct Ack_SkipSequence_G2U : IMSerialize
{
	RESULT_t result; // 현재 맵 안의 액터 정보 Inform 처리 후 마지막으로 전송
	
	Ack_SkipSequence_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_SkipSequence_G2U);
		
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

struct Inf_PlayerLevelActionSequence_G2U : IMSerialize
{
	AID_t actorID; // 트리거 발동시킨 Actor ID (서버에서 생성한 키값)
	NAME_t levelSequenceName; // 레벨 시퀀스이름
	
	Inf_PlayerLevelActionSequence_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_PlayerLevelActionSequence_G2U);
		
		actorID = INVALID_AID;
		levelSequenceName = INVALID_NAME;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> levelSequenceName;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << levelSequenceName;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"levelSequenceName : %s", levelSequenceName.c_str() );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_TriggerDisable_G2U : IMSerialize
{
	TID_t triggerNodeTID; // 비활성화되는 트리거 TID
	
	Inf_TriggerDisable_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_TriggerDisable_G2U);
		
		triggerNodeTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> triggerNodeTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << triggerNodeTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"triggerNodeTID : %d", triggerNodeTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_UsedTriggerList_G2U : IMSerialize
{
	std::list<TID_t> listUsedTrigger; // 사용된 트리거 목록
	
	Inf_UsedTriggerList_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_UsedTriggerList_G2U);
		
		listUsedTrigger.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listUsedTrigger;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listUsedTrigger;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, listUsedTrigger );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_Warp_G2U : IMSerialize
{
	AID_t actorID; // 워프하는 Actor ID (서버에서 생성한 키값)
	TID_t coordinateTID; // CoordinateData 등록된 위치로 이동. 맵 이동 스탭과 동일하게 이동
	
	Inf_Warp_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_Warp_G2U);
		
		actorID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> actorID >> coordinateTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << actorID << coordinateTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"actorID : %llu", actorID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"coordinateTID : %d", coordinateTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_Cheat_U2G : IMSerialize
{
	std::wstring cheatdata; // 치트 생성하고자 하는 Actor Type
	
	Req_Cheat_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_Cheat_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> cheatdata;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << cheatdata;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cheatdata : %s", cheatdata );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_Cheat_G2U : IMSerialize
{
	RESULT_t result; // 치트 결과
	
	Ack_Cheat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_Cheat_G2U);
		
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

struct Req_ChangePreset_U2G : IMSerialize
{
	TID_t costumeTID; // 프리셋 TID
	bool bResspawnRequired;
	
	Req_ChangePreset_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_ChangePreset_U2G);
		
		costumeTID = INVALID_TID;
		bResspawnRequired = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> costumeTID >> bResspawnRequired;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << costumeTID << bResspawnRequired;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"costumeTID : %d", costumeTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bResspawnRequired : %d", bResspawnRequired );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_ChangePreset_G2U : IMSerialize
{
	RESULT_t result; // 결과
	TID_t costumeTID; // 프리셋 TID
	
	Ack_ChangePreset_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_ChangePreset_G2U);
		
		result = INVALID_RESULT;
		costumeTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> costumeTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << costumeTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"costumeTID : %d", costumeTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_ChangePreset_G2U : IMSerialize
{
	AID_t playerAID; // 프리셋을 변경한 Player ID
	TID_t costumeTID; // 프리셋 TID
	
	Inf_ChangePreset_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ChangePreset_G2U);
		
		playerAID = INVALID_AID;
		costumeTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> costumeTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << costumeTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"costumeTID : %d", costumeTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 댄스 시작 요청
struct Req_BeginDance_U2G : IMSerialize
{
	TID_t danceGroupTID; // 댄스 종류
	
	Req_BeginDance_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_BeginDance_U2G);
		
		danceGroupTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> danceGroupTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << danceGroupTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceGroupTID : %d", danceGroupTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 댄스 시작 응답
struct Ack_BeginDance_G2U : IMSerialize
{
	RESULT_t result; // 결과
	TID_t danceGroupTID; // 댄스 종류
	
	Ack_BeginDance_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_BeginDance_G2U);
		
		result = INVALID_RESULT;
		danceGroupTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> danceGroupTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << danceGroupTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceGroupTID : %d", danceGroupTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 댄스 종료 요청
struct Req_EndDance_U2G : IMSerialize
{
	
	Req_EndDance_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_EndDance_U2G);
		
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

// 댄스 종료 응답
struct Ack_EndDance_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_EndDance_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_EndDance_G2U);
		
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

// 댄스 단계 변경 요청
struct Req_ChangeDanceStep_U2G : IMSerialize
{
	TID_t danceGroupTID; // 댄스 종류
	TID_t danceTID; // 댄스 단계
	
	Req_ChangeDanceStep_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_ChangeDanceStep_U2G);
		
		danceGroupTID = INVALID_TID;
		danceTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> danceGroupTID >> danceTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << danceGroupTID << danceTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceGroupTID : %d", danceGroupTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceTID : %d", danceTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 댄스 단계 변경 응답
struct Ack_ChangeDanceStep_G2U : IMSerialize
{
	RESULT_t result; // 결과
	TID_t danceGroupTID; // 댄스 종류
	TID_t danceTID; // 댄스 단계
	
	Ack_ChangeDanceStep_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_ChangeDanceStep_G2U);
		
		result = INVALID_RESULT;
		danceGroupTID = INVALID_TID;
		danceTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> danceGroupTID >> danceTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << danceGroupTID << danceTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceGroupTID : %d", danceGroupTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceTID : %d", danceTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 댄스 단계 변경 알림
struct Inf_ChangeDanceStep_G2U : IMSerialize
{
	AID_t AID; // 댄스 단계 변경 대상 AID
	TID_t danceGroupTID; // 댄스 종류
	TID_t danceTID; // 댄스 단계
	
	Inf_ChangeDanceStep_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ChangeDanceStep_G2U);
		
		AID = INVALID_AID;
		danceGroupTID = INVALID_TID;
		danceTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> AID >> danceGroupTID >> danceTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << AID << danceGroupTID << danceTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AID : %llu", AID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceGroupTID : %d", danceGroupTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"danceTID : %d", danceTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 대화 시작 요청
struct Req_BeginTalking_U2G : IMSerialize
{
	AID_t npcAID; // NPC 액터 아이디
	bool bRoaming; // 로밍 여부 (true : 진행, false : 멈춤)
	
	Req_BeginTalking_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_BeginTalking_U2G);
		
		npcAID = INVALID_AID;
		bRoaming = true;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> npcAID >> bRoaming;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << npcAID << bRoaming;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAID : %llu", npcAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bRoaming : %d", bRoaming );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 대화 시작 응답
struct Ack_BeginTalking_G2U : IMSerialize
{
	RESULT_t result; // 성공 결과
	AID_t npcAID; // NPC 액터 아이디
	
	Ack_BeginTalking_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_BeginTalking_G2U);
		
		result = INVALID_RESULT;
		npcAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> npcAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << npcAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAID : %llu", npcAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 대화 종료 요청
struct Req_EndTalking_U2G : IMSerialize
{
	AID_t npcAID; // NPC 액터 아이디
	
	Req_EndTalking_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_EndTalking_U2G);
		
		npcAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> npcAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << npcAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAID : %llu", npcAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 대화 종료 응답
struct Ack_EndTalking_G2U : IMSerialize
{
	RESULT_t result; // 성공 결과
	AID_t npcAID; // NPC 액터 아이디
	float npcAngle; // 회전
	
	Ack_EndTalking_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_EndTalking_G2U);
		
		result = INVALID_RESULT;
		npcAID = INVALID_AID;
		npcAngle = 0.0f;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> npcAID >> npcAngle;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << npcAID << npcAngle;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAID : %llu", npcAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"npcAngle : %f", npcAngle );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SocialAction_U2G : IMSerialize
{
	TID_t socialAnimationTID; // 소셜 행동 TID
	AID_t targetAID; // 소셜 행동 타켓 AID
	
	Req_SocialAction_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_SocialAction_U2G);
		
		socialAnimationTID = INVALID_TID;
		targetAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> socialAnimationTID >> targetAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << socialAnimationTID << targetAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"socialAnimationTID : %d", socialAnimationTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SocialAction_G2U : IMSerialize
{
	RESULT_t result; // 현재 맵 안의 액터 정보 Inform 처리 후 마지막으로 전송
	TID_t socialAnimationTID; // 소셜 행동 TID
	AID_t targetAID; // 소셜 행동 타켓 AID
	
	Ack_SocialAction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_SocialAction_G2U);
		
		result = INVALID_RESULT;
		socialAnimationTID = INVALID_TID;
		targetAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> socialAnimationTID >> targetAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << socialAnimationTID << targetAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"socialAnimationTID : %d", socialAnimationTID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_SocialAction_G2U : IMSerialize
{
	AID_t playerAID; // 소셜 행동을 하는 액터 ID
	AID_t targetAID; // 소셜 행동 타켓 AID
	TID_t socialAnimationTID; // 소셜 행동 TID
	
	Inf_SocialAction_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SocialAction_G2U);
		
		playerAID = INVALID_AID;
		targetAID = INVALID_AID;
		socialAnimationTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> targetAID >> socialAnimationTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << targetAID << socialAnimationTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"targetAID : %llu", targetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"socialAnimationTID : %d", socialAnimationTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_Unoccupy_IO_U2G : IMSerialize
{
	
	Req_Unoccupy_IO_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_Unoccupy_IO_U2G);
		
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

struct Ack_Unoccupy_IO_G2U : IMSerialize
{
	RESULT_t result; // 현재 맵 안의 액터 정보 Inform 처리 후 마지막으로 전송
	AID_t occupyPlayerAID; // 점유 Player ID
	AID_t occupyIOAID; // 점유 IO ID
	
	Ack_Unoccupy_IO_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_Unoccupy_IO_G2U);
		
		result = INVALID_RESULT;
		occupyPlayerAID = INVALID_AID;
		occupyIOAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> occupyPlayerAID >> occupyIOAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << occupyPlayerAID << occupyIOAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"occupyPlayerAID : %llu", occupyPlayerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"occupyIOAID : %llu", occupyIOAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SuggestCheers_U2G : IMSerialize
{
	AID_t AnswerAID; // 건배제의 대상 액터 ID
	
	Req_SuggestCheers_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_SuggestCheers_U2G);
		
		AnswerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> AnswerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << AnswerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AnswerAID : %llu", AnswerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SuggestCheers_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t AnswerAID; // 건배제의 대상 액터 ID
	
	Ack_SuggestCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_SuggestCheers_G2U);
		
		result = INVALID_RESULT;
		AnswerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> AnswerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << AnswerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AnswerAID : %llu", AnswerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_SuggestCheers_G2U : IMSerialize
{
	AID_t SuggestAID; // 건배제의 액터 ID
	uint8 sugeestCheersCondition; // 건배상태 - EWRCheersConditionType::en
	AID_t AnswerAID; // 건배제의 대상 액터 ID
	uint8 answerCheersCondition; // 건배상태 - EWRCheersConditionType::en
	
	Inf_SuggestCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SuggestCheers_G2U);
		
		SuggestAID = INVALID_AID;
		sugeestCheersCondition = 0;
		AnswerAID = INVALID_AID;
		answerCheersCondition = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> SuggestAID >> sugeestCheersCondition >> AnswerAID >> answerCheersCondition;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << SuggestAID << sugeestCheersCondition << AnswerAID << answerCheersCondition;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"SuggestAID : %llu", SuggestAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"sugeestCheersCondition : %u", sugeestCheersCondition );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AnswerAID : %llu", AnswerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"answerCheersCondition : %u", answerCheersCondition );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_AnswerCheers_U2G : IMSerialize
{
	AID_t SuggestAID; // 건배제의 액터 ID
	bool bAccept; // 수락여부(수락/거절)
	
	Req_AnswerCheers_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_AnswerCheers_U2G);
		
		SuggestAID = INVALID_AID;
		bAccept = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> SuggestAID >> bAccept;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << SuggestAID << bAccept;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"SuggestAID : %llu", SuggestAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bAccept : %d", bAccept );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_AnswerCheers_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t SuggestAID; // 건배제의 액터 ID
	bool bAccept; // 수락여부(수락/거절)
	
	Ack_AnswerCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_AnswerCheers_G2U);
		
		result = INVALID_RESULT;
		SuggestAID = INVALID_AID;
		bAccept = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> SuggestAID >> bAccept;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << SuggestAID << bAccept;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"SuggestAID : %llu", SuggestAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bAccept : %d", bAccept );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_AnswerCheers_G2U : IMSerialize
{
	AID_t SuggestAID; // 건배제의 액터 ID
	uint8 sugeestCheersCondition; // 건배상태 - EWRCheersConditionType::en
	AID_t AnswerAID; // 건배제의 대상 액터 ID
	uint8 answerCheersCondition; // 건배상태 - EWRCheersConditionType::en
	bool bAccept; // 수락여부(수락/거절)
	
	Inf_AnswerCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_AnswerCheers_G2U);
		
		SuggestAID = INVALID_AID;
		sugeestCheersCondition = 0;
		AnswerAID = INVALID_AID;
		answerCheersCondition = 0;
		bAccept = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> SuggestAID >> sugeestCheersCondition >> AnswerAID >> answerCheersCondition >> bAccept;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << SuggestAID << sugeestCheersCondition << AnswerAID << answerCheersCondition << bAccept;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"SuggestAID : %llu", SuggestAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"sugeestCheersCondition : %u", sugeestCheersCondition );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AnswerAID : %llu", AnswerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"answerCheersCondition : %u", answerCheersCondition );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bAccept : %d", bAccept );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_BeginCheers_U2G : IMSerialize
{
	AID_t AnswerAID; // 건배제의 대상 액터 ID
	
	Req_BeginCheers_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_BeginCheers_U2G);
		
		AnswerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> AnswerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << AnswerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AnswerAID : %llu", AnswerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_BeginCheers_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t AnswerAID; // 건배제의 대상 액터 ID
	
	Ack_BeginCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_BeginCheers_G2U);
		
		result = INVALID_RESULT;
		AnswerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> AnswerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << AnswerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AnswerAID : %llu", AnswerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_BeginCheers_G2U : IMSerialize
{
	AID_t SuggestAID; // 건배제의 액터 ID
	uint8 sugeestCheersCondition; // 건배상태 - EWRCheersConditionType::en
	AID_t AnswerAID; // 건배제의 대상 액터 ID
	uint8 answerCheersCondition; // 건배상태 - EWRCheersConditionType::en
	
	Inf_BeginCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_BeginCheers_G2U);
		
		SuggestAID = INVALID_AID;
		sugeestCheersCondition = 0;
		AnswerAID = INVALID_AID;
		answerCheersCondition = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> SuggestAID >> sugeestCheersCondition >> AnswerAID >> answerCheersCondition;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << SuggestAID << sugeestCheersCondition << AnswerAID << answerCheersCondition;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"SuggestAID : %llu", SuggestAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"sugeestCheersCondition : %u", sugeestCheersCondition );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"AnswerAID : %llu", AnswerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"answerCheersCondition : %u", answerCheersCondition );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_CancelCheers_U2G : IMSerialize
{
	AID_t cnacelTargetAID; // 건배제의 취소 대상 액터 ID
	
	Req_CancelCheers_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_CancelCheers_U2G);
		
		cnacelTargetAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> cnacelTargetAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << cnacelTargetAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cnacelTargetAID : %llu", cnacelTargetAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_CancelCheers_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t cnacelTargetAID; // 건배제의 취소 대상 액터 ID
	
	Ack_CancelCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_CancelCheers_G2U);
		
		result = INVALID_RESULT;
		cnacelTargetAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> cnacelTargetAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << cnacelTargetAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cnacelTargetAID : %llu", cnacelTargetAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Inf_CancelCheers_G2U : IMSerialize
{
	AID_t cancelAID; // 건배제의 취소 액터 ID
	uint8 cancelCheersCondition; // 건배상태 - EWRCheersConditionType::en
	AID_t cnacelTargetAID; // 건배제의 취소 대상 액터 ID
	uint8 cnacelTargetCheersCondition; // 건배상태 - EWRCheersConditionType::en
	AID_t suggestAID; // 건배제의 액터 ID
	
	Inf_CancelCheers_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_CancelCheers_G2U);
		
		cancelAID = INVALID_AID;
		cancelCheersCondition = 0;
		cnacelTargetAID = INVALID_AID;
		cnacelTargetCheersCondition = 0;
		suggestAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> cancelAID >> cancelCheersCondition >> cnacelTargetAID >> cnacelTargetCheersCondition >> suggestAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << cancelAID << cancelCheersCondition << cnacelTargetAID << cnacelTargetCheersCondition << suggestAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cancelAID : %llu", cancelAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cancelCheersCondition : %u", cancelCheersCondition );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cnacelTargetAID : %llu", cnacelTargetAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"cnacelTargetCheersCondition : %u", cnacelTargetCheersCondition );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"suggestAID : %llu", suggestAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Req_SpeechBubble_U2G : IMSerialize
{
	uint8 language; // 대화 언어
	std::wstring msg; // 대화 내용
	
	Req_SpeechBubble_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_SpeechBubble_U2G);
		
		language = (uint8)EWRLanguageType::MAX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> language >> msg;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << language << msg;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"language : %u", language );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"msg : %s", msg );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

struct Ack_SpeechBubble_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_SpeechBubble_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_SpeechBubble_G2U);
		
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

struct Inf_SpeechBubble_G2U : IMSerialize
{
	AID_t playerAID; // 대화 대상
	uint8 language; // 대화 언어
	std::wstring msg; // 대화 내용
	
	Inf_SpeechBubble_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SpeechBubble_G2U);
		
		playerAID = INVALID_AID;
		language = (uint8)EWRLanguageType::MAX;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> language >> msg;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << language << msg;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"language : %u", language );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"msg : %s", msg );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성 대화 메세지 요청
struct Req_SetFocusedAID_U2G : IMSerialize
{
	AID_t focusedAID; // 포커스된 상대
	
	Req_SetFocusedAID_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_SetFocusedAID_U2G);
		
		focusedAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> focusedAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << focusedAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"focusedAID : %llu", focusedAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성 대화 메세지 응답
struct Ack_SetFocusedAID_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_SetFocusedAID_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_SetFocusedAID_G2U);
		
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

// 음성 대화 메세지 알림
struct Inf_SetFocusedAID_G2U : IMSerialize
{
	AID_t playerAID; // 음성 대화 요청자
	
	Inf_SetFocusedAID_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SetFocusedAID_G2U);
		
		playerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성 대화 메세지 요청
struct Req_VoiceMessage_U2G : IMSerialize
{
	std::wstring voiceMsg; // 음성 대화 내용
	
	Req_VoiceMessage_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_VoiceMessage_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> voiceMsg;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << voiceMsg;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"voiceMsg : %s", voiceMsg );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성 대화 메세지 응답
struct Ack_VoiceMessage_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_VoiceMessage_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_VoiceMessage_G2U);
		
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

// 음성 대화 메세지 알림
struct Inf_VoiceMessage_G2U : IMSerialize
{
	AID_t playerAID; // 음성 대화 요청자
	std::wstring voiceMsg; // 음성 대화 내용
	
	Inf_VoiceMessage_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_VoiceMessage_G2U);
		
		playerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> voiceMsg;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << voiceMsg;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"voiceMsg : %s", voiceMsg );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성 채팅 요청
struct Req_VoiceChat_U2G : IMSerialize
{
	uint16 length; // 길이
	std::wstring voiceChatMsg; // 음성 채팅 내용
	
	Req_VoiceChat_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_VoiceChat_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> length >> voiceChatMsg;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << length << voiceChatMsg;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"length : %u", length );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"voiceChatMsg : %s", voiceChatMsg );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 음성 채팅 응답
struct Ack_VoiceChat_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_VoiceChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_VoiceChat_G2U);
		
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

// 음성 채팅 알림
struct Inf_VoiceChat_G2U : IMSerialize
{
	AID_t playerAID; // 음성 채팅 요청자
	uint16 length; // 길이
	std::wstring voiceChatMsg; // 음성 채팅 내용
	
	Inf_VoiceChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_VoiceChat_G2U);
		
		playerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> length >> voiceChatMsg;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << length << voiceChatMsg;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"length : %u", length );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"voiceChatMsg : %s", voiceChatMsg );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 언어 설정 요청
struct Req_SetLanguageType_U2G : IMSerialize
{
	uint8 languageType; // 설정 언어
	
	Req_SetLanguageType_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_SetLanguageType_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> languageType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << languageType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"languageType : %u", languageType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 언어 설정 응답
struct Ack_SetLanguageType_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_SetLanguageType_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_SetLanguageType_G2U);
		
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

// 언어 설정 응답
struct Inf_SetLanguageType_G2U : IMSerialize
{
	AID_t playerAID; // 설정 언어 플레이어 aid
	uint8 languageType; // 설정 언어
	
	Inf_SetLanguageType_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_SetLanguageType_G2U);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> playerAID >> languageType;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << playerAID << languageType;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"languageType : %u", languageType );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 허락 요청
struct Req_AgreeToSecretDialog_U2G : IMSerialize
{
	AID_t partnerAID; // 대화상대
	
	Req_AgreeToSecretDialog_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_AgreeToSecretDialog_U2G);
		
		partnerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> partnerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << partnerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"partnerAID : %llu", partnerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 허락 응답
struct Ack_AgreeToSecretDialog_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t partnerAID; // 대화상대
	bool agree; // 동의여부
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	
	Ack_AgreeToSecretDialog_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_AgreeToSecretDialog_G2U);
		
		result = INVALID_RESULT;
		partnerAID = INVALID_AID;
		agree = false;
		groupID = INVALID_CHAT_GROUP_ID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> partnerAID >> agree >> groupID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << partnerAID << agree << groupID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"partnerAID : %llu", partnerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"agree : %d", agree );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 허락 응답
struct Inf_AgreeToSecretDialog_G2U : IMSerialize
{
	std::list<AID_t> lstMember; // 대화상대
	bool agree; // 동의여부
	uint8 chatType; // 채팅타입 - Normal, Secret, Group
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	
	Inf_AgreeToSecretDialog_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_AgreeToSecretDialog_G2U);
		
		lstMember.clear();
		agree = false;
		chatType = 0;
		groupID = INVALID_CHAT_GROUP_ID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> lstMember >> agree >> chatType >> groupID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << lstMember << agree << chatType << groupID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstMember );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"agree : %d", agree );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"chatType : %u", chatType );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 의향 타진
struct Cmd_AgreeToSecretDialog_G2U : IMSerialize
{
	AID_t partnerAID; // 대화상대
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	
	Cmd_AgreeToSecretDialog_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Cmd_AgreeToSecretDialog_G2U);
		
		partnerAID = INVALID_AID;
		groupID = INVALID_CHAT_GROUP_ID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> partnerAID >> groupID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << partnerAID << groupID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"partnerAID : %llu", partnerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 의향 보고
struct Rpt_AgreeToSecretDialog_U2G : IMSerialize
{
	AID_t partnerAID; // 대화상대
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	bool agree; // 동의여부
	
	Rpt_AgreeToSecretDialog_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Rpt_AgreeToSecretDialog_U2G);
		
		partnerAID = INVALID_AID;
		groupID = INVALID_CHAT_GROUP_ID;
		agree = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> partnerAID >> groupID >> agree;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << partnerAID << groupID << agree;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"partnerAID : %llu", partnerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"agree : %d", agree );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 허락 요청
struct Req_EndOfSecretDialog_U2G : IMSerialize
{
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	
	Req_EndOfSecretDialog_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_EndOfSecretDialog_U2G);
		
		groupID = INVALID_CHAT_GROUP_ID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> groupID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << groupID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 허락 응답
struct Ack_EndOfSecretDialog_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_EndOfSecretDialog_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_EndOfSecretDialog_G2U);
		
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

// 비밀대화 허락 응답
struct Inf_EndOfSecretDialog_G2U : IMSerialize
{
	AID_t partnerAID; // 대화상대
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	
	Inf_EndOfSecretDialog_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_EndOfSecretDialog_G2U);
		
		partnerAID = INVALID_AID;
		groupID = INVALID_CHAT_GROUP_ID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> partnerAID >> groupID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << partnerAID << groupID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"partnerAID : %llu", partnerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 허락 요청
struct Req_SecretDialogInfo_U2G : IMSerialize
{
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	
	Req_SecretDialogInfo_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_SecretDialogInfo_U2G);
		
		groupID = INVALID_CHAT_GROUP_ID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> groupID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << groupID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 비밀대화 허락 응답
struct Ack_SecretDialogInfo_G2U : IMSerialize
{
	RESULT_t result; // 결과
	CHAT_GROUP_ID_t groupID; // 대화그룹 아이디
	std::list<AID_t> lstSecretDlgMember; // 비밀대화 맴버AID
	
	Ack_SecretDialogInfo_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_SecretDialogInfo_G2U);
		
		result = INVALID_RESULT;
		groupID = INVALID_CHAT_GROUP_ID;
		lstSecretDlgMember.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> groupID >> lstSecretDlgMember;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << groupID << lstSecretDlgMember;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"groupID : %u", groupID );
		ADD_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstSecretDlgMember );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// NPC에대한 답변 저장 요청
struct Req_DBSaveAnswerToNpc_U2G : IMSerialize
{
	TID_t dialogueTID; // DialogueData TID
	
	Req_DBSaveAnswerToNpc_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_DBSaveAnswerToNpc_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> dialogueTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << dialogueTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dialogueTID : %d", dialogueTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// NPC에대한 답변 저장 응답
struct Ack_DBSaveAnswerToNpc_G2U : IMSerialize
{
	RESULT_t result;
	TID_t dialogueTID; // DialogueData TID
	
	Ack_DBSaveAnswerToNpc_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_DBSaveAnswerToNpc_G2U);
		
		result = INVALID_RESULT;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> dialogueTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << dialogueTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dialogueTID : %d", dialogueTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 일상 이벤트 보상 요청
struct Req_LifeEventReward_U2G : IMSerialize
{
	TID_t dialogueTID; // DialogueData TID
	
	Req_LifeEventReward_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_LifeEventReward_U2G);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> dialogueTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << dialogueTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"dialogueTID : %d", dialogueTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 일상 이벤트 보상 응답
struct Ack_LifeEventReward_G2U : IMSerialize
{
	RESULT_t result;
	
	Ack_LifeEventReward_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_LifeEventReward_G2U);
		
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

// 번역 등록 요청
struct Req_RegistSpeaker_U2G : IMSerialize
{
	AID_t speakerAID; // 화자
	
	Req_RegistSpeaker_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_RegistSpeaker_U2G);
		
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 응답
struct Ack_RegistSpeaker_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t speakerAID; // 화자
	
	Ack_RegistSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_RegistSpeaker_G2U);
		
		result = INVALID_RESULT;
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 알림
struct Inf_RegistSpeaker_G2U : IMSerialize
{
	AID_t speakerAID; // 화자
	AID_t listenerAID; // 청자
	
	Inf_RegistSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_RegistSpeaker_G2U);
		
		speakerAID = INVALID_AID;
		listenerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID >> listenerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID << listenerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"listenerAID : %llu", listenerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 요청
struct Req_DeregistSpeaker_U2G : IMSerialize
{
	AID_t speakerAID; // 화자
	
	Req_DeregistSpeaker_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_DeregistSpeaker_U2G);
		
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 응답
struct Ack_DeregistSpeaker_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t speakerAID; // 화자
	
	Ack_DeregistSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_DeregistSpeaker_G2U);
		
		result = INVALID_RESULT;
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 알림
struct Inf_DeregistSpeaker_G2U : IMSerialize
{
	AID_t speakerAID; // 화자
	AID_t listenerAID; // 청자
	
	Inf_DeregistSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_DeregistSpeaker_G2U);
		
		speakerAID = INVALID_AID;
		listenerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID >> listenerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID << listenerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"listenerAID : %llu", listenerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 리스너 삭제 요청
struct Req_ReleaseListener_U2G : IMSerialize
{
	AID_t listenerAID; // 화자
	
	Req_ReleaseListener_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_ReleaseListener_U2G);
		
		listenerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> listenerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << listenerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"listenerAID : %llu", listenerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 리스터 삭제 응답
struct Ack_ReleaseListener_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t listenerAID; // 화자
	
	Ack_ReleaseListener_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_ReleaseListener_G2U);
		
		result = INVALID_RESULT;
		listenerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> listenerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << listenerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"listenerAID : %llu", listenerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 알림
struct Inf_ReleaseListener_G2U : IMSerialize
{
	AID_t speakerAID; // 화자
	AID_t listenerAID; // 청자
	
	Inf_ReleaseListener_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ReleaseListener_G2U);
		
		speakerAID = INVALID_AID;
		listenerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID >> listenerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID << listenerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"listenerAID : %llu", listenerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 리스트 요청
struct Req_VideoChatList_U2G : IMSerialize
{
	
	Req_VideoChatList_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_VideoChatList_U2G);
		
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

// 화상통화 리스트 응답
struct Ack_VideoChatList_G2U : IMSerialize
{
	RESULT_t result; // 결과
	std::list<stVideoChatMember> lstVideoChatMember; // 리스트
	
	Ack_VideoChatList_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_VideoChatList_G2U);
		
		result = INVALID_RESULT;
		lstVideoChatMember.clear();
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> lstVideoChatMember;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << lstVideoChatMember;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, lstVideoChatMember, L"stVideoChatMember" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 시작 요청
struct Req_BeginVideoChat_U2G : IMSerialize
{
	AID_t aidReceiver; // 상대방
	
	Req_BeginVideoChat_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_BeginVideoChat_U2G);
		
		aidReceiver = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aidReceiver;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aidReceiver;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aidReceiver : %llu", aidReceiver );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 시작 응답
struct Ack_BeginVideoChat_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t aidPartner; // 상대방
	
	Ack_BeginVideoChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_BeginVideoChat_G2U);
		
		result = INVALID_RESULT;
		aidPartner = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> aidPartner;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << aidPartner;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aidPartner : %llu", aidPartner );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 시작 알림
struct Inf_BeginVideoChat_G2U : IMSerialize
{
	stVideoChatMember requester; // 요청자
	
	Inf_BeginVideoChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_BeginVideoChat_G2U);
		
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> requester;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << requester;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, requester, L"stVideoChatMember" );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 시작 요청
struct Req_Agree2BeginVideoChat_U2G : IMSerialize
{
	AID_t aidRequester; // 요청자
	bool bAgree; // 수락여부
	
	Req_Agree2BeginVideoChat_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_Agree2BeginVideoChat_U2G);
		
		aidRequester = INVALID_AID;
		bAgree = false;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aidRequester >> bAgree;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aidRequester << bAgree;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aidRequester : %llu", aidRequester );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bAgree : %d", bAgree );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 시작 응답
struct Ack_Agree2BeginVideoChat_G2U : IMSerialize
{
	RESULT_t result; // 결과
	bool bAgree; // 수락여부
	AID_t aidRequester; // 요청자
	
	Ack_Agree2BeginVideoChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_Agree2BeginVideoChat_G2U);
		
		result = INVALID_RESULT;
		bAgree = false;
		aidRequester = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> bAgree >> aidRequester;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << bAgree << aidRequester;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bAgree : %d", bAgree );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aidRequester : %llu", aidRequester );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 시작 알림
struct Inf_Agree2BeginVideoChat_G2U : IMSerialize
{
	bool bAgree; // 수락여부
	AID_t aidPartner; // 상대방
	
	Inf_Agree2BeginVideoChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_Agree2BeginVideoChat_G2U);
		
		bAgree = false;
		aidPartner = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> bAgree >> aidPartner;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << bAgree << aidPartner;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bAgree : %d", bAgree );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aidPartner : %llu", aidPartner );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 종료 요청
struct Req_EndVideoChat_U2G : IMSerialize
{
	AID_t aidPartner; // 상대방
	
	Req_EndVideoChat_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_EndVideoChat_U2G);
		
		aidPartner = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aidPartner;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aidPartner;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aidPartner : %llu", aidPartner );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 종료 응답
struct Ack_EndVideoChat_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_EndVideoChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_EndVideoChat_G2U);
		
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

// 화상통화 종료 알림
struct Inf_EndVideoChat_G2U : IMSerialize
{
	
	Inf_EndVideoChat_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_EndVideoChat_G2U);
		
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

struct Inf_ChangeAPState_G2U : IMSerialize
{
	AID_t apAID; // ActionProp AID (서버에서 생성한 키값)
	uint8 apState; // 상태 - EWRAPState::en
	
	Inf_ChangeAPState_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_ChangeAPState_G2U);
		
		apAID = INVALID_AID;
		apState = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> apAID >> apState;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << apAID << apState;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"apAID : %llu", apAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"apState : %u", apState );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 마이크 에니메이션 요청
struct Req_MicAnimation_U2G : IMSerialize
{
	TID_t animationTID;
	
	Req_MicAnimation_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_MicAnimation_U2G);
		
		animationTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> animationTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << animationTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"animationTID : %d", animationTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 화상통화 종료 응답
struct Ack_MicAnimation_G2U : IMSerialize
{
	RESULT_t result; // 결과
	
	Ack_MicAnimation_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_MicAnimation_G2U);
		
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

// 화상통화 종료 알림
struct Inf_MicAnimation_G2U : IMSerialize
{
	AID_t aid; // 주체
	TID_t animationTID;
	
	Inf_MicAnimation_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_MicAnimation_G2U);
		
		aid = INVALID_AID;
		animationTID = INVALID_TID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> aid >> animationTID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << aid << animationTID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"aid : %llu", aid );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"animationTID : %d", animationTID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 요청
struct Req_RegistVideoSpeaker_U2G : IMSerialize
{
	AID_t speakerAID; // 화자
	
	Req_RegistVideoSpeaker_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_RegistVideoSpeaker_U2G);
		
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 응답
struct Ack_RegistVideoSpeaker_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t speakerAID; // 화자
	
	Ack_RegistVideoSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_RegistVideoSpeaker_G2U);
		
		result = INVALID_RESULT;
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 알림
struct Inf_RegistVideoSpeaker_G2U : IMSerialize
{
	AID_t speakerAID; // 화자
	AID_t listenerAID; // 청자
	
	Inf_RegistVideoSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_RegistVideoSpeaker_G2U);
		
		speakerAID = INVALID_AID;
		listenerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID >> listenerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID << listenerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"listenerAID : %llu", listenerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 요청
struct Req_DeregistVideoSpeaker_U2G : IMSerialize
{
	AID_t speakerAID; // 화자
	
	Req_DeregistVideoSpeaker_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_DeregistVideoSpeaker_U2G);
		
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 응답
struct Ack_DeregistVideoSpeaker_G2U : IMSerialize
{
	RESULT_t result; // 결과
	AID_t speakerAID; // 화자
	
	Ack_DeregistVideoSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_DeregistVideoSpeaker_G2U);
		
		result = INVALID_RESULT;
		speakerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> speakerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << speakerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 번역 등록 알림
struct Inf_DeregistVideoSpeaker_G2U : IMSerialize
{
	AID_t speakerAID; // 화자
	AID_t listenerAID; // 청자
	
	Inf_DeregistVideoSpeaker_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_DeregistVideoSpeaker_G2U);
		
		speakerAID = INVALID_AID;
		listenerAID = INVALID_AID;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> speakerAID >> listenerAID;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << speakerAID << listenerAID;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"speakerAID : %llu", speakerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"listenerAID : %llu", listenerAID );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 브로드캐스트 테스트 패킷 요청
struct Req_BroadcastTestPacket_U2G : IMSerialize
{
	stVector3 LinearVelocity;
	stVector3 AngularVelocity;
	stVector3 Location;
	stVector3 Rotation;
	uint8 bSimulatedPhysicSleep;
	uint8 bRepPhysics;
	uint8 LocationQuantizationLevel;
	uint8 VelocityQuantizationLevel;
	uint8 RotationQuantizationLevel;
	
	Req_BroadcastTestPacket_U2G()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Req_BroadcastTestPacket_U2G);
		
		bSimulatedPhysicSleep = 1;
		bRepPhysics = 1;
		LocationQuantizationLevel = 0;
		VelocityQuantizationLevel = 0;
		RotationQuantizationLevel = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> LinearVelocity >> AngularVelocity >> Location >> Rotation >> bSimulatedPhysicSleep >> bRepPhysics >> LocationQuantizationLevel >> VelocityQuantizationLevel >> RotationQuantizationLevel;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << LinearVelocity << AngularVelocity << Location << Rotation << bSimulatedPhysicSleep << bRepPhysics << LocationQuantizationLevel << VelocityQuantizationLevel << RotationQuantizationLevel;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, LinearVelocity, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, AngularVelocity, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Location, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Rotation, L"stVector3" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bSimulatedPhysicSleep : %u", bSimulatedPhysicSleep );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bRepPhysics : %u", bRepPhysics );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"LocationQuantizationLevel : %u", LocationQuantizationLevel );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"VelocityQuantizationLevel : %u", VelocityQuantizationLevel );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationQuantizationLevel : %u", RotationQuantizationLevel );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 브로드캐스트 테스트 패킷 응답
struct Ack_BroadcastTestPacket_G2U : IMSerialize
{
	RESULT_t result;
	stVector3 LinearVelocity;
	stVector3 AngularVelocity;
	stVector3 Location;
	stVector3 Rotation;
	uint8 bSimulatedPhysicSleep;
	uint8 bRepPhysics;
	uint8 LocationQuantizationLevel;
	uint8 VelocityQuantizationLevel;
	uint8 RotationQuantizationLevel;
	
	Ack_BroadcastTestPacket_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Ack_BroadcastTestPacket_G2U);
		
		result = INVALID_RESULT;
		bSimulatedPhysicSleep = 1;
		bRepPhysics = 1;
		LocationQuantizationLevel = 0;
		VelocityQuantizationLevel = 0;
		RotationQuantizationLevel = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> result >> LinearVelocity >> AngularVelocity >> Location >> Rotation >> bSimulatedPhysicSleep >> bRepPhysics >> LocationQuantizationLevel >> VelocityQuantizationLevel >> RotationQuantizationLevel;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << result << LinearVelocity << AngularVelocity << Location << Rotation << bSimulatedPhysicSleep << bRepPhysics << LocationQuantizationLevel << VelocityQuantizationLevel << RotationQuantizationLevel;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"result : %u", result );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, LinearVelocity, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, AngularVelocity, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Location, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Rotation, L"stVector3" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bSimulatedPhysicSleep : %u", bSimulatedPhysicSleep );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bRepPhysics : %u", bRepPhysics );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"LocationQuantizationLevel : %u", LocationQuantizationLevel );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"VelocityQuantizationLevel : %u", VelocityQuantizationLevel );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationQuantizationLevel : %u", RotationQuantizationLevel );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};

// 브로드캐스트 테스트 패킷 알림
struct Inf_BroadcastTestPacket_G2U : IMSerialize
{
	stVector3 LinearVelocity;
	stVector3 AngularVelocity;
	stVector3 Location;
	stVector3 Rotation;
	uint8 bSimulatedPhysicSleep;
	uint8 bRepPhysics;
	uint8 LocationQuantizationLevel;
	uint8 VelocityQuantizationLevel;
	uint8 RotationQuantizationLevel;
	
	Inf_BroadcastTestPacket_G2U()
	{
		m_Protocol = _PM::MakeProtocol( _PS::Seg_Game_Actor, EGameActorType::Inf_BroadcastTestPacket_G2U);
		
		bSimulatedPhysicSleep = 1;
		bRepPhysics = 1;
		LocationQuantizationLevel = 0;
		VelocityQuantizationLevel = 0;
		RotationQuantizationLevel = 0;
	}
	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg  >> LinearVelocity >> AngularVelocity >> Location >> Rotation >> bSimulatedPhysicSleep >> bRepPhysics >> LocationQuantizationLevel >> VelocityQuantizationLevel >> RotationQuantizationLevel;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg  << LinearVelocity << AngularVelocity << Location << Rotation << bSimulatedPhysicSleep << bRepPhysics << LocationQuantizationLevel << VelocityQuantizationLevel << RotationQuantizationLevel;
	}
	#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, GetProtocolString( _PM::GetSegment( m_Protocol ), _PM::GetType( m_Protocol ) ) );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, LinearVelocity, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, AngularVelocity, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Location, L"stVector3" );
		ADD_STRUCT_NAME_AND_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, Rotation, L"stVector3" );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bSimulatedPhysicSleep : %u", bSimulatedPhysicSleep );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"bRepPhysics : %u", bRepPhysics );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"LocationQuantizationLevel : %u", LocationQuantizationLevel );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"VelocityQuantizationLevel : %u", VelocityQuantizationLevel );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"RotationQuantizationLevel : %u", RotationQuantizationLevel );
		
	}
	#endif	// USE_PROTOCOL_TOSTRING
};
}
