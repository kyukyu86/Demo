//******************************************************************************
//	FileName: 	Define_Type.h
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

#define _USE_MATH_DEFINES

#include "Define_ShareType.h"
#include <math.h>


//-----------------------------------------------------
//	네트워크 및 공통
//-----------------------------------------------------
typedef		int32							SERVER_INDEX_t;			// 서버 인덱스
#define		INVALID_SERVER_INDEX			-1
#define		MOVING_WORLD_TEMP_INDEX			-2

typedef		int32							LTID_t;					// Local Thread Index
#define		INVALID_LTID					(LTID_t)(-1)

typedef		uint32							WSN_t;					// World Serial Number (서버군)
#define		INVALID_WSN						0

typedef		std::wstring					WORLDNAME_t;			// World Name
#define		INVALID_WORLDNAME				L"*"

typedef		uint32							SERVER_PROTOCOL_VER_t;	// User Protocol Version
#define		INVALID_SERVER_PROTOCOL_VERSION	0						// ICM_SERVER_PROTOCOL_VERSION	

typedef		uint32							UINTIP_t;				// IP (unsigned int address)
#define		INVALID_UINTIP					0

typedef		std::wstring					IP_t;					// IP (string address)
#define		INVALID_IP						L"*"

typedef		std::wstring					HOST_t;					// Host 
#define		INVALID_HOST					L"*"

typedef		uint16							PORT_t;					// Port
#define		INVALID_PORT					0

typedef		std::string						PATH_t;					//	경로
#define		INVALID_PATH					"*"

typedef		std::string						FILE_NAME_t;			//	파일 명
#define		INVALID_FILE_NAME				"*"

typedef		uint32							PROTOCOL_VERSION_t;		// User Protocol Version
#define		INVALID_PROTOCOL_VERSION		0						// WXR_PROTOCOL_VERSION	

typedef		std::wstring					ID_t;					// ID
#define		INVALID_ID						L"*"

typedef		uint32							DKEY_t;					// Dynamic Key
#define		INVALID_DKEY					0

typedef		int32							SOCKET_INDEX_t;			// Socket Index
#define		INVALID_SOCKET_INDEX			-1

typedef		uint32							RESULT_t;				// 패킷 결과
#define		INVALID_RESULT					0

typedef		uint16							FORCEKICK_t;			// 강제종료명령 타입
#define		INVALID_FORCEKICK				0


//-----------------------------------------------------
//	유저 / 기기
//-----------------------------------------------------
typedef		uint32							USN_t;					// User Serial Number ... User DBKEY (Lobby / Game)
#define		INVALID_USN						0

typedef		std::string						MAID_t;					// Mac Address ID
#define		INVALID_MAID					"*"

typedef		std::string						UDID_t;					// User Device ID
#define		INVALID_UDID					"*"

typedef		std::wstring					UUID_t;					//	User Unique Device ID - MDC 기능에서 키로 사용 (UDID_t와 다른 값)
#define		INVALID_UUID					L"*"					//	게스트도 이 값으로 처리됨

typedef     uint8							USER_GRADE_t;			//	계정 등급
#define     BASE_USER_GRADE					0						//	EWRUserGrade::User


//-----------------------------------------------------
//	인증 및 빌링
//-----------------------------------------------------

//	HTTP 통신
typedef		std::wstring					HTTP_URL_t;				// HTTP Request URL 타입
#define		INVALID_HTTP_URL				L"*"

typedef		std::wstring					HTTP_QUERY_t;			// HTTP Request Query 타입
#define		INVALID_HTTP_QUERY				L"*"

//	IAP 결과
typedef		int16							IAP_VERIFY_RESCODE_t;	// Verify 결과 코드
#define		INVALID_IAP_VERIFY_RESCODE		-1			
#define		SUCCESS_IAP_VERIFY_RESCODE		0						// Verify 결과 코드 성공

typedef		int16							IAP_VERIFY_RESULT_t;	// Verify 결과
#define		INVALID_IAP_VERIFY_RESULT		-1						

//	시스템 로그 관련
typedef		int32							EVENT_TYPE_t;			// EWREventType::en
#define		INVALID_EVENT_TYPE				-1								

