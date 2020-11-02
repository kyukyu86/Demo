// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Define/WRTypeDefine.h"
#include "Enum/EWRInputSystem.h"
#include "Network/Share/Define/Define_Type.h"
#include "UI/Panel/WRUIPanel_HMDGuide.h"
#include "Controller/WRVideoChatController.h"
#include "Controller/WRSecretDialogController.h"

/**
*
*/

// 

class WRFocusTalkController
{
public:
	WRFocusTalkController() {}
	WRFocusTalkController(const WRTargetLocationGuideHandle IN InHandle, const AID_t IN InPlayerAID) : PlayerAID(InPlayerAID), TargetLocationGuideHanlde(InHandle) {}
public:
	void Tick(const float IN InDeltaTime);
	FORCEINLINE void ResetCoolTime() { FlowCoolTime = 0.f; };
	FORCEINLINE void ForcedResetTargetLocationGuideHandle() { TargetLocationGuideHanlde = INVALID_TARGET_LOCATION_GUIDE_HANDLE; }	// 강제로 TargetLocationGuideHandle을 Reset한다.
	void RemoveTargetLocationGuide();
	bool IsVisibleFocusTalkUI() const;
private:
	void CheckDist();
private:
	AID_t PlayerAID = 0;
	float FlowCoolTime = 0.f;
	float FlowCheckTime = 0.f;
	float FlowTargetLocationGuideHoldingTime = 0.f;
	WRTargetLocationGuideHandle TargetLocationGuideHanlde = 0;
};

class WR_API WRVoiceContentsManager : public WRSingleton<WRVoiceContentsManager>
{
	CDECLARE_MANAGER(WRVoiceContentsManager);

private:
	WRVoiceContentsManager() {}
	virtual ~WRVoiceContentsManager() {}

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

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

public:
	void StartDefaultVoiceRecording();
	void StartDefaultVoiceRecordingAtRetry();
	void StartDefaultVoiceRecordingAtTrigger();

	void StartChatRecording();
	
	void StopVoiceRecording();
	void CancelRecording();	// Voice, Chat Recording 둘다 캔슬되는 함수

