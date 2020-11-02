// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRConsole.h"
#include "Network/Share/Define/Define_Type.h"

/**
*
*/
class WR_API WRConsoleManager : public WRSingleton<WRConsoleManager>
{
	CDECLARE_MANAGER(WRConsoleManager);
	
private:
	WRConsoleManager();
	virtual ~WRConsoleManager();

	typedef void (WRConsoleManager::*FuncPointer)(IConsoleVariable*);
	
public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================
	
	// [ 2019-7-23 : magedoga ] GetConsole
	bool GetBool(EWRConsole InEConsole);
	int32 GetInt(EWRConsole InEConsole);
	float GetFloat(EWRConsole InEConsole);
	FString GetString(EWRConsole InEConsole);

	IConsoleVariable* GetConsoleVariable(EWRConsole InEConsole) const { return ConsoleCommand[InEConsole]; }

	// [ 2020-9-1 : magedoga ] Engine Console
	bool SetEngineConsole(const FString& InName, const int32 InValue);
	bool SetEngineConsole(const FString& InName, const float InValue);
private:

	void CreateConsoleVariables();
	void AddCallbackForChange(IConsoleVariable* InConsoleVariable, FuncPointer InFuncPointer);

	// [ 2019-8-26 : animal14 ] 
	void OnChangeConsole_TestFunction(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_TODDayCycle(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_TODTimeType(IConsoleVariable* InConsoleVariable);

	// [ 2019-8-21 : magedoga ] Callback Functions
	void OnChangeConsole_CharacterRotationMode(IConsoleVariable* InConsoleVariable);

	// [ 2019-9-18 : kyu ] IO Console Changed
	void OnChangeConsole_IORecognitionCollisionShow(IConsoleVariable* InConsoleVariable);

	// [ 2019-9-25 : hansang88 ] Pad 진동 Test
	void OnChangeConsole_PlayForceFeedback(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_StopForceFeedback(IConsoleVariable* InConsoleVariable);
	
	void OnChangeConsole_TestConsoleInt(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_PullMotionValueZ(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_PullMotionValueY(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_PushMotionValueZ(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_PushMotionValueY(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_TutorialGet(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_TutorialClear(IConsoleVariable* InConsoleVariable);

	// [ 2019-11-15 : vuvle ] Network Login IP 변경
	void OnChangeConsole_NetworkLoginServerIP(IConsoleVariable* InConsoleVariable);

	// [ 2020-03-18 : vuvle ] Map Move 치트
	void OnChangeConsole_MapMove(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_OpenCheat(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_KeepFocusToLastUI(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_OpenInventory(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_ChangeUserID(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_RevertUserID(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_InteractionSkip(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_DebugScreenMessage(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_ShowWidgetInteractionLay(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_ServerCheat(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_TestYSY(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_TestKyu(IConsoleVariable* InConsoleVariable);
	void OnTestKyu(AID_t IN InAID, AID_t IN InAID2, int8 IN InPal);

	void OnChangeConsole_ShowDebugUI(IConsoleVariable* InConsoleVariable);


	void OnChangeConsole_TestASpawnNPC(IConsoleVariable* InConsoleVariable);
	void OnCompleteSpawnNPC(UObject* InActor, FString InHash);
	void OnChangeConsole_TestSSpawnNPC(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_VoiceVolume1(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_VoiceVolume2(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_VoiceVolume3(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_VoiceVolume4(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_VoiceVolume5(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_LanguageType(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_CollectSyncLoadAssetName(IConsoleVariable* InConsoleVariable);
	void OnChangeConsole_WidgetComponentVisible(IConsoleVariable* InConsoleVariable);

	void OnChangeConsole_DisplayURODebug(IConsoleVariable* InConsoleVariable);
private:

	TMap<EWRConsole, IConsoleVariable*>	ConsoleCommand;
	
};