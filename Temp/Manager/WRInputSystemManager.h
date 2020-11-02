// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRInputSystem.h"
#include "Enum/EWRCharacter.h"
#include "InputCoreTypes.h"
#include "GameFrameWork/PlayerController.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"
#include "GameFramework/PlayerController.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "UWRVoiceController.h"

#include "WRInputSystemManager.generated.h"

class ACharacter;
class AWRCharacterIO;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHMDTargetChanged, ACharacter*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHMDMultiTargetAction, TArray<ACharacter*>)
//DECLARE_DELEGATE(FOnKeepPressedInputEvent)

class WRHMDController
{
public:
	WRHMDController() {};
	~WRHMDController() {};

	void Init();
	void OnShutdown();

	bool OnTick(float InDeltaTime);
	
	AActor* GetLookAtActor();
	FVector GetHMDForwardVector();
	FVector GetHMDRightVector();

	void TargettingProcess();
	void GetHMDLocationAndRotation();

	void StartMultiTargetMode();
	/*TArray<ACharacter*>*/void EndMultiTargetMode();
	void ActionMultiTargetMode();

	bool IsTargetActor(const AActor* IN InActor);
	FORCEINLINE bool IsLockOnMode() { return bLockOnMode; }

	bool IsMoved() const { return this->IsMovedLocation() || this->IsRotated(); }
	bool IsMovedLocation() const { return bMovedLocation; }
	bool IsRotated() const { return bRotated; }

	void SetTargetLock(const bool IN bInLock);

	void ResetOrientation();

	bool GetSweepSingleByChannelUsingHMD(FVector& InoutLocation);

	FOnHMDTargetChanged			HMDTargetChangedDelegate;
	FOnHMDMultiTargetAction		HMDMultiTargetActionDelegate;
	bool						bMultiTargetMode	= false;
	bool						bLockOnMode = false;
	int32						MaxMultiCount = 0;
		
	ACharacter*					TargetActor = nullptr;
	TArray<ACharacter*>			TargetActors;

	FVector						HMDLocation = FVector::ZeroVector;
	FVector						PreHMDLocation = FVector::ZeroVector;
	FRotator					HMDRotation = FRotator::ZeroRotator;
	FRotator					PreHMDRotation = FRotator::ZeroRotator;

	bool						bMovedLocation = false;
	bool						bRotated = false;

	BitFlag8					TickFlag = EWRHMDTick::All;
};


UCLASS()
class UWRPadController : public UObject
{
	GENERATED_BODY()

public:

	void Init();
	void OnShutdown();

	bool OnTick(float InDeltaTime);

	void InputKeyEvent(EWRInput InInputType, EInputEvent InEventType);

	void TouchPadX(float InScale);
	void TouchPadY(float InScale);

	void InputEventLStickX(float IN InScale);
	void InputEventLStickY(float IN InScale);

	void SetMotionValueZ(EWRPadMotionType InType, float InValue) { MotionValueZ[InType] = InValue; }
	void SetMotionValueY(EWRPadMotionType InType, float InValue) { MotionValueY[InType] = InValue; }

private:

	bool			bMotion[EWRPadMotionType::MAX];
	int				DeferredCount[EWRPadMotionType::MAX];
	float			MotionValueZ[EWRPadMotionType::MAX];
	float			MotionValueY[EWRPadMotionType::MAX];

	FVector2D		StartTouchPos;
	FVector2D		PreTouchPos;
	FVector2D		TouchDragDistance;

	FVector			ListenerPos;

#if WITH_EDITOR
	//====================================================================================
private:
	bool bL1;
	int8 nL2;
public:
	// [ 2019-8-20 : kyu ] Input Test
	void KyuInputTest();
	void Kyu_L1_Pressed();
	void Kyu_L1_Released();
	void Kyu_L2_Pressed();
	void Kyu_L2_Released();
	void InputEventLShiftBtn(EInputEvent InEventType);
	// [ 2019-8-20 : Ysy ] Input Test
	void PressYsyTest();
	void KorLanPressed();
	void JpnLanPressed();
	void ChnLanPressed();
	void EngLanPressed();
	// [ 2020-1-6 : magedoga ] Input Test
	void JinTestPressed();
	void JinTestPressed2();
	//====================================================================================
#else // WITH_EDITOR
#endif // WITH_EDITOR
};

