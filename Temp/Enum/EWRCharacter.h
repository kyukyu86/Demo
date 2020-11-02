#pragma once
#include "Define/WRTypeDefine.h"
#include "WRPCH.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_BluePrint.h"

//임시로 지정
//typedef uint64 AID_t;
//typedef uint64 SKILL_SEQ_t;
typedef FPlatformTypes::int32 WRRecordTID;
typedef FPlatformTypes::uint16 BitFlag16;
//typedef int32 WRTableID;

static const AID_t INCREASE_STANDALONE_UID = 100;
static const AID_t INITIALIZE_STANDALONE_AUID = 99999;		// 스폰 트리거에 의해 생성될 때 최소값( 100000 ~ 999999 )
static const AID_t MAX_STANDALONE_AUID = 999999;		// 스폰 트리거에 의해 생성될 때 최대값
static const float VEHICLE_TICK_TIME_TO_SYNC = 0.2f;

//Skill

//#define ONLY_STANDALONE_MODE
#define INVALID_RECORD_TID 0
#define REVIVE_FX_PATH_TABLEID 1

#define DEF_YUL_TEST
#define DEF_FIX_CLEARAIMOVETO
#define DEF_TEMP_FIX_ATTACH_TIMING_CHARACTER_TO_VEHICLE
//#define DEF_NOT_CHECK_VEHICLE_OWNER_WHEN_GETON_4_BUILD_TEMPORARY
//#define DEF_USE_ROOT_MOTION_WHEN_GETON_VEHICLE


UENUM(BlueprintType)
enum class EWRInput : uint8
{
	None,
	Triangle,
	Square,
	Circle,
	Cross,
	DpadUP,
	DpadDown,
	DpadLeft,
	DpadRight,
	L1,
	L2,
	L3,
	R1,
	R2,
	R3,
	Option,
	Share,
	TouchPadDragUpY,	// [ 2019-10-25 : hansang88 ] 터치 패드 드래그 UP(갈고리 발사입력)
	TouchPadDragDownY,	// [ 2019-11-21 : hansang88 ] 터치 패드 드래그 DOWN (갈고리 당기는 입력)
	TouchPadDragLeftX,
	TouchPadDragRightX,
	JoystickPull,		// [ 2019-10-25 : hansang88 ] 게임패드 당기는 모션(갈고리 당기는 입력)
	JoystickPush,		// [ 2019-11-13 : kyu ] 게임패드 미는 모션
	TouchPadClick,
	RStickRight,
	RStickLeft,
	RStickUp,
	RStickDown,
	RStickX,
	RStickY,
	LStickRight,
	LStickLeft,
	LStickUp,
	LStickDown,
	LStickX,
	LStickY,

	InteractionSkip, // [ 2019-12-20 : kyu ] 인터렉션 개발용 Skip
};

UENUM(BlueprintType)
enum class EWRCCType : uint8
{
	None,
	Weak,
};

UENUM(BlueprintType)
enum class EWRWeaponType : uint8
{
	TwoHand,
	OneHandMain,
	OneHandSub,
};

UENUM(BlueprintType)
enum class EWRTargetingType : uint8
{
	Normal,
	Multi,
};

UENUM(BlueprintType)
enum class EWRVisibility : uint8
{
	None = 0x0000,
	DistanceCulling = 0x0001,
	SpecialConversation = 0x0002,
	DistanceCullingSpecialConversation = 0x0003 UMETA(DisplayName = "DistanceCulling | SpecialConversation"),
	Max,
};

UENUM(BlueprintType)
enum class EWREquipPart : uint8
{
	None,

	// [ 2020-6-10 : animal14 ] 
	Neckwear,
	Ring,

	Glasses,
	Mask,
	FaceMask,
	Backpack,

	RightHand,
	LeftHand,

	Top,
	Bottom,
	Outfit,

	Gloves,
	Shoes,

	// [ 2020-6-10 : animal14 ] 
	//RightArm,
	//LeftArm,
	//RightLeg,
	//LeftLeg,
	Body,
	Face,

