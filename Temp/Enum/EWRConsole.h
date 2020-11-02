// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

UENUM(BlueprintType)
enum class EWRConsole :uint8
{
	TestConsoleInt,
	TestConsoleFloat,
	TestConsoleString,
	TestConsole,
	TestYSY,
	TestYSY2,
	TestKyu,
	Test_ASpawnNPC,
	Test_SSpawnNPC,
	Test_360Movie_3DUse,
	Test_NoFaction,

	Async_Spawn_Actor,

	VR_DebugDrawLine_Axis,
	VR_StartHeight,

	Character_RotationMode,
	Character_RotationAnglePerAction,
	Character_RotationAnglePerActionTime,
	Character_RotationAngleForTraceHMD,
	Character_RotationSpeedForTraceHMD,
	Character_RotationSpeedForThumbStick,
	Character_RotationThumbstickMovingDetectValue,
	Character_RotationThumbstickMovingIgnoreValue,
	Character_SyncHMDHeadClampYaw,
	Character_SyncHMDHeadClampPitch,
	Character_SyncHMDHeadClampRoll,
	Character_SyncHMDHeadReqTime,
	Character_SyncHMDDist,
	Character_3rdCharacterCameraOffsetZ,
	Character_3rdCharacterCameraOffsetX,
	Character_3rdCharacterCameraOffsetZForDance,
	Character_3rdCharacterCameraOffsetXForDance,

	Cable_CollisionRadius,
	Cable_CutDirectionTime,

	Camera_MaxPitchAngleWhenHMDOff,
	Camera_DeltaPitchPerSecond,

	VehicleSpawnDirection,
	VehicleMaxVolume,

	Sound_BGMDuration,

	CCD_Radius,
	Spawn_Distribution,

	TOD_DayCycle,
	TOD_TimeType,

	HitRed_Duration,
	HitBlue_Duration,

	Dodge_Duration,

	HeavyAttack_Duration,
	SkillAttack_Duration,

	Dash_Duration,
	Dash_Ratio,

	Add_Impulse,
	Movement_Amount,

	Slice_DirectionWeight,
	Slice_ActiveMultiSlice,
	Slice_ActiveImpulse,
	//Slice_DirectionWeight,

	Dead_Duration,
	BloodDecal_Duration,

	WhiteOutIn_Duration,
	BlackOutIn_Enable,
	BlackOutIn_Duration,
	BlackOutIn_Ratio,

	MicVolumeMultiplier,

	EnableRemoveWav,

	EnableDistanceCulling,
	CullingDistance,
	CullingRefreshTime,

	ResponseTime,

	UI_Cheat,
	UI_Inventory,
	UI_KeepFocusToLastUI,
	UI_CyberAniSpeed,
	UI_3DMarkDIst,
	UI_IdleSpeechTime,
	UI_DefaultStyleNoticeTime,
	UI_DefaultChatBubbleTime,
	UI_ChatBubbleLimitCount,
	UI_MaxZoomScale_FocusButton,
	UI_ZoomTime_FocusButton,
	UI_FadeOutTime_ClosePanel,
	UI_AutoCloseSocialMenu_Distance,
	UI_AutoCloseSocialMenu_Degree,
	UI_GuideOn,
	UI_FocusTalkCoolTime,

	HMD_TargettingRayLength,
	HMD_TargettingCountMax,
	HMD_TargettingSphereRadius,
	HMD_TargettingRotIgnoreAngle,
	HMD_TargettingHoldingTime,
	HMD_Movie_Distance,
	HMD_WidgetInteractionLay,
	HMD_TopAttack_Degree,
	HMD_BottomAttack_Degree,
	

	CommonUI_FixedSize,
	TargetUI_FixedSize,
	SpeakDialogueIcon_FixedSize,
	SelectDialogue_FixedSize,

	IO_CollisionShow,
	IO_NoneTargetHighLightValue,
	IO_TargetHighLightValue,
	IO_TargetUIFixedSize,
	IO_HookDashEffectDuration,
	IO_IgnoreQuest,
	IO_InteractionSkip,

	IO_Lifespan,
	IO_ThrowSpeed,
	IO_ThrowDegree,

	IK_LookIKDistanceRatio,
	IK_LookIKLimitYaw,
	IK_LookIKLimitPitch,
	IK_LookIKBlendSpeed,
	IK_LookIKLimitYawForDialog,

	Anim_AvoidSectionRate,
	Anim_BlendSpaceTransformSpeed,
	Anim_DisplayURODebug,

	PAD_PlayForceFeedback,
	PAD_StopForceFeedback,
	PAD_PullMotionValueZ,
	PAD_PullMotionValueY,
	PAD_DragYValue,
	PAD_DragXValue,
	PAD_PushMotionValueZ,
	PAD_PushMotionValueY,

