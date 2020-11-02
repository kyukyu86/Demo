#pragma once

struct EWRError
{
	enum en
	{
		Failure_SystemError																= 0,		// Server Query Error, critical
		Success																			= 1,		// ����
		Failure_DB_Not_Exist_Main_Config												= 2,		// ���� Config �������� ����
		Failure_DB_Not_Exist_Login_Config												= 3,		// �α��� Config �������� ����
		Failure_DB_Not_Exist_Game_Config												= 4,		// ���� Config �������� ����

		//-----------------------------------------------------
		//	DB (1 ~ 10000)
		//-----------------------------------------------------

		// DB : �ý���(0~10)
		Failure_DB_SP_Error																= 10,		// DB ���� ����

		// DB : ����(11~100)
		Failure_DB_User_Already_Exist_USN												= 11,		// �̹� �����ϴ� USN

 		// DB : �÷��̾�(101~200)
 		Failure_DB_Player_Not_Exist_Player												= 101,		// �÷��̾ DB���� �������� ����
		Failure_DB_Player_Already_Exist_Player											= 102,		// �÷��̾ DB���� �̹� ������
		Failure_DB_Player_Already_Exist_PlayerName										= 103,		// �÷��̾ DB���� �̹� ������� �÷��̾� �̸�
		Failure_DB_Player_Not_Exist_SelectPlayerName									= 104,		// �÷��̾ DB���� ������ �÷��̾� �̸��� ���ڶ�


		//-----------------------------------------------------
		//	������ ��� (10001 ~ 12000)
		//-----------------------------------------------------
		//	���� ������ Main �� ��� ���� ( 10001 ~ 10100 )
		Failure_Common_Register_LoginServer												= 10001,	// LoginServer ��� ����
		Failure_Common_Register_GameServer												= 10002,	// GameServer ��� ����

		//	��� ���� ã�� ( 10101 ~ 10200 )
		Failure_Common_Not_Register_GameServer											= 10101,	// ��ϵ� ���Ӽ����� �����ϴ�.


		//	���� ���� ( 11901 ~ 12000 )
		Failure_Common_Non_Develop_Item													= 11999,	// ���� �׸��� �ƴ�
		Failure_Common_Developing														= 12000,	// ������

		//-----------------------------------------------------
		// ���� ������ ���� (12001 ~ 14000)
		//-----------------------------------------------------
 		
		//	��Ʈ��ũ ���� (12001 ~ 12100)
		Failure_Common_PacketError														= 12001,	// ��Ŷ ����
		Failure_Common_Different_Server_Protocol_Version								= 12002,	// ���� �������� ������ �ٸ���.
		Failure_Common_Not_Exist_User													= 12003,	// ������ �������� �ʽ��ϴ�.
		Failure_Common_Already_Exist_User												= 12004,	// �̹� ��ϵ� ������ �����մϴ�.
		Failure_Common_Already_Accept													= 12005,	// ���Ӹ� �Ǿ��ִ� �����Դϴ�.
		Failure_Common_Already_CertifyRegistWaiting										= 12006,	// ����/��� ��� �����Դϴ�.
		Failure_Common_Already_InLoginServer											= 12007,	// �α��� ���� ���� �����Դϴ�.
		Failure_Common_Already_MovingToGameServer										= 12008,	// ���Ӽ����� �̵����� �����Դϴ�.
		Failure_Common_Already_InGameServer												= 12009,	// ���� ���� ���� �����Դϴ�.
		Failure_Common_Not_Exist_Server													= 12010,	// ������ ã�� ����

		Failure_Common_Previous_User_CertifyRegistWaiting								= 12011,	// ���� ������ ����/��� ��� �����Դϴ�.
		Failure_Common_Previous_User_MovingToGameServer									= 12012,	// ���� ������ ���Ӽ����� �̵����� �����Դϴ�.

		Failure_Common_Different_User_ID												= 12051,	// ������ ���̵� �ٸ���.
		Failure_Common_Different_User_DKey												= 12052,	// ������ ���̳���Ű�� �ٸ���.

		//	DB SP (12101 ~ 12200) 
		Failure_Common_ExecSP															= 12101,	// DB SP ���� ����

		//	������ ��Ŷ ���� ���� (12201 ~ 12300) 
		Failure_Common_Send_Packet_To_MainServer										= 12201,	// ���μ����� ��Ŷ ���� ����

		//	Invalid Component (12401 ~ 12500)
		Failure_Common_Not_Exist_Component_OwnerActor									= 12401,	// Component �� ���� ���Ͱ� ���� �ȵ�
		Failure_Common_Not_Exist_Component_Cheat										= 12402,	// CompCheat ������ ����.
		Failure_Common_Not_Exist_Component_Quest										= 12403,	// CompQuest ������ ����.
		Failure_Common_Not_Exist_Component_Stat											= 12404,	// CompStat ������ ����.
		Failure_Common_Not_Exist_Component_Move											= 12405,	// CompMove ������ ����.
		Failure_Common_Not_Exist_Component_Skill										= 12406,	// CompSkill ������ ����.
		Failure_Common_Not_Exist_Component_AI											= 12407,	// CompAI ������ ����.
		Failure_Common_Not_Exist_Component_Inventory									= 12408,	// CompInventory ������ ����.
		Failure_Common_Not_Exist_Component_Vehicle										= 12409,	// CompVehicle ������ ����.
		Failure_Common_Not_Exist_Component_Currency										= 12410,	// CompCurrency ������ ����.
		Failure_Common_Not_Exist_Component_Chat											= 12411,	// CompChat ������ ����.
		Failure_Common_Not_Exist_Component_Community									= 12412,	// CompCommunity ������ ����.
		

