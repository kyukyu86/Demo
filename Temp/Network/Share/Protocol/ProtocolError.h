#pragma once

struct EWRError
{
	enum en
	{
		Failure_SystemError																= 0,		// Server Query Error, critical
		Success																			= 1,		// 성공
		Failure_DB_Not_Exist_Main_Config												= 2,		// 메인 Config 존재하지 않음
		Failure_DB_Not_Exist_Login_Config												= 3,		// 로그인 Config 존재하지 않음
		Failure_DB_Not_Exist_Game_Config												= 4,		// 게임 Config 존재하지 않음

		//-----------------------------------------------------
		//	DB (1 ~ 10000)
		//-----------------------------------------------------

		// DB : 시스템(0~10)
		Failure_DB_SP_Error																= 10,		// DB 구문 오류

		// DB : 유저(11~100)
		Failure_DB_User_Already_Exist_USN												= 11,		// 이미 존재하는 USN

 		// DB : 플레이어(101~200)
 		Failure_DB_Player_Not_Exist_Player												= 101,		// 플레이어가 DB에서 존재하지 않음
		Failure_DB_Player_Already_Exist_Player											= 102,		// 플레이어가 DB에서 이미 존재함
		Failure_DB_Player_Already_Exist_PlayerName										= 103,		// 플레이어가 DB에서 이미 사용중인 플레이어 이름
		Failure_DB_Player_Not_Exist_SelectPlayerName									= 104,		// 플레이어가 DB에서 선택할 플레이어 이름이 모자람


		//-----------------------------------------------------
		//	서버간 통신 (10001 ~ 12000)
		//-----------------------------------------------------
		//	개별 서버를 Main 에 등록 실패 ( 10001 ~ 10100 )
		Failure_Common_Register_LoginServer												= 10001,	// LoginServer 등록 실패
		Failure_Common_Register_GameServer												= 10002,	// GameServer 등록 실패

		//	등록 서버 찾기 ( 10101 ~ 10200 )
		Failure_Common_Not_Register_GameServer											= 10101,	// 등록된 게임서버가 없습니다.


		//	개발 관련 ( 11901 ~ 12000 )
		Failure_Common_Non_Develop_Item													= 11999,	// 개발 항목이 아님
		Failure_Common_Developing														= 12000,	// 개발중

		//-----------------------------------------------------
		// 서버 컨텐츠 공통 (12001 ~ 14000)
		//-----------------------------------------------------
 		
		//	네트워크 오류 (12001 ~ 12100)
		Failure_Common_PacketError														= 12001,	// 패킷 오류
		Failure_Common_Different_Server_Protocol_Version								= 12002,	// 서버 프로토콜 버전이 다르다.
		Failure_Common_Not_Exist_User													= 12003,	// 유저가 존재하지 않습니다.
		Failure_Common_Already_Exist_User												= 12004,	// 이미 등록된 유저가 존재합니다.
		Failure_Common_Already_Accept													= 12005,	// 접속만 되어있는 상태입니다.
		Failure_Common_Already_CertifyRegistWaiting										= 12006,	// 인증/등록 대기 상태입니다.
		Failure_Common_Already_InLoginServer											= 12007,	// 로그인 서버 진입 상태입니다.
		Failure_Common_Already_MovingToGameServer										= 12008,	// 게임서버로 이동중인 상태입니다.
		Failure_Common_Already_InGameServer												= 12009,	// 게임 서버 진입 상태입니다.
		Failure_Common_Not_Exist_Server													= 12010,	// 서버를 찾지 못함

		Failure_Common_Previous_User_CertifyRegistWaiting								= 12011,	// 이전 유저가 인증/등록 대기 상태입니다.
		Failure_Common_Previous_User_MovingToGameServer									= 12012,	// 이전 유저가 게임서버로 이동중인 상태입니다.

		Failure_Common_Different_User_ID												= 12051,	// 유저의 아이디가 다르다.
		Failure_Common_Different_User_DKey												= 12052,	// 유저의 다이나믹키가 다르다.

		//	DB SP (12101 ~ 12200) 
		Failure_Common_ExecSP															= 12101,	// DB SP 실행 실패

		//	서버간 패킷 전송 실패 (12201 ~ 12300) 
		Failure_Common_Send_Packet_To_MainServer										= 12201,	// 메인서버에 패킷 전송 실패

		//	Invalid Component (12401 ~ 12500)
		Failure_Common_Not_Exist_Component_OwnerActor									= 12401,	// Component 에 주인 액터가 설정 안됨
		Failure_Common_Not_Exist_Component_Cheat										= 12402,	// CompCheat 정보가 없다.
		Failure_Common_Not_Exist_Component_Quest										= 12403,	// CompQuest 정보가 없다.
		Failure_Common_Not_Exist_Component_Stat											= 12404,	// CompStat 정보가 없다.
		Failure_Common_Not_Exist_Component_Move											= 12405,	// CompMove 정보가 없다.
		Failure_Common_Not_Exist_Component_Skill										= 12406,	// CompSkill 정보가 없다.
		Failure_Common_Not_Exist_Component_AI											= 12407,	// CompAI 정보가 없다.
		Failure_Common_Not_Exist_Component_Inventory									= 12408,	// CompInventory 정보가 없다.
		Failure_Common_Not_Exist_Component_Vehicle										= 12409,	// CompVehicle 정보가 없다.
		Failure_Common_Not_Exist_Component_Currency										= 12410,	// CompCurrency 정보가 없다.
		Failure_Common_Not_Exist_Component_Chat											= 12411,	// CompChat 정보가 없다.
		Failure_Common_Not_Exist_Component_Community									= 12412,	// CompCommunity 정보가 없다.
		

