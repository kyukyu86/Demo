// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

UENUM()
enum class EWRCharacterComponent : uint16
{
	None,
	VR,
	FSM,
	Attack,
	VFX,
	SFX,
	ManageMaterial,
	EquipmentLook,
	ConnectbyCable,
	DynamicMovement,
	StereoLayerManager,
	InteractionObject,
	Interaction,
	Lipsync,
	Buff,
	IdleSpeechDialogue,
	AcceptQuest,
	CommonNpcTalk,
	StateMark,
	DanceMode,
	PlayerSocial,
	NpcSocial,
	Revive,
	Play360Movie,
	Ladder,
	ActorInfo,
	EventAction,
	VoiceChatAudio,
	ChatRecording,
	Mic,
	Max,
};

UENUM(BlueprintType)
enum class EWRFXComponent : uint8
{
	None,
	VFXAttach,
	VFXMove,
	VFXSound,
	VFXSpawn,
	VFXLaser,
	Max,
};

UENUM(BlueprintType)
enum class EWRVehicleComponent : uint8
{
	None,
	ManageMaterial,
	Max,
};