		//	���̺� ������ / ���ҽ� �ε� (12501 ~ 13000)
		Failure_Common_Not_Exist_TableData_Map											= 12501,	// KMap ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_Item											= 12502,	// ������ ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_Quest										= 12503,	// ����Ʈ ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_QuestTask									= 12504,	// ����Ʈ �½�ũ ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_Coordinate									= 12505,	// ��ǥ ������ ������ ����.
		Failure_Common_Not_Exist_TableData_Npc											= 12506,	// NPC ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_ObjectData									= 12507,	// ������Ʈ ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_SocialRecord_Animation						= 12508,	// SocialRecord_Animation �ش� ���ڵ带 ã�� ����
		Failure_Common_TransportRecord_Elevator_Is_Null									= 12509,	// TransportRecord_Elevator �� �������� ����.
		Failure_Common_TransportRecord_Monorail_Is_Null									= 12510,	// TransportRecord_Monorail �� �������� ����.
		Failure_Common_TransportRecord_Tardis_Is_Null									= 12511,	// TransportRecord_Tardis �� �������� ����.
		Failure_Common_TransportRecord_FloatingBoard_Is_Null							= 12512,	// TransportRecord_FloatingBoard �� �������� ����.
		Failure_Common_TransportRecord_Limit_Station_Count								= 12513,	// �� �ּ� ���� ���� ���� (2�̻�)
		Failure_Common_Not_Exist_TableData_SocialRecord_Dance							= 12514,	// �Ҽ� �� ���̺� ������ ������ ����
		Failure_Common_Not_Exist_TableData_SocialRecord_DanceGroup						= 12515,	// �Ҽ� ���׷� ���̺� ������ ������ ����
		Failure_Common_Not_Exist_TableData_BattleField									= 12516,	// BattleField ���̺� ������ ������ ����.
		Failure_Common_Wrong_Range														= 12517,	// ����Ÿ ������ ����
		Failure_Common_Not_Exist_TableData_Currency										= 12518,	// ��ȭ ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_DialogueData									= 12519,	// ���̾�α� ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_DialogueData_DialogueSet						= 12520,	// ���̾�α� ��Ʈ ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_DialogueData_DialogueSetGroup				= 12521,	// ���̾�α� ��Ʈ �׷� ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_RewardData									= 12522,	// ���� ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_LifeEventData								= 12523,	// �ϻ� �̺�Ʈ ���̺� ������ ������ ����.
		Failure_Common_MapDataPack_Is_Null												= 12524,	// MapDataPack ã�� ����
		Failure_Common_TriggerData_Is_Null												= 12525,	// TriggerData ��ü ���� ����
		Failure_Common_ActionNode_Is_Null												= 12526,	// ActionNode ��ü ���� ����
		Failure_Common_TransitionNode_Is_Null											= 12527,	// TransitionNode ��ü ���� ����
		Failure_Common_TriggerNode_Is_Null												= 12528,	// TriggerNode ��ü ���� ����
		Failure_Common_Not_Exist_TableData_ShopData										= 12529,	// ���� ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_ShopCatalogData								= 12530,	// ���� īŻ�α� ���̺� ������ ������ ����.
		Failure_Common_Not_Exist_TableData_Field										= 12531,	// Field ���̺� ������ ������ ����.

		//	Invalid (13001 ~ 13100)
		Failure_Common_Invalid_TID														= 13001,	// ��ȿ���� ���� TID
		Failure_Common_Invalid_Server_Index												= 13002,	// ��ȿ���� ���� SERVER_ID
		Failure_Common_Invalid_USN														= 13003,	// ��ȿ���� ���� USN
		Failure_Common_Invalid_PSN														= 13004,	// ��ȿ���� ���� PSN
		Failure_Common_Invalid_LanguageType												= 13005,	// ��ȿ���� ���� ���

		//	���� ������ MainServer�� ��� ���� ( 13101 ~ 13200 )
		Failure_Common_RegisterLoginServer												= 13101,	// LoginServer ��� ����
		Failure_Common_DeregisterLoginServer											= 13102,	// LoginServer ��� ���� ����
		Failure_Common_RegisterGameServer												= 13103,	// GameServer ��� ����
		Failure_Common_DeregisterGameServer												= 13104,	// GameServer ��� ���� ����
		Failure_Common_RegisterChatServer												= 13105,	// ChatServer ��� ����
		Failure_Common_DeregisterChatServer												= 13106,	// ChatServer ��� ���� ����

		Failure_Register_Exist_Server_Index												= 13110,	// ������ ���� �ε����� ������
		Failure_Register_Invalid_LimitStaticGameServer									= 13111,	// ���� ���� ���� ������ ��ȿ���� ����

		//	���� ���� (13201 ~ 13300)
		Failure_Common_DB_Call_Error													= 13201,	// DB ȣ�� ����
		Failure_Common_Not_Exist_Player													= 13202,	// �÷��̾ �������� �ʽ��ϴ�.

		//  HTTP �� ���� ���� ���� ���� (13301 ~ 13400)
		Failure_Http_API_Error															= 13301,	// http ��û�� ���� ����
		Failure_Http_Error_Invalid_Result												= 13302,	// http ���信 ���� ����
		Failure_Http_GoogleTranslation_Json_Parsing										= 13303,	// json parsing

		//	�� ���� ���� (13401 ~ 13500)
		Failure_Common_Invalid_Map_Index												= 13401,	// ��ȿ���� ���� Map Index
		Failure_Common_MapGroup_Is_Null													= 13402,	// MapGroup ��ü ������ ã�� ����
		Failure_Common_Map_Is_Null														= 13403,	// Map ��ü ������ ã�� ����
		Failure_Common_MapInfoGroup_Is_Null												= 13404,	// MapInfoGroup ��ü ������ ã�� ����
		Failure_Common_MapInfo_Is_Null													= 13405,	// MapInfo ��ü ������ ã�� ����
		Failure_Common_MapRecord_Is_Null												= 13406,	// MapRecord ��ü ������ ã�� ����
		Failure_Common_Sector_Is_Null													= 13407,	// ���Ͱ� ����
		Failure_Common_ListSectorIndex_Is_Null											= 13408,	// ���� ��Ŷ�� ���� ��� �����Ͱ� ��

