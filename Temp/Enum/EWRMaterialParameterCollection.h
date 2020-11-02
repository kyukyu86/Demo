#pragma once

UENUM(BlueprintType)
enum class EWRMPC : uint8
{
	Character,
	UI,
	Foliage,
	LevelLoad,
	Monitor,
	Movie,
};

enum class EWRMaterialParameter : uint8
{
	Dodge,
	Dash,
	DashOut,
	DashIn,
	HP,
	WhiteOut,
	WhiteIn,
	ReviveOut,
	ReviveIn,
	BlackOut,
	Break,
	ENPCToPCBlur,
	PCToENPCBlur,
	SpiderWeb,
	Block,
	Max,
};