// struct FWRKeepPressedInputEvent
// {
// 	FWRKeepPressedInputEvent() : InputKey(EKeys::Invalid), PressedTime(0.0f), PressedGoalTime(0.0f)
// 	{
// 	}
// 	FKey InputKey;
// 	float PressedTime;
// 	float PressedGoalTime;
// 	FOnKeepPressedInputEvent Delegate;
// };

/**
*
*/
class WR_API WRInputSystemManager : public WRSingleton<WRInputSystemManager>
{
	CDECLARE_MANAGER(WRInputSystemManager);
	friend class UWRPadController;
private:
	WRInputSystemManager();
	virtual ~WRInputSystemManager();

public:
	//====================================================================================
	// [ 2019-8-14 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================
	
	virtual void EndLoadLevelProcess() override;

	virtual bool OnTick(float InDeltaTime) override;

public:
	void SetUpInputBind(UInputComponent* PlayerInputComponent);

	void OnTickProcessAvoiding();
	void OnTickHMDPicking();

	// [ 2019-8-23 : hansang88 ] 바라보고 있는 Actor 검출
	AActor* GetLookAtActor();

// 	void StartMultiTargetMode();
// 	TArray<ACharacter*> EndMultiTargetMode();

	FVector GetHMDLocation() { return HMDController.HMDLocation; }
	FRotator GetHMDRotation() { return HMDController.HMDRotation; }
	FVector GetHMDForwardVector() { return HMDController.GetHMDForwardVector(); }
	FVector GetHMDRightVector() { return HMDController.GetHMDRightVector(); }
	void UpdateHMDInfo() { HMDController.GetHMDLocationAndRotation(); }

	void AddHMDTickFlag(BitFlag8 InTickFlag) 
	{
		HMDController.TickFlag |= InTickFlag;
	}
	void RemoveHMDTickFlag(BitFlag8 InTickFlag) 
	{
		HMDController.TickFlag &= ~InTickFlag; 
	}

	void HMDTickOnAll() { HMDController.TickFlag = (BitFlag8)EWRHMDTick::All; }
	void HMDTickOffAll() { HMDController.TickFlag = (BitFlag8)EWRHMDTick::None; }


	FOnHMDTargetChanged& GetHMDTargetChangedDelegate() { return HMDController.HMDTargetChangedDelegate; }
	FOnHMDMultiTargetAction& GetHMDMultiTargetActionDelegate() { return HMDController.HMDMultiTargetActionDelegate; }

	void StartRecording(const EWRVoiceContentType InVoiceContentType, TArray<FWRSubCommandData>& subCommand, FWRVoiceEventDelegate callback);
	void StartChatRecording();
	void StopRecording();
	void CancelRecording();
	void SetImpossibleVoiceRecording(const bool IN bInImpossibleVoiceRecording);
	bool IsVoiceRecording() { return (VoiceController ? VoiceController->IsVoiceRecording() : false); }
	bool IsChatRecording() { return (VoiceController ? VoiceController->IsChatRecording() : false); }
	EWRVoiceContentType GetVoiceContentType() { return (VoiceController ? VoiceController->GetVoiceContentType() : EWRVoiceContentType::NONE); }
	EWRVoiceRecordingStateType GetVoiceRecordingState() { return (VoiceController ? VoiceController->GetVoiceRecordingState() : EWRVoiceRecordingStateType::NONE); }
	bool IsImpossibleVoiceRecording() { return (VoiceController ? VoiceController->IsImpossibleVoiceRecording() : true); }
	void SetVoiceRecordingState(EWRVoiceRecordingStateType state);
	void FinishedRecognition( FVoiceRecognition* InFinishedTask);
	void OnResponseGoogleSTT(FString Str);