	void OnDefaultVoiceRecordResulted(EWRVoiceWordType IN InVoiceEventType, WRTableID InVoiceCommandDataTID);
	void OnVoiceRecordingStateChanged(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void OnImpossibleVoiceRecordingChanged(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void OnVoiceChatSended();	// 번역에 대한 처리가 요청됨(끊어지는 부분)
	void OnChatRecordingForciblyStoped();	// 번역기 강제로 멈추었을때 불리는 함수
	void OnCharacterDestroyed(const AID_t IN InAID);
	void OnVideoChatConnected(const AID_t IN InTargetAID);	// 화상채팅이 연결되었을때

	// [ 2020-3-20 : SeungYeop ] Voice Recognition/Recommend Volume
	void AddAvailableVoiceCommand(const TArray<WRTableID>& IN InVoiceCommandTIDs);
	void RemoveAvailableVoiceCommand(const TArray<WRTableID>& IN InVoiceCommandTIDs);

	void AddRecommendedVoiceCommand(const TArray<WRTableID>& IN InRecommendedVoiceCommandTIDs);
	void RemoveRecommendedVoiceCommand(const TArray<WRTableID>& IN InRecommendedVoiceCommandTIDs);
	void GetRemmendedVoiceCommand(TArray<WRTableID>& OUT OutRecommendedVoiceCommandTIDs);

	// FocusTalk
	void ReserveToRemoveFocusTalkController(const AID_t IN InPlayerAID);
	bool RotateHMDInDirectionOfFocusTalkTarget();	// 바라봤으면 true 아니면 false

	AID_t GetTargetAIDForHMDRotationInFocusTalk();	// 포커스톡에서 HMDRotation 을 위한 타겟의 AID
	bool IsVisibleFocusTalkUI(const AID_t IN InPlayerAID);

	// ChatRecording 관련 내용(청자 기준 번역)
	const TDoubleLinkedList<AID_t>& GetMyTranslatingPlayerList();
	const TDoubleLinkedList<AID_t>& GetTranslatingMePlayerList();

	bool ContainsInMyTranslatingPlayerList(const AID_t IN InPlayerAID);
	bool ContainsInTranslatingMePlayerList(const AID_t IN InPlayerAID);
	bool ContainsInWaitTranslatingPacketList(const AID_t IN InPlayerAID);

	bool IsEmptyMyTranslatingPlayerList();
	bool IsEmptyTranslatingMePlayerList();

	void AddMyTranslatingPlayer(const AID_t IN InPlayerAID);
	void AddTranslatingMePlayer(const AID_t IN InPlayerAID);

	void RemoveMyTranslatingPlayer(const AID_t IN InPlayerAID);
	void RemoveTranslatingMePlayer(const AID_t IN InPlayerAID);

	bool CheckLookingTargetHeadForTranslating();	// 번역에서 사용하는 머리 바라보기 함수
	

	// VideoChat(화상채팅)
	WRVideoChatController& GetVideoChatController() { return VideoChatController; }


	//////////////////////////////////////////////////////////////////////////////////
	// Packet
	void RequestRegistSpeaker(const AID_t IN InPlayerAID);
	void RequestDeregistSpeaker(const AID_t IN InPlayerAID);

	void OnAckRegistSpeakerReceived(const bool IN bInSuccess, const AID_t IN InPlayerAID);
	void OnAckDeregistSpeakerReceived(const bool IN bInSuccess, const AID_t IN InPlayerAID);
	void OnAckReleaseListenerReceived(const AID_t IN InPlayerAID);
	
	void OnInformRegistSpeakerReceived(const AID_t IN InPlayerAID);
	void OnInformDeregistSpeakerReceived(const AID_t IN InPlayerAID);
	void OnInformReleaseListenerReceived(const AID_t IN InPlayerAID);
	

	void OnInformVoiceMessage(const AID_t IN InSpeakerAID);
	void OnInformVoiceChat(const AID_t IN InSpeakerAID);

	void OnInformSetFocusedAID(const AID_t IN InSpeakerAID);
public:
	// [ 2020-7-14 : EZ ] Secret Dialog 관련 함수 추가
	bool OnInputKeyForAgreeSecretDialog(const EWRInput IN InInput);

	void OnCmdAgreeToSecretDialog(const AID_t IN InPartnerAID, const CHAT_GROUP_ID_t IN InGroupID);
	void OnAgreeToSecretDialog(const TArray<AID_t>& IN InAIDs, const CHAT_GROUP_ID_t IN InGroupID, bool InAgree);
	void OnEndSecretDialog();
	void OnEndSecretDialog(const CHAT_GROUP_ID_t IN InGroupID);
	
	void OnPopUpOKForSecretDialog();
	void OnPopUpCancelForSecretDialog();

	TArray<AID_t> GetActorListInGroupChatList(const CHAT_GROUP_ID_t InGroupID);
	FORCEINLINE WRSecretDialogController& GetSecretDialogController() {	return SecretDialogController;}

public:
	FORCEINLINE void AddGroupChat(const CHAT_GROUP_ID_t IN InGroupID, const TArray<AID_t>& IN InAIDList)
	{
		if (GroupChatList.Contains(InGroupID) != true)
			GroupChatList.Add(InGroupID, InAIDList);
	}

private:
	void UpdateUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void UpdateDefaultUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void UpdateSelectDialogueUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void UpdateMenuUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void UpdateSocialMenuUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void UpdateVideoChatUserListUI(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void UpdateVideoChatScreen(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void UpdateChatRecording(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);

	bool CheckAvailableVoiceRecording();

	// FocusTalk
	void Tick_ReservedRemoveFocusTalk();
	void AddFocusTalkController(const AID_t IN InSpeakerAID);
	void OnAutomaticallyRemoveTargetLocationGuide(const AID_t IN InSpeakerAID);
	void UpdateUIForHMDRotationInFocusTalk();
private:
	// [ 2020-3-20 : SeungYeop ] Voice Recognition/Recommend Volume
	TMap<WRTableID, int32> AvailableVoiceCommandTIDs; // <VoiceCommandTID, Count>
	TMap<WRTableID, int32> RecommendedVoiceCommandTIDs; // <VoiceCommandTID, Count>

	bool bRequestPacketToServer = false;	// server에 음성인식 시작 패킷을 보내야 하는가?
	bool bStartDialogueWhenVoiceRecording = false; // server에 음성인식 시작 패킷을 보낼 떄 시작 대사가 나와야 하는가?
	int8 RetryDefaultVoiceRecordingCount = 0;

	// Focus Talk
	TMap<AID_t, WRFocusTalkController> FocusTalkControllerMap;
	TDoubleLinkedList<AID_t> ReservedRemoveFocusTalkList;

private:
	// ChatRecording 관련 내용(청자 기준 번역)
	TDoubleLinkedList<AID_t> MyTranslatingPlayerList;	// 내가 번역하고 있는 플레이어 리스트
	TDoubleLinkedList<AID_t> TranslatingMePlayerList;	// 나를 번역하고 있는 플레이어 리스트
	TDoubleLinkedList<AID_t> WaitTranslatingPacketList;	// 번역관련 패킷을 기다리는 리스트

private:
	// [ 2020-7-14 : EZ ] Secret Dialog 관련 함수 추가
	WRSecretDialogController SecretDialogController;
	TMap<CHAT_GROUP_ID_t, TArray<AID_t>> GroupChatList;
	bool IsReservedOpenSecretDialogUI = false;

private:
	// 화상 채팅 관련
	WRVideoChatController VideoChatController;
};