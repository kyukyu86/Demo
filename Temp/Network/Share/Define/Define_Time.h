//******************************************************************************
//	FileName: 	Define_Time.h
//------------------------------------------------------------------------------
//	Created	:	2019/6/25
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeIO Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	시간관리용
//******************************************************************************


#pragma once
#include "../Protocol/IMMessage.h"


//-----------------------------------------------------
//	시간 관리용 
//-----------------------------------------------------
#define YEAR_BY_SEC					31104000	//	초단위로 표현한 값 - 1년 (30일)
#define MONTH_BY_SEC				2592000		//	초단위로 표현한 값 - 1개월 (30일)
#define WEEK_BY_SEC					604800		//	초단위로 표현한 값 - 1주   
#define DAY_BY_SEC					86400		//	초단위로 표현한 값 - 1일   
#define HOUR_BY_SEC					3600		//	초단위로 표현한 값 - 1시간 
#define MIN_BY_SEC					60			//	초단위로 표현한 값 - 1분   

#define	MIN_VALID_YEAR				1601		//	시간 관리용
#define	MAX_VALID_YEAR				30827		//	시간 관리용


//게임내 요일 (COleDateTime::GetDayOfWeek() == EWRDayOfTheWeek::en+1)
struct EWRDayOfTheWeek
{
	enum en
	{
		eSunday		= 0,
		eMonday		,
		eTuesday	,
		eWednesday	,
		eThursday	,
		eFriday		,
		eSaturday	,
		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( eSunday,			L"sun",		L"Sunday" );
		InsertMap( eMonday,			L"mon",		L"Monday" );
		InsertMap( eTuesday,		L"tue",		L"Tuesday" );
		InsertMap( eWednesday,		L"wed",		L"Wednesday" );
		InsertMap( eThursday,		L"thu",		L"Thursday" );
		InsertMap( eFriday,			L"fri",		L"Friday" );
		InsertMap( eSaturday,		L"sat",		L"Saturday" );
	}

#ifdef USE_CLIENT_ONLY
	static EWRDayOfTheWeek::en ConvertToDayOfWeek(EDayOfWeek Day)
	{
		EWRDayOfTheWeek::en DayOfWeek = EWRDayOfTheWeek::MAX;
		switch (Day)
		{
		case EDayOfWeek::Monday:	DayOfWeek = EWRDayOfTheWeek::eMonday;		break;
		case EDayOfWeek::Tuesday:	DayOfWeek = EWRDayOfTheWeek::eTuesday;		break;
		case EDayOfWeek::Wednesday:	DayOfWeek = EWRDayOfTheWeek::eWednesday;	break;
		case EDayOfWeek::Thursday:	DayOfWeek = EWRDayOfTheWeek::eThursday;		break;
		case EDayOfWeek::Friday:	DayOfWeek = EWRDayOfTheWeek::eFriday;		break;
		case EDayOfWeek::Saturday:	DayOfWeek = EWRDayOfTheWeek::eSaturday;		break;
		case EDayOfWeek::Sunday:	DayOfWeek = EWRDayOfTheWeek::eSunday;		break;
		}

		return DayOfWeek;
	}
#endif //USE_CLIENT_ONLY
};


//시간 구조체
struct stDateTime : IMSerialize
{
	uint16	year;
	uint16	month;
	uint16	day;
	uint16	hour;
	uint16	minute;
	uint16	second;
	uint16	milliSecond;

	stDateTime()
		: year(0)
		, month(0)
		, day(0)
		, hour(0)
		, minute(0)
		, second(0)
		, milliSecond(0)
	{}

	void Clear() 
	{
		year		= 0;
		month		= 0; 
		day			= 0;
		hour		= 0;
		minute		= 0;
		second		= 0;
		milliSecond = 0;
	}

	void Copy(stDateTime* pDateTime)
	{
		if (pDateTime == nullptr)
			return;

		year		= pDateTime->year;
		month		= pDateTime->month;
		day			= pDateTime->day;
		hour		= pDateTime->hour;
		minute		= pDateTime->minute;
		second		= pDateTime->second;
		milliSecond = pDateTime->milliSecond;
	}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> year >> month >> day >> hour >> minute >> second >> milliSecond;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << year << month << day << hour << minute << second << milliSecond;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"year : %u, month : %u, day : %u", year, month, day);
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"hour : %u, minute : %u, second : %u, milliSecond : %u", hour, minute, second, milliSecond);
	}
#endif	//  USE_PROTOCOL_TOSTRING
};



//시간 구조체 : 간소화형(7byte)
struct stSmallDateTime : IMSerialize
{
	uint16	year;
	uint8	month;
	uint8	day;
	uint8	hour;
	uint8	minute;
	uint8	second;

	stSmallDateTime()
		: year(0)
		, month(0)
		, day(0)
		, hour(0)
		, minute(0)
		, second(0)
	{}

	void Clear()
	{
		year	= 0;
		month	= 0;
		day		= 0;
		hour	= 0;
		minute	= 0;
		second	= 0;
	}