		//	���� (13501 ~ 13600)
		Failure_Common_Actor_Is_Null													= 13501,	// Actor ��ü ������ ã�� ����
		Failure_Common_Player_Is_Null													= 13502,	// Player ��ü ������ ã�� ����
		Failure_Common_User_Is_Null														= 13503,	// User ��ü ������ ã�� ����
		Failure_Common_Invalid_ActorType												= 13504,	// Actor Ÿ���� �߸���
		Failure_Invalid_Actor_State														= 13505,	// ActorState
		Failure_Common_ActorManager_Is_Null												= 13506,	// ActorManager ��ü ������ ã�� ����
		Failure_Invalid_Costume_TID														= 13507,	// ��ȿ���� ���� CostumeTID
		Failure_Get_Player_Name															= 13510,	// ĳ���� �̸� �������� ����
		Failure_Common_Invalid_Select_PSN												= 13511,	// user �� ���õ� PSN �� ����
		Failure_INvalid_AI_State														= 13512,	// AI ���� �̻�
		Failure_Common_Invalid_VehicleAID												= 13513,	// vehicle AID �� ����
		Failure_Common_Vehicle_Is_Null													= 13514,	// vehicle ��ü ������ ã�� ����	

		//	�������� ����ű ��Ȳ �߻� (13901 ~ 14000)
		Failure_ForceKick_Invalid														= 13901,	// ���� �������� ���� ForceKick ����
		Failure_ForceKick_Push															= 13902,	// ���������� ������������ ��������
		Failure_ForceKick_GM															= 13903,	// ���񽺿�� ���� ��������
		Failure_ForceKick_Error															= 13904,	// ���α׷� ������ ���� ��������
		Failure_ForceKick_DBError														= 13905,	// DB ������ ���� ��������
		Failure_ForceKick_Timeout														= 13906,	// ��Ⱓ �������� ��������					
		Failure_ForceKick_Etc															= 13907,	// ��Ÿ���������� ��������
		Failure_ForceKick_Channel														= 13908,	// ä�κ��濡���� ��������
		Failure_ForceKick_ShutdownTime													= 13909,	// �˴ٿ� ������ ���� ��������
		Failure_ForceKick_AutoCheck														= 13910,	// ���� üũ�� ���� ���� ����
		Failure_ForceKick_MoveServer													= 13911,	// ���� ���� �������� ����
		Failure_ForceKick_AbuseMovingCheck												= 13912,	// �������� �̵����� ���� ��������
		Failure_ForceKick_HackingPacket													= 13913,	// �������� ��Ŷ���� ���� ��������
		Failure_ForceKick_SequenceNumberCheck											= 13914,	// �޽��� ������ �ѹ� ���������� ���� ���� ����
		Failure_ForceKick_Disconnect													= 13915,	// ���� ���迡 ���� ���� ����
		Failure_ForceKick_InvalidCurrencyDetected										= 13916,	// ��ȿ���� ���� ��ȭ�� ȹ�� ������ ���� ���� ����
		Failure_ForceKick_ToolCommand													= 13917,	// ����� ����� ���� ���� (���� ��)
			

		Failure_Common_Not_Ready_For_Service											= 14000,	// ������ �غ���� ����

		//-----------------------------------------------------
		// ���� ������ ���� (14001 ~ )
		//-----------------------------------------------------

		//	�� ���� (14001 ~ 14100)
		Failure_Map_Create_Already_Exist_Map_Index										= 14001,	// ������ MID �� �����Ǿ� �ִ� ���� ������
		Failure_Map_Create_MapGroup_Insert_Failed										= 14002,	// MapGroup �����̳� ��� ����
		Failure_Map_Create_MapInfoGroup_Insert_Failed									= 14003,	// MapInfoGroup �����̳� ��� ����
		Failure_Map_Create_Init_Failed													= 14004,	// �� �ʱ�ȭ ����
		Failure_Map_Create_MapDataPack_Is_Null											= 14005,	// MapDataPack ã�� ����
		Failure_Map_Create_TriggerManager_Init_Failed									= 14006,	// TriggerManager ������ ����
		Failure_Map_Create_MapRecord_Is_Null											= 14007,	// MapRecord ã�� ����
		Failure_Map_Create_JsonData_Is_Null												= 14008,	// JsonData ã�� ����
		Failure_Map_Create_Invalid_JsonData_Type										= 14009,	// JsonData_Type ã�� ����
		Failure_Map_Create_MapRespawnInfo_Insert_Failed									= 14010,	// MapRespawnInfo �����̳� ��� ����
		Failure_Map_Create_SectorData_Is_Null											= 14011,	// SectorData ã�� ����
		Failure_Map_Create_SectorTotalCount_Is_Invalid									= 14012,	// Sector �� ���� ����
		Failure_Map_Create_Insert_Sector												= 14013,	// Sector �� �߰� ����
		Failure_Map_Create_ActorManager_Init_Failed										= 14014,	// ActorManager �ʱ�ȭ ����
		Failure_Map_Create_TriggerVolumeCenter_Init_Failed								= 14015,	// TriggerVolumeCenter �ʱ�ȭ ����

		//	�� ���� (14101 ~ 14200)
		Failure_Map_Destroy_Remove_MapGroup												= 14101,	// MapGroup ���� �����ϱ� ����		
		Failure_Map_Destroy_Aready_Destroying											= 14102,	// MapInfo �̹� ���� �������
		Failure_Map_Destroy_In_DestroyContainer											= 14103,	// Map �̹� ���� ��� �����̳ʿ� ����

		//	MainServer
		Failure_MapInfo_Destroy_Remove_MapInfoGroup										= 14151,	// MapInfoGroup ���� �����ϱ� ����
		Failure_MapInfo_Destroy_Aready_Destroying										= 14152,	// MapInfo �̹� ���� �������
		Failure_MapInfo_Destroy_In_DestroyContainer										= 14153,	// MapInfo �̹� ���� ��� �����̳ʿ� ����

		//	�� ����Ÿ �� (14201 ~ 14300)
		Failure_MapData_Pack_Empty_SpawnIORecordList									= 14201,	// SpawnIORecordList �� �������
		Failure_MapData_Pack_Empty_SpawnENpcRecordList									= 14202,	// SpawnENpcRecordList �� �������
		Failure_MapData_Pack_Empty_SpawnNpcRecordList									= 14203,	// SpawnNpcRecordList �� �������
		
