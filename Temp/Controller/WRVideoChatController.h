// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "Network/WRProtocolHeaders.h"
#include "NameTypes.h"
#include "Enum/EWRCharacter.h"

class AWRCharacterPlayer;
#define INVALID_SHORTCUT_NUMBER -1
struct FWRVideoChatUserData
{
private:
	FWRVideoChatUserData() {}
public:
	FWRVideoChatUserData(const AID_t IN InPlayerAID) : PlayerAID(InPlayerAID) {}
	FWRVideoChatUserData(const AID_t IN InPlayerAID, const FName& IN InCharacterName, const FName& IN InNickName, const int32 IN InShortcutNumber, const bool IN bInOnline) : PlayerAID(InPlayerAID), CharacterName(InCharacterName), NickName(InNickName), ShortcutNumber(InShortcutNumber), bOnline(bInOnline) {}
	FWRVideoChatUserData(const stVideoChatMember& IN InVideoChatMember);

	AID_t PlayerAID = INVALID_AID;
	FName CharacterName;
	FName NickName;
	int32 ShortcutNumber = INVALID_SHORTCUT_NUMBER;
	bool bOnline = false;
	TArray<WRTableID> CostumeList;

	//EWRGender
	WRTableID CharacterPresetTID;
	stCustomizeInfo CustomizeInfo;
	
};

class WRVideoChatController
{
public:
	enum EWRVideoChatState { None, Calling, Receiving, Connected };
public:
	WRVideoChatController();
	~WRVideoChatController();
public:
	void OnShutdown();
	void OnLoadLevelStart();
	void OnLoadLevelProcessEnded();
public:
	void OpenUI_VideoChatUserList();
	const FWRVideoChatUserData* FindVideoChatUserData(const AID_t IN InAID);
	void CallVideoChatByShortcutNumber(const int32 IN InShortcutNumber, const bool bUpdateUserList = false);
	void CallVideoChatByNickName(const FString& IN InNickName, const bool bUpdateUserList = false);

	void StartVideoChat();
	void StopVideoChat();
	void CancelVideoChat();

	void GetVideChatUserDataArray(TArray<FWRVideoChatUserData>& OUT OutVideoChatUserDataArray);
	FORCEINLINE EWRVideoChatState GetCurrentVideoChatState() { return CurrentVideoChatState; }
	FORCEINLINE AID_t GetVideoChatPlayerAID() { return VideoChatPlayerAID; }

	bool IsPossibleVoiceRecognitionInVideoChatScreen();

	void OnPlayerCharacterSpawned(AWRCharacterPlayer* IN InCharacter);
	void OnCharacterDestroyed(const AID_t IN InAID);
	bool OnInputEvent(const EWRInput IN InEvent);
	void OnCallingEventReceivedFromUI(const AID_t IN InPlayerAID);
	void OnRejectEventReceivedFromUI(const AID_t IN InPlayerAID);

	// Packet
	void RequestVideoChatList();
	void RequestBeginVideoChat(const AID_t IN InReceiverAID);
	void RequestEndVideoChat(const AID_t IN InPlayerAID);
	void RequestVideoChatMessage(const FString& IN InvoiceData, const bool IN bInfirst, const bool IN bInLast);
	void RequestAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InRequesterAID);
	
	void OnAckVideoChatListReceived(const bool IN bInSuccess, const std::list<stVideoChatMember>& IN InVideoChatMember);
	void OnAckBeginVideoChatReceived(const bool IN bInSuccess, const AID_t IN InReceiverAID);
	void OnAckEndVideoChatReceived();
	void OnAckVideoChatMessageReceived();
	void OnAckAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InRequesterAID);
	void OnAckRegistSpeakerReceived(const AID_t IN InPlayerAID);
	void OnAckDeregistSpeakerReceived(const AID_t IN InPlayerAID);

	void OnInfEndVideoChatReceived();
	void OnInfVideoChatMessageReceived();
	void OnInfAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InReceiverAID);
	void OnInformVoiceMessage(const AID_t IN InSpeakerAID);

	void OnInfBeginVideoChatReceived(const stVideoChatMember& IN InRequesterData);

	void OnInformSpeechBubbleReceived(const AID_t IN InAID, const EWRLanguageType::en IN InLanguageType, const FString& InStr);
	/////////////////////////////////////////////////////////////////////////////////////////////
private:
	void Clear();

	const FWRVideoChatUserData* FindVideoChatUserData(const int32 IN InShortcutNumber);
	const FWRVideoChatUserData* FindVideoChatUserData(const FString& IN InNickName);

	void AddVideoChatUserData(const AID_t IN InPlayerAID, const FName& IN InCharacterName, const FName& IN InNickName = "", const int16 IN InSpeedDialNumber = 0, const bool IN bInOnline = true);
	void AddVideoChatUserData(const FWRVideoChatUserData& IN InVideoChatUserData);

	void SetVideoChatState(const EWRVideoChatState IN InVideoChatState, const AID_t IN InPlayerAID = INVALID_AID);
private:
	TArray<FWRVideoChatUserData>	VideoChatUserDataArray;
	EWRVideoChatState CurrentVideoChatState = EWRVideoChatState::None;
	AID_t VideoChatPlayerAID = INVALID_AID;
	int32 CallReservedShortcutNumber = INVALID_SHORTCUT_NUMBER;	// 전화 예약이 되어있는 단축번호, 리스트정보를 받아온후 전화를 바로해준다.
	FString CallReservedNickName;	// 전화 예약이 되어있는 단축번호, 리스트정보를 받아온후 전화를 바로해준다.
};
