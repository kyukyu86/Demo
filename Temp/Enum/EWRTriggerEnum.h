// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.

#pragma once
#include "WRPCH.h"
#include "../Network/Share/Define/Define_Trigger.h"

struct EWRTriggerType
{
	enum en
	{
		None,
		Box,
		Sphere,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, TEXT("None"));
		InsertMap(Box, TEXT("Box"));
		InsertMap(Sphere, TEXT("Sphere"));
	}
};

struct EWRTriggerPurpose
{
	enum en
	{
		None,							// 없음
		FinishTriggerBegin,				// 끝장 트리거 시작 - 아래 순서 바꾸지 말 것!!

		Win = FinishTriggerBegin,		// 승리( 스테이지 클리어 )
		Lose,							// 패배( 내 플레이어 죽음 )
		Draw,							// 비기기( 결투장 )

		FinishTriggerEnd = Draw,		// 끝장 트리거 종료 - 위 순서 바꾸지 말 것!!

		Groggy,			// 그로기 모드
		Ultimate,		// 궁극기

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, TEXT("None"));
		InsertMap(Win, TEXT("Win"));
		InsertMap(Lose, TEXT("Lose"));
		InsertMap(Lose, TEXT("Draw"));
		InsertMap(Groggy, TEXT("Groggy"));
		InsertMap(Ultimate, TEXT("Ultimate"));
	}
};


struct EWRTrgActType
{
	enum en
	{
		Enable,         // Enable [Tid]                         : 트리거 켜기
		Disable,        // Disable [Tid]                        : 트리거 끄기

		Activate,       // Activate [Tid]                       : 트리거 활성화
		Deactivate,     // Deactivate [Tid]                     : 트리거 비활성화

		PlayAnim,       // PlayAnim [Uid] [AnimName]            : 애니메이션 재생

		StageClear,     // 스테이지 종료 ( 성공 [1] / 실패 [0] )     : 스테이지 종료

		Spawn,          // Spawn [Tid]                          : 몬스터 생성
		UnSpawn,        // UnSpawn                              : 궁극기용 펠로우 해제

		Summon,         // Summon ( [0]:탑승 / [1]:동행 )       : 소환 / 탑승
		UnSummon,       // UnSummon                             : 하차 / 해제

		ShowUI,         // ShowUI ( [0]: 모든 / [1]~ : 각각 )    : UI 보이기
		HideUI,         // HideUI ( [0]: 모든 / [1]~ : 각각 )    : UI 숨기기
		SetBossInfo,    // SetBossInfo [UID]                    : 보스연출용 UI 및 보스체력게이지 / 이름UI 세팅 

		SetPosUI,       // SetPosUI ( [1]~ : 각각 )             : UI 위치 옮기기
		SetAngleUI,     // SetAngleUI ( [1]~ : 각각 )           : UI 회전 주기

		PlaySnd,        // PlaySnd [eSoundType] [Path]          : 사운드 재생
		StopSnd,		// PlaySnd [eSoundType]					: 사운드 정지

		EnableInput,    // EnableInput                          : 입력 가능
		DisableInput,   // DisableInput                         : 입력 불가능

		ColliderOn,     // ColliderOn                           : 충돌메쉬 on
		ColliderOff,    // ColliderOff                          : 충돌메쉬 off

		TimeScale,      // TimeScale [ 시간 ]                     : 시간 흐름 제어

		Watch,          // Watch [Tid]                          : 관찰 대상 설정
		UnWatch,        // UnWatch [Tid]                        : 관찰 대상 해제

		BGDark,         // BGDark [ 시간 ]                        : 암전 시작
		UnBGDark,       // BGDark [ 시간 ]                        : 암전 회복

		ShowActor,      // ShowActor [Tid]                      : 액터 보이기
		HideActor,      // HideActor [Tid]                      : 액터 숨기기

		Shake,          // Shake [Uid] [Tid]                    : Shake기능 발동

		SetWayPoint,    // SetWayPoint [Tid]                    : WayPoint 지정

		StartAutoCombat,// StartAutoCombat                      : 자동전투시작
		StopAutoCombat, // StopAutoCombat                       : 자동전투종료

		SaveAutoCombat,// SaveAutoCombat						: 자동전투 상태 저장
		RestoreAutoCombat, // RestoreAutoCombat                 : 자동전투 상태 복구

		EnableAI,       // EnableAI [eCharType]                 : 대상타입에 AI 작동 시작
		DisableAI,      // DisableAI [eCharType]                : 대상타입에 AI 작동 시작

		AddBuff,		// AddBuff [Uid] [Tid]					: 대상에게 버프 부여
		RemoveBuff,     // RemoveBuff [Uid] [Tid]               : 대상에게 버프 해제