		//	�÷��̾� �� �̵� (14301 ~ 14400)
		Failure_Player_MoveToMap_Different_PSN											= 14301,	// ���� ���õ� PSN �� �ٸ�
		Failure_Player_MoveToMap_Already_In_Map											= 14302,	// �̹� �ش� �ʿ� ������
		Failure_Player_MoveToMap_Already_Make_Reservation								= 14303,	// �̹� ����Ǿ� ����
		Failure_Player_MoveToMap_Not_Found_Player										= 14304,	// �ش� �ʿ��� �÷��̾� ã�� ����
		Failure_Player_MoveToMap_Not_Found_Reservation									= 14305,	// ������ ã�� ����
		Failure_Player_MoveToMap_Aleady_Enter_map										= 14306,	// �̹� ������ �ִ� ���� ����. ���� �ʿ��� ���� �ʿ�
		Failure_Player_MoveToMap_DestoryMap												= 14307,	// ���� ������� ��
		Failure_Player_MoveToMap_Reserving												= 14308,	// ���� ������	
		Failure_Player_MoveToMap_MC_Same_Map											= 14309,	// Map Connector �� �̵� ��û�Ͽ�����, �̵��ϰ��� �ϴ� MapTID �� ����� ����

		//	���� ���� (15001 ~ 15200)
		Failure_Actor_CreateActor_Invalid_Map											= 15001,	// �� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_ActorTID										= 15002,	// ���� ���̺� ���̵� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Generate_ActorID										= 15003,	// ���� ���̵� ������ �����Ͽ����ϴ�.
		Failure_Actor_CreateActor_Invalid_ActorType										= 15004,	// ������ ���� ���� ���� ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Not_Exist_User										= 15005,	// ������ �÷��̾��� ���� ������ �������� �ʽ��ϴ�.
		Failure_Actor_CreateActor_InitASM												= 15006,	// ASM �ʱ�ȭ�� ���� �Ͽ����ϴ�.
		Failure_Actor_CreateActor_EnterMap												= 15007,	// �� ���Կ� ���� �Ͽ����ϴ�.
		Failure_Actor_CreateActor_SetUpStat												= 15008,	// ���� ������ ���� �Ͽ����ϴ�.
		Failure_Actor_CreateActor_SetRealmInfo											= 15009,	// ���� ������ ���� �Ͽ����ϴ�.
		Failure_Actor_CreateActor_TableRecordActor										= 15010,	// TableRecordActor ��ü�� ����

		Failure_Actor_CreateActor_Invalid_CreateInfoActor								= 15011,	// ���� ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoPlayer								= 15012,	// �÷��̾� ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoFellow								= 15013,	// ��ο� ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoNpc									= 15014,	// NPC ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoInteractionObject					= 15015,	// ���ͷ��� ���� ������Ʈ ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoPortal								= 15016,	// ��Ż ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoBrick								= 15017,	// �긯 ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoSkillObject							= 15018,	// ��ų ������Ʈ ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoDropObject							= 15019,	// ��� ������Ʈ ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoActiveObject						= 15020,	// ��Ƽ�� ������Ʈ ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoActionProp							= 15021,	// ActionProp ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoElevator							= 15022,	// Elevator ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoMonorail							= 15023,	// Monorail ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoTardis								= 15024,	// Tardis ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_CreateInfoFloatingBoard						= 15025,	// FloatingBoard ���� ������ ��ȿ���� �ʽ��ϴ�.
			

		Failure_Actor_CreateActor_Invalid_Actor											= 15031,	// ���� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Player										= 15032,	// �÷��̾� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Fellow										= 15033,	// ��ο� ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Npc											= 15034,	// NPC ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_IO											= 15035,	// ���ͷ��� ������Ʈ ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Portal										= 15036,	// ��Ż ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Brick											= 15037,	// �긯 ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_SO											= 15038,	// ��ų ������Ʈ ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_DropObject									= 15039,	// ��� ������Ʈ ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_AO											= 15040,	// ��Ƽ�� ������Ʈ ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_ActionProp									= 15041,	// ActionProp ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Elevator										= 15042,	// Elevator ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Monorail										= 15043,	// Monorail ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_Tardis										= 15044,	// Tardis ������ ��ȿ���� �ʽ��ϴ�.
		Failure_Actor_CreateActor_Invalid_FloatingBoard									= 15045,	// FloatingBoard ������ ��ȿ���� �ʽ��ϴ�.

		Failure_Actor_CreateActor_Already_Exist_Actor									= 15071,	// ������ ���Ͱ� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Player									= 15072,	// ������ �÷��̾ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Fellow									= 15073,	// ������ ��ο찡 �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Npc										= 15074,	// ������ NPC�� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_IO										= 15075,	// ������ ���ͷ��� ������Ʈ�� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Portal									= 15076,	// ������ ��Ż�� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Brick									= 15077,	// ������ �긯�� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_SO										= 15078,	// ������ ��ų ������Ʈ�� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_DropObject								= 15079,	// ������ ��� ������Ʈ�� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_AO										= 15080,	// ������ ��Ƽ�� ������Ʈ�� �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_ActionProp								= 15081,	// ������ ActionProp �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Elevator								= 15082,	// ������ Elevator �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Monorail								= 15083,	// ������ Monorail �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Tardis									= 15084,	// ������ Tardis �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_FloatingBoard							= 15085,	// ������ FloatingBoard �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Vehicle									= 15086,	// ������ Vehicle �����մϴ�.
			
		Failure_Actor_CreateActor_Invalid_PSN											= 15091,	// ��ȿ���� ���� PSN
		Failure_Actor_CreateActor_Invalid_Name											= 15092,	// ��ȿ���� ���� Name
		Failure_Actor_CreateActor_Invalid_ActorState									= 15093,	// ��ȿ���� ���� ActorState
		Failure_Actor_CreateActor_For_NonPlayer											= 15094,	// �� �÷��̾� �����Լ��� �����
		Failure_Actor_CreateActor_For_Player											= 15095,	// �÷��̾� �����Լ��� �����
		Failure_Actor_CreateActor_SpawnRecord_Is_Null									= 15096,	// stISpawnRecord ������ ã�� ����
		Failure_Actor_CreateActor_Derived_SpawnRecord_Is_Null							= 15097,	// ��ӹ��� SpawnRecord ������ ã�� ����
		Failure_Actor_CreateActor_Not_Exist_SpawnInfo									= 15098,	// SpawnInfo ������ ã�� ����
		Failure_Actor_CreateActor_ReadyToPlay_EnteredMap_Is_Null						= 15099,	// EnteredMap ������ ã�� ����

