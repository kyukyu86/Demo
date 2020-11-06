// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../UI/Base/DMUISlot.h"

DECLARE_DELEGATE_OneParam(FDMSlotUILoadCompletedDelegate, UDMUISlot*)

UENUM(BlueprintType)
enum class EDMWidgetCreationType : uint8
{
	None,
	PanelKind,
	WidgetPath,
};

UENUM(BlueprintType)
enum class EDMPanelKind : uint8
{
	None,
	Debug,
	Main,
	Menu,
	Inventory,
	Charge,
	SkillNotice,
};

UENUM(BlueprintType)
enum class EDMSlotKind : uint8
{
	None,
	Preview,
};

UENUM(BlueprintType)
enum class EDMWidgetType : uint8
{
	None,
	Widget2D,
	Widget3D,
	WidgetActor,
};

UENUM(Meta = (BitFlags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDMWidgetComponentFlag : uint8
{
	None = 0x0000,

	Billboarrd = 0x0001,
	Focusable = 0x0002,
};
ENUM_CLASS_FLAGS(EDMWidgetComponentFlag)