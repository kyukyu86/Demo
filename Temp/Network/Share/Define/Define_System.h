//******************************************************************************
//	FileName: 	Define_System.h
//------------------------------------------------------------------------------
//	Created	:	2019/08/26
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	���� / Ŭ�� ���� define ����
//******************************************************************************


#pragma once


//-----------------------------------------------------
//	Math 
//-----------------------------------------------------
#define SHAPE_RADIUS							0.25f
#define UNIT_FACTOR								(1.0f)
#define TO_GAME_COORD(X)						((X)/UNIT_FACTOR+0.0002f)	//������ �Ǽ����� ������ �����ϱ� ���Ͽ� 0.0002f �� ����
#define TO_PATH_COORD(X)						((long)((X)*UNIT_FACTOR))
#define DUPLEX_FLOOR_HEIGHT_LIMIT				(5.0f)						//���� �Ǵ� ����


//-----------------------------------------------------
//	���� �� �ֿ� ���� ȯ��
//-----------------------------------------------------
#define	WXR_PER_FRACTION						10000					//	���� �� ȯ�� ���� (������ - ��뿹 - stat)
#define	WXR_PER_MILLION							1000000					//	�鸸����
#define	WXR_PER_TEN_MILLION						10000000				//	õ������

#define	WXR_SEC									1000					//	��
#define	WXR_SEC_F								1000.0f					//	��


//-----------------------------------------------------
//	Ÿ�̸� �ð�
//-----------------------------------------------------
#define PERIOD_TIMER_ACTOR_MAIN					10			//	Actor ����
#define PERIOD_TIMER_ACTOR_ACTIVE				1000		//	Actor Ȱ��ȭ ��ü

#define PERIOD_TIMER_NATURAL_HEALING			2000		//	HP �ڿ� ġ��

#define PERIOD_TIMER_MAP_MAIN					10			//	MAP ����
#define PERIOD_TIMER_MAP_ACTIVE					1000		//	MAP Ȱ��ȭ ��ü

#define PERIOD_TIMER_SYNC_REGIST_GAMESERVER		180000		//	���Ӽ��� ��� ����ȭ �˸� Ÿ�̸�

#define CHEAT_ACTOR_DESTROY_TIME				600000		//	ġƮŰ�� ������ NPC �Ҹ� �ð�

#define PERIOD_INFINITE_TOWER_PLAY				180000		//	������ ž �÷��� �ð� ( 3�� )
#define	RESET_HOUR_BUY_SHOP_ITEM				4			//	���� 4�ÿ� ���� ������ ����Ƚ�� �ʱ�ȭ
#define DAY_OF_WEEK								7			//	�� ���� ��¥ ��
#define RESET_DAY_OF_WEEK_BUY_SHOP_ITEM			2			//	���� ������ ����Ƚ�� �ʱ�ȭ ����(�� �� ������ ��� ���)- COleDateTime : �Ͽ��� = 1, ������ = 2, ...

#define	WAITING_TIME_FOR_RESERVED_USER			180000		//	��� ������ ��ٸ��� �ð� (3��)
#define	WAITING_TIME_FOR_CLOSE_USER				20000		//	��� ������ ���� ������ ���� ���Ḧ ��ٸ��� �ð� (20��)
#define	WAITING_TIME_FOR_DISCONNECT_USER		180000		//	���� ���� ���� ��ٸ��� �ð� (3��)


//-----------------------------------------------------
//	Table Data ����
//-----------------------------------------------------
#define CHARDATA_NAME_SIZE						32
#define CHARDATA_TABLE_NAME_SIZE				64
#define CHARDATA_PATH_SIZE						256

#define MAX_RANDOM_POS_ARR						6			//	ENPC/IO/SO �ִ� ���� ������ �迭 ����


//-----------------------------------------------------
//	��
//-----------------------------------------------------
#define MAX_MAP_START_POS_BASE					5					//	�� ������ġ ���� ����
#define MAX_MAP_START_POS_FIELD					5					//	
#define MAX_MAP_START_POS_BATTLEFIELD			6					//	
#define MAX_MAP_START_POS_STAGE					1					//	
#define MAX_MAP_START_POS_TOWN					1					//	
#define MAX_MAP_START_POS_DAILY					1					//	
#define MAX_MAP_START_POS_RAID					1					//	
#define MAX_MAP_START_POS_ALL					10					//	
#define MAX_MAP_REVIVE_BUFF_NUM					3