		LookIK,			// Look [Uid]							: 대상을 바라보도록 설정
		Move,			// Move									: 대상을 해당지점으로 이동
		Jump,			// Jump									: 대상을 해당지점으로 점프하여 이동
		JumpFall,		// JumpFall								: 대상을 해당지점으로 점프하여 이동 ( 지형의 높이의 영향을 받음 )
		JumpFallParam,	// JumpFall								: JumpFall Trigger 에서 필요한 정보를 받아 셋팅
		FellowRide,		// FellowRide [Uid]						: 대상에게 탑승하도록 설정
		
		CamPlayDirector,// CamPlayDirector [대상Uid] [BP명] 	: 카메라 액션 재생 ( PlayDirector )
		CamBaseRecord,	// CamBaseRecord [대상Uid] [Tid] [시간]	: 기본 카메라 변경 ( SetViewTarget )

		SetForcedRotation,	// SetForcedRotation [대상Uid] [Pitch] [Roll] [ 시간 ] : 강제 회전값 설정

		PauseAI,		// PauseAI [Uid]							: AI 작동 멈춤
		ResumeAI,		// ResumeAI [Uid]							: AI 작동 재개
		
		PlayMatinee,	// PlayMatinee [MatineeName] [BossTid] [StartTime] [EndTime] : 마티네 재생
		
		PlayBossAppear, // PlayBossAppear [LevelSequenceName] [BossTid] [BossAppearPanelDelayTime] [PostTriggerID] : 보스 연출 시퀀스
		PlayBossAppearParam,
		PlayLevelActionSequence, // PlayActionSequence [LevelSequenceName] : 레벨 액션 시퀀스
		PlayLevelSequence, // PlayLevelSequence [LevelSequenceName] : 레벨 연출 시퀀스 트리거
		PlayQuestSequence, // PlayQuestSequence [LevelSequenceName] : 퀘스트 시퀀스
		SpawnEmitter,	// SpawnEmitter [EmitterPath] [AutoDestroy]	: 이펙트 스폰

		AreaCamera,		// AreaCamera [ 카메라이름 ] [ 블랜드시간 ] [ 블랜드타입 ] [ 추적타입 ]
		AreaCameraTraceValue,	// AreaCamera [Yaw] [Pitch] [Roll] [distance] 
		MoviePlay,				// MoviePlay

		OpenChatBubble,		// OpenChatBubble [EWRCharacterType] [CharacterUid] [LocalStringid] : 말풍선 채팅 추가 / 변경
		CloseChatBubble,	// CloseChatBubble [EWRCharacterType] [CharacterTid] : 말풍선 채팅 삭제

		SetFaction,		// SetFaction [대상Uid] [EWRCharacterFactionType] : 대상의 Faction 을 변경함

		AddFlyHeightMin,	// AddFlyHeightMin [ 대상Uid ] [ 추가값 ] : 대상의 FlyHeightMin 값을 추가함 ( 해당맵에서만 임의 사용 )

		LoadActors4Hide,	// LoadActors4Hide [eCharType]	: 액터 종류별 숨기기 저장한 애들 보이기
		SaveActors4Hide,	// SaveActors4Hide [eCharType]	: 액터 종류별 숨기기 및 저장하기

		StartQuest,		// StartQuest [Id]		: Quest 시작
		CompleteQuest,	// CompleteQuest [Id]	: Quest 완료
		StartQuestTime, // StartQuest			: QuestTime 시작
		StopQuestTime, // StartQuest			: QuestTime 멈춤

		AmIHere,			// AmIHere					: 파티원 position send
		QuestReady,			// QuestReady 				: 파티원 퀘스트 종료 대기 : 서버로 퀘스트 시작
		PartyReady,			// PartyReady [Tid]			: 파티원 입장 대기 : 모든 파티원 입장시, 트리거 발동
		PartyMove2Me,		// PartyMove2Me				: 파티원 위치 이동 ( 나에게 ) : 승리포즈를 위하여
		PartyPlayAnim,		// PartyPlayAnim			: 파티원 애니메이션 재생

		Tutorial,			// Tutorial [Id]	: 튜토리얼 시작
		CheckOneWay,		// CheckOneWay [Uid] : 내 플레이어와 방향이 맞는지 체크하여, show/hide 및 collier on/off 적용 (IO 일때에만 작동 )	

		ChangeTutorialState,// ChgTutorialState [EWRTutorialState::en ( int형 )] : 튜토리얼 상태를 변경 ( 서버로 전송 )..클래스명이 32자를 넘어가질 못해 어쩔 수 없이 chg로 씀

		ShowDialogue, // ShowDialogue [id][Next Trigger] : Dialogue 출력

		GotoMap,        // GotoMap [MapTid] [StartPositionIndex] [GotoNow] : 해당 맵으로 이동
		GotoPos,        // GotoPos [X] [Y] [Z]          : 해당 위치로 이동

