// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#pragma once

//#include "WRPCH.h"

DECLARE_LOG_CATEGORY_EXTERN(WRNetwork, Display, All);

struct FIMInformParams
{
public:
	FIMInformParams(const char* InData, unsigned short InDataLength) : Data(InData), DataLength(InDataLength)
	{
	}

	const char* Data;
	unsigned short DataLength;
};

struct FIMAcknowledgementParams
{
public:
	FIMAcknowledgementParams(unsigned char InSegment, unsigned char InType, const char* InData, unsigned short InDataLength)
		: Data(InData), DataLength(InDataLength), Segment(InSegment), Type(InType)
	{
	}

	const char* Data;
	unsigned short DataLength;
	unsigned char Segment;
	unsigned char Type;
};

#define REGISTER_RESULT_FUNCTION(Type) Type
#define REGISTER_INFORM_FUNCTION

class IMMessageReceiver
{
public:
	void OnReceived(const std::wstring& IN InServerName, const char* IN InData, unsigned short IN InDataLength);
	void OnError(unsigned short result);

private:

public:
	// -------------------------------------------------- ACK --------------------------------------------------
	// Login
	void OnAck_ReserveUser_L2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_LoginUser_L2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_LogoutUser_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EnterGame_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_MoveToLab_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_PlayerList_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CreatePlayer_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_DeletePlayer_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SelectPlayerName_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SelectPlayer_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Map
	void OnAck_ReserveMap_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EnterMap_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_ReadyToPlay_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_ExitMap_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Actor
	void OnAck_Revive_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BeginTalking_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndTalking_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SocialAction_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_ChangePreset_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_Unoccupy_IO_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SuggestCheers_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_AnswerCheers_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BeginCheers_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CancelCheers_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_MicAnimation_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BroadcastTestPacket_G2U(const FIMAcknowledgementParams& IN InputParams);
	