	// [ 2020-6-19 : animal14 ] 
	Hair,
	FacialHair,

	// [ 2020-7-21 : animal14 ] 
	Ego,

	Max,
};

UENUM(BlueprintType)
enum class EWREquipmentLookAction : uint8
{
	None,
	Equipment,
	Unequipment,
	Exchange,
};

UENUM(BlueprintType)
enum class EWRAdditionalAbility : uint8
{
	None,
	Hook,
	ShieldProjectile,
	Light,
};

UENUM(BlueprintType)
enum class EWRCharacterType: uint8
{
	None,
	Player,
	NPC,
	ENPC,
	IO,
	SO,
};

UENUM(BlueprintType)
enum class EWRDisplayType : uint8
{
	None,
	ENPCToPCBlur,
	PCToENPCBlur,
};

//====================================================================================

UENUM(BlueprintType)
enum class EWRChangeEquipment : uint8
{
	MainWeapon,
	SubWeapon,
	Temporary,
};

// UENUM(BlueprintType)
// enum class EWRDynamicMovementType: uint8
// {
// 	None,
// 
// 	FrontMoveToPosition,	//설정된 거리만큼 이동
// 	FrontMoveToActor,		//대상에서 설정된거리만큼 떨어진 위치로 이동
// 	BackMoveToPosition,		//설정된 거리만큼 뒤로 이동
// 	BackMoveToActor,		//대상에서 설정된 거리만큼 뒤로 이동
// 
// 	PushedToPosition,		//대상을 설정된 거리만큼 민다
// 	PulledToActor,			//대상을 설정된 거리만큼 떨어진 위치로 당긴다
// 
// 	MoveToInputDirection,	//입력받은 방향으로 이동, 입력이 없을경우 정면이동
// };

UENUM(BlueprintType)
enum class EWRMoveAnimType : uint8
{
	None,

	Montage,
	BlendSpace,
};

UENUM(BlueprintType)
enum class EWRInteractEvent : uint8
{
	Throw,
};

UENUM(BlueprintType)
enum class EWRHookEvent : uint8
{
	None,
	Throw,
	Action,
};

UENUM(BlueprintType)
enum class EWRObjectHighLight : uint8
{
	None,
	Recognition,
	Targeted,
};

UENUM(BlueprintType)
enum class EWRInvokeSkillTargetType : uint8
{
	None,
	Enemy,
	IO,
};

UENUM(BlueprintType)
enum class EWRLadderHand : uint8
{
	None,
	Left,
	Right,
};

UENUM(BlueprintType)
enum class EWRDeadEffectType : uint8
{
	None,
	ragdoll,
	slice,
	Destructible,
};

UENUM(BlueprintType)
enum class EWREnvType : uint8
{
	None,
	Elevator,
	FloatingBoard,
	Tardis,
	DynamicFoliage,
	Destructible,
	SecretDialogArea,
	Interior,
};

UENUM(BlueprintType)
enum class EWRVehicleSeatSide : uint8
{
	None,
	LEFT_SIDE,
	RIGHT_SIDE,
};

UENUM(BlueprintType)
enum class EWRMoveStateType : uint8
{
	None,
	OnFoot,
	OnTransport,
	OnMovingwalk,
};

UENUM(BlueprintType)
enum class EWRSpawnSimulationState : uint8
{
	None,
	AskedSpawn,	//스폰 요청후 완료 대기중
	ReadyToSimulation,	//스폰이 완료되어 위치 결정중
	InvalidLocation,
	MAX,
};

UENUM(BlueprintType)
enum class EWRSimulationOverlapEvnet : uint8
{
	None,
	BeginOverlap,
	EndOverlap,
	MAX,
};

UENUM(BlueprintType)
enum class EWRIdleType : uint8
{
	None,
	Sit,
	StandMic,
};