		SetActorRotation,	// SetActorRotation [tid] [Roll][Pitch][Yaw] : 대상을 강제로 각도 조절

		SetStartIndex,		// SetStartIndex [value]        : Start Index

		PartyTrigger,		// PartyTrigger [tid]			: 해당 트리거를 파티장이 발동시킴!!
		AvoidanceOn,		// AvoidanceOn [UID]			: 대상의 Avoidance 를 on
		AvoidanceOff,		// AvoidanceOff [UID]			: 대상의 Avoidance 를 off

		CheckAirLine,		// CheckAirLine [UID]			: 대상 AirLine 을 방향 판단 기준으로 삼음

		MAX,			// 트리거 추가시 항상 아래 BuildMap () 에도 추가! 그렇지 않으면 Draw Fail 발생!
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Enable, TEXT("Enable"));
		InsertMap(Disable, TEXT("Disable"));

		InsertMap(Activate, TEXT("Activate"));
		InsertMap(Deactivate, TEXT("Deactivate"));

		InsertMap(PlayAnim, TEXT("PlayAnim"));

		InsertMap(StageClear, TEXT("StageClear"));

		InsertMap(Spawn, TEXT("Spawn"));
		InsertMap(UnSpawn, TEXT("UnSpawn"));

		InsertMap(Summon, TEXT("Summon"));
		InsertMap(UnSummon, TEXT("UnSummon"));

		InsertMap(ShowUI, TEXT("ShowUI"));
		InsertMap(HideUI, TEXT("HideUI"));

		InsertMap(SetBossInfo, TEXT("SetBossInfo"));

		InsertMap(SetPosUI, TEXT("SetPosUI"));
		InsertMap(SetAngleUI, TEXT("SetAngleUI"));

		InsertMap(PlaySnd, TEXT("PlaySnd"));
		InsertMap(StopSnd, TEXT("StopSnd"));

		InsertMap(EnableInput, TEXT("EnableInput"));
		InsertMap(DisableInput, TEXT("DisableInput"));

		InsertMap(ColliderOn, TEXT("ColliderOn"));
		InsertMap(ColliderOff, TEXT("ColliderOff"));

		InsertMap(TimeScale, TEXT("TimeScale"));

		InsertMap(Watch, TEXT("Watch"));
		InsertMap(UnWatch, TEXT("UnWatch"));

		InsertMap(BGDark, TEXT("BGDark"));
		InsertMap(UnBGDark, TEXT("UnBGDark"));

		InsertMap(ShowActor, TEXT("ShowActor"));
		InsertMap(HideActor, TEXT("HideActor"));

		InsertMap(Shake, TEXT("Shake"));

		InsertMap(SetWayPoint, TEXT("SetWayPoint"));

		InsertMap(StartAutoCombat, TEXT("StartAutoCombat"));
		InsertMap(StopAutoCombat, TEXT("StopAutoCombat"));

		InsertMap(SaveAutoCombat, TEXT("SaveAutoCombat"));
		InsertMap(RestoreAutoCombat, TEXT("RestoreAutoCombat"));

		InsertMap(EnableAI, TEXT("EnableAI"));
		InsertMap(DisableAI, TEXT("DisableAI"));

		InsertMap(AddBuff, TEXT("AddBuff"));
		InsertMap(RemoveBuff, TEXT("RemoveBuff"));

		InsertMap(LookIK, TEXT("LookIK"));
		InsertMap(Move, TEXT("Move"));
		InsertMap(Jump, TEXT("Jump"));
		InsertMap(JumpFall, TEXT("JumpFall"));
		InsertMap(JumpFallParam, TEXT("JumpFallParam"));
		InsertMap(FellowRide, TEXT("FellowRide"));

		InsertMap(CamPlayDirector, TEXT("CamPlayDirector"));
		InsertMap(CamBaseRecord, TEXT("CamBaseRecord"));

		InsertMap(SetForcedRotation, TEXT("SetForcedRotation"));

		InsertMap(PauseAI, TEXT("PauseAI"));
		InsertMap(ResumeAI, TEXT("ResumeAI"));

		InsertMap(PlayMatinee, TEXT("PlayMatinee"));
		InsertMap(PlayBossAppear, TEXT("PlayBossAppear"));
		InsertMap(PlayBossAppearParam, TEXT("PlayBossAppearParam"));
		InsertMap(PlayLevelActionSequence, TEXT("PlayLevelActionSequence"));
		InsertMap(PlayLevelSequence, TEXT("PlayLevelSequence"));
		InsertMap(PlayQuestSequence, TEXT("PlayQuestSequence"));
		InsertMap(SpawnEmitter, TEXT("SpawnEmitter"));

		InsertMap(AreaCamera, TEXT("AreaCamera"));
		InsertMap(AreaCameraTraceValue, TEXT("AreaCameraTraceValue"));
		InsertMap(MoviePlay, TEXT("MoviePlay"));

