// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../UI/Base/DMUISlot.h"

DECLARE_DELEGATE_OneParam(FDMSlotUILoadCompletedDelegate, UDMUISlot*)

UENUM(BlueprintType)
enum class EDMPanelKind : uint8
{
	None,

	Main,
	Menu,
	Inventory,
};

UENUM(BlueprintType)
enum class EDMSlotKind : uint8
{
	None,

	Preview,
};

UENUM(Meta = (BitFlags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDMWidgetComponentFlag : uint8
{
	None = 0x0000,

	Billboarrd = 0x0001,
	Focusable = 0x0002,
};
ENUM_CLASS_FLAGS(EDMWidgetComponentFlag)