typedef		std::string						COUPON_t;				// 쿠폰 값
#define		INVALID_COUPON					"*"

//	이용 약관
typedef		uint32							TERMS_VERSION_t;
#define		INVALID_TERMS_VERSION			0


//-----------------------------------------------------
//	게임 정보
//-----------------------------------------------------
typedef		uint32							PSN_t;					//	Player Serial Number ... Player DBKEY (Lobby / Game)
#define		INVALID_PSN						0
#define		GROUP_PSN						(uint32)(MAXLONG)		//	Player Group 을 의미함

typedef		uint64							AID_t;					//	Actor Serial ID ... Unique key
#define		INVALID_AID						(AID_t)0
#define		CLASSIFY_AID_UNIT				(AID_t)(100)			//	EIMCharacterType::MAX 보다 커야 함. Actor Type 구별용 단계 - 하위 00 자리를 구분용으로 사용
#define		MAX_AID_SRC						(AID_t)(MAXLONGLONG / CLASSIFY_AID_UNIT)	//	계산 원본용 최대값

typedef		uint32							ACTOR_TYPE_t;			//	Actor Type		//	EWRActorType
#define		INVALID_ACTOR_TYPE				99						//	무효 액터 유형

typedef		uint64							ISN_t;					// Item Serial Number ... Item DBKEY 
#define		INVALID_ISN						(ISN_t)0

typedef		int32							TID_t;					//	Table ID (각 테이블마다 고유) ... 즉, 유형별로 동일할 가능성 있음
#define		INVALID_TID						(TID_t)0

typedef		uint64							SKILL_SEQ_t;			//	스킬 사건 번호
#define		INVALID_SKILL_SEQ				(SKILL_SEQ_t)0

typedef		uint32							TIME32_t;				//	시간 GetTickCount()

typedef		uint64							MAIL_SN_t;				//	우편 시리얼 넘버
#define		INVALID_MAILSN					(MAIL_SN_t)0

typedef		uint32							GUILD_SN_t;				//	길드 시리얼 넘버
#define		INVALID_GUILD_SN				0

typedef		uint64							PARTY_ID_t;				//	Party Serial ID ... Unique key
#define		INVALID_PARTY_ID				(PARTY_ID_t)0

typedef		int16							LEVEL_t;				//	Level
#define		INVALID_LEVEL					0						//	레벨은 1부터 시작
#define		INITIALIZE_LEVEL				1						

typedef		int64							EXP_t;					//	경험치 (0부터 시작, 계산을 위하여 signed 형식 사용)

typedef		int64							CURRENCY_t;				//	통합적 재화 단위
#define		INVALID_CURRENCY				(CURRENCY_t)-1
#define		MAX_CURRENCY_VALUE				999999999				//	통합적 재화 최대치

typedef		int32							GAME_MONEY_t;			//	인게임 재화
#define		INVALID_GAME_MONEY				(GAME_MONEY_t)-1

typedef		int32							CASH_MONEY_t;			//	유료 재화
#define		INVALID_CASH_MONEY				(CASH_MONEY_t)-1

typedef		int32							POINT_t;				//	포인트 재화
#define		INVALID_POINT					(POINT_t)-1

typedef		int32							TICKET_t;				//	티켓 재화
#define		INVALID_TICKET					(TICKET_t)-1

typedef		std::wstring					NAME_t;					//	Name W... 기본적으로 이름 형식 사용시
#define		INVALID_NAME					L"*"

typedef		std::string						NAMEA_t;				//	Name A ... 기본적으로 이름 형식 사용시
#define		INVALID_NAMEA					"*"

typedef		int16							CHANNEL_INDEX_t;		//	맵 채널 인덱스
#define		INVALID_CHANNEL_INDEX			(CHANNEL_INDEX_t)(-1)
#define		MAX_CHANNEL_INDEX				30000					//	채널 최대치
#define		MIN_CHANNEL_INDEX				10						//	채널 최소치
#define		BOSS_CHANNEL_INDEX				30001					//	필드 레이드 채널 인덱스 (고정)
#define		DEFAULT_USER_NUM				300						//	최저 인원 수
#define		LAWLESS_CHANNEL_INDEX			1						//	무법 채널 인덱스 (고정)

