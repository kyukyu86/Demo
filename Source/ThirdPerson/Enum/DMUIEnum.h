// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../UI/Base/DMUISlot.h"

UENUM(BlueprintType)
enum class EDMPanelKind : uint8
{
	None,

	Main,
};

UENUM(BlueprintType)
enum class EDMSlotKind : uint8
{
	None,

	Preview,
};

DECLARE_DELEGATE_OneParam(FDMSlotUILoadCompletedDelegate, UDMUISlot*)