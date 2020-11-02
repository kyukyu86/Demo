//******************************************************************************
//	FileName: 	Define_Trigger.h
//------------------------------------------------------------------------------
//	Created	:	2019/09/27
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeWR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	트리거 관련
//******************************************************************************


#pragma once


#define TRIGGER_NODE_COUNT_MAX 600
#define SPAWN_TRANSFORMINFO_MAX 6
#define LINKED_OUT_PIN_MAX 20
#define RANDOM_SPAWN_INFO_MAX 20
#define FOLLOW_ACTOR_MAX 10


struct EWRTriggerNodeType
{
	enum en
	{
		None,

		//	TriggerNode
		TriggerNodeStart,

		Trigger_None,
		Trigger_Box,
		Trigger_Sphere,

		Trigger_MainAreaBox,
		Trigger_SubAreaBox,

		TriggerNodeEnd,


		//	ActionNode
		ActionNodeStart = 1000,

		Action_AreaCamera,
		Action_AvoidanceOff,
		Action_AvoidanceOn,
		Action_BGDark,
		Action_CamBaseRecord,
		Action_CamPlayDirector,
		Action_CheckOneWay,
		Action_ChangeTutorialState,
		Action_CloseChatBubble,
		Action_ColliderOff,
		Action_ColliderOn,
		Action_DisableAI,
		Action_DisableInput,
		Action_EnableAI,
		Action_EnableInput,
		Action_GotoMap,
		Action_GotoPos,
		Action_HideActor,
		Action_HideUI,
		Action_OpenChatBubble,
		Action_PauseAI,
		Action_PlayLevelActionSequence,
		Action_PlayLevelSequence,
		Action_PlayQuestSequence,
		Action_PlayAnim,
		Action_PlayBossAppear,
		Action_PlayBossAppearParams,
		Action_PlayMatinee,
		Action_PlaySound,
		Action_RestoreAutoCombat,
		Action_ResumeAI,
		Action_SaveAutoCombat,
		Action_SetActorRotation,
		Action_SetFaction,
		Action_ResetFaction,
		Action_SetFollowPlayer,
		Action_PlayerWayPoint,
		Action_ShowActor,
		Action_ShowDialogue,
		Action_ShowUI,
		Action_StageClear,
		Action_StartAutoCombat,
		Action_StartQuestTime,
		Action_StopAutoCombat,
		Action_StopSnd,
		Action_Summon,
		Action_TimeScale,
		Action_UnBGDark,
		Action_UnSpawn,
		Action_UnSummon,
		Action_UnWatch,
		Action_Watch,
		Action_NPCWayPoint,
		Action_SetWayPoint,
		
		ActionSpawnStart,
		Action_NpcSpawn,
		Action_ENpcSpawn,			//	ENPC / NPC 통합
		Action_PortalSpawn,
		Action_IOSpawn,
		Action_SOSpawn,
		Action_AOSpawn,
		Action_ActionPropSpawn,
		Action_BrickSpawn,
		Action_ElevatorSpawn,
		Action_FloatingBoardSpawn,
		Action_TardisSpawn,
		Action_SiteSpawn,
				
		ActionRandomSpawnStart,
		Action_RandomNpcSpawn,
		ActionRandomSpawnEnd,

		ActionSpawnEnd,

		Action_Active,
		Action_Deactive,
		Action_Enable,
		Action_Disable,

		Action_ClientDirectSync,
		Action_DungeonSuccess,
		Action_HideEmitter,
		Action_ShowEmitter,
		Action_CheckAirLine,
		Action_Tutorial,
		Action_PartyReady,
		Action_StartDisplayFinished,
		Action_EnableFollow,
		Action_DisableFollow,
		Action_TimeCheck,
		Action_SetCameraRule,
		Action_ShowRacingDistance,
		Action_BotTestStart,
		Action_BotTestEnd,
		Action_PlayMovie,
		Action_GotoCharacterSelect,
		Action_BoostGaugeRecharge,
		Action_ScreenWater,
		Action_ShowNotice,
		Action_ShowSpeech,
		Action_SearchAO,
		Action_PartyWarp,
		Action_SpawnAllKill,
		Action_TODControl,
		Action_WeatherControl,
		Action_WeatherOption,
		Action_AcceptQuest,
		Action_FirstGamePlayTutorial,
		Action_ShowAllActor,
		Action_HideAllActor,
		Action_CamMoveBackward,
		Action_PlayStreamingMovie,

