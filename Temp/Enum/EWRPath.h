// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

UENUM(BlueprintType)
enum class EWRPath : uint8
{
	// [ 2019-7-17 : magedoga ] Root
	Asset,
	Movie,

	// [ 2019-7-17 : magedoga ] Root 1 Depth
	Animation,
	Blueprint,
	Character,
	Data,
	Effect,
	Shader,
	Sound,
	UI,

	// [ 2019-7-17 : magedoga ] Root 2 Depth
	//Blueprint_Character,
	//Blueprint_UI,
	//Blueprint_Effect,

	Data_Table,
	Data_Map,
	
	//Shader_PostProcess,

	Sound_SoundCue,
	
	// [ 2019-8-20 : vuvle ] Root 3 Depth (UI Second depth)
	//Blueprint_UI_Panel,
	//Blueprint_UI_Slot,

	// [ 2020-10-6 : animal14 ] 
	UI_Panel,
	UI_Slot,	
	UI_Images,

	// [ 2020-1-10 : hansang88 ]  Moive Root 1 Depth
	MediaPlayer,
	MediaMaterial,
	MediaSource,
	MediaTexture,
};