		//	테이블 데이터 / 리소스 로딩 (12501 ~ 13000)
		Failure_Common_Not_Exist_TableData_Map											= 12501,	// KMap 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_Item											= 12502,	// 아이템 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_Quest										= 12503,	// 퀘스트 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_QuestTask									= 12504,	// 퀘스트 태스크 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_Coordinate									= 12505,	// 좌표 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_Npc											= 12506,	// NPC 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_ObjectData									= 12507,	// 오브젝트 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_SocialRecord_Animation						= 12508,	// SocialRecord_Animation 해당 레코드를 찾지 못함
		Failure_Common_TransportRecord_Elevator_Is_Null									= 12509,	// TransportRecord_Elevator 가 존재하지 않음.
		Failure_Common_TransportRecord_Monorail_Is_Null									= 12510,	// TransportRecord_Monorail 가 존재하지 않음.
		Failure_Common_TransportRecord_Tardis_Is_Null									= 12511,	// TransportRecord_Tardis 가 존재하지 않음.
		Failure_Common_TransportRecord_FloatingBoard_Is_Null							= 12512,	// TransportRecord_FloatingBoard 가 존재하지 않음.
		Failure_Common_TransportRecord_Limit_Station_Count								= 12513,	// 역 최소 개수 설정 오류 (2이상)
		Failure_Common_Not_Exist_TableData_SocialRecord_Dance							= 12514,	// 소셜 댄스 테이블 데이터 정보가 없음
		Failure_Common_Not_Exist_TableData_SocialRecord_DanceGroup						= 12515,	// 소셜 댄스그룹 테이블 데이터 정보가 없음
		Failure_Common_Not_Exist_TableData_BattleField									= 12516,	// BattleField 테이블 데이터 정보가 없다.
		Failure_Common_Wrong_Range														= 12517,	// 데이타 범위값 오류
		Failure_Common_Not_Exist_TableData_Currency										= 12518,	// 재화 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_DialogueData									= 12519,	// 다이얼로그 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_DialogueData_DialogueSet						= 12520,	// 다이얼로그 세트 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_DialogueData_DialogueSetGroup				= 12521,	// 다이얼로그 세트 그룹 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_RewardData									= 12522,	// 보상 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_LifeEventData								= 12523,	// 일상 이벤트 테이블 데이터 정보가 없다.
		Failure_Common_MapDataPack_Is_Null												= 12524,	// MapDataPack 찾지 못함
		Failure_Common_TriggerData_Is_Null												= 12525,	// TriggerData 객체 정보 없음
		Failure_Common_ActionNode_Is_Null												= 12526,	// ActionNode 객체 정보 없음
		Failure_Common_TransitionNode_Is_Null											= 12527,	// TransitionNode 객체 정보 없음
		Failure_Common_TriggerNode_Is_Null												= 12528,	// TriggerNode 객체 정보 없음
		Failure_Common_Not_Exist_TableData_ShopData										= 12529,	// 상점 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_ShopCatalogData								= 12530,	// 상점 카탈로그 테이블 데이터 정보가 없다.
		Failure_Common_Not_Exist_TableData_Field										= 12531,	// Field 테이블 데이터 정보가 없다.

		//	Invalid (13001 ~ 13100)
		Failure_Common_Invalid_TID														= 13001,	// 유효하지 않은 TID
		Failure_Common_Invalid_Server_Index												= 13002,	// 유효하지 않은 SERVER_ID
		Failure_Common_Invalid_USN														= 13003,	// 유효하지 않은 USN
		Failure_Common_Invalid_PSN														= 13004,	// 유효하지 않은 PSN
		Failure_Common_Invalid_LanguageType												= 13005,	// 유효하지 않은 언어

		//	개별 서버를 MainServer에 등록 실패 ( 13101 ~ 13200 )
		Failure_Common_RegisterLoginServer												= 13101,	// LoginServer 등록 실패
		Failure_Common_DeregisterLoginServer											= 13102,	// LoginServer 등록 해제 실패
		Failure_Common_RegisterGameServer												= 13103,	// GameServer 등록 실패
		Failure_Common_DeregisterGameServer												= 13104,	// GameServer 등록 해제 실패
		Failure_Common_RegisterChatServer												= 13105,	// ChatServer 등록 실패
		Failure_Common_DeregisterChatServer												= 13106,	// ChatServer 등록 해제 실패

		Failure_Register_Exist_Server_Index												= 13110,	// 동일한 서버 인덱스가 존재함
		Failure_Register_Invalid_LimitStaticGameServer									= 13111,	// 정적 게임 서버 개수가 유효하지 않음

		//	게임 오류 (13201 ~ 13300)
		Failure_Common_DB_Call_Error													= 13201,	// DB 호출 실패
		Failure_Common_Not_Exist_Player													= 13202,	// 플레이어가 존재하지 않습니다.

		//  HTTP 및 구글 번역 관련 오류 (13301 ~ 13400)
		Failure_Http_API_Error															= 13301,	// http 요청에 대한 오류
		Failure_Http_Error_Invalid_Result												= 13302,	// http 응답에 대한 오류
		Failure_Http_GoogleTranslation_Json_Parsing										= 13303,	// json parsing

		//	맵 관련 정보 (13401 ~ 13500)
		Failure_Common_Invalid_Map_Index												= 13401,	// 유효하지 않은 Map Index
		Failure_Common_MapGroup_Is_Null													= 13402,	// MapGroup 객체 정보를 찾지 못함
		Failure_Common_Map_Is_Null														= 13403,	// Map 객체 정보를 찾지 못함
		Failure_Common_MapInfoGroup_Is_Null												= 13404,	// MapInfoGroup 객체 정보를 찾지 못함
		Failure_Common_MapInfo_Is_Null													= 13405,	// MapInfo 객체 정보를 찾지 못함
		Failure_Common_MapRecord_Is_Null												= 13406,	// MapRecord 객체 정보를 찾지 못함
		Failure_Common_Sector_Is_Null													= 13407,	// 섹터가 없음
		Failure_Common_ListSectorIndex_Is_Null											= 13408,	// 섹터 패킷용 섹터 목록 포인터가 널