		InsertMap(OpenChatBubble, TEXT("OpenChatBubble"));
		InsertMap(CloseChatBubble, TEXT("CloseChatBubble"));

		InsertMap(SetFaction, TEXT("SetFaction"));

		InsertMap(AddFlyHeightMin, TEXT("AddFlyHeightMin"));

		InsertMap(LoadActors4Hide, TEXT("LoadActors4Hide"));
		InsertMap(SaveActors4Hide, TEXT("SaveActors4Hide"));

		InsertMap(StartQuest, TEXT("StartQuest"));
		InsertMap(CompleteQuest, TEXT("CompleteQuest"));
		InsertMap(StartQuestTime, TEXT("StartQuestTime"));
		InsertMap(StopQuestTime, TEXT("StopQuestTime"));

		InsertMap(AmIHere, TEXT("AmIHere"));
		InsertMap(QuestReady, TEXT("QuestReady"));
		InsertMap(PartyReady, TEXT("PartyReady"));
		InsertMap(PartyMove2Me, TEXT("PartyMove2Me"));
		InsertMap(PartyPlayAnim, TEXT("PartyPlayAnim"));

		InsertMap(Tutorial, TEXT("Tutorial"));
		InsertMap(CheckOneWay, TEXT("CheckOneWay"));

		InsertMap(ChangeTutorialState, TEXT("ChgTutorialState"));
		
		InsertMap(ShowDialogue, TEXT("ShowDialogue"));

		InsertMap(GotoMap, TEXT("GotoMap"));
		InsertMap(GotoPos, TEXT("GotoPos"));

		InsertMap(SetActorRotation, TEXT("SetActorRotation"));

		InsertMap(SetStartIndex, TEXT("SetStartIndex"));

		InsertMap(PartyTrigger, TEXT("PartyTrigger"));
		InsertMap(AvoidanceOn, TEXT("AvoidanceOn"));
		InsertMap(AvoidanceOff, TEXT("AvoidanceOff"));

		InsertMap(CheckAirLine, TEXT("CheckAirLine"));
	}
};

UENUM(BlueprintType)
namespace FEWRTrgActType
{
	enum en
	{
		Enable,         // Enable [Tid]                         : 트리거 켜기
		Disable,        // Disable [Tid]                        : 트리거 끄기

		Activate,       // Activate [Tid]                       : 트리거 활성화
		Deactivate,     // Deactivate [Tid]                     : 트리거 비활성화

		PlayAnim,       // PlayAnim [Uid] [AnimName]            : 애니메이션 재생

		StageClear,     // 스테이지 종료 ( 성공 [1] / 실패 [0] )     : 스테이지 종료

		Spawn,          // Spawn [Tid]                          : 몬스터 생성
		UnSpawn,        // UnSpawn                              : 궁극기용 펠로우 해제

		Summon,         // Summon ( [0]: 탑승 / [1]: 동행 )       : 소환 / 탑승
		UnSummon,       // UnSummon                             : 하차 / 해제

		ShowUI,         // ShowUI ( [0]: 모든 / [1]~ : 각각 )    : UI 보이기
		HideUI,         // HideUI ( [0]: 모든 / [1]~ : 각각 )    : UI 숨기기
		SetBossInfo,    // SetBossInfo [UID]                    : 보스연출용UI 및 보스체력게이지 / 이름UI 세팅 

		SetPosUI,       // SetPosUI ( [1]~ : 각각 )             : UI 위치 옮기기
		SetAngleUI,     // SetAngleUI ( [1]~ : 각각 )           : UI 회전 주기

		PlaySnd,        // PlaySnd [eSoundType] [Path]          : 사운드 재생
		StopSnd,		// PlaySnd [eSoundType]					: 사운드 정지

		EnableInput,    // EnableInput                          : 입력 가능
		DisableInput,   // DisableInput                         : 입력 불가능

		ColliderOn,     // ColliderOn                           : 충돌메쉬 on
		ColliderOff,    // ColliderOff                          : 충돌메쉬 off

		TimeScale,      // TimeScale [ 시간 ]                     : 시간 흐름 제어

		Watch,          // Watch [Tid]                          : 관찰 대상 설정
		UnWatch,        // UnWatch [Tid]                        : 관찰 대상 해제

		BGDark,         // BGDark [ 시간 ]                        : 암전 시작
		UnBGDark,       // BGDark [ 시간 ]                        : 암전 회복

		ShowActor,      // ShowActor [Tid]                      : 액터 보이기
		HideActor,      // HideActor [Tid]                      : 액터 숨기기

		Shake,          // Shake [Uid] [Tid]                    : Shake기능 발동

		SetWayPoint,    // SetWayPoint [Tid]                    : WayPoint 지정

