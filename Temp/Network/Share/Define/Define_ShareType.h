//******************************************************************************
//	FileName: 	Define_ShareType.h
//------------------------------------------------------------------------------
//	Created	:	2019/6/25
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	�ڷ��� ��ȯ�� ����
//				...SN - DB ���� ������ Unique key
//				...ID - �ܺο��� ������ Unique key
//						, ���� �⵿�� Unique key (������ ��⵿�Ǹ� �ٲ�� ���)
//				...Index - ������ key (key�� �ش��ϴ� ���� ���� �����ϸ� �� ���� ����� ���ɼ� ����)
//******************************************************************************
//	Advice :	* �Ʒ� �̻� �ڷ��� �߰����� �����ּ���. (�߰��� ��Ʈ�� ��� �޾��ּ���)
//				* ���⿡�� Ű���̳� ���� �� �߿� �׸�鿡 ���ؼ��� �����մϴ�.
//				* �̿� �׸���� ���� ��ȯ���� �⺻ �ڷ��� ����ϼ���.
//				* ī��Ʈ���� INVALID_ �� ������� �ʽ��ϴ�. (0���� �����մϴ�.)
//				* INVALID_ ���� �ش� ������ ���κп��� INVALID_ �˻簡 ���� �մϴ�.
//				* INVALID_ �̿� define ����� �ڿ� �� ���� �ǹ̸� �ּ����� �޾��ּ���.
//******************************************************************************


#pragma once


//-----------------------------------------------------
//	Ŭ���̾�Ʈ(�𸮾�) �ڷ����� ���� ����ϵ��� �ϱ� ����
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