		//	액터 (13501 ~ 13600)
		Failure_Common_Actor_Is_Null													= 13501,	// Actor 객체 정보를 찾지 못함
		Failure_Common_Player_Is_Null													= 13502,	// Player 객체 정보를 찾지 못함
		Failure_Common_User_Is_Null														= 13503,	// User 객체 정보를 찾지 못함
		Failure_Common_Invalid_ActorType												= 13504,	// Actor 타입이 잘못됨
		Failure_Invalid_Actor_State														= 13505,	// ActorState
		Failure_Common_ActorManager_Is_Null												= 13506,	// ActorManager 객체 정보를 찾지 못함
		Failure_Invalid_Costume_TID														= 13507,	// 유효하지 않은 CostumeTID
		Failure_Get_Player_Name															= 13510,	// 캐릭터 이름 가져오기 실패
		Failure_Common_Invalid_Select_PSN												= 13511,	// user 에 선택된 PSN 이 없음
		Failure_INvalid_AI_State														= 13512,	// AI 상태 이상
		Failure_Common_Invalid_VehicleAID												= 13513,	// vehicle AID 가 없음
		Failure_Common_Vehicle_Is_Null													= 13514,	// vehicle 객체 정보를 찾지 못함	

		//	서버에서 강제킥 상황 발생 (13901 ~ 14000)
		Failure_ForceKick_Invalid														= 13901,	// 에러 설정되지 않은 ForceKick 유형
		Failure_ForceKick_Push															= 13902,	// 같은계정의 접속으로인한 강제종료
		Failure_ForceKick_GM															= 13903,	// 서비스운영에 의한 강제종료
		Failure_ForceKick_Error															= 13904,	// 프로그램 오류에 의한 강제종료
		Failure_ForceKick_DBError														= 13905,	// DB 오류에 의한 강제종료
		Failure_ForceKick_Timeout														= 13906,	// 장기간 대기로인한 강제종료					
		Failure_ForceKick_Etc															= 13907,	// 기타사유에의한 강제종료
		Failure_ForceKick_Channel														= 13908,	// 채널변경에의한 강제종료
		Failure_ForceKick_ShutdownTime													= 13909,	// 셧다운 제도에 의한 강제종료
		Failure_ForceKick_AutoCheck														= 13910,	// 오토 체크에 의한 강제 종료
		Failure_ForceKick_MoveServer													= 13911,	// 물리 서버 변경으로 종료
		Failure_ForceKick_AbuseMovingCheck												= 13912,	// 비정상적 이동으로 인한 강제종료
		Failure_ForceKick_HackingPacket													= 13913,	// 비정상적 패킷으로 인한 강제종료
		Failure_ForceKick_SequenceNumberCheck											= 13914,	// 메시지 시퀀스 넘버 비정상으로 인한 강제 종료
		Failure_ForceKick_Disconnect													= 13915,	// 연결 끊김에 의한 강제 종료
		Failure_ForceKick_InvalidCurrencyDetected										= 13916,	// 유효하지 않은 재화값 획득 감지에 의한 강제 종료
		Failure_ForceKick_ToolCommand													= 13917,	// 운영툴을 사용한 연결 끊김 (강제 밴)
			

		Failure_Common_Not_Ready_For_Service											= 14000,	// 서버스 준비되지 않음

		//-----------------------------------------------------
		// 서버 컨텐츠 개별 (14001 ~ )
		//-----------------------------------------------------

		//	맵 생성 (14001 ~ 14100)
		Failure_Map_Create_Already_Exist_Map_Index										= 14001,	// 동일한 MID 로 생성되어 있는 맵이 존재함
		Failure_Map_Create_MapGroup_Insert_Failed										= 14002,	// MapGroup 컨테이너 등록 실패
		Failure_Map_Create_MapInfoGroup_Insert_Failed									= 14003,	// MapInfoGroup 컨테이너 등록 실패
		Failure_Map_Create_Init_Failed													= 14004,	// 맵 초기화 실패
		Failure_Map_Create_MapDataPack_Is_Null											= 14005,	// MapDataPack 찾지 못함
		Failure_Map_Create_TriggerManager_Init_Failed									= 14006,	// TriggerManager 구성에 실패
		Failure_Map_Create_MapRecord_Is_Null											= 14007,	// MapRecord 찾지 못함
		Failure_Map_Create_JsonData_Is_Null												= 14008,	// JsonData 찾지 못함
		Failure_Map_Create_Invalid_JsonData_Type										= 14009,	// JsonData_Type 찾지 못함
		Failure_Map_Create_MapRespawnInfo_Insert_Failed									= 14010,	// MapRespawnInfo 컨테이너 등록 실패
		Failure_Map_Create_SectorData_Is_Null											= 14011,	// SectorData 찾지 못함
		Failure_Map_Create_SectorTotalCount_Is_Invalid									= 14012,	// Sector 총 개수 오류
		Failure_Map_Create_Insert_Sector												= 14013,	// Sector 에 추가 실패
		Failure_Map_Create_ActorManager_Init_Failed										= 14014,	// ActorManager 초기화 실패
		Failure_Map_Create_TriggerVolumeCenter_Init_Failed								= 14015,	// TriggerVolumeCenter 초기화 실패

		//	맵 제거 (14101 ~ 14200)
		Failure_Map_Destroy_Remove_MapGroup												= 14101,	// MapGroup 에서 제거하기 실패		
		Failure_Map_Destroy_Aready_Destroying											= 14102,	// MapInfo 이미 삭제 대기중임
		Failure_Map_Destroy_In_DestroyContainer											= 14103,	// Map 이미 삭제 대기 컨테이너에 있음

		//	MainServer
		Failure_MapInfo_Destroy_Remove_MapInfoGroup										= 14151,	// MapInfoGroup 에서 제거하기 실패
		Failure_MapInfo_Destroy_Aready_Destroying										= 14152,	// MapInfo 이미 삭제 대기중임
		Failure_MapInfo_Destroy_In_DestroyContainer										= 14153,	// MapInfo 이미 삭제 대기 컨테이너에 있음

		//	맵 데이타 팩 (14201 ~ 14300)
		Failure_MapData_Pack_Empty_SpawnIORecordList									= 14201,	// SpawnIORecordList 가 비어있음
		Failure_MapData_Pack_Empty_SpawnENpcRecordList									= 14202,	// SpawnENpcRecordList 가 비어있음
		Failure_MapData_Pack_Empty_SpawnNpcRecordList									= 14203,	// SpawnNpcRecordList 가 비어있음
		