		StartAutoCombat,// StartAutoCombat                      : 자동전투시작
		StopAutoCombat, // StopAutoCombat                       : 자동전투종료

		SaveAutoCombat,// SaveAutoCombat						: 자동전투 상태 저장
		RestoreAutoCombat, // RestoreAutoCombat                 : 자동전투 상태 복구

		EnableAI,       // EnableAI [eCharType]                 : 대상타입에 AI 작동 시작
		DisableAI,      // DisableAI [eCharType]                : 대상타입에 AI 작동 시작

		AddBuff,		// AddBuff [Uid] [Tid]					: 대상에게 버프 부여
		RemoveBuff,     // RemoveBuff [Uid] [Tid]               : 대상에게 버프 해제

		LookIK,			// Look [Uid]							: 대상을 바라보도록 설정
		Move,			// Move									: 대상을 해당지점으로 이동
		Jump,			// Jump									: 대상을 해당지점으로 점프하여 이동
		JumpFall,		// JumpFall								: 대상을 해당지점으로 점프하여 이동 ( 지형의 높이의 영향을 받음 )
		JumpFallParam,	// JumpFall								: JumpFall Trigger 에서 필요한 정보를 받아 셋팅
		FellowRide,		// FellowRide [Uid]						: 대상에게 탑승하도록 설정

		CamPlayDirector,// CamPlayDirector [대상Uid] [BP명] 	: 카메라 액션 재생 ( PlayDirector )
		CamBaseRecord,	// CamBaseRecord [ 대상Uid ] [Tid] [ 시간 ]	: 기본 카메라 변경 ( SetViewTarget )

		SetForcedRotation,	// SetForcedRotation [대상Uid] [Pitch] [Roll] [ 시간 ] : 강제 회전값 설정

		PauseAI,		// PauseAI [Uid]							: AI 작동 멈춤
		ResumeAI,		// ResumeAI [Uid]							: AI 작동 재개

		PlayMatinee,	// PlayMatinee [MatineeName] [BossTid] [StartTime] [EndTime] : 마티네 재생
		SpawnEmitter,	// SpawnEmitter [EmitterPath] [AutoDestroy]	: 이펙트 스폰

		AreaCamera,		// AreaCamera [ 카메라이름 ] [ 블랜드시간 ] [ 블랜드타입 ] [ 추적타입 ]
		AreaCameraTraceValue,	// AreaCamera [Yaw] [Pitch] [Roll] [distance] 
		MoviePlay,				// MoviePlay

		OpenChatBubble,		// OpenChatBubble [EWRCharacterType] [CharacterUid] [LocalStringid] : 말풍선 채팅 추가 / 변경
		CloseChatBubble,	// CloseChatBubble [EWRCharacterType] [CharacterTid] : 말풍선 채팅 삭제

		SetFaction,		// SetFaction [대상Uid] [EWRCharacterFactionType] : 대상의 Faction 을 변경함

		AddFlyHeightMin,	// AddFlyHeightMin [대상Uid] [추가값] : 대상의 FlyHeightMin 값을 추가함 ( 해당맵에서만 임의 사용 )

		LoadActors4Hide,	// LoadActors4Hide [eCharType]	: 액터 종류별 숨기기 저장한 애들 보이기
		SaveActors4Hide,	// SaveActors4Hide [eCharType]	: 액터 종류별 숨기기 및 저장하기

		StartQuest,		// StartQuest [Id]		: Quest 시작
		CompleteQuest,	// CompleteQuest [Id]	: Quest 완료
		StartQuestTime, // StartQuest			: QuestTime 시작
		StopQuestTime, // StartQuest			: QuestTime 멈춤

		AmIHere,			// AmIHere					: 파티원 position send
		QuestReady,			// QuestReady 				: 파티원 퀘스트 종료 대기 : 서버로 퀘스트 시작
		PartyReady,			// PartyReady [Tid]			: 파티원 입장 대기 : 모든 파티원 입장시, 트리거 발동
		PartyMove2Me,		// PartyMove2Me				: 파티원 위치 이동 ( 나에게 ) : 승리포즈를 위하여
		PartyPlayAnim,		// PartyPlayAnim			: 파티원 애니메이션 재생

		Tutorial,			// Tutorial [Id]	: 튜토리얼 시작
		CheckOneWay,		// CheckOneWay [Uid] : 내 플레이어와 방향이 맞는지 체크하여, show/hide 및 collier on/off 적용 (IO 일때에만 작동 )	

		ChangeTutorialState,// ChgTutorialState [EWRTutorialState::en( int형 )] : 튜토리얼 상태를 변경 ( 서버로 전송 )..클래스명이 32자를 넘어가질 못해 어쩔 수 없이 chg로 씀

		ShowDialogue, // ShowDialogue [id][Next Trigger] : Dialogue 출력

