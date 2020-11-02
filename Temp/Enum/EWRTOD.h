#pragma once

UENUM(BlueprintType)
enum class EWRTODActor : uint8
{
	Global = 0,

	Local_0 = 1,
	Local_1 = 2,
	Local_2 = 3,
	Local_3 = 4,
	Local_4 = 5,
	Local_5 = 6,
	Local_6 = 7,
	Local_7 = 8,
	Local_8 = 9,
	Local_9 = 10,

	Event_0 = 101,
	Event_1 = 102,
	Event_2 = 103,
	Event_3 = 104,
	Event_4 = 105,
	Event_5 = 106,
	Event_6 = 107,
	Event_7 = 108,
	Event_8 = 109,
	Event_9 = 110,
};

UENUM(BlueprintType)
enum class EWRTODETC : uint8
{
	None,
	FogDensity,
	HeightFalloff,
	FogColor,
	FogOpacity,
	StartDistance,

	SecondFogDensity,
	SecondHeightFalloff,
	SecondHeightOffset,

	RainAmount,
	RainVelocity,
	MistAmount,

	TODEmissive,

	Max,
};