		//	플레이어 맵 이동 (14301 ~ 14400)
		Failure_Player_MoveToMap_Different_PSN											= 14301,	// 현재 선택된 PSN 과 다름
		Failure_Player_MoveToMap_Already_In_Map											= 14302,	// 이미 해당 맵에 존재함
		Failure_Player_MoveToMap_Already_Make_Reservation								= 14303,	// 이미 예약되어 있음
		Failure_Player_MoveToMap_Not_Found_Player										= 14304,	// 해당 맵에서 플레이어 찾지 못함
		Failure_Player_MoveToMap_Not_Found_Reservation									= 14305,	// 예약자 찾지 못함
		Failure_Player_MoveToMap_Aleady_Enter_map										= 14306,	// 이미 입장해 있는 맵이 있음. 기존 맵에서 퇴장 필요
		Failure_Player_MoveToMap_DestoryMap												= 14307,	// 삭제 대기중인 맵
		Failure_Player_MoveToMap_Reserving												= 14308,	// 예약 진행중	
		Failure_Player_MoveToMap_MC_Same_Map											= 14309,	// Map Connector 로 이동 요청하였으나, 이동하고자 하는 MapTID 가 현재와 동일

		//	액터 생성 (15001 ~ 15200)
		Failure_Actor_CreateActor_Invalid_Map											= 15001,	// 맵 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_ActorTID										= 15002,	// 액터 테이블 아이디 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Generate_ActorID										= 15003,	// 액터 아이디 생성이 실패하였습니다.
		Failure_Actor_CreateActor_Invalid_ActorType										= 15004,	// 생성될 액터 유형 값이 유효하지 않습니다.
		Failure_Actor_CreateActor_Not_Exist_User										= 15005,	// 생성될 플레이어의 유저 정보가 존재하지 않습니다.
		Failure_Actor_CreateActor_InitASM												= 15006,	// ASM 초기화가 실패 하였습니다.
		Failure_Actor_CreateActor_EnterMap												= 15007,	// 맵 진입에 실패 하였습니다.
		Failure_Actor_CreateActor_SetUpStat												= 15008,	// 스텟 설정에 실패 하였습니다.
		Failure_Actor_CreateActor_SetRealmInfo											= 15009,	// 세력 설정에 실패 하였습니다.
		Failure_Actor_CreateActor_TableRecordActor										= 15010,	// TableRecordActor 객체가 없음

		Failure_Actor_CreateActor_Invalid_CreateInfoActor								= 15011,	// 액터 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoPlayer								= 15012,	// 플레이어 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoFellow								= 15013,	// 펠로우 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoNpc									= 15014,	// NPC 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoInteractionObject					= 15015,	// 인터렉션 생성 오브젝트 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoPortal								= 15016,	// 포탈 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoBrick								= 15017,	// 브릭 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoSkillObject							= 15018,	// 스킬 오브젝트 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoDropObject							= 15019,	// 드랍 오브젝트 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoActiveObject						= 15020,	// 액티브 오브젝트 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoActionProp							= 15021,	// ActionProp 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoElevator							= 15022,	// Elevator 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoMonorail							= 15023,	// Monorail 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoTardis								= 15024,	// Tardis 생성 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_CreateInfoFloatingBoard						= 15025,	// FloatingBoard 생성 정보가 유효하지 않습니다.
			

		Failure_Actor_CreateActor_Invalid_Actor											= 15031,	// 액터 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Player										= 15032,	// 플레이어 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Fellow										= 15033,	// 펠로우 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Npc											= 15034,	// NPC 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_IO											= 15035,	// 인터렉션 오브젝트 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Portal										= 15036,	// 포탈 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Brick											= 15037,	// 브릭 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_SO											= 15038,	// 스킬 오브젝트 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_DropObject									= 15039,	// 드랍 오브젝트 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_AO											= 15040,	// 액티브 오브젝트 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_ActionProp									= 15041,	// ActionProp 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Elevator										= 15042,	// Elevator 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Monorail										= 15043,	// Monorail 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_Tardis										= 15044,	// Tardis 정보가 유효하지 않습니다.
		Failure_Actor_CreateActor_Invalid_FloatingBoard									= 15045,	// FloatingBoard 정보가 유효하지 않습니다.

		Failure_Actor_CreateActor_Already_Exist_Actor									= 15071,	// 생성될 액터가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Player									= 15072,	// 생성될 플레이어가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Fellow									= 15073,	// 생성될 펠로우가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Npc										= 15074,	// 생성될 NPC가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_IO										= 15075,	// 생성될 인터렉션 오브젝트가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Portal									= 15076,	// 생성될 포탈이 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Brick									= 15077,	// 생성될 브릭이 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_SO										= 15078,	// 생성될 스킬 오브젝트가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_DropObject								= 15079,	// 생성될 드랍 오브젝트가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_AO										= 15080,	// 생성될 액티브 오브젝트가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_ActionProp								= 15081,	// 생성될 ActionProp 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Elevator								= 15082,	// 생성될 Elevator 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Monorail								= 15083,	// 생성될 Monorail 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Tardis									= 15084,	// 생성될 Tardis 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_FloatingBoard							= 15085,	// 생성될 FloatingBoard 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Vehicle									= 15086,	// 생성될 Vehicle 존재합니다.
			
		Failure_Actor_CreateActor_Invalid_PSN											= 15091,	// 유효하지 않은 PSN
		Failure_Actor_CreateActor_Invalid_Name											= 15092,	// 유효하지 않은 Name
		Failure_Actor_CreateActor_Invalid_ActorState									= 15093,	// 유효하지 않은 ActorState
		Failure_Actor_CreateActor_For_NonPlayer											= 15094,	// 비 플레이어 전용함수를 사용함
		Failure_Actor_CreateActor_For_Player											= 15095,	// 플레이어 전용함수를 사용함
		Failure_Actor_CreateActor_SpawnRecord_Is_Null									= 15096,	// stISpawnRecord 정보를 찾지 못함
		Failure_Actor_CreateActor_Derived_SpawnRecord_Is_Null							= 15097,	// 상속받은 SpawnRecord 정보를 찾지 못함
		Failure_Actor_CreateActor_Not_Exist_SpawnInfo									= 15098,	// SpawnInfo 정보를 찾지 못함
		Failure_Actor_CreateActor_ReadyToPlay_EnteredMap_Is_Null						= 15099,	// EnteredMap 정보를 찾지 못함

