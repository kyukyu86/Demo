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
	FORCEINLINE void ForcedResetTargetLocationGuideHandle() { TargetLocationGuideHanlde = INVALID_TARGET_LOCATION_GUIDE_HANDLE; }	// ������ TargetLocationGuideHandle�� Reset�Ѵ�.
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
	// [ 2019-8-14 : hansang88 ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

public:
	void StartDefaultVoiceRecording();
	void StartDefaultVoiceRecordingAtRetry();
	void StartDefaultVoiceRecordingAtTrigger();

	void StartChatRecording();
	
	void StopVoiceRecording();
	void CancelRecording();	// Voice, Chat Recording �Ѵ� ĵ���Ǵ� �Լ�

	void OnDefaultVoiceRecordResulted(EWRVoiceWordType IN InVoiceEventType, WRTableID InVoiceCommandDataTID);
	void OnVoiceRecordingStateChanged(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void OnImpossibleVoiceRecordingChanged(const EWRVoiceRecordingStateType IN InVoiceRecordingState, const EWRVoiceContentType IN InVoiceContentType, const bool IN bInChatRecording, const bool bInImpossibleVoiceRecording);
	void OnVoiceChatSended();	// ������ ���� ó���� ��û��(�������� �κ�)
	void OnChatRecordingForciblyStoped();	// ������ ������ ���߾����� �Ҹ��� �Լ�
	void OnCharacterDestroyed(const AID_t IN InAID);
	void OnVideoChatConnected(const AID_t IN InTargetAID);	// ȭ��ä���� ����Ǿ�����

	// [ 2020-3-20 : SeungYeop ] Voice Recognition/Recommend Volume
	void AddAvailableVoiceCommand(const TArray<WRTableID>& IN InVoiceCommandTIDs);
	void RemoveAvailableVoiceCommand(const TArray<WRTableID>& IN InVoiceCommandTIDs);

	void AddRecommendedVoiceCommand(const TArray<WRTableID>& IN InRecommendedVoiceCommandTIDs);
	void RemoveRecommendedVoiceCommand(const TArray<WRTableID>& IN InRecommendedVoiceCommandTIDs);
	void GetRemmendedVoiceCommand(TArray<WRTableID>& OUT OutRecommendedVoiceCommandTIDs);

	// FocusTalk
	void ReserveToRemoveFocusTalkController(const AID_t IN InPlayerAID);
	bool RotateHMDInDirectionOfFocusTalkTarget();	// �ٶ������ true �ƴϸ� false

	AID_t GetTargetAIDForHMDRotationInFocusTalk();	// ��Ŀ���忡�� HMDRotation �� ���� Ÿ���� AID
	bool IsVisibleFocusTalkUI(const AID_t IN InPlayerAID);

	// ChatRecording ���� ����(û�� ���� ����)
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

	bool CheckLookingTargetHeadForTranslating();	// �������� ����ϴ� �Ӹ� �ٶ󺸱� �Լ�
	

	// VideoChat(ȭ��ä��)
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
	// [ 2020-7-14 : EZ ] Secret Dialog ���� �Լ� �߰�
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

	bool bRequestPacketToServer = false;	// server�� �����ν� ���� ��Ŷ�� ������ �ϴ°�?
	bool bStartDialogueWhenVoiceRecording = false; // server�� �����ν� ���� ��Ŷ�� ���� �� ���� ��簡 ���;� �ϴ°�?
	int8 RetryDefaultVoiceRecordingCount = 0;

	// Focus Talk
	TMap<AID_t, WRFocusTalkController> FocusTalkControllerMap;
	TDoubleLinkedList<AID_t> ReservedRemoveFocusTalkList;

private:
	// ChatRecording ���� ����(û�� ���� ����)
	TDoubleLinkedList<AID_t> MyTranslatingPlayerList;	// ���� �����ϰ� �ִ� �÷��̾� ����Ʈ
	TDoubleLinkedList<AID_t> TranslatingMePlayerList;	// ���� �����ϰ� �ִ� �÷��̾� ����Ʈ
	TDoubleLinkedList<AID_t> WaitTranslatingPacketList;	// �������� ��Ŷ�� ��ٸ��� ����Ʈ

private:
	// [ 2020-7-14 : EZ ] Secret Dialog ���� �Լ� �߰�
	WRSecretDialogController SecretDialogController;
	TMap<CHAT_GROUP_ID_t, TArray<AID_t>> GroupChatList;
	bool IsReservedOpenSecretDialogUI = false;

private:
	// ȭ�� ä�� ����
	WRVideoChatController VideoChatController;
};