		GotoMap,        // GotoMap [MapTid] [StartPositionIndex] [GotoNow] : 해당 맵으로 이동
		GotoPos,        // GotoPos [X] [Y] [Z]          : 해당 위치로 이동

		SetActorRotation,	// SetActorRotation [tid] [Roll][Pitch][Yaw] : 대상을 강제로 각도 조절

		SetStartIndex,		// SetStartIndex [value]        : Start Index

		PartyTrigger,		// PartyTrigger [tid]			: 해당 트리거를 파티장이 발동시킴!!
		AvoidanceOn,		// AvoidanceOn [UID]			: 대상의 Avoidance 를 on
		AvoidanceOff,		// AvoidanceOff [UID]			: 대상의 Avoidance 를 off

		CheckAirLine,		// CheckAirLine [UID]			: 대상 AirLine 을 방향 판단 기준으로 삼음

		MAX,			// 트리거 추가시 항상 아래 BuildMap () 에도 추가! 그렇지 않으면 Draw Fail 발생!
	};
}



struct EWRTrgParamUnitType
{
	enum en
	{
		Int,
		String,
		Float,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Int, TEXT("Int"));
		InsertMap(String, TEXT("String"));
		InsertMap(Float, TEXT("Float"));
	}
};

struct EWRTrgParamType
{
	enum en
	{
		Int_1,      // 첫번째 인자
		String_1,
		Float_1,

		Int_2,      // 두번째 인자
		String_2,
		Float_2,

		Int_3,      // 세번째 인자
		String_3,
		Float_3,

		Int_4,      // 네번째 인자
		String_4,
		Float_4,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Int_1, TEXT("Int_1"));
		InsertMap(String_1, TEXT("String_1"));
		InsertMap(Float_1, TEXT("Float_1"));

		InsertMap(Int_2, TEXT("Int_2"));
		InsertMap(String_2, TEXT("String_2"));
		InsertMap(Float_2, TEXT("Float_2"));

		InsertMap(Int_3, TEXT("Int_3"));
		InsertMap(String_3, TEXT("String_3"));
		InsertMap(Float_3, TEXT("Float_3"));

		InsertMap(Int_4, TEXT("Int_4"));
		InsertMap(String_4, TEXT("String_4"));
		InsertMap(Float_4, TEXT("Float_4"));
	}
};


struct EWRSwitchType
{
	enum en
	{
		Maintain,
		Activate,
		Deactivate,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Maintain, TEXT("Maintain"));
		InsertMap(Activate, TEXT("Activate"));
		InsertMap(Deactivate, TEXT("Deactivate"));
	}
};


// struct EWRUserInputType
// {
// 	enum en
// 	{
// 		All,
// 		Move,
// 		Attack,
// 		SelectTarget,
// 
// 		MAX,
// 	};
// 
// 	DeclareMapTable();
// 	static inline void BuildMap()
// 	{
// 		InsertMap(All, TEXT("All"));
// 		InsertMap(Move, TEXT("Move"));
// 		InsertMap(Attack, TEXT("Attack"));
// 		InsertMap(SelectTarget, TEXT("SelectTarget"));
// 	}
// };

UENUM(BlueprintType)
namespace EWRUserInputType
{
	enum en
	{
		All,
		Move,
		Attack,
		SelectTarget,

		MAX,
	};
}
namespace EWRUserInputType
{
	static const FString GetStr(const EWRUserInputType::en InEnum)
	{
		switch (InEnum)
		{
		case EWRUserInputType::All:				return TEXT("All");
		case EWRUserInputType::Move:			return TEXT("Move");
		case EWRUserInputType::Attack:			return TEXT("Attack");
		case EWRUserInputType::SelectTarget:	return TEXT("SelectTarget");
		default:								return TEXT("None");
		}
	}

	static const EWRUserInputType::en GetEnum(const FString& InEnumString)
	{
		for (EWRUserInputType::en It = EWRUserInputType::All; It < EWRUserInputType::MAX; It = (EWRUserInputType::en)(It + 1))
		{
			if (InEnumString.Equals(GetStr(It), ESearchCase::IgnoreCase))
				return It;
		}

		return EWRUserInputType::MAX;
	}
}

struct EWRTriggerTransitionType
{
	enum en
	{
		Enter,
		Exit,
		Active,
		DeActive,
		Etc,
		Loop,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Enter, TEXT("Enter"));
		InsertMap(Exit, TEXT("Exit"));
		InsertMap(Active, TEXT("Active"));
		InsertMap(DeActive, TEXT("DeActive"));
		InsertMap(Etc, TEXT("Etc"));
		InsertMap(Loop, TEXT("Loop"));
	}
};


struct EWRTriggerWatchType
{
	enum en
	{
		Trigger = 0,
		Actor,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Trigger, TEXT("Trigger"));
		InsertMap(Actor, TEXT("Actor"));
	}
};

