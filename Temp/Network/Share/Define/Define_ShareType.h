//******************************************************************************
//	FileName: 	Define_ShareType.h
//------------------------------------------------------------------------------
//	Created	:	2019/6/25
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	자료형 변환명 정의
//				...SN - DB 에서 생성된 Unique key
//				...ID - 외부에서 생성된 Unique key
//						, 서버 기동시 Unique key (서버가 재기동되면 바뀌는 경우)
//				...Index - 관리용 key (key에 해당하는 대응 값이 존재하며 이 값이 변경될 가능성 있음)
//******************************************************************************
//	Advice :	* 아래 이상 자료형 추가하지 말아주세요. (추가시 파트장 허락 받아주세요)
//				* 여기에는 키값이나 게임 내 중요 항목들에 대해서만 관리합니다.
//				* 이외 항목들은 굳이 변환없이 기본 자료형 사용하세요.
//				* 카운트형은 INVALID_ 를 사용하지 않습니다. (0부터 시작합니다.)
//				* INVALID_ 사용시 해당 변수는 사용부분에서 INVALID_ 검사가 들어가야 합니다.
//				* INVALID_ 이외 define 상수는 뒤에 이 값의 의미를 주석으로 달아주세요.
//******************************************************************************


#pragma once


//-----------------------------------------------------
//	클라이언트(언리얼) 자료형을 공통 사용하도록 하기 위함
//-----------------------------------------------------
#ifdef	USE_SERVER_ONLY
typedef unsigned __int8						uint8;
typedef unsigned __int16					uint16;
typedef unsigned __int32					uint32;
typedef unsigned __int64					uint64;

typedef signed __int8						int8;
typedef signed __int16						int16;
typedef signed __int32						int32;
typedef signed __int64						int64;
#endif	//	USE_SERVER_ONLY

#ifdef	USE_PACKET_SAMPLE
typedef unsigned __int8						uint8;
typedef unsigned __int16					uint16;
typedef unsigned __int32					uint32;
typedef unsigned __int64					uint64;

typedef signed __int8						int8;
typedef signed __int16						int16;
typedef signed __int32						int32;
typedef signed __int64						int64;
#endif	//	USE_PACKET_SAMPLE