UENUM(BlueprintType)
enum class EWRSteeringWheelState: uint8
{
	None,
	StartAutoPilot,
	EndAutoPilot,
	AutoIdle,
	ManualIdle,

};

//====================================================================================

class AWRCharacter;
class WRFSMInfo
{
public:
	bool operator==(const WRFSMInfo& other) const
	{
		return (OwnerCharacter == other.OwnerCharacter)
			&& (FSMType == other.FSMType)
			&& (PreviousFSMType == other.PreviousFSMType)
			&& (SkillTableID == other.SkillTableID)
			&& (PreviousSkillTableID == other.PreviousSkillTableID)
			&& (CombatSeqID == other.CombatSeqID)
			&& (bImmediateUpdate == other.bImmediateUpdate)
			&& (VoidPtr == other.VoidPtr)
			&& (JumpToSectionName == other.JumpToSectionName)
			&& (MontageTID == other.MontageTID)
			&& (DataString == other.DataString);
	}

public:
	//WRFSMInfo() {}

	WRFSMInfo()
		: OwnerCharacter(nullptr)
		, FSMType(EWRFSM::Idle)
		, PreviousFSMType(EWRFSM::Idle)
		, SkillTableID(INVALID_TABLE_TID)
		, PreviousSkillTableID(INVALID_TABLE_TID)
		, CombatSeqID(INVALID_SKILL_SEQ)
		, bImmediateUpdate(false)
		, VoidPtr(nullptr)
		, JumpToSectionName("")
		, MontageTID(INVALID_TABLE_TID)
	{
	}

	WRFSMInfo(EWRFSM eType, WRTableID InSkillTableID, uint64 InTargetID, uint32 InCombatID, bool bImmediately = false, void* pVoid = nullptr, FString InJumpToSectionName = "", WRTableID InMontageTID = INVALID_TABLE_TID)
		: OwnerCharacter(nullptr)
		, FSMType(eType)
		, PreviousFSMType(EWRFSM::Idle) 
		, SkillTableID(InSkillTableID)
		, PreviousSkillTableID(INVALID_TABLE_TID)
		, CombatSeqID(InCombatID)
		, bImmediateUpdate(bImmediately)
		, VoidPtr(pVoid)
		, JumpToSectionName(InJumpToSectionName)
		, MontageTID(InMontageTID)
	{
	}

	WRFSMInfo(EWRFSM eType, bool bImmediately = false, void* pVoid = nullptr, FString InJumpToSectionName = "")
		: OwnerCharacter(nullptr)
		, FSMType(eType)
		, PreviousFSMType(EWRFSM::Idle)
		, SkillTableID(INVALID_TABLE_TID)
		, PreviousSkillTableID(INVALID_TABLE_TID)
		, CombatSeqID(0)
		, bImmediateUpdate(bImmediately)
		, VoidPtr(pVoid)
		, JumpToSectionName(InJumpToSectionName)
		, MontageTID(INVALID_TABLE_TID)
	{
	}

public:
	AWRCharacter*	OwnerCharacter;
	EWRFSM			FSMType;
	EWRFSM			PreviousFSMType;
	WRTableID		SkillTableID;
	WRTableID		PreviousSkillTableID;
	SKILL_SEQ_t		CombatSeqID;
	bool			bImmediateUpdate;

	//사용되지 않아도 무방한 값
	void*			VoidPtr;	//특정 FSM에서 명시된 형식의 데이터가 필요할때 사용
	FString			JumpToSectionName;
	WRTableID		MontageTID;

	TArray<FString> DataString;	//문자열로 만들어서 알아서 가공해서 분해해서 사용하시오! 안써도 고만
};

UENUM(BlueprintType)
enum class EWRCustomizeDirectionAction : uint8
{
	None,
	Spawn,
	Unspawn,
	Rotation,
	ZoomIn,
	ZoomOut,
};

UENUM(BlueprintType)
enum class EWRSetCollisionTo : uint8
{
	None,
	CapsuleAndMesh,
	CapsuleOnly,
	MeshOnly,
};