	void Copy(stSmallDateTime* pSmallDateTime)
	{
		if (nullptr == pSmallDateTime)
			return;

		year	= pSmallDateTime->year;
		month	= pSmallDateTime->month;
		day		= pSmallDateTime->day;
		hour	= pSmallDateTime->hour;
		minute	= pSmallDateTime->minute;
		second	= pSmallDateTime->second;
	}

	void CopyFromDateTime(stDateTime* pDateTime)
	{
		if (nullptr == pDateTime)
			return;

		year	= (uint16)pDateTime->year;
		month	= (uint8)pDateTime->month;
		day		= (uint8)pDateTime->day;
		hour	= (uint8)pDateTime->hour;
		minute	= (uint8)pDateTime->minute;
		second	= (uint8)pDateTime->second;
	}

	//	대략적인 검사용으로 1주일 이내만 체크. 이상 시간에는 계산 오류 발생
	//	true : 해당 시간만큼 경과함
	const bool CheckElapsedTime( IN stSmallDateTime& rCheckSmallDateTime, const int32 checkSec )
	{
		if( ( WEEK_BY_SEC < checkSec ) || ( 0 >= checkSec ) )
		{
			//	1주이내까지만 체크
			return true;
		}

		int32 temp_Year		= ( rCheckSmallDateTime.year - year ) * YEAR_BY_SEC;
		int32 temp_month	= ( rCheckSmallDateTime.month - month ) * MONTH_BY_SEC;
		int32 temp_day		= ( rCheckSmallDateTime.day - day ) * DAY_BY_SEC;
		int32 temp_hour		= ( rCheckSmallDateTime.hour - hour ) * HOUR_BY_SEC;
		int32 temp_minute	= ( rCheckSmallDateTime.minute - minute ) * MIN_BY_SEC;
		int32 temp_second	= ( rCheckSmallDateTime.second - second );
		int32 totalTime		= temp_Year + temp_month + temp_day + temp_hour + temp_minute + temp_second;

		if ( checkSec > totalTime )
			return false;

		return true;
	}
#ifdef USE_SERVER_ONLY
	const bool SetupSmalldateTime_From_Systemtime( const SYSTEMTIME& inSystemTime )
	{
		//얻어온 값이 유효한 범위인지 체크
		if ( inSystemTime.wYear <= MIN_VALID_YEAR || inSystemTime.wYear >= MAX_VALID_YEAR )
			return false;

		year	= inSystemTime.wYear;
		month	= (uint8)inSystemTime.wMonth;
		day		= (uint8)inSystemTime.wDay;
		hour	= (uint8)inSystemTime.wHour;
		minute	= (uint8)inSystemTime.wMinute;
		second	= (uint8)inSystemTime.wSecond;

		return true;
	}

	const bool SetupCOleDateTime_From_SmalldateTime( OUT COleDateTime& rOutCOleDateTime )
	{
		rOutCOleDateTime.SetDateTime( year, month, day, hour, minute, second );
		return true;
	}

	const bool SetupSmalldateTime_From_COleDateTime( const COleDateTime& inCOleDateTime )
	{
		SYSTEMTIME systemTime;
		inCOleDateTime.GetAsSystemTime( systemTime );

		return SetupSmalldateTime_From_Systemtime( systemTime );
	}

	const bool SetupCurTime()
	{
		return SetupSmalldateTime_From_COleDateTime( COleDateTime::GetCurrentTime() );
	}
#endif //USE_SERVER_ONLY

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> year >> month >> day >> hour >> minute >> second;
	}

	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << year << month << day << hour << minute << second;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"year : %u, month : %u, day : %u", year, month, day);
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"hour : %u, minute : %u, second : %u", hour, minute, second);
	}
#endif	//  USE_PROTOCOL_TOSTRING

	bool IsValid() const
	{
#ifdef USE_CLIENT_ONLY

		if (year == 0 && month == 0 && day == 0)
		{
	#if UE_BUILD_DEBUG
			ensureMsgf(false, TEXT("stSmallDateTime is Ensure"));
	#endif
			return false;
		}
		else
			return true;
#else
		return true;
#endif //USE_CLIENT_ONLY
	}
};

//날짜 정보 구조체
struct stDate : IMSerialize
{
	uint16	year;
	uint8	month;
	uint8	day;

	stDate()
		: year(0)
		, month(0)
		, day(0)
	{}
	stDate(uint16 _year, uint8 _month, uint8 _day)
		: year(_year)
		, month(_month)
		, day(_day)
	{}

	void Clear()
	{
		year	= 0;
		month	= 0;
		day		= 0;
	}

	void Copy(stDate* pScr)
	{
		year	= pScr->year;
		month	= pScr->month;
		day		= pScr->day;
	}

	bool IsAfter(stDate& rDate)
	{
		if(year > rDate.year)
		{
			return true;
		}
		else if(year == rDate.year)
		{
			if(month > rDate.month)
			{
				return true;
			}
			else if(month == rDate.month)
			{
				if (day > rDate.day)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool IsEqual(stDate& rDate)
	{
		if (year == rDate.year && month == rDate.month && day == rDate.day)
			return true;

		return false;
	}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> year >> month >> day;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << year << month << day;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"year : %u, month : %u, day : %u", year, month, day);
	}
#endif	//  USE_PROTOCOL_TOSTRING
};
