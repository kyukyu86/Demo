//******************************************************************************
//	FileName: 	Define_System.h
//------------------------------------------------------------------------------
//	Created	:	2019/08/26
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	서버 / 클라 공통 define 모음
//******************************************************************************


#pragma once


//-----------------------------------------------------
//	Math 
//-----------------------------------------------------
#define SHAPE_RADIUS							0.25f
#define UNIT_FACTOR								(1.0f)
#define TO_GAME_COORD(X)						((X)/UNIT_FACTOR+0.0002f)	//정수와 실수간의 오차를 보정하기 위하여 0.0002f 를 더함
#define TO_PATH_COORD(X)						((long)((X)*UNIT_FACTOR))
#define DUPLEX_FLOOR_HEIGHT_LIMIT				(5.0f)						//복층 판단 높이


//-----------------------------------------------------
//	게임 내 주요 단위 환산
//-----------------------------------------------------
#define	WXR_PER_FRACTION						10000					//	게임 내 환산 단위 (만분율 - 사용예 - stat)
#define	WXR_PER_MILLION							1000000					//	백만분율
#define	WXR_PER_TEN_MILLION						10000000				//	천만분율

#define	WXR_SEC									1000					//	초
#define	WXR_SEC_F								1000.0f					//	초


//-----------------------------------------------------
//	타이머 시간
//-----------------------------------------------------
#define PERIOD_TIMER_ACTOR_MAIN					10			//	Actor 공통
#define PERIOD_TIMER_ACTOR_ACTIVE				1000		//	Actor 활성화 객체

#define PERIOD_TIMER_NATURAL_HEALING			2000		//	HP 자연 치유

#define PERIOD_TIMER_MAP_MAIN					10			//	MAP 공통
#define PERIOD_TIMER_MAP_ACTIVE					1000		//	MAP 활성화 객체

#define PERIOD_TIMER_SYNC_REGIST_GAMESERVER		180000		//	게임서버 등록 동기화 알림 타이머

#define CHEAT_ACTOR_DESTROY_TIME				600000		//	치트키로 생성된 NPC 소멸 시간

#define PERIOD_INFINITE_TOWER_PLAY				180000		//	무한의 탑 플레이 시간 ( 3분 )
#define	RESET_HOUR_BUY_SHOP_ITEM				4			//	새벽 4시에 상점 아이템 구매횟수 초기화
#define DAY_OF_WEEK								7			//	한 주의 날짜 수
#define RESET_DAY_OF_WEEK_BUY_SHOP_ITEM			2			//	상점 아이템 구매횟수 초기화 요일(한 주 기준일 경우 사용)- COleDateTime : 일요일 = 1, 월요일 = 2, ...

#define	WAITING_TIME_FOR_RESERVED_USER			180000		//	대기 유저가 기다리는 시간 (3분)
#define	WAITING_TIME_FOR_CLOSE_USER				20000		//	대기 유저가 기존 유저가 접속 종료를 기다리는 시간 (20초)
#define	WAITING_TIME_FOR_DISCONNECT_USER		180000		//	접속 끊긴 유저 기다리는 시간 (3분)


//-----------------------------------------------------
//	Table Data 관련
//-----------------------------------------------------
#define CHARDATA_NAME_SIZE						32
#define CHARDATA_TABLE_NAME_SIZE				64
#define CHARDATA_PATH_SIZE						256

#define MAX_RANDOM_POS_ARR						6			//	ENPC/IO/SO 최대 랜덤 포지션 배열 개수


//-----------------------------------------------------
//	맵
//-----------------------------------------------------
#define MAX_MAP_START_POS_BASE					5					//	맵 시작위치 정보 개수
#define MAX_MAP_START_POS_FIELD					5					//	
#define MAX_MAP_START_POS_BATTLEFIELD			6					//	
#define MAX_MAP_START_POS_STAGE					1					//	
#define MAX_MAP_START_POS_TOWN					1					//	
#define MAX_MAP_START_POS_DAILY					1					//	
#define MAX_MAP_START_POS_RAID					1					//	
#define MAX_MAP_START_POS_ALL					10					//	
#define MAX_MAP_REVIVE_BUFF_NUM					3