typedef		uint32							MAP_INDEX_t;			//	Map 고유 인덱스 (월드 내)
#define		INVALID_MAP_INDEX				0						//	
#define		MAX_MAP_INDEX					(MAP_INDEX_t)(MAXLONG)	//	MAP_INDEX_t 최대값
#define		MAX_GEN_MAP_INDEX_COUNT			10000					//	Gen 회수

typedef		uint32							SECTOR_INDEX_t;			//	Sector 인덱스
#define		INVALID_SECTOR_INDEX			(SECTOR_INDEX_t)(-1)	//	
#define		MAX_SECTOR_SIZE					3000					//	최대 섹터 크기

typedef		uint32							SKILL_POINT_t;			//	스킬 포인트
typedef		int16							SKILL_LEVEL_t;			//	스킬 Level 

typedef		int32							ITEM_COUNT_t;			//	아이템 개수
typedef		uint16							ITEM_SLOT_COUNT_t;		//	아이템 슬롯 개수

typedef		int32							QUEST_TASK_VALUE_t;		//	퀘스트 태스크 진행 값
#define		INVALID_QUEST_TASK_VALUE		-1

typedef		uint16							BUFF_PRIORITY_t;			//  버프 우선순위
#define		DEFAULT_BUFF_PRIORITY			0

typedef		uint64							TIME_t;					//	시간 GetTickCount64()
#define		ZERO_TIME						(TIME_t)0

typedef		uint32							TIME32_t;				//	시간 GetTickCount()
#define		ZERO_TIME32						(TIME32_t)0

typedef		uint32							WAYBILL_t;				//	운송장 번호
#define		INVALID_WAYBILL					(WAYBILL_t)0

#define		INVALID_SEAT_INDEX				(uint8)(-1)

typedef		uint32							CHAT_GROUP_ID_t;		// chat 그룹 아이디
#define		INVALID_CHAT_GROUP_ID			(uint32)(0)

//-----------------------------------------------------
//	Stat
//-----------------------------------------------------
typedef		int32							STAT_t;					//	기본 스탯 형태

typedef		STAT_t							HP_t;					//	HP
typedef		STAT_t							ATK_t;					//	공격력
typedef		STAT_t							DEF_t;					//	방어력

typedef		STAT_t							CRI_RATE_t;				//	치명률
typedef		STAT_t							CRI_RESI_t;				//	치명저항

typedef		STAT_t							CRI_DMG_t;				//	크리 피해 증가
typedef		STAT_t							CRI_DMG_RESI_t;			//	크리 데미지 저항

typedef		STAT_t							PEN_DMG_t;				//	관통 데미지
typedef		STAT_t							PEN_RATE_t;				//	관통률
typedef		STAT_t							PEN_RESI_t;				//	관통저항

typedef		STAT_t							SPEED_t;				//	각종 속도 (걷기, 뛰기, 뒷걸음질)

typedef		STAT_t							RANGE_t;				//	기본 거리

typedef		STAT_t							GRO_GAGE_t;				//	그로기 게이지

typedef		STAT_t							DMG_t;					//	데미지

//-----------------------------------------------------
typedef		uint16							PER_RATE_t;				//	백분율

typedef		double							RATE_t;					//	기본 스탯 형태

typedef		RATE_t							VAMP_RATE_t;			//	흡혈
typedef		RATE_t							DMG_RATE_t;				//	데미지 %
typedef		RATE_t							HP_RATE_t;				//	HP %

typedef		RATE_t							AGGRO_t;				//	어그로
typedef		RATE_t							SCALE_t;				//	스케일
typedef		RATE_t							POWER_t;				//	전투력

typedef		uint16							PERMANENT_STAT_TYPE_t;	// 영구 스탯 유형
#define		INVALID_PERMANENT_STAT_TYPE		0						// EWRStatOptionType::None


//-----------------------------------------------------
//	포지션형
//-----------------------------------------------------

#define PI_VALUE 3.14159265358979323846

typedef		float							ANGLE_t;				//	각도


