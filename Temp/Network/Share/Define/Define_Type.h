//******************************************************************************
//	FileName: 	Define_Type.h
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

#define _USE_MATH_DEFINES

#include "Define_ShareType.h"
#include <math.h>


//-----------------------------------------------------
//	��Ʈ��ũ �� ����
//-----------------------------------------------------
typedef		int32							SERVER_INDEX_t;			// ���� �ε���
#define		INVALID_SERVER_INDEX			-1
#define		MOVING_WORLD_TEMP_INDEX			-2

typedef		int32							LTID_t;					// Local Thread Index
#define		INVALID_LTID					(LTID_t)(-1)

typedef		uint32							WSN_t;					// World Serial Number (������)
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

typedef		std::string						PATH_t;					//	���
#define		INVALID_PATH					"*"

typedef		std::string						FILE_NAME_t;			//	���� ��
#define		INVALID_FILE_NAME				"*"

typedef		uint32							PROTOCOL_VERSION_t;		// User Protocol Version
#define		INVALID_PROTOCOL_VERSION		0						// WXR_PROTOCOL_VERSION	

typedef		std::wstring					ID_t;					// ID
#define		INVALID_ID						L"*"

typedef		uint32							DKEY_t;					// Dynamic Key
#define		INVALID_DKEY					0

typedef		int32							SOCKET_INDEX_t;			// Socket Index
#define		INVALID_SOCKET_INDEX			-1

typedef		uint32							RESULT_t;				// ��Ŷ ���
#define		INVALID_RESULT					0

typedef		uint16							FORCEKICK_t;			// ���������� Ÿ��
#define		INVALID_FORCEKICK				0


//-----------------------------------------------------
//	���� / ���
//-----------------------------------------------------
typedef		uint32							USN_t;					// User Serial Number ... User DBKEY (Lobby / Game)
#define		INVALID_USN						0

typedef		std::string						MAID_t;					// Mac Address ID
#define		INVALID_MAID					"*"

typedef		std::string						UDID_t;					// User Device ID
#define		INVALID_UDID					"*"

typedef		std::wstring					UUID_t;					//	User Unique Device ID - MDC ��ɿ��� Ű�� ��� (UDID_t�� �ٸ� ��)
#define		INVALID_UUID					L"*"					//	�Խ�Ʈ�� �� ������ ó����

typedef     uint8							USER_GRADE_t;			//	���� ���
#define     BASE_USER_GRADE					0						//	EWRUserGrade::User


//-----------------------------------------------------
//	���� �� ����
//-----------------------------------------------------

//	HTTP ���
typedef		std::wstring					HTTP_URL_t;				// HTTP Request URL Ÿ��
#define		INVALID_HTTP_URL				L"*"

typedef		std::wstring					HTTP_QUERY_t;			// HTTP Request Query Ÿ��
#define		INVALID_HTTP_QUERY				L"*"

//	IAP ���
typedef		int16							IAP_VERIFY_RESCODE_t;	// Verify ��� �ڵ�
#define		INVALID_IAP_VERIFY_RESCODE		-1			
#define		SUCCESS_IAP_VERIFY_RESCODE		0						// Verify ��� �ڵ� ����

typedef		int16							IAP_VERIFY_RESULT_t;	// Verify ���
#define		INVALID_IAP_VERIFY_RESULT		-1						

//	�ý��� �α� ����
typedef		int32							EVENT_TYPE_t;			// EWREventType::en
#define		INVALID_EVENT_TYPE				-1								

typedef		std::string						COUPON_t;				// ���� ��
#define		INVALID_COUPON					"*"

//	�̿� ���
typedef		uint32							TERMS_VERSION_t;
#define		INVALID_TERMS_VERSION			0


//-----------------------------------------------------
//	���� ����
//-----------------------------------------------------
typedef		uint32							PSN_t;					//	Player Serial Number ... Player DBKEY (Lobby / Game)
#define		INVALID_PSN						0
#define		GROUP_PSN						(uint32)(MAXLONG)		//	Player Group �� �ǹ���

typedef		uint64							AID_t;					//	Actor Serial ID ... Unique key
#define		INVALID_AID						(AID_t)0
#define		CLASSIFY_AID_UNIT				(AID_t)(100)			//	EIMCharacterType::MAX ���� Ŀ�� ��. Actor Type ������ �ܰ� - ���� 00 �ڸ��� ���п����� ���
#define		MAX_AID_SRC						(AID_t)(MAXLONGLONG / CLASSIFY_AID_UNIT)	//	��� ������ �ִ밪

