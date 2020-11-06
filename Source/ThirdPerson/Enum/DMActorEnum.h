#pragma once

UENUM(BlueprintType)
enum class EDMInput : uint8
{
	None,

	Key_1,
	Key_2,
	Key_3,
	Key_4,

	MouseRButton,
	MouseLButton,

	ShiftL,
};

enum class EDMComponentType : uint8
{
	None,

	FSM,
	Battle,
	Interaction,
};

//====================================================================================
// Interaction
//====================================================================================

UENUM(BlueprintType)
enum class EDMInteractionResult : uint8
{
	None,

	DropItem,
	OpenWidget,
};

UENUM(BlueprintType)
enum class EDMInteractionResultAfter : uint8
{
	Remain,
	Delete,
};

//====================================================================================
// Character
//====================================================================================

UENUM(BlueprintType)
enum class EDMIdleType : uint8
{
	None,
	Stand,
	Sit,
	Fly,
};

UENUM(BlueprintType)
enum class EDMFSMType : uint8
{
	None,
	Idle,
	AttackNormal,
	SkillNormal,
	Hitted,
};

UENUM(BlueprintType)
enum class EDMMontageType : uint8
{
	None,
	Appear,
	Attack,
	Skill,
	Die,
	Jump,
	Fly,
};

UENUM(BlueprintType)
enum class EDMCombatInputType : uint8
{
	Immediately,
	Hold,
};

enum class EDMSkillNotice
{
	MouseL,
	MouseR,
};

//====================================================================================
//
//====================================================================================