	// Move
	void OnAck_PlayerMove_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_PlayerStop_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_ActorMoveToPos_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_PlayerMoveInTransport_U2G(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_PlayerStopInTransport_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_PlayerMoveInMW_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_PlayerStopInMW_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CallADA_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_PlayerWarp_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BeginFall_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndFallen_G2U(const FIMAcknowledgementParams& IN InputParams);	

	// Combat
	void OnAck_UseSkill_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BeginSkill_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndSkill_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_GuardBreak_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SuccessToShield_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SuccessToAvoid_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_HitResult_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_DynamicMove_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SyncDynamicMoveLocation_G2U(const FIMAcknowledgementParams& IN InputParams);	

	// [ 2019-11-13 : hansang88 ] Trigger
	void OnAck_TriggerEvent_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SkipSequence_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Quest
	void OnAck_ProgressQuestList_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CompleteQuestList_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_AcceptQuest_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CompletableQuest_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CompleteQuest_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CompleteQuestTask_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Interaction
	void OnAck_BeginInteraction_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_StopInteraction_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndInteraction_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Inventory
	void OnAck_EntryInfo_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EquipItem_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_UnEquipItem_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_RegisterEquipmentSlot_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_UnRegisterEquipmentSlot_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_UseItem_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_DiscardItem_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Ladder
	void OnAck_BeginLadder_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_MoveLadder_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndLadder_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_CancelLadder_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Vehicle(transportation)
	void OnAck_GetOnTransport_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_GetOffTransport_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Vehicle
	void OnAck_SpawnVehicle_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_UnspawnVehicle_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_GetOnVehicle_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_GetOffVehicle_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BeginBrake_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndBrake_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_Acceleration_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_Steering_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_AgreeToBoard_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BeHorning_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_UpdateExterior_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Dance
	void OnAck_BeginDance_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndDance_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_ChangeDanceStep_G2U(const FIMAcknowledgementParams& IN InputParams);

	// [ 2020-3-10 : magedoga ] Sync HMD
	void OnAck_SyncHMD_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Language
	void OnAck_SpeechBubble_G2U(const FIMAcknowledgementParams& IN InputParams);

	// VoiceCommand
	void OnAck_BeginVoiceCommand_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndVoiceCommand_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Cheat
	void OnAck_Cheat_G2U(const FIMAcknowledgementParams& IN InputParams);

	void OnAck_GetOnMW_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_GetOffMW_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Voice Chat
	void OnAck_VoiceChat_G2U(const FIMAcknowledgementParams& IN InputParams);

	// chat voice
	void OnAck_VoiceMessage_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_SetFocusedAID_G2U(const FIMAcknowledgementParams& IN InputParams);
		
	// LanguageType
	void OnAck_SetLanguageType_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Dialogue
	void OnAck_DBSaveAnswerToNpc_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Life Event
	void OnAck_LifeEventReward_G2U(const FIMAcknowledgementParams& IN InputParams);


	// Secret Dialog
	void OnAck_AgreeToSecretDialog_G2U(const FIMAcknowledgementParams& IN InputParams); // 비밀대화 승낙수신

	void OnAck_EndOfSecretDialog_G2U(const FIMAcknowledgementParams& IN InputParams); // 비밀대화 종료 수신

	void OnAck_SecretDialogInfo_G2U(const FIMAcknowledgementParams& IN InputParams); // 엿보기 결과 aid리스트 수신

	// Chat Recording(청자기준 번역)
	void OnAck_RegistSpeaker_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_DeregistSpeaker_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_ReleaseListener_G2U(const FIMAcknowledgementParams& IN InputParams);


	// Interior
	void OnAck_ArrangeInteriorItem_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_RemoveInteriorItem_G2U(const FIMAcknowledgementParams& IN InputParams);

	// Video Chat
	void OnAck_VideoChatList_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_BeginVideoChat_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_EndVideoChat_G2U(const FIMAcknowledgementParams& IN InputParams);
	void OnAck_Agree2BeginVideoChat_G2U(const FIMAcknowledgementParams& IN InputParams);
	
	//void OnAck_VideoChatMessage_G2U(const FIMAcknowledgementParams& IN InputParams);





	// -------------------------------------------------- INF --------------------------------------------------
	// Login
	void OnInf_KickUser_L2U(const FIMInformParams& IN InputParams);

	// Map
	void OnInf_CompleteWarp_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ReserveMapForGroup_G2U(const FIMInformParams& IN InputParams);

	// Actor
	void OnInf_PlayerAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_NpcAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_IOAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SOAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ActorDisappear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ActorDied_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ChangeHP_G2U(const FIMInformParams& IN InputParams);
	void OnInf_Warp_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ChangeWalkSpeed_G2U(const FIMInformParams& IN InputParams);
	void OnInf_NpcDialog_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ActorCCState_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SocialAction_G2U(const FIMInformParams& IN InputParams);		
	void OnInf_Revive_G2U(const FIMInformParams& IN InputParams);
	void OnInf_FloatingBoardAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ElevatorAppear_G2U(const FIMInformParams& IN InputParams);	
	void OnInf_TardisAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ChangePreset_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SuggestCheers_G2U(const FIMInformParams& IN InputParams);
	void OnInf_AnswerCheers_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BeginCheers_G2U(const FIMInformParams& IN InputParams);
	void OnInf_CancelCheers_G2U(const FIMInformParams& IN InputParams);
	void OnInf_MicAnimation_G2U(const FIMInformParams& IN InputParams);
	void OnInf_APAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ChangeAPState_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BroadcastTestPacket_G2U(const FIMInformParams& IN InputParams);

	// Move
	void OnInf_PlayerMove_G2U(const FIMInformParams& IN InputParams);
	void OnInf_PlayerStop_G2U(const FIMInformParams& IN InputParams);
	void OnInf_PlayerMoveInTransport_G2U(const FIMInformParams& IN InputParams);
	void OnInf_PlayerStopInTransport_G2U(const FIMInformParams& IN InputParams);
	void OnInf_PlayerMoveInMW_G2U(const FIMInformParams& IN InputParams);
	void OnInf_PlayerStopInMW_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ActorMoveToPos_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ActorMoveStop_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ActorWarp_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BeginFall_G2U(const FIMInformParams& IN InputParams);
	void OnInf_EndFallen_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BattleState_G2U(const FIMInformParams& IN InputParams);

	// Combat
	void OnInf_UseSkill_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BeginSkill_G2U(const FIMInformParams& IN InputParams);
	void OnInf_EndSkill_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SetTarget_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BeginNpcHook_G2U(const FIMInformParams& IN InputParams);
	void OnInf_EndNpcHook_G2U(const FIMInformParams& IN InputParams);

	void OnInf_GuardBreak_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SuccessToShield_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SuccessToAvoid_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SyncDynamicMoveLocation_G2U(const FIMInformParams& IN InputParams);
	void OnInf_DynamicMove_G2U(const FIMInformParams& IN InputParams);
	void OnInf_HitResult_G2U(const FIMInformParams& IN InputParams);

	// [ 2019-11-13 : hansang88 ] Trigger
	void OnInf_ClientDirectSync_G2U(const FIMInformParams& IN InputParams);
	void OnInf_TriggerDisable_G2U(const FIMInformParams& IN InputParams);
	void OnInf_UsedTriggerList_G2U(const FIMInformParams& IN InputParams);
	void OnInf_PlayerLevelActionSequence_G2U(const FIMInformParams& IN InputParams);

	// Quest
	void OnInf_CompleteQuestTask_G2U(const FIMInformParams& IN InputParams);
	void OnInf_UpdateQuestTask_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ForcedQuestComplete_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ForcedQuestGiveUp_G2U(const FIMInformParams& IN InputParams);
	
	// Interaction
	void OnInf_BeginInteraction_G2U(const FIMInformParams& IN InputParams);
	void OnInf_StopInteraction_G2U(const FIMInformParams& IN InputParams);
	void OnInf_EndInteraction_G2U(const FIMInformParams& IN InputParams);

	// Inventory
	void OnInf_EntryInfo_Item_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ChangeEquipment_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ChangeCostume_G2U(const FIMInformParams& IN InputParams);
	void OnInf_AddItem_G2U(const FIMInformParams& IN InputParams);
	void OnInf_UpdateItemCount_G2U(const FIMInformParams& IN InputParams);
	void OnInf_DeleteItem_G2U(const FIMInformParams& IN InputParams);

	// Buff
	void OnInf_BeginBuff_G2U(const FIMInformParams& IN InputParams);
	void OnInf_EndBuff_G2U(const FIMInformParams& IN InputParams);
	void OnInf_DotBuff_G2U(const FIMInformParams& IN InputParams);

	// Transportation
	void OnInf_TransportState_G2U(const FIMInformParams& IN InputParams);
	void OnInf_GetOnTransport_G2U(const FIMInformParams& IN InputParams);
	void OnInf_GetOffTransport_G2U(const FIMInformParams& IN InputParams);
	void OnInf_GroupWarpTimerCount_G2U(const FIMInformParams& IN InputParams);
	void OnInf_GroupWarpStop_G2U(const FIMInformParams& IN InputParams);

	// Vehicle
	void OnInf_VehicleAppear_G2U(const FIMInformParams& IN InputParams);
	void OnInf_GetOnVehicle_G2U(const FIMInformParams& IN InputParams);
	void OnInf_GetOffVehicle_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BeginBrake_G2U(const FIMInformParams& IN InputParams);
	void OnInf_EndBrake_G2U(const FIMInformParams& IN InputParams);
	void OnInf_Acceleration_G2U(const FIMInformParams& IN InputParams);
	void OnInf_Steering_G2U(const FIMInformParams& IN InputParams);
	void OnInf_BeHorning_G2U(const FIMInformParams& IN InputParams);
	void OnInf_UpdateExterior_G2U(const FIMInformParams& IN InputParams);
	void OnInf_CancelOnBoard_G2U(const FIMInformParams& IN InputParams);

	// Dance
	void OnInf_ChangeDanceStep_G2U(const FIMInformParams& IN InputParams);

	// [ 2020-3-10 : magedoga ] Sync HMD
	void OnInf_SyncHMD_G2U(const FIMInformParams& IN InputParams);

	// Language
	void OnInf_SpeechBubble_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SetLanguageType_G2U(const FIMInformParams& IN InputParams);

	// MovingWalk
	void OnInf_GetOnMW_G2U(const FIMInformParams& IN InputParams);
	void OnInf_GetOffMW_G2U(const FIMInformParams& IN InputParams);

	// Voice Chat
	void OnInf_VoiceChat_G2U(const FIMInformParams& IN InputParams);

	void OnInf_ActorMoveStopInMW_G2U(const FIMInformParams& IN InputParams);

    // chat voice
	void OnInf_VoiceMessage_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SetFocusedAID_G2U(const FIMInformParams& IN InputParams);

	// Currency
	void OnInf_UpdateCurrency_G2U(const FIMInformParams& IN InputParams);

	// Secret Dialog
	void OnInf_SecretDialogAppear_G2U(const FIMInformParams& IN InputParams); // 비밀대화 종료
	void OnInf_AgreeToSecretDialog_G2U(const FIMInformParams& IN InputParams); // 비밀대화 종료
	void OnInf_EndOfSecretDialog_G2U(const FIMInformParams& IN InputParams); // 비밀대화 종료

	// Chat Recording(청자기준 번역)
	void OnInf_RegistSpeaker_G2U(const FIMInformParams& IN InputParams);
	void OnInf_DeregistSpeaker_G2U(const FIMInformParams& IN InputParams);
	void OnInf_ReleaseListener_G2U(const FIMInformParams& IN InputParams);

	// Video Chat
	void OnInf_BeginVideoChat_G2U(const FIMInformParams& IN InputParams);
	void OnInf_EndVideoChat_G2U(const FIMInformParams& IN InputParams);
	void OnInf_Agree2BeginVideoChat_G2U(const FIMInformParams& IN InputParams);
	//void OnInf_VideoChatMessage_G2U(const FIMInformParams& IN InputParams);

	// Sekai
	void OnInf_SekaiItemInfo_G2U(const FIMInformParams& IN InputParams);
	void OnInf_SekaiCurrencyInfo_G2U(const FIMInformParams& IN InputParams);


	// -------------------------------------------------- CMD --------------------------------------------------

	// Vehicle
	void OnCmd_AgreeToBoard_G2U(const FIMInformParams& IN InputParams);

	// Secret Dialog
	void OnCmd_AgreeToSecretDialog_G2U(const FIMInformParams& IN InputParams); // 비밀대화 의향 타진, rpt로 회신

};