	TutorialGet,
	TutorialClear,

	Network_LoginServerIP,
	Network_ChangeUserID,
	Network_RevertUserID,

	Map_Move,

	Debug_Draw_Position,
	DebugScreenMessage,
	Debug_MyRadius,
	Debug_ShowChatRecordUI_Speaker,
	Debug_WidgetComponentVisible,

	Test_Throw_Shield,
	Test_NoCool,
	Test_TID,
	Network_Voice_WebSocketURL,
	Network_Voice_WebSocketURL2,
	Network_Voice_DebugVoiceToText,

	PostProcess_EnableHP,

	ServerCheat,
	Cheat_VoiceTID,
	FootPrintDuration,

	ChatRecording_Time, // 번역 입력 시간
	Voice_DefaultTime, // 보이스 입력 기본 시간
	Voice_ContentTime, // 특정 컨텐츠 들에서 서브 Command 를 이용한 보이스 입력 시간
	Voice_RetryCount,
	Voice_RetryDialogueTID, // 디폴트 음성인식 재 실행 시 나올 Dialogue
	Voice_Icon_Time,	// 음성 아이콘 유지 시간
	Voice_SpeakAni_CoolTime,	// 음성 관련 애니의 쿨타임 시간


	Input_DoubleClickTime,
	Input_PressHoldingTime,
	LanguageType,
	CommonNPCTalk_LoadingTime,
	AbandonedThingImpactSound,

	ShowDebugUI,

	TranslucencySortPriority4VFX,
	TranslucencySortPriority4Revive,
	TranslucencySortPriority4UI,

	HookAnywhere,

	WorkingWithModularCharacters,

	TargetLocationGuide_Degree,	// TargetLocationGuide에서 Search 영역을 위해 사용하는 Degree
	TLG_SearchedEffectCoolTime,	// TLG = TargetLocationGuide

	Voice_NormalizeOn,
	Voice_Normalize,
	Voice_GoogleSTTOff,
	Voice_WavFileSave,
	Voice_UseVoiceModule,
	Voice_MicThreshold,
	Voice_DefaultDelayTime,
	Voice_ContentDelayTime,
	Voice_SelectDialogueTime,
	Voice_ChatRecordingDelayTime,
	Voice_SilenceHoldingTime,
	Voice_ChatSilenceHoldingTime,
	Voice_SilenceHoldingInitVolume,
	Voice_VoiceChatVolumeMultiplier,
	Voice_SilenceHoldingInitTime,
	Voice_NoiseVolumeCheckTime,
	Voice_NoiseVolumeScaling,
	Voice_NoneVoiceBreakTime,
	Voice_BreakVoiceDuration,
	Voice_ChatRecordingTime,
	Voice_NoneChatVoiceStopCount,
	Voice_SendChatBufferSize,
	Voice_ChatAudioPlayLoopTime,
	Voice_MinimumSendingDuration,
	Voice_AlwaysChatNoiseCheckTime,
	Voice_MicVolume,
	Voice_MicVolumeMultiplier,
	Voice_MicPitchMultiplier,
	Voice_MicNormalizeOn,
	Voice_VideoChatVolume,
	Voice_VideoChatMultiplier,
	Voice_FocusTalkVolume,
	Voice_AttenuateDistNormal,
	Voice_AttenuateDistFocusTalk,
	Voice_AttenuateDistStandMic,
	Voice_AttenuateShapeNormal,
	Voice_AttenuateShapeFocusTalk,
	Voice_AttenuateShapeStandMic,

	VoiceChat_BinauralRadius,
	VoiceChat_DefaultVolume,
	VoiceChat_FallOffDistance,
	VoiceChat_TLG_HoldingTime,	//	TLG = Target Location Guide

	PS4_Voice_Default_Volume,
	PS4_Voice_Minimum_Volume,
	PS4_Voice_Attenuation_Ignore_Distance,
	PS4_Voice_3D_Ignore_Distance,

	PS4_Voice_Distance_1,
	PS4_Voice_Distance_2,
	PS4_Voice_Distance_3,
	PS4_Voice_Distance_4,
	PS4_Voice_Distance_5,

	PS4_Voice_Volume_1,
	PS4_Voice_Volume_2,
	PS4_Voice_Volume_3,
	PS4_Voice_Volume_4,
	PS4_Voice_Volume_5,

	OP_Collect_SyncLoadAssetName,

	CustomizeDistance,
	CustomizeWaveHeight,
	CustomizeMovementSpeed,
	CustomizeDegree,
	CustomizeWireframeDuration,
	CustomizeMessageBox,
	CustomizePresetHeight,
	CustomizeCameraMovementLength,
	CustomizeCameraMovementSpeed,
};
