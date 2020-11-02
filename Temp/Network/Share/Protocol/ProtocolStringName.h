#pragma once

#include "ProtocolHeader.h"

//1st세크먼트와 프로토콜코드로 프로토콜이름 얻기
static const wchar_t* GetProtocolString(int seg, int pro)
{
	switch(seg)
	{
	case Seg_Game_Actor:
	{
		switch ( pro )
		{
			case Inf_PlayerAppear_G2U :
				return L"Seg_Game_Actor::Inf_PlayerAppear_G2U";
				break;
			case Inf_NpcAppear_G2U :
				return L"Seg_Game_Actor::Inf_NpcAppear_G2U";
				break;
			case Inf_IOAppear_G2U :
				return L"Seg_Game_Actor::Inf_IOAppear_G2U";
				break;
			case Inf_BrickAppear_G2U :
				return L"Seg_Game_Actor::Inf_BrickAppear_G2U";
				break;
			case Inf_APAppear_G2U :
				return L"Seg_Game_Actor::Inf_APAppear_G2U";
				break;
			case Inf_SOAppear_G2U :
				return L"Seg_Game_Actor::Inf_SOAppear_G2U";
				break;
			case Inf_ElevatorAppear_G2U :
				return L"Seg_Game_Actor::Inf_ElevatorAppear_G2U";
				break;
			case Inf_MonorailAppear_G2U :
				return L"Seg_Game_Actor::Inf_MonorailAppear_G2U";
				break;
			case Inf_TardisAppear_G2U :
				return L"Seg_Game_Actor::Inf_TardisAppear_G2U";
				break;
			case Inf_FloatingBoardAppear_G2U :
				return L"Seg_Game_Actor::Inf_FloatingBoardAppear_G2U";
				break;
			case Inf_VehicleAppear_G2U :
				return L"Seg_Game_Actor::Inf_VehicleAppear_G2U";
				break;
			case Inf_SecretDialogAppear_G2U :
				return L"Seg_Game_Actor::Inf_SecretDialogAppear_G2U";
				break;
			case Inf_ActorDisappear_G2U :
				return L"Seg_Game_Actor::Inf_ActorDisappear_G2U";
				break;
			case Inf_ActorDied_G2U :
				return L"Seg_Game_Actor::Inf_ActorDied_G2U";
				break;
			case Inf_ChangeHP_G2U :
				return L"Seg_Game_Actor::Inf_ChangeHP_G2U";
				break;
			case Inf_ChangeWalkSpeed_G2U :
				return L"Seg_Game_Actor::Inf_ChangeWalkSpeed_G2U";
				break;
			case Inf_ChangeSpeed_G2U :
				return L"Seg_Game_Actor::Inf_ChangeSpeed_G2U";
				break;
			case Req_Revive_U2G :
				return L"Seg_Game_Actor::Req_Revive_U2G";
				break;
			case Ack_Revive_G2U :
				return L"Seg_Game_Actor::Ack_Revive_G2U";
				break;
			case Inf_Revive_G2U :
				return L"Seg_Game_Actor::Inf_Revive_G2U";
				break;
			case Inf_ClientDirectSync_G2U :
				return L"Seg_Game_Actor::Inf_ClientDirectSync_G2U";
				break;
			case Req_TriggerEvent_U2G :
				return L"Seg_Game_Actor::Req_TriggerEvent_U2G";
				break;
			case Ack_TriggerEvent_G2U :
				return L"Seg_Game_Actor::Ack_TriggerEvent_G2U";
				break;
			case Req_SkipSequence_U2G :
				return L"Seg_Game_Actor::Req_SkipSequence_U2G";
				break;
			case Ack_SkipSequence_G2U :
				return L"Seg_Game_Actor::Ack_SkipSequence_G2U";
				break;
			case Inf_PlayerLevelActionSequence_G2U :
				return L"Seg_Game_Actor::Inf_PlayerLevelActionSequence_G2U";
				break;
			case Inf_TriggerDisable_G2U :
				return L"Seg_Game_Actor::Inf_TriggerDisable_G2U";
				break;
			case Inf_UsedTriggerList_G2U :
				return L"Seg_Game_Actor::Inf_UsedTriggerList_G2U";
				break;
			case Inf_Warp_G2U :
				return L"Seg_Game_Actor::Inf_Warp_G2U";
				break;
			case Req_Cheat_U2G :
				return L"Seg_Game_Actor::Req_Cheat_U2G";
				break;
			case Ack_Cheat_G2U :
				return L"Seg_Game_Actor::Ack_Cheat_G2U";
				break;
			case Req_ChangePreset_U2G :
				return L"Seg_Game_Actor::Req_ChangePreset_U2G";
				break;
			case Ack_ChangePreset_G2U :
				return L"Seg_Game_Actor::Ack_ChangePreset_G2U";
				break;
			case Inf_ChangePreset_G2U :
				return L"Seg_Game_Actor::Inf_ChangePreset_G2U";
				break;
			case Req_BeginDance_U2G :
				return L"Seg_Game_Actor::Req_BeginDance_U2G";
				break;
			case Ack_BeginDance_G2U :
				return L"Seg_Game_Actor::Ack_BeginDance_G2U";
				break;
			case Req_EndDance_U2G :
				return L"Seg_Game_Actor::Req_EndDance_U2G";
				break;
			case Ack_EndDance_G2U :
				return L"Seg_Game_Actor::Ack_EndDance_G2U";
				break;
			case Req_ChangeDanceStep_U2G :
				return L"Seg_Game_Actor::Req_ChangeDanceStep_U2G";
				break;
			case Ack_ChangeDanceStep_G2U :
				return L"Seg_Game_Actor::Ack_ChangeDanceStep_G2U";
				break;
			case Inf_ChangeDanceStep_G2U :
				return L"Seg_Game_Actor::Inf_ChangeDanceStep_G2U";
				break;
			case Req_BeginTalking_U2G :
				return L"Seg_Game_Actor::Req_BeginTalking_U2G";
				break;
			case Ack_BeginTalking_G2U :
				return L"Seg_Game_Actor::Ack_BeginTalking_G2U";
				break;
			case Req_EndTalking_U2G :
				return L"Seg_Game_Actor::Req_EndTalking_U2G";
				break;
			case Ack_EndTalking_G2U :
				return L"Seg_Game_Actor::Ack_EndTalking_G2U";
				break;
			case Req_SocialAction_U2G :
				return L"Seg_Game_Actor::Req_SocialAction_U2G";
				break;
			case Ack_SocialAction_G2U :
				return L"Seg_Game_Actor::Ack_SocialAction_G2U";
				break;
			case Inf_SocialAction_G2U :
				return L"Seg_Game_Actor::Inf_SocialAction_G2U";
				break;
			case Req_Unoccupy_IO_U2G :
				return L"Seg_Game_Actor::Req_Unoccupy_IO_U2G";
				break;
			case Ack_Unoccupy_IO_G2U :
				return L"Seg_Game_Actor::Ack_Unoccupy_IO_G2U";
				break;
			case Req_SuggestCheers_U2G :
				return L"Seg_Game_Actor::Req_SuggestCheers_U2G";
				break;
			case Ack_SuggestCheers_G2U :
				return L"Seg_Game_Actor::Ack_SuggestCheers_G2U";
				break;
			case Inf_SuggestCheers_G2U :
				return L"Seg_Game_Actor::Inf_SuggestCheers_G2U";
				break;
			case Req_AnswerCheers_U2G :
				return L"Seg_Game_Actor::Req_AnswerCheers_U2G";
				break;
			case Ack_AnswerCheers_G2U :
				return L"Seg_Game_Actor::Ack_AnswerCheers_G2U";
				break;
			case Inf_AnswerCheers_G2U :
				return L"Seg_Game_Actor::Inf_AnswerCheers_G2U";
				break;
			case Req_BeginCheers_U2G :
				return L"Seg_Game_Actor::Req_BeginCheers_U2G";
				break;
			case Ack_BeginCheers_G2U :
				return L"Seg_Game_Actor::Ack_BeginCheers_G2U";
				break;
			case Inf_BeginCheers_G2U :
				return L"Seg_Game_Actor::Inf_BeginCheers_G2U";
				break;
			case Req_CancelCheers_U2G :
				return L"Seg_Game_Actor::Req_CancelCheers_U2G";
				break;
			case Ack_CancelCheers_G2U :
				return L"Seg_Game_Actor::Ack_CancelCheers_G2U";
				break;
			case Inf_CancelCheers_G2U :
				return L"Seg_Game_Actor::Inf_CancelCheers_G2U";
				break;
			case Req_SpeechBubble_U2G :
				return L"Seg_Game_Actor::Req_SpeechBubble_U2G";
				break;
			case Ack_SpeechBubble_G2U :
				return L"Seg_Game_Actor::Ack_SpeechBubble_G2U";
				break;
			case Inf_SpeechBubble_G2U :
				return L"Seg_Game_Actor::Inf_SpeechBubble_G2U";
				break;
			case Req_SetFocusedAID_U2G :
				return L"Seg_Game_Actor::Req_SetFocusedAID_U2G";
				break;
			case Ack_SetFocusedAID_G2U :
				return L"Seg_Game_Actor::Ack_SetFocusedAID_G2U";
				break;
			case Inf_SetFocusedAID_G2U :
				return L"Seg_Game_Actor::Inf_SetFocusedAID_G2U";
				break;
			case Req_VoiceMessage_U2G :
				return L"Seg_Game_Actor::Req_VoiceMessage_U2G";
				break;
			case Ack_VoiceMessage_G2U :
				return L"Seg_Game_Actor::Ack_VoiceMessage_G2U";
				break;
			case Inf_VoiceMessage_G2U :
				return L"Seg_Game_Actor::Inf_VoiceMessage_G2U";
				break;
			case Req_VoiceChat_U2G :
				return L"Seg_Game_Actor::Req_VoiceChat_U2G";
				break;
			case Ack_VoiceChat_G2U :
				return L"Seg_Game_Actor::Ack_VoiceChat_G2U";
				break;
			case Inf_VoiceChat_G2U :
				return L"Seg_Game_Actor::Inf_VoiceChat_G2U";
				break;
			case Req_SetLanguageType_U2G :
				return L"Seg_Game_Actor::Req_SetLanguageType_U2G";
				break;
			case Ack_SetLanguageType_G2U :
				return L"Seg_Game_Actor::Ack_SetLanguageType_G2U";
				break;
			case Inf_SetLanguageType_G2U :
				return L"Seg_Game_Actor::Inf_SetLanguageType_G2U";
				break;
			case Req_AgreeToSecretDialog_U2G :
				return L"Seg_Game_Actor::Req_AgreeToSecretDialog_U2G";
				break;
			case Ack_AgreeToSecretDialog_G2U :
				return L"Seg_Game_Actor::Ack_AgreeToSecretDialog_G2U";
				break;
			case Inf_AgreeToSecretDialog_G2U :
				return L"Seg_Game_Actor::Inf_AgreeToSecretDialog_G2U";
				break;
			case Cmd_AgreeToSecretDialog_G2U :
				return L"Seg_Game_Actor::Cmd_AgreeToSecretDialog_G2U";
				break;
			case Rpt_AgreeToSecretDialog_U2G :
				return L"Seg_Game_Actor::Rpt_AgreeToSecretDialog_U2G";
				break;
			case Req_EndOfSecretDialog_U2G :
				return L"Seg_Game_Actor::Req_EndOfSecretDialog_U2G";
				break;
			case Ack_EndOfSecretDialog_G2U :
				return L"Seg_Game_Actor::Ack_EndOfSecretDialog_G2U";
				break;
			case Inf_EndOfSecretDialog_G2U :
				return L"Seg_Game_Actor::Inf_EndOfSecretDialog_G2U";
				break;
			case Req_SecretDialogInfo_U2G :
				return L"Seg_Game_Actor::Req_SecretDialogInfo_U2G";
				break;
			case Ack_SecretDialogInfo_G2U :
				return L"Seg_Game_Actor::Ack_SecretDialogInfo_G2U";
				break;
			case Req_DBSaveAnswerToNpc_U2G :
				return L"Seg_Game_Actor::Req_DBSaveAnswerToNpc_U2G";
				break;
			case Ack_DBSaveAnswerToNpc_G2U :
				return L"Seg_Game_Actor::Ack_DBSaveAnswerToNpc_G2U";
				break;
			case Req_LifeEventReward_U2G :
				return L"Seg_Game_Actor::Req_LifeEventReward_U2G";
				break;
			case Ack_LifeEventReward_G2U :
				return L"Seg_Game_Actor::Ack_LifeEventReward_G2U";
				break;
			case Req_RegistSpeaker_U2G :
				return L"Seg_Game_Actor::Req_RegistSpeaker_U2G";
				break;
			case Ack_RegistSpeaker_G2U :
				return L"Seg_Game_Actor::Ack_RegistSpeaker_G2U";
				break;
			case Inf_RegistSpeaker_G2U :
				return L"Seg_Game_Actor::Inf_RegistSpeaker_G2U";
				break;
			case Req_DeregistSpeaker_U2G :
				return L"Seg_Game_Actor::Req_DeregistSpeaker_U2G";
				break;
			case Ack_DeregistSpeaker_G2U :
				return L"Seg_Game_Actor::Ack_DeregistSpeaker_G2U";
				break;
			case Inf_DeregistSpeaker_G2U :
				return L"Seg_Game_Actor::Inf_DeregistSpeaker_G2U";
				break;
			case Req_ReleaseListener_U2G :
				return L"Seg_Game_Actor::Req_ReleaseListener_U2G";
				break;
			case Ack_ReleaseListener_G2U :
				return L"Seg_Game_Actor::Ack_ReleaseListener_G2U";
				break;
			case Inf_ReleaseListener_G2U :
				return L"Seg_Game_Actor::Inf_ReleaseListener_G2U";
				break;
			case Req_VideoChatList_U2G :
				return L"Seg_Game_Actor::Req_VideoChatList_U2G";
				break;
			case Ack_VideoChatList_G2U :
				return L"Seg_Game_Actor::Ack_VideoChatList_G2U";
				break;
			case Req_BeginVideoChat_U2G :
				return L"Seg_Game_Actor::Req_BeginVideoChat_U2G";
				break;
			case Ack_BeginVideoChat_G2U :
				return L"Seg_Game_Actor::Ack_BeginVideoChat_G2U";
				break;
			case Inf_BeginVideoChat_G2U :
				return L"Seg_Game_Actor::Inf_BeginVideoChat_G2U";
				break;
			case Req_Agree2BeginVideoChat_U2G :
				return L"Seg_Game_Actor::Req_Agree2BeginVideoChat_U2G";
				break;
			case Ack_Agree2BeginVideoChat_G2U :
				return L"Seg_Game_Actor::Ack_Agree2BeginVideoChat_G2U";
				break;
			case Inf_Agree2BeginVideoChat_G2U :
				return L"Seg_Game_Actor::Inf_Agree2BeginVideoChat_G2U";
				break;
			case Req_EndVideoChat_U2G :
				return L"Seg_Game_Actor::Req_EndVideoChat_U2G";
				break;
			case Ack_EndVideoChat_G2U :
				return L"Seg_Game_Actor::Ack_EndVideoChat_G2U";
				break;
			case Inf_EndVideoChat_G2U :
				return L"Seg_Game_Actor::Inf_EndVideoChat_G2U";
				break;
			case Inf_ChangeAPState_G2U :
				return L"Seg_Game_Actor::Inf_ChangeAPState_G2U";
				break;
			case Req_MicAnimation_U2G :
				return L"Seg_Game_Actor::Req_MicAnimation_U2G";
				break;
			case Ack_MicAnimation_G2U :
				return L"Seg_Game_Actor::Ack_MicAnimation_G2U";
				break;
			case Inf_MicAnimation_G2U :
				return L"Seg_Game_Actor::Inf_MicAnimation_G2U";
				break;
			case Req_RegistVideoSpeaker_U2G :
				return L"Seg_Game_Actor::Req_RegistVideoSpeaker_U2G";
				break;
			case Ack_RegistVideoSpeaker_G2U :
				return L"Seg_Game_Actor::Ack_RegistVideoSpeaker_G2U";
				break;
			case Inf_RegistVideoSpeaker_G2U :
				return L"Seg_Game_Actor::Inf_RegistVideoSpeaker_G2U";
				break;
			case Req_DeregistVideoSpeaker_U2G :
				return L"Seg_Game_Actor::Req_DeregistVideoSpeaker_U2G";
				break;
			case Ack_DeregistVideoSpeaker_G2U :
				return L"Seg_Game_Actor::Ack_DeregistVideoSpeaker_G2U";
				break;
			case Inf_DeregistVideoSpeaker_G2U :
				return L"Seg_Game_Actor::Inf_DeregistVideoSpeaker_G2U";
				break;
			case Req_BroadcastTestPacket_U2G :
				return L"Seg_Game_Actor::Req_BroadcastTestPacket_U2G";
				break;
			case Ack_BroadcastTestPacket_G2U :
				return L"Seg_Game_Actor::Ack_BroadcastTestPacket_G2U";
				break;
			case Inf_BroadcastTestPacket_G2U :
				return L"Seg_Game_Actor::Inf_BroadcastTestPacket_G2U";
				break;
			case Inf_SekaiItemInfo_G2U :
				return L"Seg_Game_Actor::Inf_SekaiItemInfo_G2U";
				break;
			case Inf_SekaiCurrencyInfo_G2U :
				return L"Seg_Game_Actor::Inf_SekaiCurrencyInfo_G2U";
				break;
		}
		break;
	}
	break;
	case Seg_Game_User:
	{
		switch ( pro )
		{
			case Req_EnterGame_U2G :
				return L"Seg_Game_User::Req_EnterGame_U2G";
				break;
			case Ack_EnterGame_G2U :
				return L"Seg_Game_User::Ack_EnterGame_G2U";
				break;
			case Inf_KickUser_G2U :
				return L"Seg_Game_User::Inf_KickUser_G2U";
				break;
			case Req_LogoutUser_U2G :
				return L"Seg_Game_User::Req_LogoutUser_U2G";
				break;
			case Ack_LogoutUser_G2U :
				return L"Seg_Game_User::Ack_LogoutUser_G2U";
				break;
			case Req_MoveToLab_U2G :
				return L"Seg_Game_User::Req_MoveToLab_U2G";
				break;
			case Ack_MoveToLab_G2U :
				return L"Seg_Game_User::Ack_MoveToLab_G2U";
				break;
			case Req_PlayerList_U2G :
				return L"Seg_Game_User::Req_PlayerList_U2G";
				break;
			case Ack_PlayerList_G2U :
				return L"Seg_Game_User::Ack_PlayerList_G2U";
				break;
			case Req_SelectPlayerName_U2G :
				return L"Seg_Game_User::Req_SelectPlayerName_U2G";
				break;
			case Ack_SelectPlayerName_G2U :
				return L"Seg_Game_User::Ack_SelectPlayerName_G2U";
				break;
			case Req_CreatePlayer_U2G :
				return L"Seg_Game_User::Req_CreatePlayer_U2G";
				break;
			case Ack_CreatePlayer_G2U :
				return L"Seg_Game_User::Ack_CreatePlayer_G2U";
				break;
			case Req_DeletePlayer_U2G :
				return L"Seg_Game_User::Req_DeletePlayer_U2G";
				break;
			case Ack_DeletePlayer_G2U :
				return L"Seg_Game_User::Ack_DeletePlayer_G2U";
				break;
			case Req_SelectPlayer_U2G :
				return L"Seg_Game_User::Req_SelectPlayer_U2G";
				break;
			case Ack_SelectPlayer_G2U :
				return L"Seg_Game_User::Ack_SelectPlayer_G2U";
				break;
			case Req_ReserveMap_U2G :
				return L"Seg_Game_User::Req_ReserveMap_U2G";
				break;
			case Ack_ReserveMap_G2U :
				return L"Seg_Game_User::Ack_ReserveMap_G2U";
				break;
			case Req_ExitMap_U2G :
				return L"Seg_Game_User::Req_ExitMap_U2G";
				break;
			case Ack_ExitMap_G2U :
				return L"Seg_Game_User::Ack_ExitMap_G2U";
				break;
			case Req_EnterMap_U2G :
				return L"Seg_Game_User::Req_EnterMap_U2G";
				break;
			case Ack_EnterMap_G2U :
				return L"Seg_Game_User::Ack_EnterMap_G2U";
				break;
			case Req_ReadyToPlay_U2G :
				return L"Seg_Game_User::Req_ReadyToPlay_U2G";
				break;
			case Ack_ReadyToPlay_G2U :
				return L"Seg_Game_User::Ack_ReadyToPlay_G2U";
				break;
			case Inf_ReserveMapForGroup_G2U :
				return L"Seg_Game_User::Inf_ReserveMapForGroup_G2U";
				break;
			case Req_EntryInfo_U2G :
				return L"Seg_Game_User::Req_EntryInfo_U2G";
				break;
			case Ack_EntryInfo_G2U :
				return L"Seg_Game_User::Ack_EntryInfo_G2U";
				break;
			case Inf_EntryInfo_Item_G2U :
				return L"Seg_Game_User::Inf_EntryInfo_Item_G2U";
				break;
			case Req_PlayerMove_U2G :
				return L"Seg_Game_User::Req_PlayerMove_U2G";
				break;
			case Ack_PlayerMove_G2U :
				return L"Seg_Game_User::Ack_PlayerMove_G2U";
				break;
			case Inf_PlayerMove_G2U :
				return L"Seg_Game_User::Inf_PlayerMove_G2U";
				break;
			case Req_PlayerStop_U2G :
				return L"Seg_Game_User::Req_PlayerStop_U2G";
				break;
			case Ack_PlayerStop_G2U :
				return L"Seg_Game_User::Ack_PlayerStop_G2U";
				break;
			case Inf_PlayerStop_G2U :
				return L"Seg_Game_User::Inf_PlayerStop_G2U";
				break;
			case Inf_NpcMove_G2U :
				return L"Seg_Game_User::Inf_NpcMove_G2U";
				break;
			case Req_UseSkill_U2G :
				return L"Seg_Game_User::Req_UseSkill_U2G";
				break;
			case Ack_UseSkill_G2U :
				return L"Seg_Game_User::Ack_UseSkill_G2U";
				break;
			case Inf_UseSkill_G2U :
				return L"Seg_Game_User::Inf_UseSkill_G2U";
				break;
			case Req_GuardBreak_U2G :
				return L"Seg_Game_User::Req_GuardBreak_U2G";
				break;
			case Ack_GuardBreak_G2U :
				return L"Seg_Game_User::Ack_GuardBreak_G2U";
				break;
			case Inf_GuardBreak_G2U :
				return L"Seg_Game_User::Inf_GuardBreak_G2U";
				break;
			case Req_SuccessToShield_U2G :
				return L"Seg_Game_User::Req_SuccessToShield_U2G";
				break;
			case Ack_SuccessToShield_G2U :
				return L"Seg_Game_User::Ack_SuccessToShield_G2U";
				break;
			case Inf_SuccessToShield_G2U :
				return L"Seg_Game_User::Inf_SuccessToShield_G2U";
				break;
			case Req_HitResult_U2G :
				return L"Seg_Game_User::Req_HitResult_U2G";
				break;
			case Ack_HitResult_G2U :
				return L"Seg_Game_User::Ack_HitResult_G2U";
				break;
			case Inf_HitResult_G2U :
				return L"Seg_Game_User::Inf_HitResult_G2U";
				break;
			case Req_SuccessToAvoid_U2G :
				return L"Seg_Game_User::Req_SuccessToAvoid_U2G";
				break;
			case Ack_SuccessToAvoid_G2U :
				return L"Seg_Game_User::Ack_SuccessToAvoid_G2U";
				break;
			case Inf_SuccessToAvoid_G2U :
				return L"Seg_Game_User::Inf_SuccessToAvoid_G2U";
				break;
			case Req_ActorMoveToPos_U2G :
				return L"Seg_Game_User::Req_ActorMoveToPos_U2G";
				break;
			case Ack_ActorMoveToPos_G2U :
				return L"Seg_Game_User::Ack_ActorMoveToPos_G2U";
				break;
			case Inf_ActorMoveToPos_G2U :
				return L"Seg_Game_User::Inf_ActorMoveToPos_G2U";
				break;
			case Inf_ActorMoveStop_G2U :
				return L"Seg_Game_User::Inf_ActorMoveStop_G2U";
				break;
			case Req_SyncDynamicMoveLocation_U2G :
				return L"Seg_Game_User::Req_SyncDynamicMoveLocation_U2G";
				break;
			case Ack_SyncDynamicMoveLocation_G2U :
				return L"Seg_Game_User::Ack_SyncDynamicMoveLocation_G2U";
				break;
			case Inf_SyncDynamicMoveLocation_G2U :
				return L"Seg_Game_User::Inf_SyncDynamicMoveLocation_G2U";
				break;
			case Req_BeginSkill_U2G :
				return L"Seg_Game_User::Req_BeginSkill_U2G";
				break;
			case Ack_BeginSkill_G2U :
				return L"Seg_Game_User::Ack_BeginSkill_G2U";
				break;
			case Inf_BeginSkill_G2U :
				return L"Seg_Game_User::Inf_BeginSkill_G2U";
				break;
			case Req_EndSkill_U2G :
				return L"Seg_Game_User::Req_EndSkill_U2G";
				break;
			case Ack_EndSkill_G2U :
				return L"Seg_Game_User::Ack_EndSkill_G2U";
				break;
			case Inf_EndSkill_G2U :
				return L"Seg_Game_User::Inf_EndSkill_G2U";
				break;
			case Inf_SetTarget_G2U :
				return L"Seg_Game_User::Inf_SetTarget_G2U";
				break;
			case Req_BeginInteraction_U2G :
				return L"Seg_Game_User::Req_BeginInteraction_U2G";
				break;
			case Ack_BeginInteraction_G2U :
				return L"Seg_Game_User::Ack_BeginInteraction_G2U";
				break;
			case Inf_BeginInteraction_G2U :
				return L"Seg_Game_User::Inf_BeginInteraction_G2U";
				break;
			case Req_StopInteraction_U2G :
				return L"Seg_Game_User::Req_StopInteraction_U2G";
				break;
			case Ack_StopInteraction_G2U :
				return L"Seg_Game_User::Ack_StopInteraction_G2U";
				break;
			case Inf_StopInteraction_G2U :
				return L"Seg_Game_User::Inf_StopInteraction_G2U";
				break;
			case Req_EndInteraction_U2G :
				return L"Seg_Game_User::Req_EndInteraction_U2G";
				break;
			case Ack_EndInteraction_G2U :
				return L"Seg_Game_User::Ack_EndInteraction_G2U";
				break;
			case Inf_EndInteraction_G2U :
				return L"Seg_Game_User::Inf_EndInteraction_G2U";
				break;
			case Inf_NpcDialog_G2U :
				return L"Seg_Game_User::Inf_NpcDialog_G2U";
				break;
			case Inf_ActorCCState_G2U :
				return L"Seg_Game_User::Inf_ActorCCState_G2U";
				break;
			case Inf_ActorWarp_G2U :
				return L"Seg_Game_User::Inf_ActorWarp_G2U";
				break;
			case Inf_BeginBuff_G2U :
				return L"Seg_Game_User::Inf_BeginBuff_G2U";
				break;
			case Inf_EndBuff_G2U :
				return L"Seg_Game_User::Inf_EndBuff_G2U";
				break;
			case Inf_DotBuff_G2U :
				return L"Seg_Game_User::Inf_DotBuff_G2U";
				break;
			case Req_CallADA_U2G :
				return L"Seg_Game_User::Req_CallADA_U2G";
				break;
			case Ack_CallADA_G2U :
				return L"Seg_Game_User::Ack_CallADA_G2U";
				break;
			case Req_PlayerWarp_U2G :
				return L"Seg_Game_User::Req_PlayerWarp_U2G";
				break;
			case Ack_PlayerWarp_G2U :
				return L"Seg_Game_User::Ack_PlayerWarp_G2U";
				break;
			case Inf_BeginNpcHook_G2U :
				return L"Seg_Game_User::Inf_BeginNpcHook_G2U";
				break;
			case Inf_EndNpcHook_G2U :
				return L"Seg_Game_User::Inf_EndNpcHook_G2U";
				break;
			case Req_DynamicMove_U2G :
				return L"Seg_Game_User::Req_DynamicMove_U2G";
				break;
			case Ack_DynamicMove_G2U :
				return L"Seg_Game_User::Ack_DynamicMove_G2U";
				break;
			case Inf_DynamicMove_G2U :
				return L"Seg_Game_User::Inf_DynamicMove_G2U";
				break;
			case Req_BeginLadder_U2G :
				return L"Seg_Game_User::Req_BeginLadder_U2G";
				break;
			case Ack_BeginLadder_G2U :
				return L"Seg_Game_User::Ack_BeginLadder_G2U";
				break;
			case Req_MoveLadder_U2G :
				return L"Seg_Game_User::Req_MoveLadder_U2G";
				break;
			case Ack_MoveLadder_G2U :
				return L"Seg_Game_User::Ack_MoveLadder_G2U";
				break;
			case Req_EndLadder_U2G :
				return L"Seg_Game_User::Req_EndLadder_U2G";
				break;
			case Ack_EndLadder_G2U :
				return L"Seg_Game_User::Ack_EndLadder_G2U";
				break;
			case Req_CancelLadder_U2G :
				return L"Seg_Game_User::Req_CancelLadder_U2G";
				break;
			case Ack_CancelLadder_G2U :
				return L"Seg_Game_User::Ack_CancelLadder_G2U";
				break;
			case Req_BeginFall_U2G :
				return L"Seg_Game_User::Req_BeginFall_U2G";
				break;
			case Ack_BeginFall_G2U :
				return L"Seg_Game_User::Ack_BeginFall_G2U";
				break;
			case Inf_BeginFall_G2U :
				return L"Seg_Game_User::Inf_BeginFall_G2U";
				break;
			case Req_EndFallen_U2G :
				return L"Seg_Game_User::Req_EndFallen_U2G";
				break;
			case Ack_EndFallen_G2U :
				return L"Seg_Game_User::Ack_EndFallen_G2U";
				break;
			case Inf_EndFallen_G2U :
				return L"Seg_Game_User::Inf_EndFallen_G2U";
				break;
			case Req_SyncHMD_U2G :
				return L"Seg_Game_User::Req_SyncHMD_U2G";
				break;
			case Ack_SyncHMD_G2U :
				return L"Seg_Game_User::Ack_SyncHMD_G2U";
				break;
			case Inf_SyncHMD_G2U :
				return L"Seg_Game_User::Inf_SyncHMD_G2U";
				break;
			case Inf_BattleState_G2U :
				return L"Seg_Game_User::Inf_BattleState_G2U";
				break;
			case Req_BeginVoiceCommand_U2G :
				return L"Seg_Game_User::Req_BeginVoiceCommand_U2G";
				break;
			case Ack_BeginVoiceCommand_G2U :
				return L"Seg_Game_User::Ack_BeginVoiceCommand_G2U";
				break;
			case Req_EndVoiceCommand_U2G :
				return L"Seg_Game_User::Req_EndVoiceCommand_U2G";
				break;
			case Ack_EndVoiceCommand_G2U :
				return L"Seg_Game_User::Ack_EndVoiceCommand_G2U";
				break;
		}
		break;
	}
	break;
	case Seg_Game_User_Currency:
	{
		switch ( pro )
		{
			case Inf_UpdateCurrency_G2U :
				return L"Seg_Game_User_Currency::Inf_UpdateCurrency_G2U";
				break;
		}
		break;
	}
	break;
	case Seg_Game_User_Inventory:
	{
		switch ( pro )
		{
			case Req_EquipItem_U2G :
				return L"Seg_Game_User_Inventory::Req_EquipItem_U2G";
				break;
			case Ack_EquipItem_G2U :
				return L"Seg_Game_User_Inventory::Ack_EquipItem_G2U";
				break;
			case Req_UnEquipItem_U2G :
				return L"Seg_Game_User_Inventory::Req_UnEquipItem_U2G";
				break;
			case Ack_UnEquipItem_G2U :
				return L"Seg_Game_User_Inventory::Ack_UnEquipItem_G2U";
				break;
			case Req_RegisterEquipmentSlot_U2G :
				return L"Seg_Game_User_Inventory::Req_RegisterEquipmentSlot_U2G";
				break;
			case Ack_RegisterEquipmentSlot_G2U :
				return L"Seg_Game_User_Inventory::Ack_RegisterEquipmentSlot_G2U";
				break;
			case Req_UnRegisterEquipmentSlot_U2G :
				return L"Seg_Game_User_Inventory::Req_UnRegisterEquipmentSlot_U2G";
				break;
			case Ack_UnRegisterEquipmentSlot_G2U :
				return L"Seg_Game_User_Inventory::Ack_UnRegisterEquipmentSlot_G2U";
				break;
			case Inf_ChangeEquipment_G2U :
				return L"Seg_Game_User_Inventory::Inf_ChangeEquipment_G2U";
				break;
			case Inf_ChangeCostume_G2U :
				return L"Seg_Game_User_Inventory::Inf_ChangeCostume_G2U";
				break;
			case Inf_AddItem_G2U :
				return L"Seg_Game_User_Inventory::Inf_AddItem_G2U";
				break;
			case Inf_DeleteItem_G2U :
				return L"Seg_Game_User_Inventory::Inf_DeleteItem_G2U";
				break;
			case Inf_UpdateItemCount_G2U :
				return L"Seg_Game_User_Inventory::Inf_UpdateItemCount_G2U";
				break;
			case Req_DiscardItem_U2G :
				return L"Seg_Game_User_Inventory::Req_DiscardItem_U2G";
				break;
			case Ack_DiscardItem_G2U :
				return L"Seg_Game_User_Inventory::Ack_DiscardItem_G2U";
				break;
			case Req_UseItem_U2G :
				return L"Seg_Game_User_Inventory::Req_UseItem_U2G";
				break;
			case Ack_UseItem_G2U :
				return L"Seg_Game_User_Inventory::Ack_UseItem_G2U";
				break;
			case Req_ArrangeInteriorItem_U2G :
				return L"Seg_Game_User_Inventory::Req_ArrangeInteriorItem_U2G";
				break;
			case Ack_ArrangeInteriorItem_G2U :
				return L"Seg_Game_User_Inventory::Ack_ArrangeInteriorItem_G2U";
				break;
			case Req_RemoveInteriorItem_U2G :
				return L"Seg_Game_User_Inventory::Req_RemoveInteriorItem_U2G";
				break;
			case Ack_RemoveInteriorItem_G2U :
				return L"Seg_Game_User_Inventory::Ack_RemoveInteriorItem_G2U";
				break;
		}
		break;
	}
	break;
	case Seg_Game_User_Quest:
	{
		switch ( pro )
		{
			case Req_ProgressQuestList_U2G :
				return L"Seg_Game_User_Quest::Req_ProgressQuestList_U2G";
				break;
			case Ack_ProgressQuestList_G2U :
				return L"Seg_Game_User_Quest::Ack_ProgressQuestList_G2U";
				break;
			case Req_CompleteQuestList_U2G :
				return L"Seg_Game_User_Quest::Req_CompleteQuestList_U2G";
				break;
			case Ack_CompleteQuestList_G2U :
				return L"Seg_Game_User_Quest::Ack_CompleteQuestList_G2U";
				break;
			case Req_AcceptQuest_U2G :
				return L"Seg_Game_User_Quest::Req_AcceptQuest_U2G";
				break;
			case Ack_AcceptQuest_G2U :
				return L"Seg_Game_User_Quest::Ack_AcceptQuest_G2U";
				break;
			case Req_CompletableQuest_U2G :
				return L"Seg_Game_User_Quest::Req_CompletableQuest_U2G";
				break;
			case Ack_CompletableQuest_G2U :
				return L"Seg_Game_User_Quest::Ack_CompletableQuest_G2U";
				break;
			case Req_CompleteQuest_U2G :
				return L"Seg_Game_User_Quest::Req_CompleteQuest_U2G";
				break;
			case Ack_CompleteQuest_G2U :
				return L"Seg_Game_User_Quest::Ack_CompleteQuest_G2U";
				break;
			case Req_CompleteQuestTask_U2G :
				return L"Seg_Game_User_Quest::Req_CompleteQuestTask_U2G";
				break;
			case Ack_CompleteQuestTask_G2U :
				return L"Seg_Game_User_Quest::Ack_CompleteQuestTask_G2U";
				break;
			case Inf_CompleteQuestTask_G2U :
				return L"Seg_Game_User_Quest::Inf_CompleteQuestTask_G2U";
				break;
			case Inf_UpdateQuestTask_G2U :
				return L"Seg_Game_User_Quest::Inf_UpdateQuestTask_G2U";
				break;
			case Inf_ForcedQuestComplete_G2U :
				return L"Seg_Game_User_Quest::Inf_ForcedQuestComplete_G2U";
				break;
			case Inf_ForcedQuestGiveUp_G2U :
				return L"Seg_Game_User_Quest::Inf_ForcedQuestGiveUp_G2U";
				break;
		}
		break;
	}
	break;
	case Seg_Game_Vehicle:
	{
		switch ( pro )
		{
			case Inf_TransportState_G2U :
				return L"Seg_Game_Vehicle::Inf_TransportState_G2U";
				break;
			case Req_GetTransport_U2G :
				return L"Seg_Game_Vehicle::Req_GetTransport_U2G";
				break;
			case Ack_GetOnTransport_G2U :
				return L"Seg_Game_Vehicle::Ack_GetOnTransport_G2U";
				break;
			case Inf_GetOnTransport_G2U :
				return L"Seg_Game_Vehicle::Inf_GetOnTransport_G2U";
				break;
			case Req_GetOffTransport_U2G :
				return L"Seg_Game_Vehicle::Req_GetOffTransport_U2G";
				break;
			case Ack_GetOffTransport_G2U :
				return L"Seg_Game_Vehicle::Ack_GetOffTransport_G2U";
				break;
			case Inf_GetOffTransport_G2U :
				return L"Seg_Game_Vehicle::Inf_GetOffTransport_G2U";
				break;
			case Inf_CompleteWarp_G2U :
				return L"Seg_Game_Vehicle::Inf_CompleteWarp_G2U";
				break;
			case Inf_GroupWarpTimerCount_G2U :
				return L"Seg_Game_Vehicle::Inf_GroupWarpTimerCount_G2U";
				break;
			case Inf_GroupWarpStop_G2U :
				return L"Seg_Game_Vehicle::Inf_GroupWarpStop_G2U";
				break;
			case Req_PlayerMoveInTransport_U2G :
				return L"Seg_Game_Vehicle::Req_PlayerMoveInTransport_U2G";
				break;
			case Ack_PlayerMoveInTransport_U2G :
				return L"Seg_Game_Vehicle::Ack_PlayerMoveInTransport_U2G";
				break;
			case Inf_PlayerMoveInTransport_G2U :
				return L"Seg_Game_Vehicle::Inf_PlayerMoveInTransport_G2U";
				break;
			case Req_PlayerStopInTransport_U2G :
				return L"Seg_Game_Vehicle::Req_PlayerStopInTransport_U2G";
				break;
			case Ack_PlayerStopInTransport_G2U :
				return L"Seg_Game_Vehicle::Ack_PlayerStopInTransport_G2U";
				break;
			case Inf_PlayerStopInTransport_G2U :
				return L"Seg_Game_Vehicle::Inf_PlayerStopInTransport_G2U";
				break;
			case Inf_ActorMoveStopInVehicle_G2U :
				return L"Seg_Game_Vehicle::Inf_ActorMoveStopInVehicle_G2U";
				break;
			case Req_GetOnMW_U2G :
				return L"Seg_Game_Vehicle::Req_GetOnMW_U2G";
				break;
			case Ack_GetOnMW_G2U :
				return L"Seg_Game_Vehicle::Ack_GetOnMW_G2U";
				break;
			case Inf_GetOnMW_G2U :
				return L"Seg_Game_Vehicle::Inf_GetOnMW_G2U";
				break;
			case Req_GetOffMW_U2G :
				return L"Seg_Game_Vehicle::Req_GetOffMW_U2G";
				break;
			case Ack_GetOffMW_G2U :
				return L"Seg_Game_Vehicle::Ack_GetOffMW_G2U";
				break;
			case Inf_GetOffMW_G2U :
				return L"Seg_Game_Vehicle::Inf_GetOffMW_G2U";
				break;
			case Req_PlayerMoveInMW_U2G :
				return L"Seg_Game_Vehicle::Req_PlayerMoveInMW_U2G";
				break;
			case Ack_PlayerMoveInMW_G2U :
				return L"Seg_Game_Vehicle::Ack_PlayerMoveInMW_G2U";
				break;
			case Inf_PlayerMoveInMW_G2U :
				return L"Seg_Game_Vehicle::Inf_PlayerMoveInMW_G2U";
				break;
			case Req_PlayerStopInMW_U2G :
				return L"Seg_Game_Vehicle::Req_PlayerStopInMW_U2G";
				break;
			case Ack_PlayerStopInMW_G2U :
				return L"Seg_Game_Vehicle::Ack_PlayerStopInMW_G2U";
				break;
			case Inf_PlayerStopInMW_G2U :
				return L"Seg_Game_Vehicle::Inf_PlayerStopInMW_G2U";
				break;
			case Inf_ActorMoveStopInMW_G2U :
				return L"Seg_Game_Vehicle::Inf_ActorMoveStopInMW_G2U";
				break;
			case Req_SpawnVehicle_U2G :
				return L"Seg_Game_Vehicle::Req_SpawnVehicle_U2G";
				break;
			case Ack_SpawnVehicle_G2U :
				return L"Seg_Game_Vehicle::Ack_SpawnVehicle_G2U";
				break;
			case Req_UnspawnVehicle_U2G :
				return L"Seg_Game_Vehicle::Req_UnspawnVehicle_U2G";
				break;
			case Ack_UnspawnVehicle_G2U :
				return L"Seg_Game_Vehicle::Ack_UnspawnVehicle_G2U";
				break;
			case Req_GetOnVehicle_U2G :
				return L"Seg_Game_Vehicle::Req_GetOnVehicle_U2G";
				break;
			case Ack_GetOnVehicle_G2U :
				return L"Seg_Game_Vehicle::Ack_GetOnVehicle_G2U";
				break;
			case Inf_GetOnVehicle_G2U :
				return L"Seg_Game_Vehicle::Inf_GetOnVehicle_G2U";
				break;
			case Req_AgreeToBoard_U2G :
				return L"Seg_Game_Vehicle::Req_AgreeToBoard_U2G";
				break;
			case Ack_AgreeToBoard_G2U :
				return L"Seg_Game_Vehicle::Ack_AgreeToBoard_G2U";
				break;
			case Cmd_AgreeToBoard_G2U :
				return L"Seg_Game_Vehicle::Cmd_AgreeToBoard_G2U";
				break;
			case Rpt_AgreeToBoard_U2G :
				return L"Seg_Game_Vehicle::Rpt_AgreeToBoard_U2G";
				break;
			case Req_GetOffVehicle_U2G :
				return L"Seg_Game_Vehicle::Req_GetOffVehicle_U2G";
				break;
			case Ack_GetOffVehicle_G2U :
				return L"Seg_Game_Vehicle::Ack_GetOffVehicle_G2U";
				break;
			case Inf_GetOffVehicle_G2U :
				return L"Seg_Game_Vehicle::Inf_GetOffVehicle_G2U";
				break;
			case Req_BeginBrake_U2G :
				return L"Seg_Game_Vehicle::Req_BeginBrake_U2G";
				break;
			case Ack_BeginBrake_G2U :
				return L"Seg_Game_Vehicle::Ack_BeginBrake_G2U";
				break;
			case Inf_BeginBrake_G2U :
				return L"Seg_Game_Vehicle::Inf_BeginBrake_G2U";
				break;
			case Req_EndBrake_U2G :
				return L"Seg_Game_Vehicle::Req_EndBrake_U2G";
				break;
			case Ack_EndBrake_G2U :
				return L"Seg_Game_Vehicle::Ack_EndBrake_G2U";
				break;
			case Inf_EndBrake_G2U :
				return L"Seg_Game_Vehicle::Inf_EndBrake_G2U";
				break;
			case Req_Acceleration_U2G :
				return L"Seg_Game_Vehicle::Req_Acceleration_U2G";
				break;
			case Ack_Acceleration_G2U :
				return L"Seg_Game_Vehicle::Ack_Acceleration_G2U";
				break;
			case Inf_Acceleration_G2U :
				return L"Seg_Game_Vehicle::Inf_Acceleration_G2U";
				break;
			case Req_Steering_U2G :
				return L"Seg_Game_Vehicle::Req_Steering_U2G";
				break;
			case Ack_Steering_G2U :
				return L"Seg_Game_Vehicle::Ack_Steering_G2U";
				break;
			case Inf_Steering_G2U :
				return L"Seg_Game_Vehicle::Inf_Steering_G2U";
				break;
			case Req_BeHorning_U2G :
				return L"Seg_Game_Vehicle::Req_BeHorning_U2G";
				break;
			case Ack_BeHorning_G2U :
				return L"Seg_Game_Vehicle::Ack_BeHorning_G2U";
				break;
			case Inf_BeHorning_G2U :
				return L"Seg_Game_Vehicle::Inf_BeHorning_G2U";
				break;
			case Req_UpdateExterior_U2G :
				return L"Seg_Game_Vehicle::Req_UpdateExterior_U2G";
				break;
			case Ack_UpdateExterior_G2U :
				return L"Seg_Game_Vehicle::Ack_UpdateExterior_G2U";
				break;
			case Inf_UpdateExterior_G2U :
				return L"Seg_Game_Vehicle::Inf_UpdateExterior_G2U";
				break;
			case Inf_CancelOnBoard_G2U :
				return L"Seg_Game_Vehicle::Inf_CancelOnBoard_G2U";
				break;
			case Req_SyncPhysicsforce_U2G :
				return L"Seg_Game_Vehicle::Req_SyncPhysicsforce_U2G";
				break;
			case Ack_SyncPhysicsforce_G2U :
				return L"Seg_Game_Vehicle::Ack_SyncPhysicsforce_G2U";
				break;
			case Inf_SyncPhysicsforce_G2U :
				return L"Seg_Game_Vehicle::Inf_SyncPhysicsforce_G2U";
				break;
		}
		break;
	}
	break;
	case Seg_Login_User:
	{
		switch ( pro )
		{
			case Req_LoginUser_U2L :
				return L"Seg_Login_User::Req_LoginUser_U2L";
				break;
			case Ack_LoginUser_L2U :
				return L"Seg_Login_User::Ack_LoginUser_L2U";
				break;
			case Req_ReserveUser_U2L :
				return L"Seg_Login_User::Req_ReserveUser_U2L";
				break;
			case Ack_ReserveUser_L2U :
				return L"Seg_Login_User::Ack_ReserveUser_L2U";
				break;
			case Inf_KickUser_L2U :
				return L"Seg_Login_User::Inf_KickUser_L2U";
				break;
		}
		break;
	}
	break;
	case Seg_Main_Chat:
	{
		switch ( pro )
		{
			case Req_RegisterChatServerInMainServer_C2M :
				return L"Seg_Main_Chat::Req_RegisterChatServerInMainServer_C2M";
				break;
			case Ack_RegisterChatServerInMainServer_M2C :
				return L"Seg_Main_Chat::Ack_RegisterChatServerInMainServer_M2C";
				break;
			case Req_DeregisterChatServerInMainServer_C2M :
				return L"Seg_Main_Chat::Req_DeregisterChatServerInMainServer_C2M";
				break;
			case Ack_DeregisterChatServerInMainServer_M2C :
				return L"Seg_Main_Chat::Ack_DeregisterChatServerInMainServer_M2C";
				break;
		}
		break;
	}
	break;
	case Seg_Main_Game:
	{
		switch ( pro )
		{
			case Req_RegisterGameServerInMainServer_G2M :
				return L"Seg_Main_Game::Req_RegisterGameServerInMainServer_G2M";
				break;
			case Ack_RegisterGameServerInMainServer_M2G :
				return L"Seg_Main_Game::Ack_RegisterGameServerInMainServer_M2G";
				break;
			case Req_DeregisterGameServerInMainServer_G2M :
				return L"Seg_Main_Game::Req_DeregisterGameServerInMainServer_G2M";
				break;
			case Ack_DeregisterGameServerInMainServer_M2G :
				return L"Seg_Main_Game::Ack_DeregisterGameServerInMainServer_M2G";
				break;
			case Req_EnterGame_G2M :
				return L"Seg_Main_Game::Req_EnterGame_G2M";
				break;
			case Ack_EnterGame_M2G :
				return L"Seg_Main_Game::Ack_EnterGame_M2G";
				break;
			case Req_LogoutUser_G2M :
				return L"Seg_Main_Game::Req_LogoutUser_G2M";
				break;
			case Ack_LogoutUser_M2G :
				return L"Seg_Main_Game::Ack_LogoutUser_M2G";
				break;
			case Cmd_KickUser_M2G :
				return L"Seg_Main_Game::Cmd_KickUser_M2G";
				break;
			case Rpt_KickUser_G2M :
				return L"Seg_Main_Game::Rpt_KickUser_G2M";
				break;
			case Req_SysBuildMap_G2M :
				return L"Seg_Main_Game::Req_SysBuildMap_G2M";
				break;
			case Ack_SysBuildMap_M2G :
				return L"Seg_Main_Game::Ack_SysBuildMap_M2G";
				break;
			case Req_ChangeMapState_G2M :
				return L"Seg_Main_Game::Req_ChangeMapState_G2M";
				break;
			case Ack_ChangeMapState_M2G :
				return L"Seg_Main_Game::Ack_ChangeMapState_M2G";
				break;
			case Req_DestroySysBuildMap_G2M :
				return L"Seg_Main_Game::Req_DestroySysBuildMap_G2M";
				break;
			case Ack_DestroySysBuildMap_M2G :
				return L"Seg_Main_Game::Ack_DestroySysBuildMap_M2G";
				break;
			case Cmd_DestroyMap_M2G :
				return L"Seg_Main_Game::Cmd_DestroyMap_M2G";
				break;
			case Rpt_DestroyMap_G2M :
				return L"Seg_Main_Game::Rpt_DestroyMap_G2M";
				break;
			case Req_ReserveMap_G2M :
				return L"Seg_Main_Game::Req_ReserveMap_G2M";
				break;
			case Ack_ReserveMap_M2G :
				return L"Seg_Main_Game::Ack_ReserveMap_M2G";
				break;
			case Cmd_CreateMapForPlayer_M2G :
				return L"Seg_Main_Game::Cmd_CreateMapForPlayer_M2G";
				break;
			case Rpt_CreateMapForPlayer_G2M :
				return L"Seg_Main_Game::Rpt_CreateMapForPlayer_G2M";
				break;
			case Req_ReserveMapForGroup_G2M :
				return L"Seg_Main_Game::Req_ReserveMapForGroup_G2M";
				break;
			case Ack_ReserveMapForGroup_M2G :
				return L"Seg_Main_Game::Ack_ReserveMapForGroup_M2G";
				break;
			case Cmd_CreateMapForGroup_M2G :
				return L"Seg_Main_Game::Cmd_CreateMapForGroup_M2G";
				break;
			case Rpt_CreateMapForGroup_G2M :
				return L"Seg_Main_Game::Rpt_CreateMapForGroup_G2M";
				break;
			case Req_ExitMap_G2M :
				return L"Seg_Main_Game::Req_ExitMap_G2M";
				break;
			case Ack_ExitMap_M2G :
				return L"Seg_Main_Game::Ack_ExitMap_M2G";
				break;
			case Req_EnterMap_G2M :
				return L"Seg_Main_Game::Req_EnterMap_G2M";
				break;
			case Ack_EnterMap_M2G :
				return L"Seg_Main_Game::Ack_EnterMap_M2G";
				break;
			case Inf_ChangeSekaiSystemState_M2G :
				return L"Seg_Main_Game::Inf_ChangeSekaiSystemState_M2G";
				break;
			case Req_SekaiSystemInfo_G2M :
				return L"Seg_Main_Game::Req_SekaiSystemInfo_G2M";
				break;
			case Ack_SekaiSystemInfo_M2G :
				return L"Seg_Main_Game::Ack_SekaiSystemInfo_M2G";
				break;
			case Inf_SekaiItemInfo_M2G :
				return L"Seg_Main_Game::Inf_SekaiItemInfo_M2G";
				break;
			case Inf_SekaiCurrencyInfo_M2G :
				return L"Seg_Main_Game::Inf_SekaiCurrencyInfo_M2G";
				break;
			case Req_Cheat_SekaiSystemSettlement_G2M :
				return L"Seg_Main_Game::Req_Cheat_SekaiSystemSettlement_G2M";
				break;
			case Ack_Cheat_SekaiSystemSettlement_M2G :
				return L"Seg_Main_Game::Ack_Cheat_SekaiSystemSettlement_M2G";
				break;
		}
		break;
	}
	break;
	case Seg_Main_Login:
	{
		switch ( pro )
		{
			case Req_RegisterLoginServerInMainServer_L2M :
				return L"Seg_Main_Login::Req_RegisterLoginServerInMainServer_L2M";
				break;
			case Ack_RegisterLoginServerInMainServer_M2L :
				return L"Seg_Main_Login::Ack_RegisterLoginServerInMainServer_M2L";
				break;
			case Req_DeregisterLoginServerInMainServer_L2M :
				return L"Seg_Main_Login::Req_DeregisterLoginServerInMainServer_L2M";
				break;
			case Ack_DeregisterLoginServerInMainServer_M2L :
				return L"Seg_Main_Login::Ack_DeregisterLoginServerInMainServer_M2L";
				break;
			case Req_LoginUser_L2M :
				return L"Seg_Main_Login::Req_LoginUser_L2M";
				break;
			case Ack_LoginUser_M2L :
				return L"Seg_Main_Login::Ack_LoginUser_M2L";
				break;
			case Req_LogoutUser_L2M :
				return L"Seg_Main_Login::Req_LogoutUser_L2M";
				break;
			case Ack_LogoutUser_M2L :
				return L"Seg_Main_Login::Ack_LogoutUser_M2L";
				break;
			case Req_ReserveUser_L2M :
				return L"Seg_Main_Login::Req_ReserveUser_L2M";
				break;
			case Ack_ReserveUser_M2L :
				return L"Seg_Main_Login::Ack_ReserveUser_M2L";
				break;
			case Cmd_KickUser_M2L :
				return L"Seg_Main_Login::Cmd_KickUser_M2L";
				break;
			case Rpt_KickUser_L2M :
				return L"Seg_Main_Login::Rpt_KickUser_L2M";
				break;
		}
		break;
	}
	break;
	}

#ifdef USE_CLIENT_ONLY	
	return *FString::Printf(TEXT("%d::%d"), (int32)seg, (int32)pro);
#else	
	static wchar_t s[256]; swprintf_s(s, L"%d::%d", seg, pro);
	return s;
#endif // USE_CLIENT_ONLY
}