	int32 Translation(EWRLanguageType::en Src, EWRLanguageType::en Tar, FString Str);
	void OnResponseTranslation(int32 Identify, EWRLanguageType::en Src, EWRLanguageType::en Tar, FString OrgStr, FString TarStr);
	void OnInf_VoiceMessage(const AID_t IN playerAID, const FString IN voiceMsg);

	//void InputVoiceEvent(WRTableID InVoiceCommandDataTID);
	void SetInputVoiceEvent(EWRVoiceWordType InEventType, WRTableID InVoiceCommonDataTID);
	void InputEvent(EWRInput InInputType, EInputEvent InEventType);
	bool IsInputEventEnabled(EWRInput InInputType, EInputEvent InEventType);

	FDynamicForceFeedbackHandle PlayDynamicForceFeedback(float Intensity, float Duration, bool bAffectsLeftLarge = true, bool bAffectsLeftSmall = true, bool bAffectsRightLarge = true, bool bAffectsRightSmall = true);
	void StopDynamicForceFeedback(FDynamicForceFeedbackHandle ActionHandle);

	void ClientPlayForceFeedback(WRTableID InForceFeedbackID);
	void ClientStopForceFeedback(WRTableID InForceFeedbackID);

	void ReleaseTargetLock();
	void OnMyCharacterDameged();

	float GetTargetDirAngle(const float InScale);

	void SetHMDStartPositionForAvoid(bool bSet);
	
	bool IsTargetActor(const AActor* IN InActor);
	void SetMultiTargetMode(const bool IN InSet, int32 MaxCount = 0);
	
	void SetMotionValueZ(EWRPadMotionType InType, float InValue);
	void SetMotionValueY(EWRPadMotionType InType, float InValue);

	void ToggleHMDPicking();
	bool ConfirmHMDPicking();
	void CancelHMDPicking();
	void UpdateHMDPickingVFX();
	void CreateHMDPickingVFX(FVector InLocation, bool bCreaetBeamVfx = true);
	void RemoveHMDPickingVFX();

	FORCEINLINE bool IsActiveMultiTargetMode() { return HMDController.bMultiTargetMode; }
	FORCEINLINE TArray<ACharacter*> GetMultiTarget() { return HMDController.TargetActors; }

	void ResetHMDOrientation();
	// [ 2019-12-11 : kyu ] Only Move Input 
	FORCEINLINE void SetOnlyMoveInput(const bool IN InSet) { bOnlyMoveInput = InSet; }
	FORCEINLINE bool IsOnlyMoveInput() { return bOnlyMoveInput; }
	FORCEINLINE void SetLocationLock(const bool InSet) { bLocationLock = InSet; }
	FORCEINLINE bool IsLocationLock() { return bLocationLock; }
	FORCEINLINE void SetRotationLock(const bool InSet) { bRotationLock = InSet; }
	FORCEINLINE bool IsRotationLock() { return bRotationLock; }

	EWRInput FindInput(const FName& IN InKeyNameInSetting);

	// for Test
	FORCEINLINE bool IsPressedLShift() { return bPressedLShift; }

	EWRAttackHeightType GetAttackHeightType();

#if PLATFORM_PS4
	#if WR_ADD_VOICE_PORT
	void SetVoiceVolume(uint32 PortIndex, float Volume);
	int GetPortIndexByDistance(AID_t InAID, AID_t InFocusedPlayerAID);
	#endif //WR_ADD_VOICE_PORT
	void OnInformVoiceChat(AID_t InAID, FString MsgBuffer, uint16 Length, AID_t InFocusedPlayerAID);
#endif //PLATFORM_PS4

private:
	void InputEventCombat(EWRInput InInputType, EInputEvent InEventType);

