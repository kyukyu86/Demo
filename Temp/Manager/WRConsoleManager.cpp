// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRConsoleManager.h"

#include "HAL/IConsoleManager.h"
#include "ConfigCacheIni.h"
#include "CoreGlobals.h"
#include "Paths.h"

#include "WRCharacterManager.h"
#include "WRVFXManager.h"
#include "WRMaterialParameterCollectionManager.h"
#include "WRInputSystemManager.h"
#include "WRTODManager.h"
#include "WRSFXManager.h"
#include "WRTutorialManager.h"
#include "WRUIManager.h"
#include "WRAccountManager.h"
#include "WRObjectInteractionManager.h"
#include "WRAsyncLoadUObjectManager.h"
#include "WRInventoryManager.h"
#include "WRPopupManager.h"
#include "WRTargetLocationGuideManager.h"
#include "WRChatBubbleManager.h"
#include "WRCharacterCustomizingManager.h"
#include "WRCharacterCustomizingManager.h"

#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterNPC.h"

#include "Define/WRPathDefine.h"

#include "Utility/WREnumUtility.h"

#include "Components/StereoLayerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Component/Character/WRActorComponentRevive.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Network/IMNetwork.h"
#include "Network/Share/Define/Define_BluePrint.h"

#include "GenericPlatformMisc.h"
#include "WidgetInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "IXRTrackingSystem.h"
#include "Enum/EWRUIEnum.h"
#include "UI/Panel/WRUIPanel_BoardingRequest.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WREnpcTable.h"