		Failure_Actor_CreateActor_Already_Exist_ActiveActor								= 15101,	// ActiveActor에 이미 존재함
		Failure_Actor_CreateActor_Already_Exist_Player_Socket							= 15102,	// Socket 맵에 존재함
		Failure_Actor_CreateActor_Already_Exist_Player_PSN								= 15103,	// PSN 맵에 존재함
		Failure_Actor_CreateActor_Already_Exist_Player_AID								= 15104,	// AID 맵에 존재함
		Failure_Actor_CreateActor_Already_Exist_Player_Name								= 15105,	// Name 맵에 존재함
		Failure_Actor_CreateActor_Already_Exist_Player_DisconnetPSN						= 15106,	// DisconnetPSN 맵에 존재함
		Failure_Actor_CreateActor_Already_Exist_SpawnActor								= 15107,	// 해당 SpawnTID 에 해당하는 Actor 가 존재함 

		Failure_Actor_CreateActor_Already_Exist_Component								= 15111,	// 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Stat							= 15112,	// Stat 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Cheat							= 15113,	// Cheat 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Inventory						= 15114,	// Inventory 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Qeust							= 15115,	// Qeust 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Move							= 15116,	// Move 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Skill							= 15117,	// Skill 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_AI							= 15118,	// AI 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_ABState						= 15119,	// ABState 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Buff							= 15120,	// Buff 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Vehicle						= 15121,	// VSM 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Currency						= 15122,	// Currency 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Chat							= 15123,	// Chat 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Reward						= 15124,	// Reward 컴포넌트 정보가 존재합니다.
		Failure_Actor_CreateActor_Already_Exist_Component_Community						= 15125,	// Community 컴포넌트 정보가 존재합니다.

		Failure_Actor_CreateActor_Init_Component										= 15151,	// 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Stat									= 15152,	// Spawn 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Cheat									= 15153,	// Cheat 컴포넌트 초기화 실패		
		Failure_Actor_CreateActor_Init_Component_Inventory								= 15154,	// Inventory 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Qeust									= 15155,	// Qeust 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Move									= 15156,	// Move 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Skill									= 15157,	// Skill 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_AI										= 15158,	// AI 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_ABState								= 15159,	// ABState 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Buff									= 15160,	// Buff 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Vehicle								= 15161,	// VSM 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Currency								= 15162,	// Currency 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Chat									= 15163,	// Chat 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Reward									= 15164,	// Reward 컴포넌트 초기화 실패
		Failure_Actor_CreateActor_Init_Component_Community								= 15165,	// Reward 컴포넌트 초기화 실패

		Failure_Actor_CreateActor_SetupComponentFirst_Base								= 15201,	// 기본 정보 설정이 실패하였습니다.
		Failure_Actor_CreateActor_SetupComponentFirst_Quest								= 15202,	// 퀘스트 정보 설정이 실패하였습니다.
		Failure_Actor_CreateActor_SetupComponentFirst_Inventory							= 15203,	// 인벤토리 정보 설정이 실패하였습니다.
		Failure_Actor_CreateActor_SetupComponentFirst_Currency							= 15204,	// 재화 정보 설정이 실패하였습니다.

		Failure_Actor_CreateActor_SetupAutoASM_ActorState_Is_Null						= 15211,	// ActorState 정보 없음
		Failure_Actor_CreateActor_SetupAutoASM_Failed									= 15212,	// ActorState 항목별 세팅 실패

		Failure_Actor_CreateActor_Invalid_VehicleState									= 15221,	// VehicleState 항목별 세팅 실패
		Failure_Actor_CreateActor_CurVehicleState_Is_Null								= 15222,	// CurVehicleState 객체 없음
		Failure_Actor_CreateActor_SetupAutoVSM_VehicleState_Is_Null						= 15223,	// VehicleState 객체 없음
		Failure_Actor_CreateActor_SetupAutoVSM_Failed									= 15224,	// SetupAutoVSM 초기화 실패

			
		//	액터 상태 (15301 ~ 15400)
		Failure_Actor_ASM_CurActorState_Is_Null											= 15301,	// 현재 선택된 액터 상태 객체가 존재하지 않음
		Failure_Actor_ASM_Unable_Next_state												= 15302,	// 다음 상태로 진행할 수 없음
		Failure_Actor_ASM_Is_Not_Alive													= 15303,	// 살아있는 상태가 아님
		Failure_Actor_Unmovable															= 15304,	// 움직일 수 없는 액터


		//	액터 제거 (15401 ~ 15500)
		Failure_Actor_Destroy_Aready_Destroying											= 15401,	// 이미 제거 진행중
		Failure_Actor_Destroy_In_DestroyContainer										= 15402,	// 이미 제거 진행중

		//  AI 상태 에러 (15501 ~ 15600)
		Failure_Actor_AI_Start_Error													= 15501,

		//	주종 관계 (15601 ~ 15700)
		Failure_Actor_Is_Not_Master_Servant_Relationship								= 15601,	// 주종 관계가 아님
		Failure_Actor_Not_Exist_Master													= 15602,	// 주인 객체를 찾지 못함
		Failure_Actor_Not_Exist_Slave													= 15603,	// 종속 객체를 찾지 못함
		Failure_Actor_Is_Not_My_Master													= 15604,	// 자신의 주인이 아님
		Failure_Actor_Is_Not_My_Slave													= 15605,	// 자신의 종속이 아님
		Failure_Actor_Already_Master													= 15606,	// 이미 주인
		Failure_Actor_Already_Slave														= 15607,	// 이미 종속
		Failure_Actor_Already_Exist_Master												= 15608,	// 이미 주인이 있음
		Failure_Actor_Already_Exist_Slave												= 15609,	// 이미 종속이 있음
		Failure_Actor_Slave_Warp_Position_Is_Invalid									= 15610,	// 종속 객체의 워프 위치 검색 실패