UENUM(BlueprintType)
namespace EWRSpawnType
{
	enum en
	{
		STARTED,
		TRIGGERED,
		MAX
	};
}

//TestMapTable(EWRSpawnTypeFinder, EWRSpawnType);

UENUM(BlueprintType)
namespace EWRRideType
{
	enum en
	{
		NONE = 0,
		COMMON_RIDE = 1,
		ACTION_RIDE
	};
}

namespace EWRTriggerNodeCategoryType
{
	enum Type
	{
		Trigger = 0,
		Transition,
		Action,
		Special,

		MAX,
	};
}



UENUM(BlueprintType)
namespace EWRTransitionNodeType
{
	enum Type
	{
		Active,
		Deactive,
		Enter,
		Exit,
		Converter,

		Max,
	};
}

UENUM(BlueprintType)
namespace EWRTriggerUseType
{
	enum Type
	{
		None,
		Client,
		Server,
		ClientAndServer,
		Max,
	};
}

 namespace EWRTransitionNodeType
 {
 	static EWRTriggerNodeType::en GetWRTriggerNodeType(EWRTransitionNodeType::Type InEnum)
 	{
 		return static_cast<EWRTriggerNodeType::en>(InEnum + EWRTriggerNodeType::TransitionNodeStart + 1);
 	}
 }



namespace EWRTriggerUidType
{
	enum Type
	{
		Trigger = 0,
		SpawnIo,
		SpawnNpc,
		SpawnEnpc,
		SpawnPortal,
		UnSpawn,
		SpawnSo,
		SpawnAo,
		SpawnActionProp,
		SpawnBrick,
		SpawnElevator,
		SpawnFloatingBoard,
		SpawnTardis,
		SpawnSite,
		RandomSpawnNpc,
		MAX,
	};

	static inline int GetTriggerUidStartValue(Type e)
	{
		switch (e)
		{
		case EWRTriggerUidType::Trigger:
			return EWRTriggerUidStartValue::en::Trigger;
 		case EWRTriggerUidType::SpawnIo:
 			return EWRTriggerUidStartValue::en::SpawnIo;
		case EWRTriggerUidType::SpawnNpc:
			return EWRTriggerUidStartValue::en::SpawnNpc;
 		case EWRTriggerUidType::SpawnEnpc:
 			return EWRTriggerUidStartValue::en::SpawnEnpc;
 		case EWRTriggerUidType::SpawnPortal:
 			return EWRTriggerUidStartValue::en::SpawnPortal;
 		case EWRTriggerUidType::UnSpawn:
 			return EWRTriggerUidStartValue::en::UnSpawn;
 		case EWRTriggerUidType::SpawnSo:
 			return EWRTriggerUidStartValue::en::SpawnSo;
		case EWRTriggerUidType::SpawnAo:
			return EWRTriggerUidStartValue::en::SpawnAo;
		case EWRTriggerUidType::SpawnActionProp:
			return EWRTriggerUidStartValue::en::SpawnActionProp;
		case EWRTriggerUidType::SpawnBrick:
			return EWRTriggerUidStartValue::en::SpawnBrick;
		case EWRTriggerUidType::SpawnElevator:
			return EWRTriggerUidStartValue::en::SpawnElevator;
		case EWRTriggerUidType::SpawnFloatingBoard:
			return EWRTriggerUidStartValue::en::SpawnFloatingBoard;
		case EWRTriggerUidType::SpawnTardis:
			return EWRTriggerUidStartValue::en::SpawnTardis;
		case EWRTriggerUidType::SpawnSite:
			return EWRTriggerUidStartValue::en::SpawnSite;
		case EWRTriggerUidType::RandomSpawnNpc:
			return EWRTriggerUidStartValue::en::RandomSpawnNpc;

		default:
			return 0;
		}
	}

}



struct EWRTriggerMainType
{
	enum en
	{
		None,
		Normal,
		Etc,
		Gather,
		Buff,
		Recovery_Hp_Add,
		Recovery_Hp_Per,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, TEXT("None"));
		InsertMap(Normal, TEXT("normal"));
		InsertMap(Etc, TEXT("etc"));
		InsertMap(Gather, TEXT("gather"));
		InsertMap(Buff, TEXT("buff"));
		InsertMap(Recovery_Hp_Add, TEXT("recovery_hp_add"));
		InsertMap(Recovery_Hp_Per, TEXT("recovery_hp_per"));
	}
};

UENUM(BlueprintType)
namespace EWRSpawnObjectType
{
	enum en
	{
		None = 0,			// 초기값
		TriggerObject = 1,			// 트리거 관련 Object
		Brick = 2,			// 방벽
		MAX
	};
}
namespace EWRSpawnObjectType
{
	static const FString GetStr(const EWRSpawnObjectType::en InEnum)
	{
		switch (InEnum)
		{
		case EWRSpawnObjectType::None:			return TEXT("None");
		case EWRSpawnObjectType::TriggerObject:	return TEXT("TriggerObject");
		case EWRSpawnObjectType::Brick:			return TEXT("Brick");
		default:							return TEXT("None");
		}
	}