#define ADD_CONSOLE_VARIABLE(EnumName, VariableName, Value, HelpText, Flags, CallbackFunc){	\
IConsoleVariable* ConsoleVariable = IConsoleManager::Get().RegisterConsoleVariable(TEXT(#VariableName), Value, HelpText, Flags);\
checkf(ConsoleVariable != nullptr, TEXT("Not Created - VariableName")); \
ConsoleCommand.Add(EnumName, ConsoleVariable);\
if(ConsoleVariable != nullptr && CallbackFunc != nullptr) {	AddCallbackForChange(ConsoleVariable, CallbackFunc); }	\
}

WRConsoleManager::WRConsoleManager()
{
}

WRConsoleManager::~WRConsoleManager()
{
}

//====================================================================================

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRConsoleManager::OnInitialize()
{
	this->CreateConsoleVariables();

#if WITH_EDITOR
	GConfig->LoadFile(*(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")));
#endif 
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.Global"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.Character"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.UI"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.Engine"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetBySystemSettingsIni);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.HMD"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.IO"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.Network"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.Pad"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);
	ApplyCVarSettingsFromIni(TEXT("CustomConsoleSetting.Voice"), *(FPaths::ProjectConfigDir() + FString("CustomConsoleSetting.ini")), ECVF_SetByGameSetting);

#if WITH_EDITOR
	if (GEngine->XRSystem == nullptr)
	{
		GetConsoleVariable(EWRConsole::BlackOutIn_Enable)->Set(false);
	}
#endif // WITH_EDITOR
}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRConsoleManager::OnShutdown()
{
	GAreScreenMessagesEnabled = true;
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRConsoleManager::OnLoadLevelStart()
{

}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRConsoleManager::OnLoadLevelComplete()
{

}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRConsoleManager::OnRenderStart()
{

}

bool WRConsoleManager::GetBool(EWRConsole InEConsole)
{
	if (ConsoleCommand.Contains(InEConsole) == false)
		return false;

	return (ConsoleCommand[InEConsole]->GetInt() > 0) ? true : false;
}

int32 WRConsoleManager::GetInt(EWRConsole InEConsole)
{
	if (ConsoleCommand.Contains(InEConsole) == false)
		return -1;

	return ConsoleCommand[InEConsole]->GetInt();
}

float WRConsoleManager::GetFloat(EWRConsole InEConsole)
{
	if (ConsoleCommand.Contains(InEConsole) == false)
		return -1;

	return ConsoleCommand[InEConsole]->GetFloat();
}

FString WRConsoleManager::GetString(EWRConsole InEConsole)
{
	if (ConsoleCommand.Contains(InEConsole) == false)
		return TEXT("");

	return ConsoleCommand[InEConsole]->GetString();
}

bool WRConsoleManager::SetEngineConsole(const FString& InName, const int32 InValue)
{
	IConsoleVariable* FoundVariable = IConsoleManager::Get().FindConsoleVariable(*InName);
	if (FoundVariable == nullptr)
		return false;

	FoundVariable->Set(InValue);
	return true;
}

bool WRConsoleManager::SetEngineConsole(const FString& InName, const float InValue)
{
	IConsoleVariable* FoundVariable = IConsoleManager::Get().FindConsoleVariable(*InName);
	if (FoundVariable == nullptr)
		return false;

	FoundVariable->Set(InValue);
	return true;
}

//====================================================================================


void WRConsoleManager::CreateConsoleVariables()
{

	//====================================================================================
	// [ 2019-7-10 : magedoga ] Test Code
	ADD_CONSOLE_VARIABLE(
		EWRConsole::TestConsoleInt,	// Console Enum
		TestConsoleInt,				// Console Name
		0,							// Value
		TEXT("My Test Console"),			// Help Hext
		ECVF_Default,				// Flag
		&WRConsoleManager::OnChangeConsole_TestConsoleInt)					// Callback

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TestConsoleFloat,
		TestConsoleFloat,
		1.0f,
		TEXT("My Test Console"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TestConsoleString,
		TestConsoleString,
		TEXT("TestText"),
		TEXT("My Test Console"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TestConsole,
		TestConsole,
		//0,
		TEXT("TestText"),
		TEXT("My Test Console"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TestFunction)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TestYSY,
		wr.TestYSY,
		0,
		TEXT("Ysy Test Console"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TestYSY)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TestYSY2,
		wr.TestYSY2,
		0,
		TEXT("Ysy Test Console"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Test_ASpawnNPC,
		wr.Test.ASpawnNPC,
		0,
		TEXT("Async SpawnNPC by TID "),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TestASpawnNPC)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Test_SSpawnNPC,
		wr.Test.SSpawnNPC,
		0,
		TEXT("Sync SpawnNPC by TID "),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TestSSpawnNPC)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Async_Spawn_Actor,
		wr.Test.AsyncSpawn,
		0,
		TEXT("Sync AsyncSpawn"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TestSSpawnNPC)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Test_360Movie_3DUse,
		wr.Test.360Movie3DUse,
		1,
		TEXT("360Movie3DUse"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Test_NoFaction,
		wr.Test.NoFaction,
		0,
		TEXT("NoFaction"),
		ECVF_Default,
		nullptr)
		

	//====================================================================================


	ADD_CONSOLE_VARIABLE(
		EWRConsole::VR_DebugDrawLine_Axis,
		wr.VR.DebugDrawAxis,
		0,
		TEXT("Debug draw for forward axis line\n")
		TEXT("0 : off (Default)\n")
		TEXT("1 (Equal or More) : HMD only\n")
		TEXT("2 (Equal or More) : 1 + Actor\n")
		TEXT("3 (Equal or More) : 2 + Camera"),
		ECVF_Default,
		nullptr)


	ADD_CONSOLE_VARIABLE(
		EWRConsole::VR_StartHeight,
		wr.VR.StartHeight,
		88.0f,
		TEXT("Setting for start HMD height"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationMode,
		wr.Character.RotationMode,
		0,
		TEXT("For Character Rotation\n")
		TEXT("0 : Use Right Thumbstick(Default)\n")
		TEXT("1 : Use Right Thumbstick per 1 action\n")
		TEXT("2 : Right Thumbstick 1 Action : Continuous / Double Action : Fixed Rotation Angle\n")
		TEXT("3 : Use HMD Direction"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_CharacterRotationMode)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationAnglePerAction,
		wr.Character.RotationAnglePerAction,
		45.0f,
		TEXT("Rotation Angle Per Action(wr.Character.RotationMode 1,2 Only)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationAnglePerActionTime,
		wr.Character.RotationAnglePerActionTime,
		0.3f,
		TEXT("Rotation time per 1 action(wr.Character.RotationMode 1,2 Only)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationAngleForTraceHMD,
		wr.Character.RotationAngleForTraceHMD,
		10.0f,
		TEXT("Character rotation angle for  trace the HMD(wr.Character.RotationMode 1,2 Only)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationSpeedForTraceHMD,
		wr.Character.RotationSpeedForTraceHMD,
		1.0f,
		TEXT("Character Rotation Speed When Trace the HMD(wr.Character.RotationMode 1,2 Only)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationSpeedForThumbStick,
		wr.Character.RotationSpeedForThumbStick,
		10.0f,
		TEXT("Character Rotation Speed When Use Thumbstick.(wr.Character.RotationMode 1,2 Only)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationThumbstickMovingDetectValue,
		wr.Character.RotationThumbstickMovingDetectValue,
		0.005f,
		TEXT(""),
		ECVF_Default,
		nullptr)
			
	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_RotationThumbstickMovingIgnoreValue,
		wr.Character.RotationThumbstickMovingIgnoreValue,
		1.0f,
		TEXT(""),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_SyncHMDHeadClampYaw,
		wr.Character.SyncHMDHeadClampYaw,
		90.0f,
		TEXT("Sync HMD & Character Head Rotation Clamp Value)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_SyncHMDHeadClampPitch,
		wr.Character.SyncHMDHeadClampPitch,
		45.f,
		TEXT("Sync HMD & Character Head Rotation Clamp Value)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_SyncHMDHeadClampRoll,
		wr.Character.SyncHMDHeadClampRoll,
		45.f,
		TEXT("Sync HMD & Character Head Rotation Clamp Value)"),
		ECVF_Default,
		nullptr)
			


	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_SyncHMDHeadReqTime,
		wr.Character.SyncHMDHeadReqTime,
		0.2f,
		TEXT("Sync HMD & Character Head Rotation Request to Server Time)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_SyncHMDDist,
		wr.Character.SyncHMDDist,
		25.0f,
		TEXT("Sync HMD & Character Head Rotation Request to Server Based on Head Move Distance)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_3rdCharacterCameraOffsetZ,
		wr.Character.3rdCharacterCameraOffsetZ,
		0.0f,
		TEXT("Camera OffsetZ when showing 3rd character"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_3rdCharacterCameraOffsetX,
		wr.Character.3rdCharacterCameraOffsetX,
		40.f,
		TEXT("Camera OffsetX when showing 3rd character"),
		ECVF_Default,
		nullptr)


	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_3rdCharacterCameraOffsetZForDance,
		wr.Character.3rdCharacterCameraOffsetZForDance,
		200.0f,
		TEXT("Camera OffsetZ when showing 3rd character For Dance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Character_3rdCharacterCameraOffsetXForDance,
		wr.Character.3rdCharacterCameraOffsetXForDance,
		-400.f,
		TEXT("Camera OffsetX when showing 3rd character For Dance"),
		ECVF_Default,
		nullptr)







	ADD_CONSOLE_VARIABLE(
		EWRConsole::Cable_CollisionRadius,
		wr.Cable.CollisionRadius,
		30.0f,
		TEXT("Judgement for arrival cable endpoint (unit:cm)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Cable_CutDirectionTime,
		wr.Cable.CutDirectionTime,
		5.0f,
		TEXT("Attaching cable time"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Camera_MaxPitchAngleWhenHMDOff,
		wr.Character.CameraMaxPitchAngleWhenHMDOff,
		70.0f,
		TEXT("Camera Rotation Limit Pitch When HMD Off."),
		ECVF_Default,
		nullptr)

	//====================================================================================

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Camera_DeltaPitchPerSecond,
		wr.Character.CameraDeltaPitchPerSecond,
		90.0f,
		TEXT("Camera Rotation Delta Pitch Angle Per Second."),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::VehicleSpawnDirection,
		wr.Vehicle.SpawnDirectionDuration,
		2.0f,
		TEXT("Spawn Direction Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::VehicleMaxVolume,
		wr.Vehicle.MaxVolume,
		1.0f,
		TEXT("Max Volume"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Sound_BGMDuration,
		wr.Sound.BGMDuration,
		1.0f,
		TEXT("BGM Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CCD_Radius,
		wr.VFX.CCDRadius,
		60.0f,
		TEXT("CCD Radius"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Spawn_Distribution,
		wr.VFX.SpawnDistribution,
		1,
		TEXT("Spawn Distribution"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TOD_DayCycle,
		wr.TOD.DayCycle,
		24.0f,
		TEXT("TOD DayCycle"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TODDayCycle)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TOD_TimeType,
		wr.TOD.TimeType,
		1,
		TEXT("TOD TimeType"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TODTimeType)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HitRed_Duration,
		wr.PP.HitRedDuration,
		1.0f,
		TEXT("HitRed Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HitBlue_Duration,
		wr.PP.HitBlueDuration,
		2.5f,
		TEXT("HitBlue Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Dodge_Duration,
		wr.PP.DodgeDuration,
		0.75f,
		TEXT("Dodge Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::SkillAttack_Duration,
		wr.CP.SkillAttackDuration,
		1.0f,
		TEXT("SkillAttack Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Dash_Duration,
		wr.PP.DashDuration,
		0.2f,
		TEXT("Dash Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Dash_Ratio,
		wr.PP.DashRatio,
		0.7f,
		TEXT("Dash Ratio"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Add_Impulse,
		wr.PA.AddImpulse,
		1000.0f,
		TEXT("Add Impulse"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Movement_Amount,
		wr.PA.MovementAmount,
		0.1f,
		TEXT("Movement Amount"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Dead_Duration,
		wr.PP.DeadDuration,
		1.0f,
		TEXT("Dead_Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::BloodDecal_Duration,
		wr.Character.BloodDecalDuration,
		10.0f,
		TEXT("BloodDecal_Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::WhiteOutIn_Duration,
		wr.PP.WhiteOutInDuration,
		4.0f,
		TEXT("WhiteOutIn_Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::BlackOutIn_Enable,
		wr.PP.BlackOutInEnable,
		false,
		TEXT("BlackOutIn_Enable"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::BlackOutIn_Duration,
		wr.PP.BlackOutInDuration,
		0.3f,
		TEXT("BlackOutIn_Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::BlackOutIn_Ratio,
		wr.PP.BlackOutInRatio,
		0.2f,
		TEXT("BlackOutIn_Ratio"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::MicVolumeMultiplier,
		wr.Character.MicVolumeMultiplier,
		1.0f,
		TEXT("Mic_VolumeMultiplier"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::EnableRemoveWav,
		wr.Character.EnableRemoveWav,
		1,
		TEXT("Enable_Remove_Wav"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::EnableDistanceCulling,
		wr.Character.EnableDistanceCulling,
		0,
		TEXT("Enable_Distance_Culling"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CullingDistance,
		wr.Character.CullingDistance,
		5000.0f,
		TEXT("Culling_Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::ResponseTime,
		wr.Character.ResponseTime,
		5.0f,
		TEXT("ResponseTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CullingRefreshTime,
		wr.Character.CullingRefreshTime,
		0.5f,
		TEXT("Culling_RefreshTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_TargettingRayLength,
		wr.HMD.TargettingRayLength,
		2500.f,
		TEXT("HMD Targetting Ray Length"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_TargettingCountMax,
		wr.HMD.TargettingCountMax,
		3,
		TEXT("HMD Targetting Count Max"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_TargettingSphereRadius,
		wr.HMD.TargettingSphereRadius,
		95.f,
		TEXT("HMD Targetting Sphere Radius"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_TargettingRotIgnoreAngle,
		wr.HMD.TargettingRotIgnoreAngle,
		5.f,
		TEXT("HMD Targetting Rot Ignore Angle"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_TargettingHoldingTime,
		wr.HMD.TargettingHoldingTime,
		1.f,
		TEXT("HMD Targetting Holding Time"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_Movie_Distance,
		wr.HMD.MovieDistance,
		10.f,
		TEXT("HMD Movie Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_TopAttack_Degree,
		wr.HMD.TopAttackDegree,
		0,
		TEXT("HMD TopAttack Degree"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_BottomAttack_Degree,
		wr.HMD.BottomAttackDegree,
		-10,
		TEXT("HMD BottomAttack Degree"),
		ECVF_Default,
		nullptr)
			
	//====================================================================================


	ADD_CONSOLE_VARIABLE(
		EWRConsole::CommonUI_FixedSize,
		wr.CommonUI_FixedSize,
		17.5f,
		TEXT("CommonUI_FixedSize"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TargetUI_FixedSize,
		wr.TargetUI_FixedSize,
		17.5f,
		TEXT("TargetUI_FixedSize"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::SpeakDialogueIcon_FixedSize,
		wr.SpeakDialogueIcon_FixedSize,
		17.5f,
		TEXT("SpeakDialogueIcon_FixedSize"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::SelectDialogue_FixedSize,
		wr.SelectDialogue_FixedSize,
		35.f,
		TEXT("SelectDialogue_FixedSize"),
		ECVF_Default,
		nullptr)


	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_Cheat,
		wr.UI.Cheat,
		0,
		TEXT("UI Cheat"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_OpenCheat)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_KeepFocusToLastUI,
		wr.UI.KeepFocusToLastUI,
		0,
		TEXT("UI KeepFocusToLastUI"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_KeepFocusToLastUI)


	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_CyberAniSpeed,
		wr.UI.CyberAniSpeed,
		3.f,
		TEXT("UI CyberAniSpeed"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_3DMarkDIst,
		wr.UI.3DMarkDIsts,
		15000,
		TEXT("UI 3DMarkDIsts"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_IdleSpeechTime,
		wr.UI.IdleSpeechTime,
		3.f,
		TEXT("UI IdleSpeechTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_DefaultStyleNoticeTime,
		wr.UI.DefaultStyleNoticeTime,
		3.f,
		TEXT("UI DefaultStyleNoticeTime"),
		ECVF_Default,
		nullptr)
			

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_DefaultChatBubbleTime,
		wr.UI.DefaultChatBubbleTime,
		3.f,
		TEXT("UI DefaultChatBubbleTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_ChatBubbleLimitCount,
		wr.UI.ChatBubbleLimitCount,
		4,
		TEXT("UI ChatBubbleLimitCount"),
		ECVF_Default,
		nullptr)
		

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_MaxZoomScale_FocusButton,
		wr.UI.MaxZoomScale_FocusButton,
		1.5f,
		TEXT("UI MaxZoomScale_FocusButton"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_ZoomTime_FocusButton,
		wr.UI.ZoomTime_FocusButton,
		0.8f,
		TEXT("UI ZoomSpeed_FocusButton"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_FadeOutTime_ClosePanel,
		wr.UI.FadeOutTime_ClosePanel,
		0.5f,
		TEXT("UI FadeOutTime_ClosePanel"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_AutoCloseSocialMenu_Distance,
		wr.UI.AutoCloseSocialMenu.Distance,
		3000,
		TEXT("UI_AutoCloseSocialMenu_Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_AutoCloseSocialMenu_Degree,
		wr.UI.AutoCloseSocialMenu.Degree,
		120.f,
		TEXT("UI_AutoCloseSocialMenu_Degree"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_GuideOn,
		wr.UI.HmdGuideOn,
		0,
		TEXT("Play HMD Guide On Startmap"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_FocusTalkCoolTime,
		wr.UI.FocusTalkCoolTime,
		30.f,
		TEXT("UI_FocusTalkCoolTime"),
		ECVF_Default,
		nullptr)

		

	//====================================================================================

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_CollisionShow,
		wr.IO.CollisionShow,
		false,
		TEXT("IO Collision Debug Show"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_IORecognitionCollisionShow)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_NoneTargetHighLightValue,
		wr.IO.NoneTargetHighLightValue,
		0.08f,
		TEXT("IO None Target HighLight Value"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_TargetHighLightValue,
		wr.IO.TargetHighLightValue,
		0.56f,
		TEXT("IO Target HighLight Value"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_TargetUIFixedSize,
		wr.IO.TargetUIFixedSize,
		30.0f,
		TEXT("IO Target UI Fixed Size"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_HookDashEffectDuration,
		wr.IO.HookDashEffectDuration,
		0.3f,
		TEXT("IO Hook Dash Effect Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_IgnoreQuest,
		wr.IO.IgnoreQuest,
		false,
		TEXT("IO Ignore Quest"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_Lifespan,
		wr.IO.Lifespan,
		10.0f,
		TEXT("IO Lifespan"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_ThrowSpeed,
		wr.IO.ThrowSpeed,
		1000.0f,
		TEXT("IO Throw Speed"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_ThrowDegree,
		wr.IO.ThrowDegree,
		30.0f,
		TEXT("IO Throw Degree"),
		ECVF_Default,
		nullptr)

	//====================================================================================
	// [ 2019-11-13 : magedoga ] IK

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IK_LookIKDistanceRatio,
		wr.IK.LookIKDistanceRatio,
		1.0f,
		TEXT("Distance ratio for Look IK"),
		ECVF_Default,
		nullptr);

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IK_LookIKLimitPitch,
		wr.IK.LookIKLimitPitch,
		45.0f,
		TEXT("Limit pitch angle for Look IK"),
		ECVF_Default,
		nullptr);

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IK_LookIKLimitYaw,
		wr.IK.LookIKLimitYaw,
		45.0f,
		TEXT("Limit yaw angle for Look IK"),
		ECVF_Default,
		nullptr);

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IK_LookIKLimitYawForDialog,
		wr.IK.LookIKLimitYawForDialog,
		70.0f,
		TEXT("Limit yaw angle for Dialog"),
		ECVF_Default,
		nullptr);


	ADD_CONSOLE_VARIABLE(
		EWRConsole::IK_LookIKBlendSpeed,
		wr.IK.LookIKBlendSpeed,
		1.0f,
		TEXT("Limit yaw angle for Look IK"),
		ECVF_Default,
		nullptr);

	//====================================================================================

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Anim_AvoidSectionRate,
		wr.Anim.AvoidRate,
		0.15f,
		TEXT("Set AvoidSection AnimRate"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Anim_BlendSpaceTransformSpeed,
		wr.Anim.BlendSpaceTransformSpeed,
		0.02f,
		TEXT("Transform Speed, Walk and Run"),
		ECVF_Default,
		nullptr);

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Anim_DisplayURODebug,
		wr.Anim.DisplayURODebug,
		0,
		TEXT("Display URO debug info, if enabled update rate optimization.\n")
		TEXT("0 : Off\n")
		TEXT("1 : On\n"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_DisplayURODebug)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_PlayForceFeedback,
		wr.Pad.PlayForceFeedback,
		0,
		TEXT("PAD Play ForceFeedback"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_PlayForceFeedback)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_StopForceFeedback,
		wr.Pad.StopForceFeedback,
		0,
		TEXT("PAD Stop ForceFeedback"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_StopForceFeedback)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_PullMotionValueZ,
		wr.Pad.PullMotionValueZ,
		3000.f,
		TEXT("PAD Pull Motion Value"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_PullMotionValueZ)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_PullMotionValueY,
		wr.Pad.PullMotionValueY,
		0,
		TEXT("PAD Pull Motion Y Value"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_PullMotionValueY)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_DragYValue,
		wr.Pad.DragYValue,
		1.f,
		TEXT("PAD DragY Value"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_DragXValue,
		wr.Pad.DragXValue,
		1.f,
		TEXT("PAD DragX Value"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_PushMotionValueZ,
		wr.Pad.PushMotionValueZ,
		-3000.f,
		TEXT("PAD Push Motion Value"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_PushMotionValueZ)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PAD_PushMotionValueY,
		wr.Pad.PushMotionValueY,
		0,
		TEXT("PAD Push Motion Y Value"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_PushMotionValueY)

	//====================================================================================

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TutorialGet,
		wr.Tutorial.Get,
		0,
		TEXT("Tutorial Get"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TutorialGet)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TutorialClear,
		wr.Tutorial.Clear,
		0,
		TEXT("Tutorial All Clear"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TutorialClear)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Network_LoginServerIP,
		wr.Network.LoginServerIP,
		TEXT("172.20.50.40"),
		TEXT("Change Network Login Server IP Address"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_NetworkLoginServerIP)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Debug_Draw_Position,
		wr.Draw.Debug,
		0,
		TEXT("Draw Debug"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::DebugScreenMessage,
		wr.Debug.ScreenMessage,
		1,
		TEXT("On/Off Screen Message"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_DebugScreenMessage)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Debug_MyRadius,
		wr.Debug.MyRadius,
		0,
		TEXT("Debug_MyRadius"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Debug_ShowChatRecordUI_Speaker,
		wr.Debug.ShowChatRecordUI_Speaker,
		0,
		TEXT("Debug_ShowChatRecordUI_Speaker"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Debug_WidgetComponentVisible,
		wr.Debug.WidgetComponentVisible,
		1,
		TEXT("Debug_WidgetComponentVisible"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_WidgetComponentVisible)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Test_Throw_Shield,
		wr.Test.Shield,
		0,
		TEXT("Test Shield"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Test_NoCool,
		wr.Test.NoCoolTime,
		0,
		TEXT("Test CoolTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Test_TID,
		wr.Test.TID,
		0,
		TEXT("Test TID"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Network_Voice_WebSocketURL,
		wr.Network.VoiceWebsocketURL,
		TEXT("ws://172.20.50.33:80/client/ws/speech"),
		TEXT("Change Network Voice WebSocket URL"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Network_Voice_WebSocketURL2,
		wr.Network.VoiceWebsocketURL2,
		TEXT("ws://172.20.50.13:80/client/ws/speech"),
		TEXT("Change Network Voice WebSocket URL2"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Network_Voice_DebugVoiceToText,
		wr.Network.DebugVoiceToText,
		0,
		TEXT("Display Voice To Text From Server"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Network_ChangeUserID,
		wr.Network.ChangeUserID,
		TEXT(""),
		TEXT("Change Account User ID"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_ChangeUserID)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Network_RevertUserID,
		wr.Network.RevertUserID,
		TEXT(""),
		TEXT("Revert Account User ID"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_RevertUserID)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::IO_InteractionSkip,
		wr.IO.Skip,
		0,
		TEXT("IO Interaction Skip"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_InteractionSkip)

//====================================================================================

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Voice_NormalizeOn,
		wr.Voice.NormailizeOn,
		0,
		TEXT("Wave volume normalize On/Off"),
		ECVF_Default,
		nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_Normalize,
			wr.Voice.Normailize,
			0.25f,
			TEXT("Wave volume normalize"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_GoogleSTTOff,
			wr.Voice.GoogleSTTOff,
			0,
			TEXT("Google STT On/Off"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_WavFileSave,
			wr.Voice.WavFileSave,
			0,
			TEXT("Recording Wav File Save"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_UseVoiceModule,
			wr.Voice.UseVoiceModule,
			0,
			TEXT("Use Voice Module"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_MicThreshold,
			wr.Voice.MicThreshold,
			0.04f,
			TEXT("MicThreshold"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_DefaultDelayTime,
			wr.Voice.DefaultDelayTime,
			1.0f,
			TEXT("Voice DefaultDelayTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_ContentDelayTime,
			wr.Voice.ContentDelayTime,
			1.0f,
			TEXT("Voice ContentDelayTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_SelectDialogueTime,
			wr.Voice.SelectDialogueTime,
			1.0f,
			TEXT("Voice SelectDialogueTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_ChatRecordingDelayTime,
			wr.Voice.ChatRecordingDelayTime,
			1.0f,
			TEXT("Voice DefaultDelayTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_SilenceHoldingTime,
			wr.Voice.SilenceHoldingTime,
			1.0f,
			TEXT("Voice SilenceHoldingTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_ChatSilenceHoldingTime,
			wr.Voice.ChatSilenceHoldingTime,
			1.0f,
			TEXT("Voice ChatSilenceHoldingTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_SilenceHoldingInitVolume,
			wr.Voice.SilenceHoldingInitVolume,
			5.0f,
			TEXT("Voice SilenceHoldingInitVolume"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_VoiceChatVolumeMultiplier,
			wr.Voice.VoiceChatVolumeMultiplier,
			5.0f,
			TEXT("Voice VoiceChatVolumeMultiplier"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_SilenceHoldingInitTime,
			wr.Voice.SilenceHoldingInitTime,
			1.0f,
			TEXT("Voice SilenceHoldingInitTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_NoiseVolumeCheckTime,
			wr.Voice.NoiseVolumeCheckTime,
			0.3f,
			TEXT("Voice NoiseVolumeCheckTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_NoiseVolumeScaling,
			wr.Voice.NoiseVolumeScaling,
			1.2f,
			TEXT("Voice NoiseVolumeScaling"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_NoneVoiceBreakTime,
			wr.Voice.NoneVoiceBreakTime,
			0.3f,
			TEXT("Voice NoneVoiceBreakTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_BreakVoiceDuration,
			wr.Voice.BreakVoiceDuration,
			0.2f,
			TEXT("Voice BreakVoiceDuration"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_ChatRecordingTime,
			wr.Voice.ChatRecordingTime,
			6.0f,
			TEXT("Voice ChatRecordingTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_NoneChatVoiceStopCount,
			wr.Voice.NoneChatVoiceStopCount,
			10,
			TEXT("Voice NoneChatVoiceStopCount"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_SendChatBufferSize,
			wr.Voice.SendChatBufferSize,
			10000,
			TEXT("Voice SendChatBufferSize"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_ChatAudioPlayLoopTime,
			wr.Voice.ChatAudioPlayLoopTime,
			1.0f,
			TEXT("Voice ChatAudioPlayLoopTime"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_MinimumSendingDuration,
			wr.Voice.MinimumSendingDuration,
			0.1f,
			TEXT("Voice MinimumSendingDuration"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_AlwaysChatNoiseCheckTime,
			wr.Voice.AlwaysChatNoiseCheckTime,
			2.0f,
			TEXT("Voice AlwaysChatNoiseCheckTime"),
			ECVF_Default,
			nullptr)


		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_MicVolume,
			wr.Voice.MicVolume,
			100.f,
			TEXT("Voice Mic Volume"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_MicVolumeMultiplier,
			wr.Voice.MicVolumeMultiplier,
			100.f,
			TEXT("Voice Mic Volume Multiplier"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_MicPitchMultiplier,
			wr.Voice.MicPitchMultiplier,
			100.f,
			TEXT("Voice Mic Pitch Multiplier"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_MicNormalizeOn,
			wr.Voice.MicNormailizeOn,
			1,
			TEXT("Mic volume normalize On/Off"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_VideoChatVolume,
			wr.Voice.VideoChatVolume,
			1.f,
			TEXT("Voice VideoChat Volume"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_VideoChatMultiplier,
			wr.Voice.VideoChatMultiplier,
			5.f,
			TEXT("Voice VideoChat Multiplier"),
			ECVF_Default,
			nullptr)

		ADD_CONSOLE_VARIABLE(
			EWRConsole::Voice_FocusTalkVolume,
			wr.Voice.FocusTalkVolume,
			1.f,
			TEXT("Voice FocusTalk Volume"),
			ECVF_Default,
			nullptr)
	//====================================================================================

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Slice_DirectionWeight,
		wr.Slice.DirectionWeight,
		2.5f,
		TEXT("Slice Impulse DirectionWeight"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Slice_ActiveMultiSlice,
		wr.Slice.ActiveMultiSlice,
		0,
		TEXT("Slice ActiveMultiSlice"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Slice_ActiveImpulse,
		wr.Slice.ActiveImpulse,
		1,
		TEXT("Slice ActiveImpulse"),
		ECVF_Default,
		nullptr)
	// [ 2019-8-26 : magedoga ] 콘솔명은 반드시 "wr.카테고리.내용" 
	// ex) 캐릭터의 이동속도 -> wr.Chracter.MoveSpeed


	ADD_CONSOLE_VARIABLE(
		EWRConsole::PostProcess_EnableHP,
		wr.PostProcess.EnableHP,
		1,
		TEXT("PostProcess Effect for Hit (HP)"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::ServerCheat,
		wr.ServerCheat,
		TEXT(""),
		TEXT("ServerCheat"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_ServerCheat)


	ADD_CONSOLE_VARIABLE(
		EWRConsole::Cheat_VoiceTID,
		wr.Cheat.VoiceTID,
		0,
		TEXT("Cheat_VoiceTID"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::FootPrintDuration,
		wr.AnimNotify.FootPrintDuration,
		5.f,
		TEXT("FootPrintDuration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HMD_WidgetInteractionLay,
		wr.HMD.WidgetInteractionLay,
		0,
		TEXT("Widget Interaction Lay"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_ShowWidgetInteractionLay)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::ChatRecording_Time,
		wr.ChatRecording.Time,
		6.f,
		TEXT("ChatRecording_Time"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Voice_DefaultTime,
		wr.Voice.DefaultTime,
		6.f,
		TEXT("Voice_DefaultTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Voice_ContentTime,
		wr.Voice.ContentTime,
		2.5f,
		TEXT("Voice_ContentTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Voice_RetryCount,
		wr.Voice.RetryCount,
		2,
		TEXT("Voice RetryCount"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Voice_RetryDialogueTID,
		wr.Voice.RetryDialogueTID,
		3151,
		TEXT("Voice RetryDialogueTID"),
		ECVF_Default,
		nullptr)	

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Voice_Icon_Time,
		wr.Voice.IconTime,
		1.f,
		TEXT("Voice_Icon_Time"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Voice_SpeakAni_CoolTime,
		wr.Voice.SpeakAni_CoolTime,
		6.f,
		TEXT("Voice_SpeakAni_CoolTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE( EWRConsole::Voice_AttenuateDistNormal, 
		wr.Voice.AttenuateDistNormal, 
		1375.f,
		TEXT("Voice_AttenuateDistNormal"), ECVF_Default, nullptr)

	ADD_CONSOLE_VARIABLE( EWRConsole::Voice_AttenuateDistFocusTalk, 
		wr.Voice.AttenuateDistFocusTalk, 
		1375.f,
		TEXT("Voice_AttenuateDistFocusTalk"),	ECVF_Default, nullptr)

	ADD_CONSOLE_VARIABLE( EWRConsole::Voice_AttenuateDistStandMic, 
		wr.Voice.AttenuateDistStandMic, 
		1500.f,
		TEXT("Voice_AttenuateDistStandMic"), ECVF_Default, nullptr)

	ADD_CONSOLE_VARIABLE( EWRConsole::Voice_AttenuateShapeNormal, 
		wr.Voice.AttenuateShapeNormal, 
		300.f, 
		TEXT("Voice_AttenuateShapeNormal"), ECVF_Default,	nullptr)

	ADD_CONSOLE_VARIABLE( EWRConsole::Voice_AttenuateShapeFocusTalk, 
		wr.Voice.AttenuateShapeFocusTalk, 
		300.f,
		TEXT("Voice_AttenuateShapeFocusTalk"), ECVF_Default, nullptr)

	ADD_CONSOLE_VARIABLE( EWRConsole::Voice_AttenuateShapeStandMic,	
		wr.Voice.AttenuateShapeStandMic, 
		400.f,
		TEXT("Voice_AttenuateShapeStandMic"),	ECVF_Default, nullptr)
					   		

	ADD_CONSOLE_VARIABLE(
		EWRConsole::VoiceChat_BinauralRadius,
		wr.VoiceChat.BinauralRadius,
		300,
		TEXT("VoiceChat_BinauralRadius"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::VoiceChat_DefaultVolume,
		wr.VoiceChat.DefaultVolume,
		1.0f,
		TEXT("VoiceChat_DefaultVolume"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::VoiceChat_FallOffDistance,
		wr.VoiceChat.FallOffDistance,
		2000,
		TEXT("VoiceChat_FallOffDistance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::VoiceChat_TLG_HoldingTime,
		wr.VoiceChat.TLG_HoldingTime,
		10.f,
		TEXT("VoiceChat_FallOffDistance"),
		ECVF_Default,
		nullptr)
			


	ADD_CONSOLE_VARIABLE(
		EWRConsole::LanguageType,
		wr.LanguageType,
		0,
		TEXT("LanguageType"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_LanguageType)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Input_DoubleClickTime,
		wr.Input.DoubleClickTime,
		0.3f,
		TEXT("Input DoubleClickTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Input_PressHoldingTime,
		wr.Input.PressHoldingTime,
		1.f,
		TEXT("Input PressHoldingTime"),
		ECVF_Default,
		nullptr)

		

	ADD_CONSOLE_VARIABLE(
		EWRConsole::Map_Move,
		wr.Map.Move,
		0,
		TEXT(" "),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_MapMove)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CommonNPCTalk_LoadingTime,
		wr.CommonNPCTalk_LoadingTime,
		2.5f,
		TEXT("CommonNPCTalk_LoadingTime"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::AbandonedThingImpactSound,
		wr.AbandonedThingImpactSound,
		100.f,
		TEXT("AbandonedThingImpactSound"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TranslucencySortPriority4VFX,
		wr.TranslucencySortPriority4VFX,
		100,
		TEXT("TranslucencySortPriority4VFX"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TranslucencySortPriority4Revive,
		wr.TranslucencySortPriority4Revive,
		1000,
		TEXT("TranslucencySortPriority4Revive"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TranslucencySortPriority4UI,
		wr.TranslucencySortPriority4UI,
		1001,
		TEXT("TranslucencySortPriority4UI"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::ShowDebugUI,
		wr.ShowDebugUI,
		TEXT(""),
		TEXT("ShowDebugUI"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_ShowDebugUI)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::HookAnywhere,
		wr.HookAnywhere,
		0,
		TEXT("Hook Anywhere"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TestKyu,
		wr.TestKyu,
		0,
		TEXT("Kyu Test Console"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_TestKyu)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::WorkingWithModularCharacters,
		wr.Character.IsWorkingWithModularCharacters,
		0,
		TEXT("Is Working With Modular Characters"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TargetLocationGuide_Degree,
		wr.TargetLocationGuide.Degree,
		30.f,
		TEXT("TargetLocationGuide_Degree"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::TLG_SearchedEffectCoolTime,
		wr.TLG.SearchedEffectCoolTime,
		15,
		TEXT("TLG_SearchedEffectCoolTime"),
		ECVF_Default,
		nullptr)
			
			

	ADD_CONSOLE_VARIABLE(
		EWRConsole::UI_Inventory,
		wr.UI.Inventory,
		0,
		TEXT("Inventory Open"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_OpenInventory)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Default_Volume,
		wr.PS4.VoiceDefalutVolume,
		0.8f,
		TEXT("Voice Default Volume"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Minimum_Volume,
		wr.PS4.VoiceMinimumVolume,
		0.1f,
		TEXT("Voice Minimum Volume"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Attenuation_Ignore_Distance,
		wr.PS4.VoiceAttenuationIgnoreDistance,
		300.f,
		TEXT("Voice Attenuation Ignore Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_3D_Ignore_Distance,
		wr.PS4.Voice3DIgnoreDistance,
		100.f,
		TEXT("Voice 3D Ignore Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Distance_1,
		wr.PS4.PS4_Voice_Distance_1,
		100.f,
		TEXT("Voice Attenuation Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Distance_2,
		wr.PS4.PS4_Voice_Distance_2,
		300.f,
		TEXT("Voice Attenuation Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Distance_3,
		wr.PS4.PS4_Voice_Distance_3,
		500.f,
		TEXT("Voice Attenuation Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Distance_4,
		wr.PS4.PS4_Voice_Distance_4,
		900.f,
		TEXT("Voice Attenuation Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Distance_5,
		wr.PS4.PS4_Voice_Distance_5,
		1200.f,
		TEXT("Voice Attenuation Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Volume_1,
		wr.PS4.PS4_Voice_Volume_1,
		1.f,
		TEXT("Voice Attenuation Volume"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_VoiceVolume1)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Volume_2,
		wr.PS4.PS4_Voice_Volume_2,
		0.8f,
		TEXT("Voice Attenuation Volume"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_VoiceVolume2)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Volume_3,
		wr.PS4.PS4_Voice_Volume_3,
		0.6f,
		TEXT("Voice Attenuation Volume"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_VoiceVolume3)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Volume_4,
		wr.PS4.PS4_Voice_Volume_4,
		0.4f,
		TEXT("Voice Attenuation Volume"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_VoiceVolume4)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::PS4_Voice_Volume_5,
		wr.PS4.PS4_Voice_Volume_5,
		0.2f,
		TEXT("Voice Attenuation Volume"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_VoiceVolume5)


	//====================================================================================

	ADD_CONSOLE_VARIABLE(
		EWRConsole::OP_Collect_SyncLoadAssetName,
		wr.OP.CollectSyncLoadAssetName,
		0,
		TEXT("Collect LoadObject Name\n")
		TEXT("0 : Off\n")
		TEXT("1 : On"),
		ECVF_Default,
		&WRConsoleManager::OnChangeConsole_CollectSyncLoadAssetName)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeDistance,
		wr.Character.CustomizeDistance,
		100.0f,
		TEXT("Customize Distance"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeWaveHeight,
		wr.Character.CustomizeWaveHeight,
		20.0f,
		TEXT("Customize sssWaveHeight"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeMovementSpeed,
		wr.Character.CustomizeMovementSpeed,
		18.0f,
		TEXT("Customize Movement Speed"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeDegree,
		wr.Character.CustomizeDegree,
		-45.0f,
		TEXT("Customize Degree"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeWireframeDuration,
		wr.Character.CustomizeWireframeDuration,
		1.0f,
		TEXT("Customize Wireframe Duration"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeMessageBox,
		wr.animal14,
		1,
		TEXT("Customize MessageBox"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizePresetHeight,
		wr.Character.CustomizePresetHeight,
		40.0f,
		TEXT("Customize Preset Height"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeCameraMovementLength,
		wr.Character.CustomizeCameraMovementLength,
		300.0f,
		TEXT("Customize Camera Movement Length"),
		ECVF_Default,
		nullptr)

	ADD_CONSOLE_VARIABLE(
		EWRConsole::CustomizeCameraMovementSpeed,
		wr.Character.CustomizeCameraMovementSpeed,
		1.0f,
		TEXT("Customize Camera Movement Speed"),
		ECVF_Default,
		nullptr)
}

void WRConsoleManager::AddCallbackForChange(IConsoleVariable* InConsoleVariable, FuncPointer InFuncPointer)
{
	if (InConsoleVariable == nullptr)
		return;

	InConsoleVariable->SetOnChangedCallback(FConsoleVariableDelegate::CreateRaw(this, InFuncPointer));
}

void WRConsoleManager::OnChangeConsole_TestFunction(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	//AWRCharacter* SelectedCharacter = WRCharacterCustomizingManager::Get()->GetSelectedCharacter();
	//if (SelectedCharacter == nullptr)
	//	return;

	//UWRActorComponentEquipmentLook* ActorComponentEquipmentLook = SelectedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	//if (ActorComponentEquipmentLook == nullptr)
	//	return;

	//int32 animal14 = InConsoleVariable->GetInt();
	//if (animal14 < 1000)
	//{
	//	ActorComponentEquipmentLook->Equip(animal14);
	//}
	//else
	//{
	//	int32 animal20 = animal14 % 100;

	//	ActorComponentEquipmentLook->Unequip((EWREquipPart)animal20);
	//}

	WRCharacterCustomizingManager::Get()->animal14(InConsoleVariable->GetString());
}

void WRConsoleManager::OnChangeConsole_TODDayCycle(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Value = InConsoleVariable->GetFloat();

	//WRTODManager::Get()->SetDayCycle(Value);
}

void WRConsoleManager::OnChangeConsole_TODTimeType(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	uint8 Value = InConsoleVariable->GetFloat();

	//WRTODManager::Get()->SetTimeType(Value);
}

//====================================================================================



//====================================================================================

void WRConsoleManager::OnChangeConsole_CharacterRotationMode(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	int32 Value = InConsoleVariable->GetInt();
	AWRCharacterMine* PlayerCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->SetRotationMode((EWRCharacterRotationType)Value);
	}
}

void WRConsoleManager::OnChangeConsole_IORecognitionCollisionShow(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	bool bShow = (bool)InConsoleVariable->GetFloat();
	WRCharacterManager::Get()->OnChangedConsole_IOCollisionShow(bShow);
}

void WRConsoleManager::OnChangeConsole_PlayForceFeedback(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	int Value = InConsoleVariable->GetInt();
	if (Value != 0)
		WRInputSystemManager::Get()->ClientPlayForceFeedback(Value);
}

void WRConsoleManager::OnChangeConsole_StopForceFeedback(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	int Value = InConsoleVariable->GetInt();
	if (Value != 0)
		WRInputSystemManager::Get()->ClientStopForceFeedback(Value);
}

void WRConsoleManager::OnChangeConsole_TestConsoleInt(IConsoleVariable* InConsoleVariable)
{
	if (UWRGameInstance::GetGameInstance() == nullptr)
		return;

	int32 _Value = InConsoleVariable->GetInt();
	
	if (_Value == 1)
	{
		FStringAssetReference SoundRef = FStringAssetReference(TEXT("/Game/Asset/Sound/Sound_Cues/HeartBeat_F.HeartBeat_F"));
		SoundRef.TryLoad();
	}

// 	bool bEnable = InConsoleVariable->GetInt() > 0;
// 
// 	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
// 	if (World == nullptr)
// 		return;
// 
// 	if (WRCharacterManager::IsValid() == false)
// 		return;
// 
// 
// 	AWRCharacter* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
// 	if (MyCharacter == nullptr)
// 		return;
// 
// 	int32 _Value = InConsoleVariable->GetInt();
// 
// 	UCameraComponent* CamComp = Cast<UCameraComponent>(MyCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
// 	if (CamComp->IsValidLowLevel() == true)
// 	{
// 		CamComp->bLockToHmd = _Value == 1 ? true : false;
// 	}
}

void WRConsoleManager::OnChangeConsole_PullMotionValueZ(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Value = InConsoleVariable->GetFloat();

	if (WRInputSystemManager::IsValid() == true)
		WRInputSystemManager::Get()->SetMotionValueZ(EWRPadMotionType::PULL, Value);
}

void WRConsoleManager::OnChangeConsole_PullMotionValueY(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Value = InConsoleVariable->GetFloat();

	if (WRInputSystemManager::IsValid() == true)
		WRInputSystemManager::Get()->SetMotionValueY(EWRPadMotionType::PULL, Value);
}

void WRConsoleManager::OnChangeConsole_PushMotionValueZ(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Value = InConsoleVariable->GetFloat();

	if (WRInputSystemManager::IsValid() == true)
		WRInputSystemManager::Get()->SetMotionValueZ(EWRPadMotionType::PUSH, Value);
}

void WRConsoleManager::OnChangeConsole_PushMotionValueY(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Value = InConsoleVariable->GetFloat();

	if (WRInputSystemManager::IsValid() == true)
		WRInputSystemManager::Get()->SetMotionValueY(EWRPadMotionType::PUSH, Value);
}

void WRConsoleManager::OnChangeConsole_TutorialGet(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRTutorialManager::IsValid() == false)
		return;

	int32 Value = InConsoleVariable->GetInt();

	WRTutorialManager::Get()->OnNewTutorial((WRTableID)Value);
}

void WRConsoleManager::OnChangeConsole_TutorialClear(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRTutorialManager::IsValid() == false)
		return;

	WRTutorialManager::Get()->ClearAllTutorials();
}

void WRConsoleManager::OnChangeConsole_NetworkLoginServerIP(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (IMNetwork::IsValid() == false)
		return;

	FString Value = InConsoleVariable->GetString();

	IMNetwork::Get()->SetLoginIPAdress(*Value);

	WRUIManager* UIManager = WRUIManager::Get();
	if (UIManager)
		UIManager->FocusSavedUI();


}

void WRConsoleManager::OnChangeConsole_MapMove(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (IMNetwork::IsValid() == false)
		return;

	if (WRMapManager::IsValid() == false)
		return;

	if (WRMapManager::Get()->IsLoading())
		return;

	int32 MapTID = InConsoleVariable->GetInt();

	IMMessageSender::RequestReserveMap(MapTID);
}

void WRConsoleManager::OnChangeConsole_OpenCheat(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRUIManager::IsValid() == false)
		return;

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::Cheat))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::Cheat);
	}
	else
	{
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Cheat, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr, WRCharacterManager::Get()->GetMyCharacter());
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Cheat, WRCharacterManager::Get()->GetMyCharacter(), FTransform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, -20.f), FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	}
}

void WRConsoleManager::OnChangeConsole_KeepFocusToLastUI(IConsoleVariable* InConsoleVariable)
{
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	WRUIManager* UIManager = WRUIManager::Get();
	if (UIManager == nullptr)
		return;

	UIManager->FocusSavedUI();
}

void WRConsoleManager::OnChangeConsole_OpenInventory(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRInventoryManager::IsValid() == false)
		return;

	int32 Value = InConsoleVariable->GetInt();

	WRInventoryManager::Get()->OpenInventory((EWRInventoryCategoryType)Value);
}

void WRConsoleManager::OnChangeConsole_ChangeUserID(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRAccountManager::IsValid() == false)
		return;

	FString Value = InConsoleVariable->GetString();
	WRAccountManager::Get()->SetUserIDByConsole(*Value);
}

void WRConsoleManager::OnChangeConsole_RevertUserID(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRAccountManager::IsValid() == false)
		return;

	WRAccountManager::Get()->RevertUserID();
}

void WRConsoleManager::OnChangeConsole_InteractionSkip(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRObjectInteractionManager::IsValid() == false)
		return;

	WRObjectInteractionManager::Get()->CurInteractionSkip();
}

void WRConsoleManager::OnChangeConsole_DebugScreenMessage(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	bool bEnable;
	if (InConsoleVariable->GetInt() == 1)
	{
		bEnable = true;
	}
	else if (InConsoleVariable->GetInt() == 0)
	{
		bEnable = false;
	}
	else
	{
		return;
	}

	GAreScreenMessagesEnabled = bEnable;
}

void WRConsoleManager::OnChangeConsole_ShowWidgetInteractionLay(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == true)
	{
		UWidgetInteractionComponent* WidgetInteractionComponent = Cast<UWidgetInteractionComponent>(MyChar->GetComponentByClass(UWidgetInteractionComponent::StaticClass()));
		if (WidgetInteractionComponent != nullptr)
		{
			int32 Value = InConsoleVariable->GetInt();
			WidgetInteractionComponent->bShowDebug = (Value == 0 ? false : true);
		}
	}
}

void WRConsoleManager::OnChangeConsole_ServerCheat(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	FString Cheat = InConsoleVariable->GetString();
	IMMessageSender::RequestCheat(TEXT("!") + Cheat);
}

void WRConsoleManager::OnChangeConsole_TestYSY(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	// Test용 코드로 TID를 강제로 박았습니다. (치트용)
	AWRCharacter* Character = WRCharacterManager::Get()->FindByTID(20000007);
	//AWRCharacter* MyCharacter =Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (Character)
		WRChatBubbleManager::Get()->OpenChatBubble(Character, FText::FromString(FString("Hello C World!")));

	//WRTargetLocationGuideManager::Get()->AddTargetLocationInfo(WRMapManager::Get()->GetCurrentMapTID(), FVector(40950, 38300, 4850), EWRUISlotKind::FocusTalk, nullptr, true);
	//if (WRUIManager::IsValid() == false)
	//	return;

	//if (InConsoleVariable->GetBool())
	//{
	//	WRUIManager::Get()->ShowAllWidgetComponents();
	//}
	//else
	//{
	//	WRUIManager::Get()->HideAllWidgetComponents();
	//}
}

void WRConsoleManager::OnChangeConsole_TestKyu(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	int32 Value = InConsoleVariable->GetInt();
// 	if (Value == 1)
// 	{
// 		FWRPopupInfo PopupInfo;
// 		PopupInfo.Title = FText::FromString(TEXT("test case 1"));
// 		PopupInfo.Contents = FText::FromString(TEXT("test-1\ntest-2"));
// 		PopupInfo.FirstBtnText = FText::FromString(TEXT("first btn"));
// 		PopupInfo.SecondBtnText = FText::FromString(TEXT("second btn"));
// 		PopupInfo.eButtonFlag = EPopupButtonFlag::OK_CANCEL;
// 		PopupInfo.eOptionFlag = EPopupOptionFlag::None;
//
// 		FWRPopupInfo PopupInfo(
// 			EPopupButtonFlag::OK_CANCEL
// 			, EPopupOptionFlag::None,
// 			, FText::FromString(TEXT("Kyu Title"))
// 			, FText::FromString(TEXT("Kyu Contents\n1234\n123"))
// 			, FOnPopupButtonDelegate::CreateRaw(this, &WRInventoryManager::OnDiscardItem)
// 		);
//
//		WRPopupManager::Get()->OpenPopupOk(PopupInfo);
//	}
// 	else if (Value == 2)
// 	{
// 		WRPopupManager::Get()->OpenPopupOkCancel();
// 	}
// 	else if (Value == 3)
// 	{
// 		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
// 		{
// 			if (InCombinedPanelData.Is3DWidget())
// 			{
// 				InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
// 				UWRUIPanel_BoardingRequest* Panel = Cast<UWRUIPanel_BoardingRequest>(InCombinedPanelData.UIPanel);
// 				if (Panel)
// 				{
// 					FWROnFinishedBoardingRequestTime BoardingTimeDelegate;
// 					BoardingTimeDelegate.BindRaw(this, &WRConsoleManager::OnTestKyu);
// 					Panel->SetBoardingType(EWRBoardingRequestType::Driver, BoardingTimeDelegate, 100, 10, 1);
// 				}
// 			}
// 		});
// 
// 		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::BoardingRequest, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
// 		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 
// 		UWRUIPanel_BoardingRequest* BoardingRequestPanel = WRUIManager::Get()->FindUI<UWRUIPanel_BoardingRequest>(EWRUIPanelKind::BoardingRequest);
// 		if (BoardingRequestPanel)
// 		{
// 			AID_t PassengerAID = BoardingRequestPanel->GetPassengerAID();
// 		}
// 	}

// 	if (Value == 1)
// 	{
// 		WRFSMInfo FSMInfo;
// 		FSMInfo.FSMType = EWRFSM::Dance;
// 		FSMInfo.MontageTID = 1011;
// 		FSMInfo.JumpToSectionName = "Dance_01_loop";
// 		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
// 		if (MyCharacter)
// 		{			
// 			MyCharacter->DetermineFSMSignal(FSMInfo);
// 			UWRAnimInstance* AnimInstance = MyCharacter->GetAnimInstance();
// 			AnimInstance->SetLayeredBlendPerBone(true);
// 		}
// 	}	
// 	else if (Value == 2)
// 	{
// 		WRFSMInfo FSMInfo;
// 		FSMInfo.FSMType = EWRFSM::Dance;
// 		FSMInfo.MontageTID = 5011;
// 		FSMInfo.JumpToSectionName = "Dance_01_loop";
// 		AWRCharacter* TargetCharacter = const_cast<AWRCharacter*>(WRCharacterManager::Get()->GetMyTarget());
// 		if (TargetCharacter)
// 		{
// 			TargetCharacter->DetermineFSMSignal(FSMInfo);
// 			UWRAnimInstance* AnimInstance = TargetCharacter->GetAnimInstance();
// 			AnimInstance->SetLayeredBlendPerBone(true);
// 		}
// 	}
// 	else
// 	{
// 		FWRWind LocalWind;
// 		FWRTODETC LocalETC;
// 		float Duration = 20.f;
// 		WRTODManager::Get()->SpawnTOD(Value, 0, Duration, LocalETC, LocalWind);
// 	}

	if (Value == 1)
	{
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::HouseSelectPopup))
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::HouseSelectPopup);
		}
		else
		{
			FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
			{
				if (InCombinedPanelData.Is3DWidget())
				{
// 	 				UWRUIPanel_TargetUI* Panel_TargetUI = Cast<UWRUIPanel_TargetUI>(InCombinedPanelData.UIPanel);
// 	 				if (Panel_TargetUI)
// 	 				{
// 	 					Panel_TargetUI->SetMulti(InMulti);
// 	 				}
				}
			});
			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::HouseSelectPopup, EWRUIWidgetCreationBitFlag::None, &Delegate);
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		}		
	}
}

void WRConsoleManager::OnTestKyu(AID_t IN InAID, AID_t IN InAID2, int8 IN InPal)
{

}

void WRConsoleManager::OnChangeConsole_TestSSpawnNPC(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRTableManager::IsValid() == false)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	int32 TID = InConsoleVariable->GetInt();

	FWRNpcTable* NPCTable = WRTableManager::Get()->FindRow<FWRNpcTable>(TID);
	if (NPCTable == nullptr)
		return;

	FVector Location = WRCharacterManager::Get()->GetMyCharacter()->GetActorLocation();
	Location += WRCharacterManager::Get()->GetMyCharacter()->GetActorForwardVector() * 200.0f;

	FWRCharacterSpawnParams Param = FWRCharacterSpawnParams(0, TID, NPCTable->BlueprintID, Location, FRotator::ZeroRotator);
	WRCharacterManager::Get()->Spawn<AWRCharacterNPC>(Param);
}


void WRConsoleManager::OnChangeConsole_LanguageType(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	if (IMNetwork::IsValid() == false)
		return;

	EWRLanguageType::en LanguageType = EWRLanguageType::en(InConsoleVariable->GetInt());
	IMMessageSender::RequestSetLanguageType(LanguageType);
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		MyCharacter->SetLanguageType(LanguageType);
	}
}


void WRConsoleManager::OnChangeConsole_VoiceVolume1(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Volume = InConsoleVariable->GetFloat();

#if PLATFORM_PS4
#if WR_ADD_VOICE_PORT
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(UWRGameInstance::GetGameInstance()->GetWorld());
	if (PS4Subsystem != nullptr)
	{
		IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
		if (VoiceInt.IsValid() == true)
		{
			VoiceInt->SetPortVolume(1, Volume);
		}
	}
#endif //PLATFORM_PS4
#endif //WR_ADD_VOICE_PORT
}

void WRConsoleManager::OnChangeConsole_VoiceVolume2(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Volume = InConsoleVariable->GetFloat();

#if PLATFORM_PS4
#if WR_ADD_VOICE_PORT
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(UWRGameInstance::GetGameInstance()->GetWorld());
	if (PS4Subsystem != nullptr)
	{
		IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
		if (VoiceInt.IsValid() == true)
		{
			VoiceInt->SetPortVolume(2, Volume);
		}
	}
#endif //PLATFORM_PS4
#endif //WR_ADD_VOICE_PORT
}

void WRConsoleManager::OnChangeConsole_VoiceVolume3(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Volume = InConsoleVariable->GetFloat();

#if PLATFORM_PS4
#if WR_ADD_VOICE_PORT
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(UWRGameInstance::GetGameInstance()->GetWorld());
	if (PS4Subsystem != nullptr)
	{
		IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
		if (VoiceInt.IsValid() == true)
		{
			VoiceInt->SetPortVolume(3, Volume);
		}
	}
#endif //PLATFORM_PS4
#endif //WR_ADD_VOICE_PORT
}

void WRConsoleManager::OnChangeConsole_VoiceVolume4(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Volume = InConsoleVariable->GetFloat();

#if PLATFORM_PS4
#if WR_ADD_VOICE_PORT
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(UWRGameInstance::GetGameInstance()->GetWorld());
	if (PS4Subsystem != nullptr)
	{
		IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
		if (VoiceInt.IsValid() == true)
		{
			VoiceInt->SetPortVolume(4, Volume);
		}
	}
#endif //PLATFORM_PS4
#endif //WR_ADD_VOICE_PORT
}

void WRConsoleManager::OnChangeConsole_VoiceVolume5(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	float Volume = InConsoleVariable->GetFloat();

#if PLATFORM_PS4
#if WR_ADD_VOICE_PORT
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(UWRGameInstance::GetGameInstance()->GetWorld());
	if (PS4Subsystem != nullptr)
	{
		IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
		if (VoiceInt.IsValid() == true)
		{
			VoiceInt->SetPortVolume(5, Volume);
		}
	}
#endif //PLATFORM_PS4
#endif //WR_ADD_VOICE_PORT
}


void WRConsoleManager::OnChangeConsole_TestASpawnNPC(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRTableManager::IsValid() == false)
		return;

	if (WRAsyncLoadUObjectManager::IsValid() == false)
		return;

	int32 TID = InConsoleVariable->GetInt();

	FWRNpcTable* NPCTable = WRTableManager::Get()->FindRow<FWRNpcTable>(TID);
	if (NPCTable == nullptr)
		return;

	WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(NPCTable->BlueprintID, FWRCompleteAsyncLoad::CreateRaw(this, &WRConsoleManager::OnCompleteSpawnNPC));
}

void WRConsoleManager::OnCompleteSpawnNPC(UObject* InActor, FString InHash)
{
	AWRCharacterNPC* NPC = Cast<AWRCharacterNPC>(InActor);
	if (NPC == nullptr)
		return;

	FVector Location = WRCharacterManager::Get()->GetMyCharacter()->GetActorLocation();
	Location += WRCharacterManager::Get()->GetMyCharacter()->GetActorForwardVector() * 200.0f;

	NPC->SetActorLocation(Location);
}

void WRConsoleManager::OnChangeConsole_ShowDebugUI(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRUIManager::IsValid() == false)
		return;

	uint16 flag = EWRDebugUIBitFlag::ConvertToUint16(InConsoleVariable->GetString());
	WRUIManager::Get()->EditDebugUIFlag(flag);
}


void WRConsoleManager::OnChangeConsole_CollectSyncLoadAssetName(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();
	if (GameInstance->IsValidLowLevel() == false)
		return;

	int32 Value = InConsoleVariable->GetInt();
	if (Value == 0)
	{
		GameInstance->SetEnableCollectSyncLoadInfo(false);
	}
	else if (Value == 1)
	{
		GameInstance->SetEnableCollectSyncLoadInfo(true);
	}
}

void WRConsoleManager::OnChangeConsole_WidgetComponentVisible(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRUIManager::IsValid() == false)
		return;

	int32 Value = InConsoleVariable->GetInt();

	if (Value == 0)
	{
		WRUIManager::Get()->HideAllWidgetComponents();
	}
	else
	{
		WRUIManager::Get()->ShowAllWidgetComponents();
	}
}

void WRConsoleManager::OnChangeConsole_DisplayURODebug(IConsoleVariable* InConsoleVariable)
{
	if (InConsoleVariable == nullptr)
		return;

	if (WRCharacterManager::IsValid() == false)
		return;

	int32 Value = InConsoleVariable->GetInt();

	WRCharacterManager::Get()->SetEnableDisplayURODebug(Value == 1);
	WRCharacterCustomizingManager::Get()->SetEnableDisplayURODebug(Value == 1);
}
