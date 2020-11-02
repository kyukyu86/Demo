// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#define ACTOR_CHARACTER_TYPE_START 10
#define ACTOR_IO_TYPE_START 50

UENUM(BlueprintType)
enum class EWRActorDetailType : uint8
{
	None,
	
	Player = ACTOR_CHARACTER_TYPE_START,
	Me,
	Npc,
	ACTOR_CHARACTER_TYPE_END,

	StaticIO = ACTOR_IO_TYPE_START,
	DynamicIO,	
	ACTOR_IO_TYPE_END,
};

static bool IsCharacterType(const EWRActorDetailType IN InActorType)
{
	if (ACTOR_CHARACTER_TYPE_START <= (uint8)InActorType && InActorType < EWRActorDetailType::ACTOR_IO_TYPE_END)
	{
		return true;
	}		
	return false;
}

static bool IsIOType(const EWRActorDetailType IN InActorType)
{
	if (ACTOR_IO_TYPE_START <= (uint8)InActorType && InActorType < EWRActorDetailType::ACTOR_IO_TYPE_END)
	{
		return true;
	}
	return false;
}