		// WR project 신규 Action
		Action_Warp,
		Action_WalkSpeedRate,
		Action_NPCSocketSwap,
		Action_ControllerVibrationStart,
		Action_ControllerVibrationEnd,
		Action_QuestComplete,
		Action_SoundSpawn,
		Action_PlayObjectAnim,
		Action_SoundUnSpawn,
		Action_TutorialMoveComplete,
		Action_ShowDialogueEnd,
		Action_StopMovie,
		Action_ShowMarker,
		Action_HideMarker,
		Action_FollowStart,
		Action_FollowEnd,
		Action_ShowFirstPersonCharacter,
		Action_ShowThirdPersonCharacter,
		Action_Play360Movie,
		Action_ServerTriggerActive,		// 클라 전용
		Action_StartVoiceRecord,
		Action_MapConnector,
		Action_PlayerCountUp,
		Action_PlayerCountDown,
		Action_ActionPropTransform,
		Action_SetMidpointPauseDirecting,
		//==============================================================================
		ActionNodeEnd,


		//	TransitionNode
		TransitionNodeStart = 10000,

		Transition_Active,
		Transition_Deactive,
		Transition_Enter,
		Transition_Exit,
		Transition_Etc,
		Transition_Loop,
				
		TransitionNodeEnd,

		//	Special
		SpecialNodeStart = 11000,
		Special_Watch,
		SPecial_Group,
		SpecialNodeEnd,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,							L"None" );
		InsertMap( TriggerNodeStart,				L"TriggerNodeStart" );
		InsertMap( Trigger_None,					L"Trigger_None" );
		InsertMap( Trigger_Box,						L"Trigger_Box" );
		InsertMap( Trigger_Sphere,					L"Trigger_Sphere" );
		InsertMap( Trigger_MainAreaBox,				L"Trigger_MainAreaBox" );
		InsertMap( Trigger_SubAreaBox,				L"Trigger_SubAreaBox" );
		InsertMap( TriggerNodeEnd,					L"TriggerNodeEnd" );
		InsertMap( ActionNodeStart,					L"ActionNodeStart" );
		InsertMap( Action_AreaCamera,				L"Action_AreaCamera" );
		InsertMap( Action_AvoidanceOff,				L"Action_AvoidanceOff" );
		InsertMap( Action_AvoidanceOn,				L"Action_AvoidanceOn" );
		InsertMap( Action_BGDark,					L"Action_BGDark" );
		InsertMap( Action_CamBaseRecord,			L"Action_CamBaseRecord" );
		InsertMap( Action_CamPlayDirector,			L"Action_CamPlayDirector" );
		InsertMap( Action_CheckOneWay,				L"Action_CheckOneWay" );
		InsertMap( Action_ChangeTutorialState,		L"Action_ChangeTutorialState" );
		InsertMap( Action_CloseChatBubble,			L"Action_CloseChatBubble" );
		InsertMap( Action_ColliderOff,				L"Action_ColliderOff" );
		InsertMap( Action_ColliderOn,				L"Action_ColliderOn" );
		InsertMap( Action_DisableAI,				L"Action_DisableAI" );
		InsertMap( Action_DisableInput,				L"Action_DisableInput" );
		InsertMap( Action_EnableAI,					L"Action_EnableAI" );
		InsertMap( Action_EnableInput,				L"Action_EnableInput" );
		InsertMap( Action_GotoMap,					L"Action_GotoMap" );
		InsertMap( Action_GotoPos,					L"Action_GotoPos" );
		InsertMap( Action_HideActor,				L"Action_HideActor" );
		InsertMap( Action_HideUI,					L"Action_HideUI" );
		InsertMap( Action_OpenChatBubble,			L"Action_OpenChatBubble" );
		InsertMap( Action_PauseAI,					L"Action_PauseAI" );
		InsertMap( Action_PlayLevelActionSequence,	L"Action_PlayLevelActionSequence" );
		InsertMap( Action_PlayLevelSequence,		L"Action_PlayLevelSequence" );
		InsertMap( Action_PlayQuestSequence,		L"Action_PlayQuestSequence" );
		InsertMap( Action_PlayAnim,					L"Action_PlayAnim" );
		InsertMap( Action_PlayBossAppear,			L"Action_PlayBossAppear" );
		InsertMap( Action_PlayBossAppearParams,		L"Action_PlayBossAppearParams" );
		InsertMap( Action_PlayMatinee,				L"Action_PlayMatinee" );
		InsertMap( Action_PlaySound,				L"Action_PlaySound" );
		InsertMap( Action_RestoreAutoCombat,		L"Action_RestoreAutoCombat" );
		InsertMap( Action_ResumeAI,					L"Action_ResumeAI" );
		InsertMap( Action_SaveAutoCombat,			L"Action_SaveAutoCombat" );
		InsertMap( Action_SetActorRotation,			L"Action_SetActorRotation" );
		InsertMap( Action_SetFaction,				L"Action_SetFaction" );
		InsertMap( Action_ResetFaction,				L"Action_ResetFaction" );
		InsertMap( Action_SetFollowPlayer,			L"Action_SetFollowPlayer" );
		InsertMap( Action_PlayerWayPoint,			L"Action_PlayerWayPoint" );
		InsertMap( Action_ShowActor,				L"Action_ShowActor" );
		InsertMap( Action_ShowDialogue,				L"Action_ShowDialogue" );
		InsertMap( Action_ShowUI,					L"Action_ShowUI" );
		InsertMap( Action_StageClear,				L"Action_StageClear" );
		InsertMap( Action_StartAutoCombat,			L"Action_StartAutoCombat" );
		InsertMap( Action_StartQuestTime,			L"Action_StartQuestTime" );
		InsertMap( Action_StopAutoCombat,			L"Action_StopAutoCombat" );
		InsertMap( Action_StopSnd,					L"Action_StopSnd" );
		InsertMap( Action_Summon,					L"Action_Summon" );
		InsertMap( Action_TimeScale,				L"Action_TimeScale" );
		InsertMap( Action_UnBGDark,					L"Action_UnBGDark" );
		InsertMap( Action_UnSpawn,					L"Action_UnSpawn" );
		InsertMap( Action_UnSummon,					L"Action_UnSummon" );
		InsertMap( Action_UnWatch,					L"Action_UnWatch" );
		InsertMap( Action_Watch,					L"Action_Watch" );
		InsertMap( Action_NPCWayPoint,				L"Action_NPCWayPoint" );
		InsertMap( ActionSpawnStart,				L"ActionSpawnStart" );
		InsertMap( Action_NpcSpawn,					L"Action_NpcSpawn" );
		InsertMap( Action_ENpcSpawn,				L"Action_ENpcSpawn" );
		InsertMap( Action_PortalSpawn,				L"Action_PortalSpawn" );
		InsertMap( Action_IOSpawn,					L"Action_IOSpawn" );
		InsertMap( Action_SOSpawn,					L"Action_SOSpawn" );
		InsertMap( Action_AOSpawn,					L"Action_AOSpawn" );
		InsertMap( Action_ActionPropSpawn,			L"Action_ActionPropSpawn");
		InsertMap( Action_BrickSpawn,				L"Action_BrickSpawn");
		InsertMap( Action_ElevatorSpawn,			L"Action_ElevatorSpawn");
		InsertMap( Action_FloatingBoardSpawn,		L"Action_FloatingBoardSpawn");
		InsertMap( Action_TardisSpawn,				L"Action_TardisSpawn");
		InsertMap( Action_SiteSpawn,				L"Action_SiteSpawn");
		InsertMap( Action_RandomNpcSpawn,			L"Action_RandomNpcSpawn" );
		InsertMap( ActionSpawnEnd,					L"ActionSpawnEnd" );
		InsertMap( Action_Active,					L"Action_Active" );
		InsertMap( Action_Deactive,					L"Action_Deactive" );
		InsertMap( Action_Enable,					L"Action_Enable" );
		InsertMap( Action_Disable,					L"Action_Disable" );
		InsertMap( Action_ClientDirectSync,			L"Action_ClientDirectSync" );
		InsertMap( Action_DungeonSuccess,			L"Action_DungeonSuccess" );
		InsertMap( Action_HideEmitter,				L"Action_HideEmitter" );
		InsertMap( Action_ShowEmitter,				L"Action_ShowEmitter" );
		InsertMap( Action_CheckAirLine,				L"Action_CheckAirLine" );
		InsertMap( Action_Tutorial,					L"Action_Tutorial" );
		InsertMap( Action_PartyReady,				L"Action_PartyReady" );
		InsertMap( Action_StartDisplayFinished,		L"Action_StartDisplayFinished" );
		InsertMap( Action_EnableFollow,				L"Action_EnableFollow" );
		InsertMap( Action_DisableFollow,			L"Action_DisableFollow" );
		InsertMap( Action_TimeCheck,				L"Action_TimeCheck" );
		InsertMap( Action_SetCameraRule,			L"Action_SetCameraRule" );
		InsertMap( Action_ShowRacingDistance,		L"Action_ShowRacingDistance" );
		InsertMap( Action_BotTestStart,				L"Action_BotTestStart" );
		InsertMap( Action_BotTestEnd,				L"Action_BotTestEnd" );
		InsertMap( Action_PlayMovie,				L"Action_PlayMovie" );
		InsertMap( Action_GotoCharacterSelect,		L"Action_GotoCharacterSelect" );
		InsertMap( Action_BoostGaugeRecharge,		L"Action_BoostGaugeRecharge" );
		InsertMap( Action_ScreenWater,				L"Action_ScreenWater" );
		InsertMap( Action_ShowNotice,				L"Action_ShowNotice" );
		InsertMap( Action_ShowSpeech,				L"Action_ShowSpeech" );
		InsertMap( Action_SearchAO,					L"Action_SearchAO" );
		InsertMap( Action_PartyWarp,				L"Action_PartyWarp" );
		InsertMap( Action_SpawnAllKill,				L"Action_SpawnAllKill" );
		InsertMap( Action_TODControl,				L"Action_TODControl" );
		InsertMap( Action_WeatherControl,			L"Action_WeatherControl" );
		InsertMap( Action_WeatherOption,			L"Action_WeatherOption" );
		InsertMap( Action_AcceptQuest,				L"Action_AcceptQuest" );
		InsertMap( Action_FirstGamePlayTutorial,	L"Action_FirstGamePlayTutorial" );
		InsertMap( Action_ShowAllActor,				L"Action_ShowAllActor" );
		InsertMap( Action_HideAllActor,				L"Action_HideAllActor" );
		InsertMap( Action_CamMoveBackward,			L"Action_CamMoveBackward" );
		InsertMap( Action_PlayStreamingMovie,		L"Action_PlayStreamingMovie" );
		// WR project 신규 Action
		InsertMap(Action_Warp,						L"Action_Warp");
		InsertMap(Action_WalkSpeedRate,				L"Action_WalkSpeedRate");
		InsertMap(Action_NPCSocketSwap,				L"Action_NPCSocketSwap");
		InsertMap(Action_ControllerVibrationStart,	L"Action_ControllerVibrationStart");
		InsertMap(Action_ControllerVibrationEnd,	L"Action_ControllerVibrationEnd");
		InsertMap(Action_QuestComplete,				L"Action_QuestComplete");
		InsertMap(Action_SoundSpawn,				L"Action_SoundSpawn");
		InsertMap(Action_PlayObjectAnim,			L"Action_PlayObjectAnim");
		InsertMap(Action_SoundUnSpawn,				L"Action_SoundUnSpawn");
		InsertMap(Action_TutorialMoveComplete,		L"Action_TutorialMoveComplete");
		InsertMap(Action_ShowDialogueEnd,			L"Action_ShowDialogueEnd");
		InsertMap(Action_StopMovie,					L"Action_StopMovie");
		InsertMap(Action_ShowMarker,				L"Action_ShowMarker");
		InsertMap(Action_HideMarker,				L"Action_HideMarker");
		InsertMap(Action_FollowStart,				L"Action_FollowStart");
		InsertMap(Action_FollowEnd,					L"Action_FollowEnd");
		InsertMap(Action_ShowFirstPersonCharacter,	L"Action_ShowFirstPersonCharacter");
		InsertMap(Action_ShowThirdPersonCharacter,	L"Action_ShowThirdPersonCharacter");
		InsertMap(Action_Play360Movie,				L"Action_Play360Movie");
		InsertMap(Action_ServerTriggerActive,		L"Action_ServerTriggerActive");			// 클라 전용
		InsertMap(Action_StartVoiceRecord,			L"Action_StartVoiceRecord");	
		InsertMap(Action_MapConnector,				L"Action_MapConnector");
		InsertMap(Action_PlayerCountUp,				L"Action_PlayerCountUp");
		InsertMap(Action_PlayerCountDown,			L"Action_PlayerCountDown");
		InsertMap(Action_ActionPropTransform,		L"Action_ActionPropTransform");
		InsertMap(Action_SetMidpointPauseDirecting, L"Action_SetMidpointPauseDirecting"); \
		//==============================================================================
		InsertMap( ActionNodeEnd,					L"ActionNodeEnd" );
		InsertMap( TransitionNodeStart,				L"TransitionNodeStart" );
		InsertMap( Transition_Active,				L"Transition_Active" );
		InsertMap( Transition_Deactive,				L"Transition_Deactive" );
		InsertMap( Transition_Enter,				L"Transition_Enter" );
		InsertMap( Transition_Exit,					L"Transition_Exit" );
		InsertMap( Transition_Etc,					L"Transition_Etc" );
		InsertMap( Transition_Loop,					L"Transition_Loop");
		InsertMap( TransitionNodeEnd,				L"TransitionNodeEnd" );
		InsertMap( SpecialNodeStart,				L"SpecialNodeStart" );
		InsertMap( Special_Watch,					L"Special_Watch" );
		InsertMap( SPecial_Group,					L"Special_Group" );
		InsertMap( Action_SetWayPoint,				L"Action_SetWayPoint" );
		InsertMap( SpecialNodeEnd,					L"SpecialNodeEnd" );
	}

	static inline bool IsVaild(EWRTriggerNodeType::en e)
	{
		int32 Start = (int32)EWRTriggerNodeType::TriggerNodeStart;
		int32 End = (int32)EWRTriggerNodeType::MAX;
		return Start < (int32)e && End >(int32)e;
	}

	static inline bool IsInRange(EWRTriggerNodeType::en e, EWRTriggerNodeType::en From, EWRTriggerNodeType::en To)
	{
		// 		if (IsVaild(From) == false || IsValid(To) == false)
		// 			return false;

		int32 Start = (int32)From;
		int32 End = (int32)To;

		return Start < (int32)e && End >(int32)e;
	}

	static inline bool IsTriggerNode(EWRTriggerNodeType::en e)
	{
		return IsInRange(e, TriggerNodeStart, TriggerNodeEnd);
	}

	static inline bool IsActionNode(EWRTriggerNodeType::en e)
	{
		return IsInRange(e, ActionNodeStart, ActionNodeEnd);
	}

	static inline bool IsSpawnActionNode(EWRTriggerNodeType::en e)
	{
		return IsInRange(e, ActionSpawnStart, ActionSpawnEnd);
	}

	static inline bool IsTransitionNode(EWRTriggerNodeType::en e)
	{
		return IsInRange(e, TransitionNodeStart, TransitionNodeEnd);
	}
	static inline bool IsSpecialNode(EWRTriggerNodeType::en e)
	{
		return IsInRange(e, SpecialNodeStart, SpecialNodeEnd);
	}
	static inline bool IsRandomSpawnNode(EWRTriggerNodeType::en e)
	{
		return IsInRange(e, ActionRandomSpawnStart, ActionRandomSpawnEnd);
	}
};


