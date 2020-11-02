// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#define CHARACTER_MAX_HP_TEMPORARY 10000

struct stCharacterStat
{
	ATK_t Atk;
	DEF_t Def;
	HP_t MaxHP;
	HP_t HP;

	stCharacterStat() : Atk(1), Def(1), MaxHP(1), HP(CHARACTER_MAX_HP_TEMPORARY) { }
};