		Failure_Actor_CreateActor_Already_Exist_ActiveActor								= 15101,	// ActiveActor�� �̹� ������
		Failure_Actor_CreateActor_Already_Exist_Player_Socket							= 15102,	// Socket �ʿ� ������
		Failure_Actor_CreateActor_Already_Exist_Player_PSN								= 15103,	// PSN �ʿ� ������
		Failure_Actor_CreateActor_Already_Exist_Player_AID								= 15104,	// AID �ʿ� ������
		Failure_Actor_CreateActor_Already_Exist_Player_Name								= 15105,	// Name �ʿ� ������
		Failure_Actor_CreateActor_Already_Exist_Player_DisconnetPSN						= 15106,	// DisconnetPSN �ʿ� ������
		Failure_Actor_CreateActor_Already_Exist_SpawnActor								= 15107,	// �ش� SpawnTID �� �ش��ϴ� Actor �� ������ 

		Failure_Actor_CreateActor_Already_Exist_Component								= 15111,	// ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Stat							= 15112,	// Stat ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Cheat							= 15113,	// Cheat ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Inventory						= 15114,	// Inventory ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Qeust							= 15115,	// Qeust ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Move							= 15116,	// Move ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Skill							= 15117,	// Skill ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_AI							= 15118,	// AI ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_ABState						= 15119,	// ABState ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Buff							= 15120,	// Buff ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Vehicle						= 15121,	// VSM ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Currency						= 15122,	// Currency ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Chat							= 15123,	// Chat ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Reward						= 15124,	// Reward ������Ʈ ������ �����մϴ�.
		Failure_Actor_CreateActor_Already_Exist_Component_Community						= 15125,	// Community ������Ʈ ������ �����մϴ�.

		Failure_Actor_CreateActor_Init_Component										= 15151,	// ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Stat									= 15152,	// Spawn ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Cheat									= 15153,	// Cheat ������Ʈ �ʱ�ȭ ����		
		Failure_Actor_CreateActor_Init_Component_Inventory								= 15154,	// Inventory ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Qeust									= 15155,	// Qeust ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Move									= 15156,	// Move ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Skill									= 15157,	// Skill ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_AI										= 15158,	// AI ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_ABState								= 15159,	// ABState ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Buff									= 15160,	// Buff ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Vehicle								= 15161,	// VSM ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Currency								= 15162,	// Currency ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Chat									= 15163,	// Chat ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Reward									= 15164,	// Reward ������Ʈ �ʱ�ȭ ����
		Failure_Actor_CreateActor_Init_Component_Community								= 15165,	// Reward ������Ʈ �ʱ�ȭ ����

		Failure_Actor_CreateActor_SetupComponentFirst_Base								= 15201,	// �⺻ ���� ������ �����Ͽ����ϴ�.
		Failure_Actor_CreateActor_SetupComponentFirst_Quest								= 15202,	// ����Ʈ ���� ������ �����Ͽ����ϴ�.
		Failure_Actor_CreateActor_SetupComponentFirst_Inventory							= 15203,	// �κ��丮 ���� ������ �����Ͽ����ϴ�.
		Failure_Actor_CreateActor_SetupComponentFirst_Currency							= 15204,	// ��ȭ ���� ������ �����Ͽ����ϴ�.

		Failure_Actor_CreateActor_SetupAutoASM_ActorState_Is_Null						= 15211,	// ActorState ���� ����
		Failure_Actor_CreateActor_SetupAutoASM_Failed									= 15212,	// ActorState �׸� ���� ����

		Failure_Actor_CreateActor_Invalid_VehicleState									= 15221,	// VehicleState �׸� ���� ����
		Failure_Actor_CreateActor_CurVehicleState_Is_Null								= 15222,	// CurVehicleState ��ü ����
		Failure_Actor_CreateActor_SetupAutoVSM_VehicleState_Is_Null						= 15223,	// VehicleState ��ü ����
		Failure_Actor_CreateActor_SetupAutoVSM_Failed									= 15224,	// SetupAutoVSM �ʱ�ȭ ����

			
		//	���� ���� (15301 ~ 15400)
		Failure_Actor_ASM_CurActorState_Is_Null											= 15301,	// ���� ���õ� ���� ���� ��ü�� �������� ����
		Failure_Actor_ASM_Unable_Next_state												= 15302,	// ���� ���·� ������ �� ����
		Failure_Actor_ASM_Is_Not_Alive													= 15303,	// ����ִ� ���°� �ƴ�
		Failure_Actor_Unmovable															= 15304,	// ������ �� ���� ����


		//	���� ���� (15401 ~ 15500)
		Failure_Actor_Destroy_Aready_Destroying											= 15401,	// �̹� ���� ������
		Failure_Actor_Destroy_In_DestroyContainer										= 15402,	// �̹� ���� ������

		//  AI ���� ���� (15501 ~ 15600)
		Failure_Actor_AI_Start_Error													= 15501,

		//	���� ���� (15601 ~ 15700)
		Failure_Actor_Is_Not_Master_Servant_Relationship								= 15601,	// ���� ���谡 �ƴ�
		Failure_Actor_Not_Exist_Master													= 15602,	// ���� ��ü�� ã�� ����
		Failure_Actor_Not_Exist_Slave													= 15603,	// ���� ��ü�� ã�� ����
		Failure_Actor_Is_Not_My_Master													= 15604,	// �ڽ��� ������ �ƴ�
		Failure_Actor_Is_Not_My_Slave													= 15605,	// �ڽ��� ������ �ƴ�
		Failure_Actor_Already_Master													= 15606,	// �̹� ����
		Failure_Actor_Already_Slave														= 15607,	// �̹� ����
		Failure_Actor_Already_Exist_Master												= 15608,	// �̹� ������ ����
		Failure_Actor_Already_Exist_Slave												= 15609,	// �̹� ������ ����
		Failure_Actor_Slave_Warp_Position_Is_Invalid									= 15610,	// ���� ��ü�� ���� ��ġ �˻� ����

		//	��Ŀ��Ʈ (15801 ~ 15900) 
		Failure_Reconnect_Not_Found_Player												= 15801,	// �÷��̾ ã�� ����

		//	UserSequence (15901 ~ 16000) 
		Failure_Lab_UserSequence_Not_InLab												= 15901,	// ���� ��ġ�� [������]�� �ƴ�
		Failure_Lab_UserSequence_Already_InLab											= 15902,	// ���� ��ġ�� �̹� [������]��
		Failure_Lab_UserSequence_Wrong													= 15903,	// �ùٸ��� ���� ���� ������

		//	�κ��丮 (16001 ~ 16300)
		Failure_Inventory_Item_Insert_Failed											= 16001,	// MapItem �����̳� ��� ����
		Failure_Inventory_Not_Found_Item												= 16002,	// �������� �ʴ� ������
		Failure_Inventory_Already_Exist_Item											= 16003,	// �̹� �����ϴ� ������
		Failure_Inventory_Invalid_Item													= 16004,	// ��ȿ���� ���� ������
		Failure_Inventory_Invalid_Value													= 16005,	// ��ȿ���� ���� ��
		Failure_Inventory_Invalid_ItemSerialNumber										= 16006,	// ��ȿ���� ���� ISN
		Failure_Inventory_Invalid_ItemCount												= 16007,	// ��ȿ���� ���� ������ ����
		Failure_Inventory_Invalid_ItemType												= 16008,	// ��ȿ���� ���� ������ Ÿ��
		Failure_Inventory_Can_Not_Remove_Already_Equipped_Item							= 16009,	// �������� ������ ���źҰ�
		Failure_Inventory_Not_Equipped_Item												= 16010,	// ���������� ���� ������
		Failure_Inventory_Already_Equipped_Item											= 16011,	// �̹� �������� ������
		Failure_Inventory_Invalid_Category												= 16012,	// ��ȿ���� ���� ī�װ� Ÿ��
		Failure_Inventory_Can_Not_Remove_Already_Registered_Item						= 16013,	// ���� ������� ������ ���źҰ�
		Failure_Inventory_Invalid_ItemEquipType											= 16014,	// ��ȿ���� ���� ���� Ÿ��
		Failure_Inventory_Not_Registered_Item											= 16015,	// ���Կ� ��ϵǾ����� ���� ������
		Failure_Inventory_Already_Registered_Item										= 16016,	// �̹� ���Կ� ������� ������
		Failure_Inventory_Invalid_RegistSlotType										= 16017,	// ��ȿ���� ���� ���� ��� Ÿ��
		Failure_Inventory_Invalid_InventoryCategoryType									= 16018,	// ��ȿ���� ���� �κ��丮 ī�װ� Ÿ��
		Failure_Inventory_Lack_Slot_Count												= 16019,	// �κ��丮 ���� ����
		Failure_Inventory_Interior_Not_Found_Actor										= 16020,	// Interior Actor ã�� ����
		Failure_Inventory_Interior_Already_Arrange										= 16021,	// �̹� ��ġ�Ǿ� ����
		Failure_Inventory_Interior_Failed_Insert_Container								= 16022,	// Interior Actor insert ����
		Failure_Inventory_Interior_Not_Interior_Item									= 16023,	// Interior Item �� �ƴ�

		// Ʈ���� (16401 ~ 16500)
		Failure_Trigger_Invalid_TransitionNode											= 16401,	// �˼� ���� Ʈ������ ��� Ÿ��
		Failure_Trigger_Process_TriggerNode												= 16402,	// Ʈ���� ���� ����
		Failure_Trigger_Not_Exist_Trigger_Data											= 16403,	// Ʈ���� ������ ����
		Failure_Trigger_MovePos_Check_Fail												= 16404,	// Ʈ���� ����� ��ġ ��ȿ�� üũ ����
		Failure_Trigger_Process_TransitionNode											= 16405,	// Ʈ������ ��� ���� ����
		Failure_Trigger_IsDisableTriggerNode											= 16406,	// ��� �Ұ����� ���
		Failure_Trigger_Not_Match_TriggerFlowIndex										= 16407,	// Ʈ���� ���� ���� ����ġ
		Failure_Trigger_IsUsedTriggerNode												= 16408,	// �̹� ���� Ʈ���� ���
		Failure_Trigger_Not_Exist_TriggerManager										= 16409,	// Ʈ���� ������ ����
		Failure_Trigger_Invalid_NodeTID													= 16410,	// ��ȿ���� ���� Ʈ���� ��� TID
		Failure_Trigger_Invalid_TriggerNodeType											= 16411,	// ��ȿ���� ���� TriggerNodeType

		// Skill (16501~16600)
		Failure_Create_Skill_Task														= 16501,
		Failure_Skill_Skill_Task_Is_Not_Found											= 16502,
		Failure_Skill_Invalid_SkillTID													= 16503,
		Failure_Not_Enough_Skill_CoolTime												= 16504,
		Failure_Player_Warp_Fail														= 16505,	// �÷��̾��� ���� ����
		Failure_Hit_Target_Defense														= 16506,	// Ÿ���� ��� �� �̹Ƿ� ��Ʈ ���� ����
		Failure_Hit_By_Ignore_Reaction													= 16507,	// ���׼� ���ÿ� ���� ��Ʈ ���õ�

		//	����Ʈ (16601 ~ 16700)
		Failure_Quest_Progress_Insert_Failed											= 16601,	// �������� ����Ʈ �����̳� ��� ����
		Failure_Quest_Complete_Insert_Failed											= 16602,	// �Ϸ�� ����Ʈ �����̳� ��� ����
		Failure_Quest_Task_Insert_Failed												= 16603,	// ����Ʈ �½�ũ �����̳� ��� ����
		Failure_Quest_Not_Found_Quest													= 16604,	// �������� �ʴ� ����Ʈ
		Failure_Quest_Not_Found_Quest_Task												= 16605,	// �������� �ʴ� ����Ʈ �½�ũ
		Failure_Quest_Already_Progress_Quest											= 16606,	// �̹� �������� ����Ʈ
		Failure_Quest_Already_Complete_Quest											= 16607,	// �̹� �Ϸ�� ����Ʈ
		Failure_Quest_Invalid_Quest														= 16608,	// ��ȿ���� ���� ����Ʈ
		Failure_Quest_Invalid_QuestType													= 16609,	// ��ȿ���� ���� ����Ʈ Ÿ��
		Failure_Quest_Invalid_QuestTaskType												= 16610,	// ��ȿ���� ���� ����Ʈ �½�ũ Ÿ��
		Failure_Quest_Invalid_QuestState												= 16611,	// ��ȿ���� ���� ����Ʈ ����
		Failure_Quest_Start_Condition_Is_Not_Correct									= 16612,	// ����Ʈ ���� ������ ���� ����
		Failure_Quest_Progress_State_incorrect											= 16613,	// ����Ʈ ���� ���°� ���� ����
		Failure_Quest_Can_Not_Completable_State											= 16614,	// �Ϸ� ���� ���°� �� �� ���� ����(�Ϸ����� ������)
		Failure_Quest_Can_Not_Receive_Reward_State										= 16615,	// �Ϸ� ������ ���� �� ���� ����(�Ϸ� �������� ������)
		Failure_Quest_Invalid_Quest_Task_Style											= 16616,	// ��ȿ���� ���� ����Ʈ �½�ũ ������
		Failure_Quest_Not_Completable_Task												= 16617,	// �Ϸ� ���� ������ �½�ũ�� �ƴ�
		Failure_Quest_Not_Dest_Map														= 16618,	// ������/��ġ���־�� �� ���� �ƴ�
		Failure_Quest_Not_Dest_Position													= 16619,	// ������ ��ǥ�� �ƴ�
		Failure_Quest_Not_Progress_Quest												= 16520,	// �������� ����Ʈ�� �ƴ�
		Failure_Quest_Not_Complete_Quest												= 16521,	// �Ϸ�� ����Ʈ�� �ƴ�
		Failure_Quest_Invalid_QuestActionType											= 16622,	// ��ȿ���� ���� ����Ʈ �׼� Ÿ��
		Failure_Quest_Invalid_StartConditionType										= 16623,	// ��ȿ���� ���� ����Ʈ ���� ���� Ÿ��
		Failure_Quest_Can_Not_Progress_IO_Quest											= 16624,	// IOä������ �������� ����Ʈ�� ����

		//	��Ȱ (16701 ~ 16800)
		Failure_Revive_Is_Not_Coma														= 16701,	// Coma ���°� �ƴ�
		Failure_Revive_Unable_MapType													= 16702,	// ��Ȱ �Ұ� �� ����
		Failure_Revive_Failed_ReBorn													= 16703,	// �⺻ ��Ȱ �Ұ�
		Failure_Revive_Remain_CoolTime_DisputeRevive									= 16704,	// ��Ÿ���� ���� �ȵ�
		Failure_Revive_Ing																= 16705,	// ��Ȱ ������
		Failure_Revive_Failed_Resurrection												= 16706,	// �⺻ ��Ȱ �Ұ�

		//	ġƮ (16801 ~ 16900)
		Failure_Cheat_Wrong_MSG															= 16801,	// ġƮ �޽����� �ƴ�
		Failure_Cheat_Cannot_Parse														= 16802,	// �Ľ� ����
		Failure_Cheat_Unknown_Command													= 16803,	// ���� ġƮ
		Failure_Cheat_The_Number_Of_Parameters_Is_Incorrect								= 16804,	// ġƮ �Ķ���� ���� ����
		Failure_Cheat_Not_Found_Cheat_Tag												= 16805,	// ġƮ���� �˸��� ������ ����
		Failure_Cheat_Is_Not_Player														= 16806,	// ġƮ ��û ����� �÷��̾ �ƴ�
		Failure_Cheat_No_Permission														= 16807,	// ġƮ ���� ����
		Failure_Cheat_Not_Found_Master													= 16808,	// ġƮ ����� �÷��̾� ��ü�� ã�� ����

		//	���ü (16801 ~ 16900)
		Failure_Vehicle_VSM_CurVehicleState_Is_Null										= 16901,	// CurVehicleState ��ü ����
		Failure_Vehicle_VSM_Unable_Next_state											= 16902,	// VehicleState ���� �ܰ� ���� ����
		Failure_Vehicle_Already_Exist_Passenger											= 16903,	// �°� ��ܿ� �̹� ������
		Failure_Vehicle_Not_Exist_Passenger												= 16904,	// �°� ��ܿ� ����
		Failure_Vehicle_Already_GetOn_Other_Vehicle										= 16905,	// �ٸ� ��ۼ��ܿ� ž���� ����
		Failure_Vehicle_Not_Allow_Boarding_State										= 16906,	// ž�� ��� ���� ���°� �ƴ�
		Failure_Vehicle_Does_Not_Allow_NonPlayer										= 16907,	// �÷��̾ �ƴ� (�÷��̾ ������ ž��ü)
		Failure_Vehicle_Is_Full															= 16908,	// ���� ��
		Failure_Vehicle_Tardis_Arrival_And_Current_Map_Is_Same							= 16909,	// Tardis - ����ʰ� �������� ������
		Failure_Vehicle_Is_Not_Vehicle_Packet											= 16910,	// ž��ü�� ��Ŷ�� �ƴ�
		Failure_Vehicle_Different_VehicleAID											= 16911,	// ���� ž���� ž��ü�� �ٸ� ž��ü AID
		Failure_Vehicle_Not_On_Board													= 16912,	// ���� ž���� ž��ü�� ����
		Failure_Vehicle_Is_Not_Vehicle													= 16913,	// ž��ü�� �ƴ�
		Failure_Vehicle_Is_Not_Passenger												= 16914,	// �°��� �ƴ�
		Failure_Vehicle_Seat_Index_Is_Invalid											= 16915,	// ��Ʈ �ε����� �ùٸ��� ����
		Failure_Vehicle_Not_Allow_Boarding_SeatIndex									= 16916,	// ������� �ʴ� ��Ʈ
		Failure_Vehicle_Is_Not_Driver													= 16917,	// ����̹��� �ƴ�

		// ��ٸ� (17001 ~ 17100)
		Failure_Ladder_Moving															= 17001,	// ��ٸ� �̵� �����Դϴ�.
		Failure_Ladder_Not_Moving														= 17002,	// ��ٸ� �̵� ���°� �ƴմϴ�.