struct EWRTriggerUidStartValue
{
	enum en
	{
		Trigger				= 1,
		SpawnIo				= 1000,
		SpawnNpc			= 2000,
		SpawnEnpc			= 3000,			//	ENPC / NPC 통합
		SpawnPortal			= 6000,
		UnSpawn				= 7000,
		SpawnSo				= 8000,
		SpawnAo				= 9000,
		SpawnActionProp		= 10000,
		SpawnBrick			= 11000,
		SpawnElevator		= 12000,
		SpawnFloatingBoard	= 13000,
		SpawnTardis			= 14000,
		SpawnSite			= 15000,

		RandomSpawnNpc		= 60000,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Trigger,				L"Trigger" );
		InsertMap( SpawnIo,				L"SpawnIo" );
		InsertMap( SpawnNpc,			L"SpawnNpc" );
		InsertMap( SpawnEnpc,			L"SpawnEnpc" );
		InsertMap( SpawnPortal,			L"SpawnPortal" );
		InsertMap( UnSpawn,				L"UnSpawn" );
		InsertMap( SpawnSo,				L"SpawnSo" );
		InsertMap( SpawnAo,				L"SpawnAo" );
		InsertMap( SpawnActionProp,		L"SpawnActionProp" );
		InsertMap( SpawnBrick,			L"SpawnBrick");
		InsertMap( SpawnElevator,		L"SpawnElevator");
		InsertMap( SpawnFloatingBoard,	L"SpawnFloatingBoard");
		InsertMap( SpawnTardis,			L"SpawnTardis");
		InsertMap( SpawnSite,			L"SpawnSite");