		//	리커넥트 (15801 ~ 15900) 
		Failure_Reconnect_Not_Found_Player												= 15801,	// 플레이어를 찾지 못함

		//	UserSequence (15901 ~ 16000) 
		Failure_Lab_UserSequence_Not_InLab												= 15901,	// 현재 위치가 [연구소]가 아님
		Failure_Lab_UserSequence_Already_InLab											= 15902,	// 현재 위치가 이미 [연구소]임
		Failure_Lab_UserSequence_Wrong													= 15903,	// 올바르지 않은 유저 시퀀스

		//	인벤토리 (16001 ~ 16300)
		Failure_Inventory_Item_Insert_Failed											= 16001,	// MapItem 컨테이너 등록 실패
		Failure_Inventory_Not_Found_Item												= 16002,	// 존재하지 않는 아이템
		Failure_Inventory_Already_Exist_Item											= 16003,	// 이미 존재하는 아이템
		Failure_Inventory_Invalid_Item													= 16004,	// 유효하지 않은 아이템
		Failure_Inventory_Invalid_Value													= 16005,	// 유효하지 않은 값
		Failure_Inventory_Invalid_ItemSerialNumber										= 16006,	// 유효하지 않은 ISN
		Failure_Inventory_Invalid_ItemCount												= 16007,	// 유효하지 않은 아이템 개수
		Failure_Inventory_Invalid_ItemType												= 16008,	// 유효하지 않은 아이템 타입
		Failure_Inventory_Can_Not_Remove_Already_Equipped_Item							= 16009,	// 장착중인 아이템 제거불가
		Failure_Inventory_Not_Equipped_Item												= 16010,	// 장착중이지 않은 아이템
		Failure_Inventory_Already_Equipped_Item											= 16011,	// 이미 장착중인 아이템
		Failure_Inventory_Invalid_Category												= 16012,	// 유효하지 않은 카테고리 타입
		Failure_Inventory_Can_Not_Remove_Already_Registered_Item						= 16013,	// 슬롯 등록중인 아이템 제거불가
		Failure_Inventory_Invalid_ItemEquipType											= 16014,	// 유효하지 않은 장착 타입
		Failure_Inventory_Not_Registered_Item											= 16015,	// 슬롯에 등록되어있지 않은 아이템
		Failure_Inventory_Already_Registered_Item										= 16016,	// 이미 슬롯에 등록중인 아이템
		Failure_Inventory_Invalid_RegistSlotType										= 16017,	// 유효하지 않은 슬롯 등록 타입
		Failure_Inventory_Invalid_InventoryCategoryType									= 16018,	// 유효하지 않은 인벤토리 카테고리 타입
		Failure_Inventory_Lack_Slot_Count												= 16019,	// 인벤토리 공간 부족
		Failure_Inventory_Interior_Not_Found_Actor										= 16020,	// Interior Actor 찾지 못함
		Failure_Inventory_Interior_Already_Arrange										= 16021,	// 이미 설치되어 있음
		Failure_Inventory_Interior_Failed_Insert_Container								= 16022,	// Interior Actor insert 실패
		Failure_Inventory_Interior_Not_Interior_Item									= 16023,	// Interior Item 이 아님

		// 트리거 (16401 ~ 16500)
		Failure_Trigger_Invalid_TransitionNode											= 16401,	// 알수 없는 트랜지션 노드 타입
		Failure_Trigger_Process_TriggerNode												= 16402,	// 트리거 실행 실패
		Failure_Trigger_Not_Exist_Trigger_Data											= 16403,	// 트리거 데이터 없음
		Failure_Trigger_MovePos_Check_Fail												= 16404,	// 트리거 실행시 위치 유효성 체크 실패
		Failure_Trigger_Process_TransitionNode											= 16405,	// 트랜지션 노드 실행 실패
		Failure_Trigger_IsDisableTriggerNode											= 16406,	// 사용 불가능한 노드
		Failure_Trigger_Not_Match_TriggerFlowIndex										= 16407,	// 트리거 실행 순서 불일치
		Failure_Trigger_IsUsedTriggerNode												= 16408,	// 이미 사용된 트리거 노드
		Failure_Trigger_Not_Exist_TriggerManager										= 16409,	// 트리거 관리자 없음
		Failure_Trigger_Invalid_NodeTID													= 16410,	// 유효하지 않은 트리거 노드 TID
		Failure_Trigger_Invalid_TriggerNodeType											= 16411,	// 유효하지 않은 TriggerNodeType

		// Skill (16501~16600)
		Failure_Create_Skill_Task														= 16501,
		Failure_Skill_Skill_Task_Is_Not_Found											= 16502,
		Failure_Skill_Invalid_SkillTID													= 16503,
		Failure_Not_Enough_Skill_CoolTime												= 16504,
		Failure_Player_Warp_Fail														= 16505,	// 플레이어의 워프 실패
		Failure_Hit_Target_Defense														= 16506,	// 타겟이 방어 중 이므로 히트 되지 않음
		Failure_Hit_By_Ignore_Reaction													= 16507,	// 리액션 무시에 의해 히트 무시됨

