#pragma once

#include "../Define/WRTriggerDefine.h"


UENUM(BlueprintType)
enum class EWRTutorialPanel : uint8
{
	None,
	Main,
	Sub,
};

UENUM(BlueprintType)
enum class EWRTutorialType : uint8
{
	None,
	ImageAndDescription,
};

UENUM(BlueprintType)
enum class EWRTutorialEndType : uint8
{
	None,	
	ShowTime,
	KeyInput,
};

UENUM(BlueprintType)
enum class EWRTutorialPreConditionType : uint8
{
	None,
	EquipMain,
	EquipShield,
	EquipHook,	
};

UENUM(BlueprintType)
enum class EWRTutorialConditionType : uint8
{
	None,
	FastAttack,
	StrongAttack,
	DropItem,
	DropMain,
	DropShield,
	DropHook,
	Equip,
	MonsterKill,
	Targeting,
	SkillUse,
	MoveToTrigger,
	KeyInput,
	BoardingVehicle,
};

UENUM(BlueprintType)
enum class EWRTutorialKey : uint8
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
	TouchPadDragY,
	JoystickPull,
	TouchPadClick,
};

enum class EWRTutorialState : uint8
{
	Init,
	Reserved,
	Opening,
	Doing,
	Complete,
};