		InsertMap( RandomSpawnNpc,	L"RandomSpawnNpc" );
	}
};


#ifdef USE_SERVER_ONLY
struct EWRServerTriggerAction
{
	enum en
	{
		NONE = 0,

		Spawn_Enpc,							// 몬스터 소환
		Spawn_Npc,							// NPC 소환
		Spawn_Portal,						// Portal 생성
		Spawn_Brick,						// 장애물 소환
		Spawn_IO,							// IO 소환
		Spawn_SO,							// SO 소환
		Spawn_AO,							// AO 소환
		Spawn_ActionProp,					// ActionProp 소환
		Spawn_Elevator,						// Elevator 소환
		Spawn_FloatingBoard,				// FloatingBoard 소환
		Spawn_Tardis,						// Tardis 소환

		UnSpawn_Enpc,						// 몬스터 제거
		UnSpawn_Npc,						// NPC 제거
		UnSpawn_Portal,						// Portal 제거
		UnSpawn_Brick,						// 장애물 제거
		UnSpawn_IO,							// IO 제거
		UnSpawn_SO,							// SO 제거
		UnSpawn_AO,							// AO 제거
		UnSpawn_ActionProp,					// ActionProp 제거
		UnSpawn_Elevator,					// Elevator 제거
		UnSpawn_FloatingBoard,				// FloatingBoard 제거
		UnSpawn_Tardis,						// Tardis 제거