		//	퀘스트 (16601 ~ 16700)
		Failure_Quest_Progress_Insert_Failed											= 16601,	// 진행중인 퀘스트 컨테이너 등록 실패
		Failure_Quest_Complete_Insert_Failed											= 16602,	// 완료된 퀘스트 컨테이너 등록 실패
		Failure_Quest_Task_Insert_Failed												= 16603,	// 퀘스트 태스크 컨테이너 등록 실패
		Failure_Quest_Not_Found_Quest													= 16604,	// 존재하지 않는 퀘스트
		Failure_Quest_Not_Found_Quest_Task												= 16605,	// 존재하지 않는 퀘스트 태스크
		Failure_Quest_Already_Progress_Quest											= 16606,	// 이미 진행중인 퀘스트
		Failure_Quest_Already_Complete_Quest											= 16607,	// 이미 완료된 퀘스트
		Failure_Quest_Invalid_Quest														= 16608,	// 유효하지 않은 퀘스트
		Failure_Quest_Invalid_QuestType													= 16609,	// 유효하지 않은 퀘스트 타입
		Failure_Quest_Invalid_QuestTaskType												= 16610,	// 유효하지 않은 퀘스트 태스크 타입
		Failure_Quest_Invalid_QuestState												= 16611,	// 유효하지 않은 퀘스트 상태
		Failure_Quest_Start_Condition_Is_Not_Correct									= 16612,	// 퀘스트 시작 조건이 맞지 않음
		Failure_Quest_Progress_State_incorrect											= 16613,	// 퀘스트 진행 상태가 맞지 않음
		Failure_Quest_Can_Not_Completable_State											= 16614,	// 완료 가능 상태가 될 수 없는 상태(완료조건 미충족)
		Failure_Quest_Can_Not_Receive_Reward_State										= 16615,	// 완료 보상을 받을 수 없는 상태(완료 보상조건 미충족)
		Failure_Quest_Invalid_Quest_Task_Style											= 16616,	// 유효하지 않은 퀘스트 태스크 진행방식
		Failure_Quest_Not_Completable_Task												= 16617,	// 완료 가능 상태의 태스크가 아님
		Failure_Quest_Not_Dest_Map														= 16618,	// 목적지/위치해있어야 할 맵이 아님
		Failure_Quest_Not_Dest_Position													= 16619,	// 목적지 좌표가 아님
		Failure_Quest_Not_Progress_Quest												= 16520,	// 진행중인 퀘스트가 아님
		Failure_Quest_Not_Complete_Quest												= 16521,	// 완료된 퀘스트가 아님
		Failure_Quest_Invalid_QuestActionType											= 16622,	// 유효하지 않은 퀘스트 액션 타입
		Failure_Quest_Invalid_StartConditionType										= 16623,	// 유효하지 않은 퀘스트 시작 조건 타입
		Failure_Quest_Can_Not_Progress_IO_Quest											= 16624,	// IO채집관련 진행중인 퀘스트가 없음

		//	부활 (16701 ~ 16800)
		Failure_Revive_Is_Not_Coma														= 16701,	// Coma 상태가 아님
		Failure_Revive_Unable_MapType													= 16702,	// 부활 불가 맵 유형
		Failure_Revive_Failed_ReBorn													= 16703,	// 기본 부활 불가
		Failure_Revive_Remain_CoolTime_DisputeRevive									= 16704,	// 쿨타임이 아직 안됨
		Failure_Revive_Ing																= 16705,	// 부활 진행중
		Failure_Revive_Failed_Resurrection												= 16706,	// 기본 부활 불가

		//	치트 (16801 ~ 16900)
		Failure_Cheat_Wrong_MSG															= 16801,	// 치트 메시지가 아님
		Failure_Cheat_Cannot_Parse														= 16802,	// 파싱 실패
		Failure_Cheat_Unknown_Command													= 16803,	// 없는 치트
		Failure_Cheat_The_Number_Of_Parameters_Is_Incorrect								= 16804,	// 치트 파라미터 개수 오류
		Failure_Cheat_Not_Found_Cheat_Tag												= 16805,	// 치트임을 알리는 형식이 없음
		Failure_Cheat_Is_Not_Player														= 16806,	// 치트 요청 대상이 플레이어가 아님
		Failure_Cheat_No_Permission														= 16807,	// 치트 권한 없음
		Failure_Cheat_Not_Found_Master													= 16808,	// 치트 사용한 플레이어 객체를 찾지 못함

		//	운송체 (16801 ~ 16900)
		Failure_Vehicle_VSM_CurVehicleState_Is_Null										= 16901,	// CurVehicleState 객체 없음
		Failure_Vehicle_VSM_Unable_Next_state											= 16902,	// VehicleState 다음 단계 진행 실패
		Failure_Vehicle_Already_Exist_Passenger											= 16903,	// 승객 명단에 이미 존재함
		Failure_Vehicle_Not_Exist_Passenger												= 16904,	// 승객 명단에 없음
		Failure_Vehicle_Already_GetOn_Other_Vehicle										= 16905,	// 다른 운송수단에 탑승해 있음
		Failure_Vehicle_Not_Allow_Boarding_State										= 16906,	// 탑승 허용 가능 상태가 아님
		Failure_Vehicle_Does_Not_Allow_NonPlayer										= 16907,	// 플레이어가 아님 (플레이어만 가능한 탑승체)
		Failure_Vehicle_Is_Full															= 16908,	// 가득 참
		Failure_Vehicle_Tardis_Arrival_And_Current_Map_Is_Same							= 16909,	// Tardis - 현재맵과 도착맵이 동일함
		Failure_Vehicle_Is_Not_Vehicle_Packet											= 16910,	// 탑승체용 패킷이 아님
		Failure_Vehicle_Different_VehicleAID											= 16911,	// 현재 탑승한 탑승체와 다른 탑승체 AID
		Failure_Vehicle_Not_On_Board													= 16912,	// 현재 탑승한 탑승체가 없음
		Failure_Vehicle_Is_Not_Vehicle													= 16913,	// 탑승체가 아님
		Failure_Vehicle_Is_Not_Passenger												= 16914,	// 승객이 아님
		Failure_Vehicle_Seat_Index_Is_Invalid											= 16915,	// 시트 인덱스가 올바르지 않음
		Failure_Vehicle_Not_Allow_Boarding_SeatIndex									= 16916,	// 허락되지 않는 시트
		Failure_Vehicle_Is_Not_Driver													= 16917,	// 드라이버가 아님

		// 사다리 (17001 ~ 17100)
		Failure_Ladder_Moving															= 17001,	// 사다리 이동 상태입니다.
		Failure_Ladder_Not_Moving														= 17002,	// 사다리 이동 상태가 아닙니다.

		// 소셜 (17101 ~ 17300)
		Failure_Social_Already_Talking													= 17101,	// 이미 대화중인 상태
		Failure_Social_Not_Talking														= 17102,	// 대화중이지 않은 상태
		Failure_Social_Already_Dancing													= 17103,	// 이미 댄스중인 상태
		Failure_Social_Not_Dancing														= 17104,	// 댄스중이지 않은 상태