typedef		uint32							ACTOR_TYPE_t;			//	Actor Type		//	EWRActorType
#define		INVALID_ACTOR_TYPE				99						//	��ȿ ���� ����

typedef		uint64							ISN_t;					// Item Serial Number ... Item DBKEY 
#define		INVALID_ISN						(ISN_t)0

typedef		int32							TID_t;					//	Table ID (�� ���̺��� ����) ... ��, �������� ������ ���ɼ� ����
#define		INVALID_TID						(TID_t)0

typedef		uint64							SKILL_SEQ_t;			//	��ų ��� ��ȣ
#define		INVALID_SKILL_SEQ				(SKILL_SEQ_t)0

typedef		uint32							TIME32_t;				//	�ð� GetTickCount()

typedef		uint64							MAIL_SN_t;				//	���� �ø��� �ѹ�
#define		INVALID_MAILSN					(MAIL_SN_t)0

typedef		uint32							GUILD_SN_t;				//	��� �ø��� �ѹ�
#define		INVALID_GUILD_SN				0

typedef		uint64							PARTY_ID_t;				//	Party Serial ID ... Unique key
#define		INVALID_PARTY_ID				(PARTY_ID_t)0

typedef		int16							LEVEL_t;				//	Level
#define		INVALID_LEVEL					0						//	������ 1���� ����
#define		INITIALIZE_LEVEL				1						

typedef		int64							EXP_t;					//	����ġ (0���� ����, ����� ���Ͽ� signed ���� ���)

typedef		int64							CURRENCY_t;				//	������ ��ȭ ����
#define		INVALID_CURRENCY				(CURRENCY_t)-1
#define		MAX_CURRENCY_VALUE				999999999				//	������ ��ȭ �ִ�ġ

typedef		int32							GAME_MONEY_t;			//	�ΰ��� ��ȭ
#define		INVALID_GAME_MONEY				(GAME_MONEY_t)-1

typedef		int32							CASH_MONEY_t;			//	���� ��ȭ
#define		INVALID_CASH_MONEY				(CASH_MONEY_t)-1

typedef		int32							POINT_t;				//	����Ʈ ��ȭ
#define		INVALID_POINT					(POINT_t)-1

typedef		int32							TICKET_t;				//	Ƽ�� ��ȭ
#define		INVALID_TICKET					(TICKET_t)-1

typedef		std::wstring					NAME_t;					//	Name W... �⺻������ �̸� ���� ����
#define		INVALID_NAME					L"*"

typedef		std::string						NAMEA_t;				//	Name A ... �⺻������ �̸� ���� ����
#define		INVALID_NAMEA					"*"

typedef		int16							CHANNEL_INDEX_t;		//	�� ä�� �ε���
#define		INVALID_CHANNEL_INDEX			(CHANNEL_INDEX_t)(-1)
#define		MAX_CHANNEL_INDEX				30000					//	ä�� �ִ�ġ
#define		MIN_CHANNEL_INDEX				10						//	ä�� �ּ�ġ
#define		BOSS_CHANNEL_INDEX				30001					//	�ʵ� ���̵� ä�� �ε��� (����)
#define		DEFAULT_USER_NUM				300						//	���� �ο� ��
#define		LAWLESS_CHANNEL_INDEX			1						//	���� ä�� �ε��� (����)

typedef		uint32							MAP_INDEX_t;			//	Map ���� �ε��� (���� ��)
#define		INVALID_MAP_INDEX				0						//	
#define		MAX_MAP_INDEX					(MAP_INDEX_t)(MAXLONG)	//	MAP_INDEX_t �ִ밪
#define		MAX_GEN_MAP_INDEX_COUNT			10000					//	Gen ȸ��

typedef		uint32							SECTOR_INDEX_t;			//	Sector �ε���
#define		INVALID_SECTOR_INDEX			(SECTOR_INDEX_t)(-1)	//	
#define		MAX_SECTOR_SIZE					3000					//	�ִ� ���� ũ��

typedef		uint32							SKILL_POINT_t;			//	��ų ����Ʈ
typedef		int16							SKILL_LEVEL_t;			//	��ų Level 

typedef		int32							ITEM_COUNT_t;			//	������ ����
typedef		uint16							ITEM_SLOT_COUNT_t;		//	������ ���� ����

typedef		int32							QUEST_TASK_VALUE_t;		//	����Ʈ �½�ũ ���� ��
#define		INVALID_QUEST_TASK_VALUE		-1