	bool InputEventTriangle(EInputEvent InEventType);
	bool InputEventSquare(EInputEvent InEventType);
	bool InputEventCircle(EInputEvent InEventType);
	bool InputEventCross(EInputEvent InEventType);
	bool InputEventL1(EInputEvent InEventType);
	bool InputEventL2(EInputEvent InEventType);
	bool InputEventL3(EInputEvent InEventType);
	bool InputEventR1(EInputEvent InEventType);
	bool InputEventR2(EInputEvent InEventType);
	bool InputEventR3(EInputEvent InEventType);
	bool InputEventDpadUp(EInputEvent InEventType);
	bool InputEventDpadDown(EInputEvent InEventType);
	bool InputEventDpadLeft(EInputEvent InEventType);
	bool InputEventDpadRight(EInputEvent InEventType);
	bool InputEventShareBtn(EInputEvent InEventType);
	bool InputEventTouchBtn(EInputEvent InEventType);
	bool InputEventOptionBtn(EInputEvent InEventType);
	bool InputEventRThumbStickBtn(EInputEvent InEventType);

	bool InputEventTouchPadDrag(FVector2D MoveDistance);
	bool InputEventMotion(EWRPadMotionType InMotionType);

	// [ 2020-4-7 : kyu ] 컨텐츠들의 입력이벤트에 반응하여 취소 될 동작들 정의
	void InputEvnetToContents(EWRInput InInputType, EInputEvent InEventType);
	
	// for Test
	void InputEventLShiftBtn(EInputEvent InEventType);

	bool OnEventPressedL2R2();

	// [ 2020-3-12 : kyu ] 입력 Press에서 체크 후 Release에서 체크 무시하도록 셋팅
	void SetInputReleaseIgnore(EWRInput IN InInput);
	bool CheckInputReleaseIgnore(EWRInput IN InInput);

	void CalculateHMDRelativePosition(OUT FVector& ResultPos, FVector InPos);
	void CalculateVoiceChatInfo(OUT FVector& OutPos, OUT float& OutGain, AID_t InAID);

private:
	WRHMDController HMDController;
	UWRVoiceController* VoiceController = nullptr;
	UWRPadController* PadController = nullptr;

	FVector HMDStartPosition;
	FRotator HMDStartRotation;
		
	EWRVoiceWordType VoiceEventType = EWRVoiceWordType::NONE;
	WRTableID VoiceCommonDataTID = INVALID_TABLE_TID;

	// 프로젝트 셋팅-> 입력  에 있는 셋팅과 EWRInput 연결 고리
	TMap<FName, EWRInput> InputMaps;

	// [ 2020-3-12 : kyu ] 입력 Press에서 체크 후 Release에서 체크 무시하도록 셋팅
	TMap<EWRInput, bool> InputReleaseIgnore;


	//TSharedRef<class IWebSocket>	WebSocket;
	//UObject		InputBindObj;

	// [ 2019-12-11 : kyu ] Only Move Input 
	bool bOnlyMoveInput = false;
	bool bLocationLock = false;
	bool bRotationLock = false;

	bool bUpdateHMDPicking = false;
	class AWRVFX* LocationVFX = nullptr;
	class AWRVFX* BeamVFX = nullptr;

	FVector HMDPickingLocation = FVector::ZeroVector;
	FString strAIMoveUIAsyncKey = "";

	// for Test
	bool bPressedL2 = false;
	bool bPressedR2 = false;
	bool bPressedLShift = false;
public:
	void DisplayAndSaveLog(const FString& InString);

private:
	char VoiceLogs[256];
	int32 IndexLogs = 0;

public:
	void InputEventLStickX(float IN InScale);
	void InputEventLStickY(float IN InScale);

	FVector2D GetLStickScale() {
		return LStickScale;
	}
private:
	FVector2D LStickScale;
};


// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRInputSystem, Display, All);