		// 점유 (17301 ~ 17400)
		Failure_Occupy_Invalid_IOAID													= 17301,	// 점유 상태가 아니다

		//	그룹 맵 이동 (17401 ~ 17500)
		Failure_Group_MoveToMap_Empty_Passenger_List									= 17401,	// 승객 명단이 비어있음
		
		// 오브젝트 들기 상호 작용 (17501 ~ 17600)
		Failure_Cheers_Suggest_Not_EquppedDrink											= 17501,	// 요청자가 술잔을 들고 있지 않음
		Failure_Cheers_Answer_Not_EquppedDrink											= 17502,	// 응답자가 술잔을 들고 있지 않음
		Failure_Cheers_Not_Allow_ActorType												= 17503,	// 건배를 할수 없는 ActorType
		Failure_Cheers_Not_NpcJobType_Drinker											= 17504,	// 건배를 할수 없는 Npc Job Type
		Failure_Cheers_Suggest_CheersCondition_Suggest									= 17505,	// 요청자가 건배 요청/대기중 상태임
		Failure_Cheers_Suggest_CheersCondition_Answer									= 17506,	// 요청자가 건배 응답/대기중 상태임
		Failure_Cheers_Answer_CheersCondition_Suggest									= 17507,	// 응답자가 건배 요청/대기중 상태임
		Failure_Cheers_Answer_CheersCondition_Answer									= 17508,	// 응답자가 건배 응답/대기중 상태임

		Failure_Cheers_Answer_Not_CheersCondition_Answer								= 17509,	// 응답자가 건배 응답/대기중 상태가 아님
		Failure_Cheers_Answer_Diffence_Suggest_AID										= 17510,	// 요청자가 다르다.
		Failure_Cheers_Answer_Diffence_Suggest_CheersCondition							= 17511,	// 응답자와 요청자의 건배 상태가 다름.

		//	재화 (17601 ~ 17700)
		Failure_Currency_Invalid_Value													= 17601,	// 잘못된 재화 값
		Failure_Currency_Invalid_Type													= 17602,	// 잘못된 재화 타입
		Failure_Currency_Value_Overflow													= 17603,	// 재화 최대치 초과
		Failure_Currency_Unit_Value_Overflow											= 17604,	// 재화 단일 획득량 초과
		Failure_Currency_Value_Underflow												= 17605,	// 재화 최소치 초과

		//  채팅 (17701 ~ 17800)
		Failure_Chat_Already_Reserved													= 17701,	// 채팅이 이미 예약됨
		Failure_Chat_Not_Reserved_Partner												= 17702,	// 예약되지 않은 파트너
		Failure_Chat_Invalid_GroupID													= 17703,	// 유효하지 않은 채팅 그룹 아이디
		Failure_Chat_Group_Create_Fail													= 17704,	// 생성 실패
		Failure_Chat_Group_Not_Found													= 17705,	// 채팅 그룹 찾기 실패
		Failure_ChatGroupManager_Is_Null												= 17706,	// 채팅 그룹 매니져가 null 이다
		Failure_Translation_Same_LanguageType											= 17707,	// 같은 언어 이므로 번역 등록 불가
		Failure_Translation_InvalidState												= 17708,	// 번역등록 불가 상태

		Failure_VideoChat_Invalid_State													= 17720,	// 화상 채팅을 하기 위한 상태가 아님
		Failure_VideoChat_NotLoginState													= 17721,	// 상대방이 접속중이지 않습니다.
		Failure_VideoChat_Cannot_connect												= 17722,	// 연결이 끊어졌습니다.
		Failure_VideoChat_TheLineIsInUse												= 17723,	// 통화중
		Failure_VideoChat_Timeout														= 17724,	// 시간 초과로 연결이 끊어집니다.

		//	보상 (17801 ~ 17900)
		Failure_Reward_Item_Insert_Failed												= 17801,	// 보상 아이템 컨테이너 등록 실패
		Failure_Reward_Currency_Insert_Failed											= 17802,	// 보상 재화 컨테이너 등록 실패

		//	일상 이벤트 (17901 ~ 18000)
		Failure_LifeEvent_Invalid_DialogueActionType									= 17901,	// 잘못된 다이얼로그 액션 타입

		//	상점 (18001 ~ 18100)
		Failure_Shop_Invalid_DisplayStandType											= 18001,	// 잘못된 진열대 타입
		Failure_Shop_Invalid_Shop_Item													= 18002,	// 잘못된 상점 아이템
		Failure_Shop_Item_Insert_Failed													= 18003,	// 상점 아이템 컨테이너 등록 실패
		Failure_Shop_Invalid_PurchasePrice												= 18004,	// 잘못된 구매가격
		Failure_Shop_Invalid_SellingPrice												= 18005,	// 잘못된 판매가격

		//	세카이 (18101 ~ 18200)
		Failure_Sekai_Invalid_CheatType													= 18101,	// 잘못된 치트타입
		Failure_Sekai_Load_Fail															= 18102,	// 세카이 정보 로드 실패
		Failure_Sekai_CheckValid_Fail													= 18103,	// 세카이 정보 검증 실패
		Failure_Sekai_SendMsg_Fail														= 18104,	// 세카이 정보 송신 실패
		Failure_Sekai_Proceeding_Settlement												= 18105,	// 정산 진행중
		Failure_Sekai_DBLoad_Fail														= 18106,	// DB 세카이 정보 로드 실패
		Failure_Sekai_Sekai_Info_Is_Nullptr												= 18107,	// 세카이 정보가 nullptr이다
		Failure_Sekai_Not_Exist_Info													= 18108,	// 세카이 정보가 존재하지않음
    };

	inline static bool IsSuccessed(const en InResult)
	{
		return InResult == en::Success;
	}

	inline static bool IsSuccessed(const uint32 InResult)
	{
		return InResult == en::Success;
	}

	inline static bool IsFailed(const en InResult)
	{
		return InResult != en::Success;
	}

	inline static bool IsFailed(const uint32 InResult)
	{
		return InResult != en::Success;
	}
};