	static const EWRSpawnObjectType::en GetEnum(const FString& InEnumString)
	{
		for (EWRSpawnObjectType::en It = EWRSpawnObjectType::None; It < EWRSpawnObjectType::MAX; It = (EWRSpawnObjectType::en)(It + 1))
		{
			if (InEnumString.Equals(GetStr(It), ESearchCase::IgnoreCase))
				return It;
		}

		return EWRSpawnObjectType::MAX;
	}
}

UENUM(BlueprintType)
namespace EWRArrivalFellowSummonType
{
	enum en
	{
		None = 0,			//초기값

		RidingFellowUnSummon = 1,			// 탑승 펠로우 소환 해제
		RidingFellowSummon = 2,			// 탑승 펠로우 소환

		MAX
	};
}
namespace EWRArrivalFellowSummonType
{
	static const FString GetStr(const EWRArrivalFellowSummonType::en InEnum)
	{
		switch (InEnum)
		{
		case EWRArrivalFellowSummonType::None:					return TEXT("None");
		case EWRArrivalFellowSummonType::RidingFellowUnSummon:	return TEXT("RidingFellowUnSummon");
		case EWRArrivalFellowSummonType::RidingFellowSummon:	return TEXT("RidingFellowSummon");
		default:												return TEXT("None");
		}
	}

	static const EWRArrivalFellowSummonType::en GetEnum(const FString& InEnumString)
	{
		for (EWRArrivalFellowSummonType::en It = EWRArrivalFellowSummonType::None; It < EWRArrivalFellowSummonType::MAX; It = (EWRArrivalFellowSummonType::en)(It + 1))
		{
			if (InEnumString.Equals(GetStr(It), ESearchCase::IgnoreCase))
				return It;
		}

		return EWRArrivalFellowSummonType::MAX;
	}
}

UENUM(BlueprintType)
namespace EWRDungeonClearGradeByTrigger
{
	enum en
	{
		None,
		B_Grade,
		A_Grade,
		S_Grade,
		
		MAX,
	};
}
namespace EWRDungeonClearGradeByTrigger
{
	static const FString GetStr(const EWRDungeonClearGradeByTrigger::en InEnum)
	{
		switch (InEnum)
		{
		case EWRDungeonClearGradeByTrigger::None:						return TEXT("None");
		case EWRDungeonClearGradeByTrigger::S_Grade:					return TEXT("S");
		case EWRDungeonClearGradeByTrigger::A_Grade:					return TEXT("A");
		case EWRDungeonClearGradeByTrigger::B_Grade:					return TEXT("B");
		
		default:									return TEXT("none");
		}
	}

	static const EWRDungeonClearGradeByTrigger::en GetEnum(const FString& InEnumString)
	{
		for (EWRDungeonClearGradeByTrigger::en It = EWRDungeonClearGradeByTrigger::None; It < EWRDungeonClearGradeByTrigger::MAX; It = (EWRDungeonClearGradeByTrigger::en)(It + 1))
		{
			if (InEnumString.Equals(GetStr(It), ESearchCase::IgnoreCase))
				return It;
		}

		return EWRDungeonClearGradeByTrigger::MAX;
	}
}

UENUM(BlueprintType)
namespace EWRTriggerConditionType
{
	enum en
	{
		None,
		CharacterClass,
		Realm,
		Guild,
		Quest,
		Level,
		Item,

		MAX,
	};
}
namespace EWRTriggerConditionType
{
	static const FString GetStr(const EWRTriggerConditionType::en InEnum)
	{
		switch (InEnum)
		{
		case EWRTriggerConditionType::None:						return TEXT("none");
		case EWRTriggerConditionType::CharacterClass:				return TEXT("characterclass");
		case EWRTriggerConditionType::Realm:						return TEXT("realm");
		case EWRTriggerConditionType::Guild:						return TEXT("guild");
		case EWRTriggerConditionType::Quest:						return TEXT("quest");
		case EWRTriggerConditionType::Level:						return TEXT("level");
		case EWRTriggerConditionType::Item:						return TEXT("item");

		default:											return TEXT("none");
		}
	}

	static const EWRTriggerConditionType::en GetEnum(const FString& InEnumString)
	{
		for (EWRTriggerConditionType::en It = EWRTriggerConditionType::None; It < EWRTriggerConditionType::MAX; It = (EWRTriggerConditionType::en)(It + 1))
		{
			if (InEnumString.Equals(GetStr(It), ESearchCase::IgnoreCase))
				return It;
		}

		return EWRTriggerConditionType::MAX;
	}
}