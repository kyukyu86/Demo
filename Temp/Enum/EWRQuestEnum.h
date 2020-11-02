#pragma once
#include "ObjectMacros.h"
#include "Network/Share/Define/Define_Quest.h"

UENUM(BlueprintType)
enum class EWRQuestAcceptType : uint8
{
	None,
	Auto,
	NPCTalk,
	MAX
};

UENUM(BlueprintType)
enum class EWRQuestCompleteType : uint8
{
	None,
	Auto,
	MAX
};