//-----------------------------------------------------
//	위치 자료형 
//-----------------------------------------------------
struct POS3_t 
{
	float		x;
	float		y;
	float		z;

	POS3_t() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {}
	POS3_t( const float dx, const float dy, const float dz ) : x( dx ), y( dy ), z( dz ) {}
	POS3_t( const POS3_t& rPos ) { x = rPos.x; y = rPos.y; z = rPos.z; }

	void			Clear()			{ x = 0.0f; y = 0.0f; z = 0.0f; }
	const bool		IsZero() const	{ return (x == 0.0f && y == 0.0f && z == 0.0f); }

	POS3_t& operator = ( const POS3_t& rPos ) 
	{ 
		x = rPos.x;
		y = rPos.y;
		z = rPos.z;
		return *this; 
	}


	// assignment operators
	POS3_t&	operator += ( const POS3_t& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	POS3_t&	operator -= ( const POS3_t& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	POS3_t&	operator *= ( float f )
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	POS3_t&	operator /= ( float f )
	{
		float fInv = 1.0f / f;
		x *= fInv;
		y *= fInv;
		z *= fInv;
		return *this;
	}


	// unary operators
	POS3_t	operator + () const
	{
		return *this;
	}

	POS3_t	operator - () const
	{
		return POS3_t( -x, -y, -z );
	}


	// binary operators
	POS3_t	operator + ( const POS3_t& v ) const
	{
		return POS3_t( x + v.x, y + v.y, z + v.z );
	}

	POS3_t	operator - ( const POS3_t& v ) const
	{
		return POS3_t( x - v.x, y - v.y, z - v.z );
	}

	POS3_t	operator * ( float f ) const
	{
		return POS3_t( x * f, y * f, z * f );
	}

	POS3_t	operator / ( float f ) const
	{
		float fInv = 1.0f / f;
		return POS3_t( x * fInv, y * fInv, z * fInv );
	}


	friend POS3_t	operator * ( float f, const struct POS3_t& v )
	{
		return POS3_t( f * v.x, f * v.y, f * v.z );
	}


	bool	operator == ( const POS3_t& v ) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool	operator != ( const POS3_t& v ) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	void	SetPosZZero() { z = 0; }

	float GetMagnitude()
	{
		return sqrt(x * x + y * y + z * z);
	}

	POS3_t Normalize()
	{
		float temp = GetMagnitude();
		float inverse = 1.0f;

		if( temp != 0.0f )
		{
			inverse = 1.0f / temp;
		}		

		POS3_t mPos;
		mPos.x = x * inverse;
		mPos.y = y * inverse;
		mPos.z = z * inverse;

		return mPos;
	}

	void NormalizeVector()
	{
		float fLength = GetMagnitude();
		if (fLength == 0.0f)
		{
			fLength = 1.0f;
		}

		x /= fLength;
		y /= fLength;
		z /= fLength;
	}

	//내적
	static float DotProduct(const POS3_t &p1, const POS3_t &p2)
	{
		return p1.x*p2.x + p1.y * p2.y + p1.z*p2.z;
	}

	POS3_t CrossVector(const POS3_t &vec) const
	{
		POS3_t mRetPos;
		mRetPos.x = y * vec.z - z * vec.y;
		mRetPos.y = z * vec.x - x * vec.z;
		mRetPos.z = x * vec.y - y * vec.x;
		return mRetPos;
	}

	//거리
	//float Distance(MVector v)
	float Distance(POS3_t v) const
	{
		float xfom = (x - v.x) * (x - v.x);
		float yfom = (y - v.y) * (y - v.y);
		float zfom = (z - v.z) * (z - v.z);

		float dist = sqrt(xfom + yfom + zfom);
		return dist;


		//return sqrt( (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z) );
	}

	static float Distance(const POS3_t& p1, const POS3_t& p2)
	{
		float xfom = ( p1.x - p2.x ) * ( p1.x - p2.x );
		float yfom = ( p1.y - p2.y ) * ( p1.y - p2.y );
		float zfom = ( p1.z - p2.z ) * ( p1.z - p2.z );

		float dist = sqrt(xfom + yfom + zfom);
		return dist;
	}

	float Distance(POS3_t v, float ignoreHeight)
	{
		float xfom = (x - v.x) * (x - v.x);
		float yfom = (y - v.y) * (y - v.y);

		float dist = 0.0f;

		//z축 차이가 ignoreHeight 보다 크면 z축을 무시하고 x, y축만으로 거리를 구함
		if (z - v.z >= ignoreHeight)
		{
			float zfom = (z - v.z) * (z - v.z);

			dist = sqrt(xfom + yfom + zfom);
		}
		else
		{
			dist = sqrt(xfom + yfom);
		}

		return dist;
	}

	//특정 지점 사이의 일정 거리 내 좌표를 구함
	POS3_t AdvancedPositionWithDirection(POS3_t direction, float distance) const
	{
		POS3_t startpos;
		startpos.x = x;
		startpos.y = y;
		startpos.z = z;

		POS3_t advancedpos(startpos);
		POS3_t normaldir(direction);

		normaldir.NormalizeVector();

		advancedpos = advancedpos + normaldir * distance;

		return advancedpos;
	}

	//두개의 위치 사이의 각도 구하기
	static float AngleBetweenPosZeroZ(POS3_t p1, POS3_t p2)
	{
		p1.z = 0;
		p2.z = 0;
		POS3_t Dirvec = p1 - p2;
		if (Dirvec.IsZero())
		{
			return 0.0f;
		}

		Dirvec.SetPosZZero();

		//normalize
		POS3_t NorPos = Dirvec.Normalize();

		//원점(0.0.0)에과 dirpos와의 각도 계산
		POS3_t mBaseVec(1, 0, 0);
		POS3_t BasePos = mBaseVec.Normalize();

		//내적된 값
		float mDot = POS3_t::DotProduct(BasePos, NorPos);
		float angle = acos(mDot);
		float newangle = (float)RadianToDegree(angle);


		if (NorPos.y < 0)
		{
			newangle = 360 - newangle;
		}

		return newangle;
	}

	//두개의 위치 사이의 각도 구하기
	static float AngleBetweenPositions(POS3_t p1, POS3_t p2)
	{
		POS3_t Dirvec = p1 - p2;
		if (Dirvec.IsZero())
		{
			return 0.0f;
		}

		Dirvec.SetPosZZero();

		//normalize
		POS3_t NorPos = Dirvec.Normalize();

		//원점(0.0.0)에과 dirpos와의 각도 계산
		POS3_t mBaseVec(1, 0, 0);
		POS3_t BasePos = mBaseVec.Normalize();

		//내적된 값
		float mDot = POS3_t::DotProduct(BasePos, NorPos);
		float angle = acos(mDot);
		float newangle = (float)RadianToDegree(angle);


		if (NorPos.y < 0)
		{
			newangle = 360 - newangle;
		}

		return newangle;
	}
	float AngleBetweenPositions(POS3_t dest) const
	{
		POS3_t startpos;
		startpos.x = x;
		startpos.y = y;
		startpos.z = z;

		POS3_t Posvec(startpos);
		POS3_t destvec(dest);
 
		POS3_t Dirvec = Posvec - destvec;
		if (Dirvec.IsZero())
		{
			return 0.0f;
		}

		Dirvec.SetPosZZero();

		//normalize
		POS3_t NorPos = Dirvec.Normalize();

		//원점(0.0.0)에과 dirpos와의 각도 계산
		POS3_t mBaseVec(1, 0, 0);
		POS3_t BasePos = mBaseVec.Normalize();

		//내적된 값
		float mDot = mBaseVec.DotProduct(BasePos, NorPos);
		float angle = acos(mDot);
		float newangle = (float)RadianToDegree(angle);


		if (NorPos.y < 0)
		{
			newangle = 360 - newangle;
		}

		return newangle;
	}

	static bool IsAlmostSamePosition(POS3_t p1, POS3_t p2)
	{
		if (Distance(p1, p2) < 1.0f)
			return true;

		return false;
	}

	static double RadianToDegree(double radian)
	{
		return radian * 180.0f / M_PI;
	}

	static double DegreeToRadian(double degree)
	{
		return degree / 180.0f * M_PI;
	}

	POS3_t AdvancedPosWithAngle(float degree, float dist) const
	{
		POS3_t direction;
		POS3_t AdvanPos;

		double pi = DegreeToRadian(degree);
		direction.x = (float)cos(pi);
		direction.y = (float)sin(pi);

		AdvanPos = AdvancedPositionWithDirection(direction, dist);
		return AdvanPos;
	}

	static POS3_t AdvancedPosWithAngle(const POS3_t& InPos, float degree, float dist)
	{
		POS3_t direction;
		POS3_t AdvanPos;

		double pi = DegreeToRadian(degree);
		direction.x = (float)cos(pi);
		direction.y = (float)sin(pi);

		AdvanPos = InPos.AdvancedPositionWithDirection(direction, dist);
		return AdvanPos;
	}

	static POS3_t RotationPos(const POS3_t& InPos, const float InRadian)
	{
		POS3_t Result;
		Result.x = InPos.x * cos(InRadian) - InPos.y * sin(InRadian);
		Result.y = InPos.x * sin(InRadian) + InPos.y * cos(InRadian);

		return Result;
	};

	static POS3_t GetCenterPosition(const POS3_t& p1, const POS3_t& p2)
	{
		float distance = POS3_t::Distance(p1, p2);
		float fAngle = POS3_t::AngleBetweenPositions(p1, p2);
		return POS3_t::AdvancedPosWithAngle(p2, fAngle, distance / 2);
	}

	float Size()
	{
		float size_v = sqrt(x*x + y*y + z*z);
		return size_v;
	}


	//FORCEINLINE float FRotator::ClampAxis(float Angle)
	//{
	//	// returns Angle in the range (-360,360)
	//	Angle = FMath::Fmod(Angle, 360.f);
	//
	//	if ( Angle < 0.f )
	//	{
	//		// shift to [0,360) range
	//		Angle += 360.f;
	//	}
	//
	//	return Angle;
	//}


	static FORCEINLINE float NormalizeAxis(float Angle)
	{
		// returns Angle in the range [0,360)
		//Angle = ClampAxis(Angle);

		if ( Angle > 180.f )
		{
			// shift to (-180,180]
			Angle -= 360.f;
		}

		return Angle;
	}
	static FORCEINLINE float UnnormalizeAxis(float degree)
	{
		if ( degree < 0 )
		{
			degree += 360.f;
		}

		return degree;
	}

	// 0~359 사이 값으로 각도를 변경해 줌
	static float ValidateDegree(float fDegree)
	{
		float fResult = 0;
		if (fDegree < 360 && fDegree >= 0)
			return fDegree;

		// 소수점 이하는 버린다.
		int nDegree = (int)fDegree;
		nDegree = nDegree % 360;
		if (nDegree < 0)
			nDegree = nDegree + 360;

		fResult = (float)nDegree;

		//float under_point = fDegree - (float)nDegree;
		//if ( nDegree >= 0 )
		//{
		//   nDegree = nDegree % 360;
		//}
		//else
		//{
		//   int cycle = abs(nDegree / 360);
		//   int nPositive = 360 * ( cycle + 1 );
		//   nDegree = nPositive + nDegree;
		//}

		//fResult = (float)nDegree + under_point;

		return fResult;
	}

	static bool IsLineCollision(const POS3_t& A1, const POS3_t& A2, const POS3_t& B1, const POS3_t& B2)
	{
		float t = 0.0f;
		float s = 0.0f;
		float under = ( B2.y - B1.y )*( A2.x - A1.x ) - ( B2.x - B1.x )*( A2.y - A1.y );
		if ( under == 0 ) 
			return false;

		float _t = ( B2.x - B1.x )*( A1.y - B1.y ) - ( B2.y - B1.y )*( A1.x - B1.x );
		float _s = ( A2.x - A1.x )*( A1.y - B1.y ) - ( A2.y - A1.y )*( A1.x - B1.x );

		t = _t / under;
		s = _s / under;

		if ( t<=0.0 || t>1.0 || s<=0.0 || s>1.0 ) 
			return false;

		return true;
	}
};