typedef		uint16							BUFF_PRIORITY_t;			//  ���� �켱����
#define		DEFAULT_BUFF_PRIORITY			0

typedef		uint64							TIME_t;					//	�ð� GetTickCount64()
#define		ZERO_TIME						(TIME_t)0

typedef		uint32							TIME32_t;				//	�ð� GetTickCount()
#define		ZERO_TIME32						(TIME32_t)0

typedef		uint32							WAYBILL_t;				//	����� ��ȣ
#define		INVALID_WAYBILL					(WAYBILL_t)0

#define		INVALID_SEAT_INDEX				(uint8)(-1)

typedef		uint32							CHAT_GROUP_ID_t;		// chat �׷� ���̵�
#define		INVALID_CHAT_GROUP_ID			(uint32)(0)

//-----------------------------------------------------
//	Stat
//-----------------------------------------------------
typedef		int32							STAT_t;					//	�⺻ ���� ����

typedef		STAT_t							HP_t;					//	HP
typedef		STAT_t							ATK_t;					//	���ݷ�
typedef		STAT_t							DEF_t;					//	����

typedef		STAT_t							CRI_RATE_t;				//	ġ���
typedef		STAT_t							CRI_RESI_t;				//	ġ������

typedef		STAT_t							CRI_DMG_t;				//	ũ�� ���� ����
typedef		STAT_t							CRI_DMG_RESI_t;			//	ũ�� ������ ����

typedef		STAT_t							PEN_DMG_t;				//	���� ������
typedef		STAT_t							PEN_RATE_t;				//	�����
typedef		STAT_t							PEN_RESI_t;				//	��������

typedef		STAT_t							SPEED_t;				//	���� �ӵ� (�ȱ�, �ٱ�, �ް�����)

typedef		STAT_t							RANGE_t;				//	�⺻ �Ÿ�

typedef		STAT_t							GRO_GAGE_t;				//	�׷α� ������

typedef		STAT_t							DMG_t;					//	������

//-----------------------------------------------------
typedef		uint16							PER_RATE_t;				//	�����

typedef		double							RATE_t;					//	�⺻ ���� ����

typedef		RATE_t							VAMP_RATE_t;			//	����
typedef		RATE_t							DMG_RATE_t;				//	������ %
typedef		RATE_t							HP_RATE_t;				//	HP %

typedef		RATE_t							AGGRO_t;				//	��׷�
typedef		RATE_t							SCALE_t;				//	������
typedef		RATE_t							POWER_t;				//	������

typedef		uint16							PERMANENT_STAT_TYPE_t;	// ���� ���� ����
#define		INVALID_PERMANENT_STAT_TYPE		0						// EWRStatOptionType::None


//-----------------------------------------------------
//	��������
//-----------------------------------------------------

#define PI_VALUE 3.14159265358979323846

typedef		float							ANGLE_t;				//	����


//-----------------------------------------------------
//	��ġ �ڷ��� 
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

	//����
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

	//�Ÿ�
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

		//z�� ���̰� ignoreHeight ���� ũ�� z���� �����ϰ� x, y�ุ���� �Ÿ��� ����
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

	//Ư�� ���� ������ ���� �Ÿ� �� ��ǥ�� ����
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

	//�ΰ��� ��ġ ������ ���� ���ϱ�
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

		//����(0.0.0)���� dirpos���� ���� ���
		POS3_t mBaseVec(1, 0, 0);
		POS3_t BasePos = mBaseVec.Normalize();

		//������ ��
		float mDot = POS3_t::DotProduct(BasePos, NorPos);
		float angle = acos(mDot);
		float newangle = (float)RadianToDegree(angle);


		if (NorPos.y < 0)
		{
			newangle = 360 - newangle;
		}

		return newangle;
	}

	//�ΰ��� ��ġ ������ ���� ���ϱ�
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

		//����(0.0.0)���� dirpos���� ���� ���
		POS3_t mBaseVec(1, 0, 0);
		POS3_t BasePos = mBaseVec.Normalize();

		//������ ��
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

		//����(0.0.0)���� dirpos���� ���� ���
		POS3_t mBaseVec(1, 0, 0);
		POS3_t BasePos = mBaseVec.Normalize();

		//������ ��
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

	// 0~359 ���� ������ ������ ������ ��
	static float ValidateDegree(float fDegree)
	{
		float fResult = 0;
		if (fDegree < 360 && fDegree >= 0)
			return fDegree;

		// �Ҽ��� ���ϴ� ������.
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