		// �Ҽ� (17101 ~ 17300)
		Failure_Social_Already_Talking													= 17101,	// �̹� ��ȭ���� ����
		Failure_Social_Not_Talking														= 17102,	// ��ȭ������ ���� ����
		Failure_Social_Already_Dancing													= 17103,	// �̹� ������ ����
		Failure_Social_Not_Dancing														= 17104,	// �������� ���� ����

		// ���� (17301 ~ 17400)
		Failure_Occupy_Invalid_IOAID													= 17301,	// ���� ���°� �ƴϴ�

		//	�׷� �� �̵� (17401 ~ 17500)
		Failure_Group_MoveToMap_Empty_Passenger_List									= 17401,	// �°� ����� �������
		
		// ������Ʈ ��� ��ȣ �ۿ� (17501 ~ 17600)
		Failure_Cheers_Suggest_Not_EquppedDrink											= 17501,	// ��û�ڰ� ������ ��� ���� ����
		Failure_Cheers_Answer_Not_EquppedDrink											= 17502,	// �����ڰ� ������ ��� ���� ����
		Failure_Cheers_Not_Allow_ActorType												= 17503,	// �ǹ踦 �Ҽ� ���� ActorType
		Failure_Cheers_Not_NpcJobType_Drinker											= 17504,	// �ǹ踦 �Ҽ� ���� Npc Job Type
		Failure_Cheers_Suggest_CheersCondition_Suggest									= 17505,	// ��û�ڰ� �ǹ� ��û/����� ������
		Failure_Cheers_Suggest_CheersCondition_Answer									= 17506,	// ��û�ڰ� �ǹ� ����/����� ������
		Failure_Cheers_Answer_CheersCondition_Suggest									= 17507,	// �����ڰ� �ǹ� ��û/����� ������
		Failure_Cheers_Answer_CheersCondition_Answer									= 17508,	// �����ڰ� �ǹ� ����/����� ������

		Failure_Cheers_Answer_Not_CheersCondition_Answer								= 17509,	// �����ڰ� �ǹ� ����/����� ���°� �ƴ�
		Failure_Cheers_Answer_Diffence_Suggest_AID										= 17510,	// ��û�ڰ� �ٸ���.
		Failure_Cheers_Answer_Diffence_Suggest_CheersCondition							= 17511,	// �����ڿ� ��û���� �ǹ� ���°� �ٸ�.

		//	��ȭ (17601 ~ 17700)
		Failure_Currency_Invalid_Value													= 17601,	// �߸��� ��ȭ ��
		Failure_Currency_Invalid_Type													= 17602,	// �߸��� ��ȭ Ÿ��
		Failure_Currency_Value_Overflow													= 17603,	// ��ȭ �ִ�ġ �ʰ�
		Failure_Currency_Unit_Value_Overflow											= 17604,	// ��ȭ ���� ȹ�淮 �ʰ�
		Failure_Currency_Value_Underflow												= 17605,	// ��ȭ �ּ�ġ �ʰ�

		//  ä�� (17701 ~ 17800)
		Failure_Chat_Already_Reserved													= 17701,	// ä���� �̹� �����
		Failure_Chat_Not_Reserved_Partner												= 17702,	// ������� ���� ��Ʈ��
		Failure_Chat_Invalid_GroupID													= 17703,	// ��ȿ���� ���� ä�� �׷� ���̵�
		Failure_Chat_Group_Create_Fail													= 17704,	// ���� ����
		Failure_Chat_Group_Not_Found													= 17705,	// ä�� �׷� ã�� ����
		Failure_ChatGroupManager_Is_Null												= 17706,	// ä�� �׷� �Ŵ����� null �̴�
		Failure_Translation_Same_LanguageType											= 17707,	// ���� ��� �̹Ƿ� ���� ��� �Ұ�
		Failure_Translation_InvalidState												= 17708,	// ������� �Ұ� ����

		Failure_VideoChat_Invalid_State													= 17720,	// ȭ�� ä���� �ϱ� ���� ���°� �ƴ�
		Failure_VideoChat_NotLoginState													= 17721,	// ������ ���������� �ʽ��ϴ�.
		Failure_VideoChat_Cannot_connect												= 17722,	// ������ ���������ϴ�.
		Failure_VideoChat_TheLineIsInUse												= 17723,	// ��ȭ��
		Failure_VideoChat_Timeout														= 17724,	// �ð� �ʰ��� ������ �������ϴ�.

		//	���� (17801 ~ 17900)
		Failure_Reward_Item_Insert_Failed												= 17801,	// ���� ������ �����̳� ��� ����
		Failure_Reward_Currency_Insert_Failed											= 17802,	// ���� ��ȭ �����̳� ��� ����

		//	�ϻ� �̺�Ʈ (17901 ~ 18000)
		Failure_LifeEvent_Invalid_DialogueActionType									= 17901,	// �߸��� ���̾�α� �׼� Ÿ��

		//	���� (18001 ~ 18100)
		Failure_Shop_Invalid_DisplayStandType											= 18001,	// �߸��� ������ Ÿ��
		Failure_Shop_Invalid_Shop_Item													= 18002,	// �߸��� ���� ������
		Failure_Shop_Item_Insert_Failed													= 18003,	// ���� ������ �����̳� ��� ����
		Failure_Shop_Invalid_PurchasePrice												= 18004,	// �߸��� ���Ű���
		Failure_Shop_Invalid_SellingPrice												= 18005,	// �߸��� �ǸŰ���

		//	��ī�� (18101 ~ 18200)
		Failure_Sekai_Invalid_CheatType													= 18101,	// �߸��� ġƮŸ��
		Failure_Sekai_Load_Fail															= 18102,	// ��ī�� ���� �ε� ����
		Failure_Sekai_CheckValid_Fail													= 18103,	// ��ī�� ���� ���� ����
		Failure_Sekai_SendMsg_Fail														= 18104,	// ��ī�� ���� �۽� ����
		Failure_Sekai_Proceeding_Settlement												= 18105,	// ���� ������
		Failure_Sekai_DBLoad_Fail														= 18106,	// DB ��ī�� ���� �ε� ����
		Failure_Sekai_Sekai_Info_Is_Nullptr												= 18107,	// ��ī�� ������ nullptr�̴�
		Failure_Sekai_Not_Exist_Info													= 18108,	// ��ī�� ������ ������������
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
