// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "Network/Share/Define/Define_Type.h"
#include "Array.h"
/**
 * 
 */

class WRSecretDialogController
{
public:
	WRSecretDialogController();
	~WRSecretDialogController();
public:
	void Tick(const float IN InDeltaTime);
	void OnShutdown();
	void OnLoadLevelStart();
	void OnLoadLevelProcessEnded();

public:
	//bool OnInputKeyForAgreeSecretDialog(const EWRInput IN InInput);

	void OnCmdAgreeToSecretDialog(const AID_t IN InPartnerAID, const CHAT_GROUP_ID_t IN InGroupID);
	void OnAgreeToSecretDialog(const TArray<AID_t>& IN InAIDs, const CHAT_GROUP_ID_t IN InGroupID, bool InAgree);
	void OnEndSecretDialog();
	void OnEndSecretDialog(const CHAT_GROUP_ID_t IN InGroupID);

	void OnPopUpOKForSecretDialog();
	void OnPopCancelForSecretDialog();

	FORCEINLINE void SetCmdData(const CHAT_GROUP_ID_t& InGroupID, const AID_t& InAID) { Cmd_GroupID = InGroupID; Cmd_PartnerID = InAID; }
	FORCEINLINE CHAT_GROUP_ID_t& GetCmdGroupID() { return Cmd_GroupID; }
	FORCEINLINE AID_t& GetCmdPartnerID() { return Cmd_PartnerID; }
private:
	CHAT_GROUP_ID_t Cmd_GroupID;
	AID_t Cmd_PartnerID;
};