		Change_AO_State,					// AO 상태 변경

		ClientDirectSync,					// 클라이언트 연출 싱크를 마추기 위해 서버 트리거로 클라이언트 트리거 실행
		Dungeon_Success,					// 던전 성공을 하고 클라이언트에게 연출을 보내고 죽은 유저가 있음 살려준다.

		Get_Waypoint,						// 목적지 설정
		Call_Player_Level_Action_Sequence,	// 액션 동기화
		Time_Check,							// 펠로우 경주 중간 목적지 통과 시간 검사
		SpawnAllKill,						// 특정 팩션의 Enpc 올킬

		//	XR 추가 기능
		STA_Warp,							//	특정 좌표로 이동
		STA_WalkSpeedRate,					//	이동 속도 변경
		STA_QuestComplete,					//	퀘스트 완료 상태로 변경. 보상은 받지 않은 상태
		STA_MapConnector,					//	맵 연결 지역
		STA_PlayerCountUp,					//	플레이어 카운트 올리기
		STA_PlayerCountDown,				//	플레이어 카운트 내리기

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Spawn_Enpc,							L"Spawn_Enpc" );
		InsertMap( Spawn_Npc,							L"Spawn_Npc" );
		InsertMap( Spawn_Portal,						L"Spawn_Portal" );
		InsertMap( Spawn_Brick,							L"Spawn_Brick" );
		InsertMap( Spawn_IO,							L"Spawn_IO" );
		InsertMap( Spawn_SO,							L"Spawn_SO" );
		InsertMap( Spawn_AO,							L"Spawn_AO" );
		InsertMap( Spawn_ActionProp,					L"Spawn_ActionProp" );
		InsertMap( Spawn_Elevator,						L"Spawn_Elevator");
		InsertMap( Spawn_FloatingBoard,					L"Spawn_FloatingBoard");
		InsertMap( Spawn_Tardis,						L"Spawn_Tardis");
		InsertMap( UnSpawn_Enpc,						L"UnSpawn_Enpc" );
		InsertMap( UnSpawn_Npc,							L"UnSpawn_Npc" );
		InsertMap( UnSpawn_Portal,						L"UnSpawn_Portal" );
		InsertMap( UnSpawn_Brick,						L"UnSpawn_Brick" );
		InsertMap( UnSpawn_IO,							L"UnSpawn_IO" );
		InsertMap( UnSpawn_SO,							L"UnSpawn_SO" );
		InsertMap( UnSpawn_AO,							L"UnSpawn_AO" );
		InsertMap( UnSpawn_Elevator,					L"UnSpawn_Elevator" );
		InsertMap( UnSpawn_FloatingBoard,				L"UnSpawn_FloatingBoard" );
		InsertMap( UnSpawn_Tardis,						L"UnSpawn_Tardis");
		InsertMap( Change_AO_State,						L"Change_AO_State" );
		InsertMap( ClientDirectSync,					L"ClientDirectSync" );
		InsertMap( Dungeon_Success,						L"Dungeon_Success" );
		InsertMap( Get_Waypoint,						L"Get_Waypoint" );
		InsertMap( Call_Player_Level_Action_Sequence,	L"Call_Player_Level_Action_Sequence" );
		InsertMap( Time_Check,							L"Time_Check" );
		InsertMap( SpawnAllKill,						L"SpawnAllKill" );
		InsertMap( STA_Warp,							L"STA_Warp" );
		InsertMap( STA_WalkSpeedRate,					L"STA_WalkSpeedRate" );
		InsertMap( STA_QuestComplete,					L"STA_QuestComplete" );
		InsertMap( STA_MapConnector,					L"STA_MapConnector" );
		InsertMap( STA_PlayerCountUp,					L"STA_PlayerCountUp" );
		InsertMap( STA_PlayerCountDown,					L"STA_PlayerCountDown" );
	}
};
#endif // USE_SERVER_ONLY