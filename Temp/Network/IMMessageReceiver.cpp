// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "IMMessageReceiver.h"
#include "IMNetwork.h"
#include "WRProtocolHeaders.h"
#include "IMMessageSender.h"
#include "WRScreenFader.h"
#include "GenericPlatformMisc.h"
#include "MessageDialog.h"

#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Actor/Vehicle/WRVehicle.h"

#include "CombatSystem/Combat/WRCombatInfo.h"

#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Component/Character/WRActorComponentLadder.h"
#include "Component/Character/WRActorComponentInteraction.h"
#include "Component/Character/WRActorComponentMic.h"

#include "Manager/WRAccountManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRMapManager.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRTriggerManager.h"
#include "Manager/WRSequenceManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRObjectInteractionManager.h"
#include "Manager/WRInventoryMAnager.h"
#include "Manager/WRDialogueManager.h"
#include "Manager/WRBuffManager.h"
#include "Manager/WRTimeManager.h"
#include "Manager/WRNoticeManager.h"
#include "Manager/WREnvInteractionManager.h"
#include "Manager/WRChatBubbleManager.h"
#include "Manager/WRCheersManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRCurrencyManager.h"
#include "Manager/WRVehicleManager.h"
#include "Manager/WRItemManager.h"
#include "Manager/WRTutorialManager.h"
#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRInteriorManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRCharacterCustomizingManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRMapTable.h"
#include "Table/WRCoordinateTable.h"
#include "Table/WRSocialTable.h"

#include "UI/Panel/WRUIPanel_MonitorInLab.h"
#include "UI/Panel/WRUIPanel_Title.h"
#include "UI/Panel/WRUIPanel_CharacterCreate.h"

#include "Enum/EWRTutorial.h"

#include "GenericPlatformMisc.h"
#include "MessageDialog.h"
#include "GenericPlatformMisc.h"
#include "MessageDialog.h"
#include "Utility/WRUIUtility.h"
#include "Utility/WRActorUtility.h"

#include "Share/Define/Define_BluePrint.h"

#include "Kismet/GameplayStatics.h"

#include "GameMode/WRGameModeLab.h"

#include "Localization/WRLocalizationProcessor.h"


DEFINE_LOG_CATEGORY(WRNetwork)
#define RESULT_FAIL 0
#define RESULT_SUCCESS 1
#define SEGMENT_BEGIN(SegmentName) case SegmentName: {
#define SEGMENT_END(SegmentName) }
#define REGISTER_ACK(ProtocolType) case ProtocolType: On##ProtocolType(FIMAcknowledgementParams(Segment, Type, InData, InDataLength)); break;
#define REGISTER_INF(ProtocolType) case ProtocolType: On##ProtocolType(FIMInformParams(InData, InDataLength)); break;
#define REGISTER_ACK_WITH_SERVERNAME(ProtocolType, ServerName) case ProtocolType: On##ProtocolType(FIMAcknowledgementParams(ucSeg, ucType, InData, InDataLength), ServerName); break;
#define REGISTER_INFORM_WITH_SERVERNAME(ProtocolType, ServerName) case ProtocolType: On##ProtocolType(FIMInformParams(InData, InDataLength), ServerName); break;
#define REGISTER_ERROR()																																	\
		default:																																			\
		{																																					\
			/*ShowDebugMSG(FString::Printf(TEXT("OnRecv : [%d]:[%d][%d] -- Not defined Type received"), Protocol, Segment, Type));*/						\
			ensureMsgf(false, TEXT("IMMessageReceiver::OnRecv : Not defined Type received. [%d] [%d] "), Segment, Type);									\
		}	
#define READ_ACK_PACKET(PacketNameSpaceStruct)																										\
	PacketNameSpaceStruct Packet;																													\
	IMNetwork::Get()->Read(Packet, InputParams.Data, InputParams.DataLength);																		\
	FString _FunctionName = __FUNCTION__;																											\
	UE_LOG(WRNetwork, Display, TEXT("[%s] Received."), *_FunctionName);																				\
if (Packet.result == RESULT_FAIL) FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(__FUNCTION__ + (FString::Printf(TEXT(": %d"), Packet.result))));

#define READ_INF_PACKET(PacketNameSpaceStruct)																										\
	PacketNameSpaceStruct Packet;																													\
	IMNetwork::Get()->Read(Packet, InputParams.Data, InputParams.DataLength);																		\
	FString _FunctionName = __FUNCTION__;																											\
	UE_LOG(WRNetwork, Display, TEXT("[%s] Received."), *_FunctionName);																				

void IMMessageReceiver::OnReceived(const std::wstring& IN InServerName, const char* IN InData, unsigned short IN InDataLength)
{
	if (InDataLength < sizeof(uint16)) return;

#ifdef WR_BIG_ENDIAN
	uint16 Protocol = ntohs(*((uint16*)InData));
#else //WR_BIG_ENDIAN
	uint16 Protocol = (*((uint16*)InData));
#endif //WR_BIG_ENDIAN
	uint8 Segment	= _PM::GetSegment(Protocol);
	uint8 Type		= _PM::GetType(Protocol);

	switch (Segment)
	{
		case Seg_Login_User:
		{
			switch (Type)
			{
				REGISTER_ACK(Ack_LoginUser_L2U);
				REGISTER_ACK(Ack_ReserveUser_L2U);
				
				REGISTER_INF(Inf_KickUser_L2U);

				REGISTER_ERROR();
			}
		}
		break;
		
		case Seg_Game_Actor:
		{
			switch (Type)
			{
				REGISTER_ACK(Ack_Revive_G2U);
				REGISTER_ACK(Ack_TriggerEvent_G2U);
				REGISTER_ACK(Ack_SkipSequence_G2U);
				REGISTER_ACK(Ack_BeginTalking_G2U);
				REGISTER_ACK(Ack_EndTalking_G2U);
				REGISTER_ACK(Ack_SocialAction_G2U);
				REGISTER_ACK(Ack_ChangePreset_G2U);
				REGISTER_ACK(Ack_BeginDance_G2U);
				REGISTER_ACK(Ack_EndDance_G2U);
				REGISTER_ACK(Ack_ChangeDanceStep_G2U);
				REGISTER_ACK(Ack_Unoccupy_IO_G2U);
				REGISTER_ACK(Ack_SpeechBubble_G2U);
				REGISTER_ACK(Ack_SuggestCheers_G2U);
				REGISTER_ACK(Ack_AnswerCheers_G2U);
				REGISTER_ACK(Ack_BeginCheers_G2U);
				REGISTER_ACK(Ack_CancelCheers_G2U);
				REGISTER_ACK(Ack_Cheat_G2U);
				REGISTER_ACK(Ack_VoiceChat_G2U);
				REGISTER_ACK(Ack_VoiceMessage_G2U);
				REGISTER_ACK(Ack_SetLanguageType_G2U);
				REGISTER_ACK(Ack_MicAnimation_G2U);
			
				REGISTER_ACK(Ack_DBSaveAnswerToNpc_G2U);
				REGISTER_ACK(Ack_LifeEventReward_G2U);
				
				REGISTER_ACK(Ack_AgreeToSecretDialog_G2U);
				REGISTER_ACK(Ack_EndOfSecretDialog_G2U);
				REGISTER_ACK(Ack_SecretDialogInfo_G2U);

				REGISTER_ACK(Ack_RegistSpeaker_G2U);
				REGISTER_ACK(Ack_DeregistSpeaker_G2U);
				REGISTER_ACK(Ack_ReleaseListener_G2U);

				REGISTER_ACK(Ack_VideoChatList_G2U);
				REGISTER_ACK(Ack_BeginVideoChat_G2U);
				REGISTER_ACK(Ack_EndVideoChat_G2U);
				REGISTER_ACK(Ack_Agree2BeginVideoChat_G2U);
				
				//REGISTER_ACK(Ack_VideoChatMessage_G2U);				

				REGISTER_ACK(Ack_SetFocusedAID_G2U);
				REGISTER_ACK(Ack_BroadcastTestPacket_G2U);

				REGISTER_INF(Inf_PlayerAppear_G2U);
				REGISTER_INF(Inf_NpcAppear_G2U);
				REGISTER_INF(Inf_IOAppear_G2U);
				REGISTER_INF(Inf_SOAppear_G2U);
				REGISTER_INF(Inf_ActorDisappear_G2U);
				REGISTER_INF(Inf_ActorDied_G2U);
				REGISTER_INF(Inf_ChangeHP_G2U);
				REGISTER_INF(Inf_ClientDirectSync_G2U);
				REGISTER_INF(Inf_TriggerDisable_G2U);
				REGISTER_INF(Inf_UsedTriggerList_G2U);
				REGISTER_INF(Inf_PlayerLevelActionSequence_G2U);
				REGISTER_INF(Inf_ChangeWalkSpeed_G2U);
				REGISTER_INF(Inf_Warp_G2U);
				REGISTER_INF(Inf_SocialAction_G2U);
				REGISTER_INF(Inf_Revive_G2U);
				REGISTER_INF(Inf_FloatingBoardAppear_G2U);
				REGISTER_INF(Inf_ElevatorAppear_G2U);
				REGISTER_INF(Inf_TardisAppear_G2U);
				REGISTER_INF(Inf_ChangePreset_G2U);
				REGISTER_INF(Inf_ChangeDanceStep_G2U);
				REGISTER_INF(Inf_SpeechBubble_G2U);
				REGISTER_INF(Inf_SuggestCheers_G2U);
				REGISTER_INF(Inf_AnswerCheers_G2U);
				REGISTER_INF(Inf_BeginCheers_G2U);
				REGISTER_INF(Inf_CancelCheers_G2U);
				REGISTER_INF(Inf_VoiceChat_G2U);
				REGISTER_INF(Inf_VoiceMessage_G2U);
				REGISTER_INF(Inf_VehicleAppear_G2U);
								
				REGISTER_INF(Inf_SecretDialogAppear_G2U);
				REGISTER_INF(Inf_AgreeToSecretDialog_G2U);
				REGISTER_INF(Inf_EndOfSecretDialog_G2U);
				REGISTER_INF(Cmd_AgreeToSecretDialog_G2U);

				REGISTER_INF(Inf_RegistSpeaker_G2U);
				REGISTER_INF(Inf_DeregistSpeaker_G2U);
				REGISTER_INF(Inf_ReleaseListener_G2U);

				REGISTER_INF(Inf_SetLanguageType_G2U);

				REGISTER_INF(Inf_EndVideoChat_G2U);
				REGISTER_INF(Inf_Agree2BeginVideoChat_G2U);
				
				//REGISTER_INF(Inf_VideoChatMessage_G2U);
				REGISTER_INF(Inf_BeginVideoChat_G2U);

				REGISTER_INF(Inf_MicAnimation_G2U);
				REGISTER_INF(Inf_SekaiItemInfo_G2U);
				REGISTER_INF(Inf_SekaiCurrencyInfo_G2U);
				REGISTER_INF(Inf_APAppear_G2U);
				REGISTER_INF(Inf_ChangeAPState_G2U);

				REGISTER_INF(Inf_SetFocusedAID_G2U);

				REGISTER_INF(Inf_BroadcastTestPacket_G2U);

				REGISTER_ERROR();
			}
		}
		break;

		case Seg_Game_User:
		{
			switch (Type)
			{
				REGISTER_ACK(Ack_LogoutUser_G2U);
				REGISTER_ACK(Ack_EnterGame_G2U);
				REGISTER_ACK(Ack_MoveToLab_G2U);
				REGISTER_ACK(Ack_PlayerList_G2U);
				REGISTER_ACK(Ack_CreatePlayer_G2U);
				REGISTER_ACK(Ack_DeletePlayer_G2U);
				REGISTER_ACK(Ack_SelectPlayer_G2U);
				REGISTER_ACK(Ack_SelectPlayerName_G2U);
				REGISTER_ACK(Ack_ReserveMap_G2U);
				REGISTER_ACK(Ack_EnterMap_G2U);
				REGISTER_ACK(Ack_ReadyToPlay_G2U);
				REGISTER_ACK(Ack_ExitMap_G2U);
				REGISTER_ACK(Ack_PlayerMove_G2U);
				REGISTER_ACK(Ack_PlayerStop_G2U);
				REGISTER_ACK(Ack_UseSkill_G2U);
				REGISTER_ACK(Ack_BeginSkill_G2U);
				REGISTER_ACK(Ack_EndSkill_G2U);
				REGISTER_ACK(Ack_GuardBreak_G2U);
				REGISTER_ACK(Ack_SuccessToShield_G2U);
				REGISTER_ACK(Ack_SuccessToAvoid_G2U);
				REGISTER_ACK(Ack_HitResult_G2U);
				REGISTER_ACK(Ack_DynamicMove_G2U);
				REGISTER_ACK(Ack_SyncDynamicMoveLocation_G2U);
				REGISTER_ACK(Ack_BeginInteraction_G2U);
				REGISTER_ACK(Ack_StopInteraction_G2U);
				REGISTER_ACK(Ack_EndInteraction_G2U);
				REGISTER_ACK(Ack_CallADA_G2U);
				REGISTER_ACK(Ack_PlayerWarp_G2U);
				REGISTER_ACK(Ack_BeginLadder_G2U);
				REGISTER_ACK(Ack_MoveLadder_G2U);
				REGISTER_ACK(Ack_EndLadder_G2U);
				REGISTER_ACK(Ack_CancelLadder_G2U);
				REGISTER_ACK(Ack_SyncHMD_G2U);
				REGISTER_ACK(Ack_BeginFall_G2U);
				REGISTER_ACK(Ack_EndFallen_G2U);
				REGISTER_ACK(Ack_BeginVoiceCommand_G2U);
				REGISTER_ACK(Ack_EndVoiceCommand_G2U);
				REGISTER_ACK(Ack_EntryInfo_G2U);
				REGISTER_ACK(Ack_ActorMoveToPos_G2U);


				REGISTER_INF(Inf_PlayerMove_G2U);	
				REGISTER_INF(Inf_PlayerStop_G2U);
				REGISTER_INF(Inf_ActorMoveToPos_G2U);
				REGISTER_INF(Inf_ActorMoveStop_G2U);
				REGISTER_INF(Inf_ActorWarp_G2U);
				REGISTER_INF(Inf_UseSkill_G2U);
				REGISTER_INF(Inf_BeginSkill_G2U);
				REGISTER_INF(Inf_EndSkill_G2U);
				REGISTER_INF(Inf_SetTarget_G2U);
				REGISTER_INF(Inf_NpcDialog_G2U);
				REGISTER_INF(Inf_ActorCCState_G2U);
				REGISTER_INF(Inf_BeginBuff_G2U);
				REGISTER_INF(Inf_EndBuff_G2U);
				REGISTER_INF(Inf_DotBuff_G2U);
				REGISTER_INF(Inf_BeginNpcHook_G2U);
				REGISTER_INF(Inf_EndNpcHook_G2U);
				REGISTER_INF(Inf_SyncHMD_G2U);
				REGISTER_INF(Inf_BeginFall_G2U);
				REGISTER_INF(Inf_EndFallen_G2U);
				REGISTER_INF(Inf_BattleState_G2U);
				REGISTER_INF(Inf_ReserveMapForGroup_G2U);
				REGISTER_INF(Inf_GuardBreak_G2U);
				REGISTER_INF(Inf_SuccessToShield_G2U);
				REGISTER_INF(Inf_SuccessToAvoid_G2U);
				REGISTER_INF(Inf_SyncDynamicMoveLocation_G2U);
				REGISTER_INF(Inf_DynamicMove_G2U);
				REGISTER_INF(Inf_HitResult_G2U);
				REGISTER_INF(Inf_BeginInteraction_G2U);
				REGISTER_INF(Inf_StopInteraction_G2U);
				REGISTER_INF(Inf_EndInteraction_G2U);
				REGISTER_INF(Inf_EntryInfo_Item_G2U);

				REGISTER_ERROR();
			}
		}
		break;

		case Seg_Game_User_Quest:
		{
			switch (Type)
			{
				REGISTER_ACK(Ack_ProgressQuestList_G2U);
				REGISTER_ACK(Ack_CompleteQuestList_G2U);
				REGISTER_ACK(Ack_AcceptQuest_G2U);
				REGISTER_ACK(Ack_CompletableQuest_G2U);
				REGISTER_ACK(Ack_CompleteQuest_G2U);
				REGISTER_ACK(Ack_CompleteQuestTask_G2U);

				REGISTER_INF(Inf_CompleteQuestTask_G2U);
				REGISTER_INF(Inf_UpdateQuestTask_G2U);
				REGISTER_INF(Inf_ForcedQuestComplete_G2U);
				REGISTER_INF(Inf_ForcedQuestGiveUp_G2U);

				REGISTER_ERROR();
			}
		}
		break;
		
		case Seg_Game_User_Inventory:
		{
			switch (Type)
			{
				REGISTER_ACK(Ack_EquipItem_G2U);
				REGISTER_ACK(Ack_UnEquipItem_G2U);
				REGISTER_ACK(Ack_RegisterEquipmentSlot_G2U);
				REGISTER_ACK(Ack_UnRegisterEquipmentSlot_G2U);
				REGISTER_ACK(Ack_UseItem_G2U);
				REGISTER_ACK(Ack_DiscardItem_G2U);
				REGISTER_ACK(Ack_ArrangeInteriorItem_G2U);
				REGISTER_ACK(Ack_RemoveInteriorItem_G2U);

				REGISTER_INF(Inf_ChangeEquipment_G2U);
				REGISTER_INF(Inf_ChangeCostume_G2U);
				REGISTER_INF(Inf_AddItem_G2U);
				REGISTER_INF(Inf_UpdateItemCount_G2U);
				REGISTER_INF(Inf_DeleteItem_G2U);
			

				REGISTER_ERROR();
			}
		}
		break;

		case Seg_Game_Vehicle:
		{
			switch (Type)
			{
				//Ack
				//tranportation
				REGISTER_ACK(Ack_GetOnTransport_G2U);
				REGISTER_ACK(Ack_GetOffTransport_G2U);
				REGISTER_ACK(Ack_GetOnMW_G2U);
				REGISTER_ACK(Ack_GetOffMW_G2U);
				REGISTER_ACK(Ack_PlayerMoveInTransport_U2G);
				REGISTER_ACK(Ack_PlayerStopInTransport_G2U);
				REGISTER_ACK(Ack_PlayerMoveInMW_G2U);
				REGISTER_ACK(Ack_PlayerStopInMW_G2U);
				
				//vehicle
				REGISTER_ACK(Ack_SpawnVehicle_G2U);
				REGISTER_ACK(Ack_UnspawnVehicle_G2U);
				REGISTER_ACK(Ack_GetOnVehicle_G2U);
				REGISTER_ACK(Ack_GetOffVehicle_G2U);
				REGISTER_ACK(Ack_BeginBrake_G2U);
				REGISTER_ACK(Ack_EndBrake_G2U);
				REGISTER_ACK(Ack_Acceleration_G2U);
				REGISTER_ACK(Ack_Steering_G2U);
				REGISTER_ACK(Ack_BeHorning_G2U);
				REGISTER_ACK(Ack_UpdateExterior_G2U);
				REGISTER_ACK(Ack_AgreeToBoard_G2U);

				//Inf
				//transportation
				REGISTER_INF(Inf_TransportState_G2U);
				REGISTER_INF(Inf_GetOnTransport_G2U);
				REGISTER_INF(Inf_GetOffTransport_G2U);
				REGISTER_INF(Inf_CompleteWarp_G2U);
				REGISTER_INF(Inf_GroupWarpTimerCount_G2U);
				REGISTER_INF(Inf_GroupWarpStop_G2U);
				REGISTER_INF(Inf_GetOnMW_G2U);
				REGISTER_INF(Inf_GetOffMW_G2U);
				REGISTER_INF(Inf_PlayerMoveInTransport_G2U);
				REGISTER_INF(Inf_PlayerStopInTransport_G2U);
				REGISTER_INF(Inf_PlayerMoveInMW_G2U);
				REGISTER_INF(Inf_PlayerStopInMW_G2U);
				REGISTER_INF(Inf_ActorMoveStopInMW_G2U);

				//vehicle
				//차량 생성은 여기서 함 :REGISTER_INF(Inf_VehicleAppear_G2U);
				REGISTER_INF(Inf_GetOnVehicle_G2U);
				REGISTER_INF(Inf_GetOffVehicle_G2U);
				REGISTER_INF(Inf_BeginBrake_G2U);
				REGISTER_INF(Inf_EndBrake_G2U);
				REGISTER_INF(Inf_Acceleration_G2U);
				REGISTER_INF(Inf_Steering_G2U);
				REGISTER_INF(Inf_BeHorning_G2U);
				REGISTER_INF(Inf_UpdateExterior_G2U);
				REGISTER_INF(Inf_CancelOnBoard_G2U);
				REGISTER_INF(Cmd_AgreeToBoard_G2U);

				REGISTER_ERROR();
			}
		}
		break;
		case Seg_Game_User_Currency:
		{
			switch (Type)
			{
				//Ack

				//Inf
				REGISTER_INF(Inf_UpdateCurrency_G2U);
				
				REGISTER_ERROR();
			}
		}
		break;

// 		SEGMENT_END(Seg_Login_User)
// 		SEGMENT_BEGIN(Seg_Game_User)
// 		SEGMENT_END(Seg_Game_User)
	}

}

void IMMessageReceiver::OnAck_LoginUser_L2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Login_User::Ack_LoginUser_L2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRAccountManager::Get()->SetDKey(Packet.userdkey);
		WRAccountManager::Get()->SetUserGrade(Packet.usergrade);
		IMMessageSender::RequestReserveUser();
	}
	else
	{
		// vuvle TODO : Error UI 노출 및 UI 에서 버튼 클릭시 재연결 고려
		FString ErrorText = FString::Printf(TEXT("Error Number : %d"), Packet.result);
		FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(ErrorText));
	}
}

void IMMessageReceiver::OnAck_LogoutUser_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_LogoutUser_G2U);
}

void IMMessageReceiver::OnAck_ReserveUser_L2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Login_User::Ack_ReserveUser_L2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		IMNetwork::Get()->DisconnectServer(L"Login");
		IMNetwork::Get()->ConnectToServer(L"Game", Packet.connectIP, Packet.connectPort);
		IMMessageSender::RequestEnterGame();
	}
	else
	{
		// vuvle TODO : Error UI 노출 
		FString ErrorText = FString::Printf(TEXT("Error Number : %d"), Packet.result);
		FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(ErrorText));
	}
}

void IMMessageReceiver::OnInf_KickUser_L2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Login_User::Inf_KickUser_L2U);

	FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(FString::Printf(TEXT("User Kicked by server."))));
}

void IMMessageReceiver::OnInf_CompleteWarp_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_CompleteWarp_G2U);
}

void IMMessageReceiver::OnInf_ReserveMapForGroup_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_ReserveMapForGroup_G2U);

	if (WRAccountManager::Get()->GetPSN() == Packet.psn)
	{
		WRMapManager::Get()->SetReservedMapTID(Packet.mapTID);
		WRMapManager::Get()->SetReservedMapIndex(Packet.mapIndex);

		if (Packet.bMoveToServer)
		{
			IMNetwork::Get()->DisconnectServer(L"Game");
			IMNetwork::Get()->ConnectToServer(L"Game", Packet.gameServerIP, Packet.gameServerPort);
		}

		if (WRMapManager::Get()->GetCurrentMapTID() == INVALID_ISN)
		{
			return;
		}
		else
		{
			IMMessageSender::RequestExitMap();
		}

	}
	else
	{
		ensureMsgf(false, TEXT("The psn inside a packet is not my psn."));
	}
}

void IMMessageReceiver::OnInf_Warp_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_Warp_G2U);

	FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(Packet.coordinateTID);
	if (CoordinateTable == nullptr)
		return;

	AWRCharacter* Character = WRCharacterManager::Get()->FindbyAID(Packet.actorID);
	if (Character == nullptr)
		return;

	FRotator Rotation = FRotator(0.0f, CoordinateTable->Angle, 0.0f);

	// [ 2020-03-24 : vuvle ] 동일 맵 이동일 경우
	if (CoordinateTable->MapTID == WRMapManager::Get()->GetCurrentMapTID())
	{
		if (Character->IsMe())
		{
			Character->SetWhiteOut(CoordinateTable->Position, Rotation);
		}
		else
		{
			Character->SetLocationAndRotation(CoordinateTable->Position, Rotation);
		}
	}
	else // [ 2020-03-24 : vuvle ] 동일 맵 이동이 아닌경우 맵이동 예약을 한다
	{
		if (Character->IsMe())
		{
			// WRAccountManager::Get()->SetMySpawnPosition(CoordinateTable->Position);
			// WRAccountManager::Get()->SetMySpawnRotator(CoordinateTable->Angle);
			WRAccountManager::Get()->SetMySpawnCoordinateTID(Packet.coordinateTID);
			IMMessageSender::RequestReserveMap(CoordinateTable->MapTID);
		}
	}
}

void IMMessageReceiver::OnAck_EnterGame_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_EnterGame_G2U);

	if(Packet.result == RESULT_SUCCESS)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::Title);
		WRAccountManager::Get()->SetUSN(Packet.usn);
		IMMessageSender::RequestMoveToLab();
	}
	else
	{
		UWRUIPanel_Title* Panel_Title = WRUIManager::Get()->FindUI<UWRUIPanel_Title>(EWRUIPanelKind::Title);
		if (Panel_Title)
			Panel_Title->SetWaitLoginPacket(false);

		FString ErrorText = FString::Printf(TEXT("Error Number : %d"), Packet.result);
		FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(ErrorText));
	}
}

void IMMessageReceiver::OnAck_MoveToLab_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_MoveToLab_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		IMMessageSender::RequestPlayerList();
	}
	else
	{
		// vuvle TODO : Error UI 노출 
		//WRUIManager::Get()->OnStereoLayerInitialize();

	}
}

void IMMessageReceiver::OnAck_PlayerList_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_PlayerList_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
//		// [ 2020-02-11 : vuvle ] 결과가 Success 이고 PlayerList 에서 psn 이 0일경우 생성된 캐릭터가 없다.
//		if (Packet.psn == 0)
//		{
//			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
//			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
//			//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CharacterCreate, FTransform(FRotator(0.f, 180.f, 0.f), FVector(172.09, -98.5, 120), FVector::OneVector), EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr);
//		}
//		else
//		{
//#ifdef DEF_USING_CHARACTER_SELECT_UI_BY_VUVLE_200211
//			WRAccountManager::Get()->SetPSN(Packet.psn);
//
//			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
//			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
//			//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CharacterCreate, FTransform(FRotator(0.f, 180.f, 0.f), FVector(172.09, -98.5, 120), FVector::OneVector), EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr);
//#else // DEF_USING_CHARACTER_SELECT_UI_BY_VUVLE_200211
//			IMMessageSender::RequestDeletePlayer(Packet.psn);
//
//			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
//			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
//			//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CharacterCreate, FTransform(FRotator(0.f, 180.f, 0.f), FVector(172.09, -98.5, 120), FVector::OneVector), EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr);
//#endif // DEF_USING_CHARACTER_SELECT_UI_BY_VUVLE_200211
//		}

		AWRGameModeLab* GameModeLab = Cast<AWRGameModeLab>(UGameplayStatics::GetGameMode(UWRGameInstance::GetGameInstance()));
		if (GameModeLab)
		{
			GameModeLab->OnPressed(Packet.psn);
		}
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_CreatePlayer_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_CreatePlayer_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRAccountManager::Get()->SetPSN(Packet.psn);
		WRAccountManager::Get()->SetMyCharacterPresetTID(Packet.costumeTID);
		IMMessageSender::RequestSelectPlayer();
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_DeletePlayer_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_DeletePlayer_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_SelectPlayerName_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_SelectPlayerName_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
#ifdef DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		WRCharacterCustomizingManager::Get()->OnSelectPlayerNameReceived(Packet.playername.c_str());
#else
		UWRUIPanel_CharacterCreate* UIPanel = WRUIManager::Get()->FindUI<UWRUIPanel_CharacterCreate>(EWRUIPanelKind::CharacterCreate);
		if (UIPanel != nullptr)
		{
			UIPanel->SetCharacterName((Packet.playername.c_str()));
		}
#endif
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_SelectPlayer_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_SelectPlayer_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		// [SeungYeop 20/06/10] : 호출 위치는 추후 변경 될수 있음, 매니저 가상함수화 해야할 수 있음
		WRCurrencyManager::Get()->OnWorldEntryInitialized();

		WRAccountManager::Get()->SetPSN(Packet.playerInfo.psn);
		WRAccountManager::Get()->SetAID(Packet.playerInfo.playerAID);
		WRAccountManager::Get()->SetMyCharacterName(Packet.playerInfo.playerName.c_str());

		WRCurrencyManager::Get()->SetCurrencyValue(EWRCurrencyType::Gold, Packet.playerInfo.gold);

		WRUIManager::Get()->CloseUI(EWRUIPanelKind::CharacterCreate);


		// [SeungYeop 20/08/24] :기획 변경으로 해당 UI 단계는 제거
		IMMessageSender::RequestReserveMap(DEF_START_MAP_TID);

		//// Monitor 띄우기
		/*FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
				InCombinedPanelData.WidgetComponentBase->SetGeometryMode(EWidgetGeometryMode::Cylinder);
				InCombinedPanelData.WidgetComponentBase->SetCylinderArcAngle(35);
				UWRUIPanel_MonitorInLab* Panel_MonitorInLab = Cast<UWRUIPanel_MonitorInLab>(InCombinedPanelData.UIPanel);
				if (Panel_MonitorInLab)
					Panel_MonitorInLab->SetStartGameUI();
			}
		});
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::MonitorInLab, EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::MonitorInLab, FTransform(FRotator(2.f, 178.f, 0.f), FVector(172.09, -98.5, 220), FVector::OneVector), EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);*/
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_ReserveMap_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_ReserveMap_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRMapManager::Get()->SetReservedMapTID(Packet.mapTID);
		WRMapManager::Get()->SetReservedMapIndex(Packet.mapIndex);

		if (Packet.bMoveToServer)
		{
			IMNetwork::Get()->DisconnectServer(L"Game");
			IMNetwork::Get()->ConnectToServer(L"Game", Packet.gameServerIP, Packet.gameServerPort);
		}

		if (WRMapManager::Get()->GetCurrentMapTID() == INVALID_ISN)
		{
			return;
		}
		else
		{
			IMMessageSender::RequestExitMap();
		}
		
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_EnterMap_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_EnterMap_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRMapManager::Get()->SetCurrentMapIndex(Packet.mapIndex);
		WRTableID ReservedMapTID = WRMapManager::Get()->GetReservedMapTID();
		WRAccountManager::Get()->SetMySpawnPosition(FVector(Packet.posX, Packet.posY, Packet.posZ));
		WRAccountManager::Get()->SetMySpawnRotator(Packet.angle);
		WRMapManager::Get()->LoadLevelByTable(ReservedMapTID);
		WRTimeManager::Get()->SetServerMapOpenTime(Packet.mapOpenTime);
		WRTimeManager::Get()->SetServerTime(Packet.curTime);
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_ReadyToPlay_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_ReadyToPlay_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		// [ 2020-2-20 : kyu ] 준비완료시 부유섬, 엘리베이터에 탑승자들을 위치시켜야 동기화가 된다
		//WREnvInteractionManager::Get()->UpdatePassengerLocation();

		IMMessageSender::RequestEntryInfo();
	}
	else
	{
		//WRUIManager::Get()->OnStereoLayerInitialize();
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_ExitMap_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_ExitMap_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		if (WRMapManager::Get()->GetReservedMapTID() != 0)
		{
			IMMessageSender::RequestEnterMap();
		}
	}
	else
	{

	}
	//IMMap
}

void IMMessageReceiver::OnAck_Revive_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_Revive_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_BeginTalking_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_BeginTalking_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		
	}
	else
	{
		
	}
}

void IMMessageReceiver::OnAck_EndTalking_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_EndTalking_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterNPC* NpcCharacter = Cast<AWRCharacterNPC>(WRCharacterManager::Get()->FindbyAID(Packet.npcAID));
		if (NpcCharacter)
		{
			NpcCharacter->ReleaseLookTargetForEndDialog(FRotator(0.f, Packet.npcAngle, 0.f));
		}
	}
	else
	{
		
	}
}

void IMMessageReceiver::OnAck_SocialAction_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_SocialAction_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter == nullptr)
			return;

		FWRSocialAnimationTable* FoundSocialAnimationTable = WRTableManager::Get()->FindRow<FWRSocialAnimationTable>(Packet.socialAnimationTID);
		if (FoundSocialAnimationTable == nullptr)
			return;

		WRFSMInfo FSMInfo;
		FSMInfo.FSMType = EWRFSM::SocialAnimation;

		if (MyCharacter->IsShowing3rdCharacter())
		{
			FSMInfo.MontageTID = FoundSocialAnimationTable->BaseSubMontageTID;
			FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(FoundSocialAnimationTable->SubMontageName);
		}
		else
		{
			FSMInfo.MontageTID = FoundSocialAnimationTable->BaseMontageTID;
			FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(FoundSocialAnimationTable->MontageName);
		}	
		
		MyCharacter->DetermineFSMSignal(FSMInfo);
		
		AWRCharacter* TargetCharacter = WRCharacterManager::Get()->FindbyAID(Packet.targetAID);
		if (TargetCharacter)
		{
			if(FoundSocialAnimationTable->TargetFXPath.IsNone() == false)
				WRVFXManager::Get()->Spawn(WRUIUtility::ConvertFNameToFString(FoundSocialAnimationTable->TargetFXPath), TargetCharacter, TargetCharacter);
		}		
	}
	else
	{
		
	}
}

void IMMessageReceiver::OnAck_ChangePreset_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_ChangePreset_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::CostumeChange);
		WRCharacterManager::Get()->ChangeMyPreset(Packet.costumeTID);
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_Unoccupy_IO_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_Unoccupy_IO_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterPlayer* Player = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(Packet.occupyPlayerAID);
		if (Player != nullptr)
		{
			UWRActorComponentInteraction* pInteractionComp = Player->GetComponent<UWRActorComponentInteraction>();
			if (pInteractionComp != nullptr)
			{
				if (pInteractionComp->IsOccupiedIO(Packet.occupyIOAID))
				{
					pInteractionComp->SetOccupiedIO(nullptr);
				}
			}
		}
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_SuggestCheers_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_SuggestCheers_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRCheersManager::Get()->Ack_SuggestCheers(Packet.AnswerAID);
	}
	else
	{
		
	}
}

void IMMessageReceiver::OnAck_AnswerCheers_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_AnswerCheers_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRCheersManager::Get()->Ack_AnswerCheers(Packet.SuggestAID, Packet.bAccept);
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_BeginCheers_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_BeginCheers_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRCheersManager::Get()->Ack_BeginCheers(Packet.AnswerAID);
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_CancelCheers_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_CancelCheers_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRCheersManager::Get()->Ack_CancelCheers(Packet.cnacelTargetAID);
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_MicAnimation_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_MicAnimation_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_BroadcastTestPacket_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_BroadcastTestPacket_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_PlayerMove_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_PlayerMove_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_PlayerStop_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_PlayerStop_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_ActorMoveToPos_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_ActorMoveToPos_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}


void IMMessageReceiver::OnAck_PlayerMoveInTransport_U2G(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_PlayerMoveInTransport_U2G);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_PlayerStopInTransport_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_PlayerStopInTransport_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_CallADA_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_CallADA_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacter* Character = WRCharacterManager::Get()->FindbyAID(Packet.adaAID);
		if (Character)
		{
			Character->SetLocation(FVector(Packet.adaPosX, Packet.adaPosY, Packet.adaPosZ));
			Character->SetRotation(FRotator(0, Packet.rotate, 0), ETeleportType::TeleportPhysics);
		}
	}
	else
	{
		
	}
}

void IMMessageReceiver::OnAck_PlayerWarp_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_PlayerWarp_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacter* Character = WRCharacterManager::Get()->FindbyAID(Packet.casterAID);
		if (Character == nullptr)
			return;

		FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(Packet.coordinateTID);
		if (CoordinateTable == nullptr)
			return;

		FRotator Rotation = FRotator(0.0f, CoordinateTable->Angle, 0.0f);

		if (Character->IsMe())
		{
			Character->SetWhiteOut(CoordinateTable->Position, Rotation);
		}
		else
		{
			Character->SetLocationAndRotation(CoordinateTable->Position, Rotation);
		}
	}
	else
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter)
		{
			MyCharacter->SetUsingWarp(false);
		}
	}
}

void IMMessageReceiver::OnAck_BeginFall_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_BeginFall_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			pMe->OnAckBeginFall(Packet);
		}
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_EndFallen_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_EndFallen_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			pMe->OnAckEndFall(Packet);
		}
	}
	else
	{
	}
}

//player
void IMMessageReceiver::OnAck_UseSkill_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_UseSkill_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			pMe->OnAckUseSkill(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
		if (Packet.result == EWRError::Failure_Not_Enough_Skill_CoolTime)
		{
			WRUIManager::Get()->OpenSkillCoolTimeUI(Packet.remainCoolTime / 1000.f);
		}
	}
}

void IMMessageReceiver::OnAck_BeginSkill_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_BeginSkill_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pChar = WRCharacterManager::Get()->GetMyCharacter();
		if (pChar)
		{
			pChar->OnAckBeginSkill(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
		if (Packet.result == EWRError::Failure_Not_Enough_Skill_CoolTime)
		{
			WRUIManager::Get()->OpenSkillCoolTimeUI(Packet.remainCoolTime / 1000.f);
		}
	}
}

void IMMessageReceiver::OnAck_EndSkill_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_EndSkill_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pChar = WRCharacterManager::Get()->GetMyCharacter();
		if (pChar)
		{
			pChar->OnAckEndSkill(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_GuardBreak_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_GuardBreak_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pChar = WRCharacterManager::Get()->GetMyCharacter();
		if (pChar)
		{
			pChar->OnAckGuardBreak(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_SuccessToShield_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_SuccessToShield_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pChar = WRCharacterManager::Get()->GetMyCharacter();
		if (pChar)
		{
			pChar->OnAckSuccessToShield(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_SuccessToAvoid_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_SuccessToAvoid_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pChar = WRCharacterManager::Get()->GetMyCharacter();
		if (pChar)
		{
			pChar->OnAckSuccessAvoid(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_HitResult_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_HitResult_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		//히트 당하는 대상
		AWRCharacterMine* pActor = WRCharacterManager::Get()->GetMyCharacter();
		if (pActor)
		{
			pActor->OnAckHitResult(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_DynamicMove_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_DynamicMove_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pActor = WRCharacterManager::Get()->GetMyCharacter();
		if (pActor)
		{
			pActor->OnAckDynamicMove(Packet);			
		}

		//WRActorUtility::DrawDebug(pChar, EWRDrawDebugType::Reaction, FVector(Packet.posX, Packet.posY, Packet.posZ));
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_SyncDynamicMoveLocation_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_SyncDynamicMoveLocation_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		AWRCharacterMine* pChar = WRCharacterManager::Get()->GetMyCharacter();
		if (pChar)
		{
			pChar->OnAckSyncDynamicMoveLocation(Packet);
		}
	}
	else
	{
		// vuvle TODO : Error UI 노출 
	}
}

void IMMessageReceiver::OnAck_TriggerEvent_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_TriggerEvent_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRTriggerManager::Get()->OnResultTriggerBoxEvent(Packet.triggerNodeTID, Packet.triggerNodeType);
	}
}

void IMMessageReceiver::OnAck_SkipSequence_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_SkipSequence_G2U);
	
	if (Packet.result == RESULT_SUCCESS)
	{
		WRSequenceManager::Get()->OnSkip();
	}
}

void IMMessageReceiver::OnAck_ProgressQuestList_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Quest::Ack_ProgressQuestList_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		
	}
}

void IMMessageReceiver::OnAck_CompleteQuestList_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Quest::Ack_CompleteQuestList_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		
	}
}

void IMMessageReceiver::OnAck_AcceptQuest_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Quest::Ack_AcceptQuest_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRQuestManager::Get()->OnAckAcceptQuestReceived(true, Packet.TID);
	}
	else
	{
		WRQuestManager::Get()->OnAckAcceptQuestReceived(false, Packet.TID);
	}
}

void IMMessageReceiver::OnAck_CompletableQuest_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Quest::Ack_CompletableQuest_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRQuestManager::Get()->OnAckCompletableQuestReceived(true, Packet.TID);
	}
	else
	{
		WRQuestManager::Get()->OnAckCompletableQuestReceived(false, Packet.TID);
	}
}

void IMMessageReceiver::OnAck_CompleteQuest_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Quest::Ack_CompleteQuest_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRQuestManager::Get()->OnAckCompletedQuestReceived(true, Packet.TID);
	}
	else
	{
		WRQuestManager::Get()->OnAckCompletedQuestReceived(false, Packet.TID);
	}
}

void IMMessageReceiver::OnAck_CompleteQuestTask_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Quest::Ack_CompleteQuestTask_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRQuestManager::Get()->OnAckCompletedQuestTaskReceived(true, Packet.TID, Packet.taskTID);
	}
	else
	{
		WRQuestManager::Get()->OnAckCompletedQuestTaskReceived(false, Packet.TID, Packet.taskTID);
	}
}

//====================================================================================
// Interaction
//====================================================================================

void IMMessageReceiver::OnAck_BeginInteraction_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_BeginInteraction_G2U);

	WRObjectInteractionManager::Get()->SetPacketWait(EWRObjectInteractionPacketWait::Begin, false);

	if (Packet.result == RESULT_SUCCESS)
	{
	
	}
	else
	{
		// message
	}
}

void IMMessageReceiver::OnAck_StopInteraction_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_StopInteraction_G2U);

	WRObjectInteractionManager::Get()->SetPacketWait(EWRObjectInteractionPacketWait::Stop, false);

	if (Packet.result == RESULT_SUCCESS)
	{
		
	}
	else
	{
		// message
	}
}

void IMMessageReceiver::OnAck_EndInteraction_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_EndInteraction_G2U);

	WRObjectInteractionManager::Get()->SetPacketWait(EWRObjectInteractionPacketWait::End, false);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{
		// message
	}
}

//====================================================================================
// Inventory
//====================================================================================

void IMMessageReceiver::OnAck_EntryInfo_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_EntryInfo_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{
		// message
	}
}

void IMMessageReceiver::OnAck_EquipItem_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_EquipItem_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRItemManager::Get()->OnEquipItemReceived(&Packet);
		WRInventoryManager::Get()->OnEquipItemReceived(Packet.ISN);

		WRItemBase* EquippedItem = WRItemManager::Get()->FindItem(Packet.ISN);
		if (EquippedItem != nullptr)
		{
			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::Equip, EquippedItem->GetTID());
		}
		
	}
	else
	{
		// message : 장비를 장착 할 수 없습니다.
	}
}

void IMMessageReceiver::OnAck_UnEquipItem_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_UnEquipItem_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRItemManager::Get()->OnUnEquipItemReceived(Packet.ISN);
		WRInventoryManager::Get()->OnUnEquipItemReceived(Packet.ISN);
	}
	else
	{
		// message
	}
}

void IMMessageReceiver::OnAck_RegisterEquipmentSlot_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_RegisterEquipmentSlot_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRInventoryManager::Get()->OnRegisterEquipmentSlot(&Packet);
	}
	else
	{
		// message
	}
}

void IMMessageReceiver::OnAck_UnRegisterEquipmentSlot_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_UnRegisterEquipmentSlot_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRInventoryManager::Get()->OnUnRegisterEquipmentSlot(&Packet);
	}
	else
	{
		// message
	}
}

void IMMessageReceiver::OnAck_UseItem_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_UseItem_G2U);
	
	if (Packet.result == RESULT_SUCCESS)
	{
		WRInventoryManager::Get()->OnUseItem(&Packet);
	}
	else
	{
		// message
	}
}

void IMMessageReceiver::OnAck_DiscardItem_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_DiscardItem_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRInventoryManager::Get()->OnDiscardItem(&Packet);
	}
	else
	{
		// message
	}
}

//====================================================================================
// Ladder
//====================================================================================

void IMMessageReceiver::OnAck_BeginLadder_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Ack_BeginLadder_G2U);

	if (Packet.result == EWRError::Success)
	{
		if (WRCharacterManager::Get()->GetMyAID() == Packet.InCharID)
			return;

		AWRCharacterPlayer* pChar = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(Packet.InCharID);
		if (pChar == nullptr)
			return;

		UWRActorComponentLadder* pLadderComp = pChar->GetComponent<UWRActorComponentLadder>();
		if (pLadderComp == nullptr)
			return;

		pLadderComp->OnAck_BeginLadder(Packet.InHandleIndex, Packet.InHand);
	}
	else
	{
		// Faliure to me
	}
}

void IMMessageReceiver::OnAck_MoveLadder_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Ack_MoveLadder_G2U);

	if (Packet.result == EWRError::Success)
	{
		if (WRCharacterManager::Get()->GetMyAID() == Packet.InCharID)
			return;

		AWRCharacterPlayer* pChar = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(Packet.InCharID);
		if (pChar == nullptr)
			return;

		UWRActorComponentLadder* pLadderComp = pChar->GetComponent<UWRActorComponentLadder>();
		if (pLadderComp == nullptr)
			return;

		pLadderComp->OnAck_MoveLadder(Packet.InHandleIndex, Packet.InHand, Packet.InDirection);
	}
	else
	{
		// Faliure to me
	}
}

void IMMessageReceiver::OnAck_EndLadder_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Ack_EndLadder_G2U);

	if (Packet.result == EWRError::Success)
	{
		if (WRCharacterManager::Get()->GetMyAID() == Packet.InCharID)
			return;

		AWRCharacterPlayer* pChar = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(Packet.InCharID);
		if (pChar == nullptr)
			return;

		UWRActorComponentLadder* pLadderComp = pChar->GetComponent<UWRActorComponentLadder>();
		if (pLadderComp == nullptr)
			return;

		pLadderComp->OnAck_EndLadder(Packet.InHandleIndex);
	}
	else
	{
		// Faliure to me
	}
}

void IMMessageReceiver::OnAck_CancelLadder_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Ack_CancelLadder_G2U);

	if (Packet.result == EWRError::Success)
	{
		if (WRCharacterManager::Get()->GetMyAID() == Packet.InCharID)
			return;

		AWRCharacterPlayer* pChar = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(Packet.InCharID);
		if (pChar == nullptr)
			return;

		UWRActorComponentLadder* pLadderComp = pChar->GetComponent<UWRActorComponentLadder>();
		if (pLadderComp == nullptr)
			return;

		pLadderComp->OnAck_CancelLadder();
	}
	else
	{
		// Faliure to me
	}
}

//====================================================================================
// Tranportation
//====================================================================================

void IMMessageReceiver::OnAck_GetOnTransport_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Ack_GetOnTransport_G2U);

}

void IMMessageReceiver::OnAck_GetOffTransport_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Ack_GetOffTransport_G2U);

}

// Vehicle
void IMMessageReceiver::OnAck_SpawnVehicle_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_SpawnVehicle_G2U);

	if (Packet.result == EWRError::Success)
	{
		WRVehicleManager::Get()->OnAckSpawnVehicle(Packet);
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_UnspawnVehicle_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_UnspawnVehicle_G2U);

	if (Packet.result == EWRError::Success)
	{
		WRVehicleManager::Get()->OnAckUnspawnVehicle(Packet);
		WRInventoryManager::Get()->OnUnspawnVehicleReceived(&Packet);
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_GetOnVehicle_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_GetOnVehicle_G2U);

	if (WRCharacterManager::Get()->GetMyCharacter())
	{
		WRCharacterManager::Get()->GetMyCharacter()->OnAckGetOnVehicle(Packet);
	};
}

void IMMessageReceiver::OnAck_GetOffVehicle_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_GetOffVehicle_G2U);

	if (Packet.result == EWRError::Success)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			pMe->OnAckGetOffVehicle(Packet);
		}
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_BeginBrake_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_BeginBrake_G2U);

	if (Packet.result == EWRError::Success)
	{
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_EndBrake_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_EndBrake_G2U);

	if (Packet.result == EWRError::Success)
	{
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_Acceleration_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_Acceleration_G2U);

	if (Packet.result == EWRError::Success)
	{
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_Steering_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_Steering_G2U);

	if (Packet.result == EWRError::Success)
	{
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_AgreeToBoard_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_AgreeToBoard_G2U);

	if (Packet.result == EWRError::Success)
	{
		AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
		if (pMe)
		{
			pMe->OnAckAgreeToBoard(Packet);
		}
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_BeHorning_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_BeHorning_G2U);

	if (Packet.result == EWRError::Success)
	{
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_UpdateExterior_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_UpdateExterior_G2U);

	if (Packet.result == EWRError::Success)
	{
	}
	else
	{
	}
}

void IMMessageReceiver::OnAck_BeginDance_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Ack_BeginDance_G2U);

	if (Packet.result == EWRError::Success)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter == nullptr)
			return;

		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent == nullptr)
			return;

		DanceModeComponent->OnAckBeginDanceReceived(Packet.danceGroupTID);
	}
}

void IMMessageReceiver::OnAck_EndDance_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Ack_EndDance_G2U);

	if (Packet.result == EWRError::Success)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter == nullptr)
			return;

		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent == nullptr)
			return;

		DanceModeComponent->OnAckEndDanceReceived();
	}
}

void IMMessageReceiver::OnAck_ChangeDanceStep_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Ack_ChangeDanceStep_G2U);

	if (Packet.result == EWRError::Success)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter == nullptr)
			return;

		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent == nullptr)
			return;

		DanceModeComponent->OnAckChangeDanceStepReceived(Packet.danceGroupTID, Packet.danceTID);
	}
}

// [ 2020-3-10 : magedoga ] Sync HMD
void IMMessageReceiver::OnAck_SyncHMD_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_SyncHMD_G2U);

	// [ 2020-4-09 : vuvle ] 해당 패킷은 Client 에서 하는 행동이 없고 Req 와 Ack 가 너무 잦은 문제가 있어 추후에 Notify 패킷을 만들지, Ack 를 받지 않을지에 대한 협의가 필요한상태. 현재 룰 파괴의 이슈로 서버 측에서 Ack 는 받기를 원함.
	if (Packet.result == RESULT_SUCCESS)
	{
		UE_LOG(WRNetwork, Display, TEXT("IMMessageReceiver::OnAck_SyncHMD_G2U"));
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_SpeechBubble_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_SpeechBubble_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_BeginVoiceCommand_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_BeginVoiceCommand_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		/*AWRCharacter* Character = WRCharacterManager::Get()->FindByTID(DEF_AIDA_TABLIE_ID);
		if (Character == nullptr)
			return;

		Character->MakeWarpDisplay();*/
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_EndVoiceCommand_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User::Ack_EndVoiceCommand_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		/*AWRCharacter* Character = WRCharacterManager::Get()->FindByTID(DEF_AIDA_TABLIE_ID);
		if (Character == nullptr)
			return;

		Character->MakeWarpDisplay();*/
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_Cheat_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_Cheat_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{

	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_GetOnMW_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_GetOnMW_G2U);
	
	WREnvInteractionManager::Get()->OnAck_GetOnMW_G2U(Packet.startTime);
}

void IMMessageReceiver::OnAck_GetOffMW_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_GetOffMW_G2U);

	WREnvInteractionManager::Get()->OnAck_GetOffMW_G2U();
}

void IMMessageReceiver::OnAck_VoiceChat_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_VoiceChat_G2U);
}

void IMMessageReceiver::OnAck_PlayerMoveInMW_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_PlayerMoveInMW_G2U);
	
	
}

void IMMessageReceiver::OnAck_PlayerStopInMW_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Vehicle::Ack_PlayerStopInMW_G2U);

}

void IMMessageReceiver::OnAck_VoiceMessage_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_VoiceMessage_G2U);
}


void IMMessageReceiver::OnAck_SetFocusedAID_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_SetFocusedAID_G2U);
}

void IMMessageReceiver::OnAck_SetLanguageType_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_SetLanguageType_G2U);
}

void IMMessageReceiver::OnAck_DBSaveAnswerToNpc_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_DBSaveAnswerToNpc_G2U);
}

void IMMessageReceiver::OnAck_LifeEventReward_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_LifeEventReward_G2U);
}



void IMMessageReceiver::OnAck_AgreeToSecretDialog_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_AgreeToSecretDialog_G2U);
	
	if (Packet.agree == true)
	{
		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, 3154);
	}
	else
	{
		if (Packet.result == RESULT_SUCCESS)
		{
		    WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, 3155);
		}
		else
		{
			switch (Packet.result)
			{
			case 17701 : // Failure_Chat_Already_Reserved
			{
				WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, 3164);
			}
			break;
			}
		}
	}
	//WRVoiceContentsManager::Get()->OnAgreeToSecretDialog(Packet.partnerAID, Packet.groupID, Packet.agree);
}

void IMMessageReceiver::OnAck_EndOfSecretDialog_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_EndOfSecretDialog_G2U);
	WRVoiceContentsManager::Get()->OnEndSecretDialog();
}

void IMMessageReceiver::OnAck_SecretDialogInfo_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_SecretDialogInfo_G2U);

	FWRSecretDialogInfo InInfo;
	InInfo.GroupID = Packet.groupID;
	InInfo.TalkPlayerList.Empty(Packet.lstSecretDlgMember.size());
	for (AID_t InID : Packet.lstSecretDlgMember)
	{
		InInfo.TalkPlayerList.Add(InID);
	}

	WREnvInteractionManager::Get()->SpawnSecretDialogArea(InInfo);
}

void IMMessageReceiver::OnAck_RegistSpeaker_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_RegistSpeaker_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRVoiceContentsManager::Get()->OnAckRegistSpeakerReceived(true, Packet.speakerAID);
	}
	else
	{
		WRVoiceContentsManager::Get()->OnAckRegistSpeakerReceived(false, Packet.speakerAID);
	}
}

void IMMessageReceiver::OnAck_DeregistSpeaker_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_DeregistSpeaker_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRVoiceContentsManager::Get()->OnAckDeregistSpeakerReceived(true, Packet.speakerAID);
	}
	else
	{
		WRVoiceContentsManager::Get()->OnAckDeregistSpeakerReceived(false, Packet.speakerAID);
	}
}

void IMMessageReceiver::OnAck_ReleaseListener_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_ReleaseListener_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRVoiceContentsManager::Get()->OnAckReleaseListenerReceived(Packet.listenerAID);
	}
}

void IMMessageReceiver::OnAck_ArrangeInteriorItem_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_ArrangeInteriorItem_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		//Packet.isn;
		WRInteriorManager::Get()->AckArrangeInteriorItem(Packet);
	}
	else
	{

	}
}

void IMMessageReceiver::OnAck_RemoveInteriorItem_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_User_Inventory::Ack_RemoveInteriorItem_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRInteriorManager::Get()->AckRemoveInteriorItem(Packet);
	}
	else
	{

	}
}




void IMMessageReceiver::OnAck_VideoChatList_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_VideoChatList_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OnAckVideoChatListReceived(true, Packet.lstVideoChatMember);
	}
	else
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OnAckVideoChatListReceived(false, Packet.lstVideoChatMember);
	}
}

void IMMessageReceiver::OnAck_BeginVideoChat_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_BeginVideoChat_G2U);
	
	if (Packet.result == RESULT_SUCCESS)
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OnAckBeginVideoChatReceived(true, Packet.aidPartner);
	}
	else
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OnAckBeginVideoChatReceived(false, Packet.aidPartner);
	}
}

void IMMessageReceiver::OnAck_EndVideoChat_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_EndVideoChat_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OnAckEndVideoChatReceived();
	}
}

void IMMessageReceiver::OnAck_Agree2BeginVideoChat_G2U(const FIMAcknowledgementParams& IN InputParams)
{
	READ_ACK_PACKET(pt_Game_Actor::Ack_Agree2BeginVideoChat_G2U);

	if (Packet.result == RESULT_SUCCESS)
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OnAckAgree2BeginVideoChat(Packet.bAgree, Packet.aidRequester);
	}
}

//void IMMessageReceiver::OnAck_VideoChatMessage_G2U(const FIMAcknowledgementParams& IN InputParams)
//{
//	READ_ACK_PACKET(pt_Game_Actor::Ack_VideoChatMessage_G2U);
//
//	if (Packet.result == RESULT_SUCCESS)
//	{
//		WRVoiceContentsManager::Get()->GetVideoChatController().OnAckVideoChatMessageReceived();
//	}
//}

// -------------------------------------------------- INF --------------------------------------------------
// -------------------------------------------------- INF --------------------------------------------------
// -------------------------------------------------- INF --------------------------------------------------
// -------------------------------------------------- INF --------------------------------------------------
// Appear

void IMMessageReceiver::OnInf_PlayerAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_PlayerAppear_G2U);

	if (WRMapManager::Get()->IsLoading())
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Received 'Inf_PlayerAppear_G2U' in loading"), TEXT("FAIL : Inf_PlayerAppear_G2U"));
	}
	else
	{
		WRCharacterManager::Get()->DetermineSpawnPlayer(Packet);
	}
}

void IMMessageReceiver::OnInf_NpcAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_NpcAppear_G2U);

	if (WRMapManager::Get()->IsLoading())
	{
		UE_LOG(WRNetwork, Display, TEXT("IMMessageReceiver::OnInf_NpcAppear_G2U - IsLoading TID : %lld"), Packet.npcTID);
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(Packet.npcTID);
		StringArgs.Add(Packet.spawnTID);

		FString Text = FString::Format(TEXT("NpcTID : {0}, SpawnTID : {1}"), StringArgs);
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("Received 'Inf_NpcAppear_G2U' in loading"));
	}
	else
	{
		UE_LOG(WRNetwork, Display, TEXT("IMMessageReceiver::OnInf_NpcAppear_G2U - notLoading TID : %lld"), Packet.npcTID);
		WRCharacterManager::Get()->SpawnNpc(Packet);
	}
}

void IMMessageReceiver::OnInf_IOAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_IOAppear_G2U);

	WRCharacterManager::Get()->SpawnIO(Packet);
}

void IMMessageReceiver::OnInf_SOAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_SOAppear_G2U);

	WRCharacterManager::Get()->SpawnSO(Packet);
}

void IMMessageReceiver::OnInf_ActorDisappear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ActorDisappear_G2U);

	WRCharacterManager::Get()->CancelAsync(Packet.aid);

	AWRCharacter* Character = WRCharacterManager::Get()->FindbyAID(Packet.aid);
	if (Character)
	{
		// [ 2020-4-2 : dlwlgus9125 ] Disappear 패킷 예외처리를 위한 기능 추가
		if (Character->GetRemoveImmediately() == false)
		{
			return;
		}

		WRCharacterManager::Get()->Remove(Character, Character->GetRemoveDisplay());
	} 
	else
	{
		//케릭터 생성이 예약된 상황도 아니면 차량aid인지 체크해서 지운다
		if (WRCharacterManager::Get()->RemovePlayerInfo(Packet.aid) == false)
		{
			AWRVehicle* Vehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.aid);
			if (Vehicle)
			{
				WRVehicleManager::Get()->RemoveVehicle(Packet.aid, true);
			}
		}

		AWREnvActor* InEnvPtr = WREnvInteractionManager::Get()->FindEnvByAID(Packet.aid);
		if (InEnvPtr != nullptr)
		{
			WREnvInteractionManager::Get()->BeginClosePortalProcess(Packet.aid);
		}
	}
}

void IMMessageReceiver::OnInf_ActorDied_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ActorDied_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.deadAID);
	if (pChar)
	{
	}
}

void IMMessageReceiver::OnInf_ChangeHP_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ChangeHP_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.aid);
	if (pChar)
	{
		pChar->OnInformChangeHP(Packet);
	}
}

void IMMessageReceiver::OnInf_ChangeWalkSpeed_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ChangeWalkSpeed_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.aid);
	if (pChar)
	{
		pChar->SetWalkMaxSpeed(Packet.walkSpeed);
	}


// 	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
// 	if (MyChar)
// 	{
// 		// [ 2019-11-28 : magedoga ] 비율아님
// 		//MyChar->SetWalkSpeedByRateValue(Packet.walkSpeed);
// 
// 		// [ 2019-11-28 : magedoga ] 값 세팅
// 		MyChar->SetWalkMaxSpeed(Packet.walkSpeed);
// 	}
}

void IMMessageReceiver::OnInf_NpcDialog_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_NpcDialog_G2U);

	WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::NpcEventTalk, Packet.dialogTID);
}

void IMMessageReceiver::OnInf_ActorCCState_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_ActorCCState_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.actorAID);
	if (pChar)
	{
		pChar->OnInformActorCCState(Packet);
	}
}

void IMMessageReceiver::OnInf_SocialAction_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_SocialAction_G2U);

	AWRCharacter* PlayerCharacter = WRCharacterManager::Get()->FindbyAID(Packet.playerAID);
	if (PlayerCharacter == nullptr)
		return;

	FWRSocialAnimationTable* FoundSocialAnimationTable = WRTableManager::Get()->FindRow<FWRSocialAnimationTable>(Packet.socialAnimationTID);
	if (FoundSocialAnimationTable == nullptr)
		return;

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::SocialAnimation;
	FSMInfo.MontageTID = FoundSocialAnimationTable->BaseSubMontageTID;
	FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(FoundSocialAnimationTable->SubMontageName);
	PlayerCharacter->DetermineFSMSignal(FSMInfo);

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	if (MyCharacter->GetActorID() == Packet.targetAID)
	{
		if(FoundSocialAnimationTable->OnlyTargetFXPath.IsNone() == false)
			WRVFXManager::Get()->Spawn(WRUIUtility::ConvertFNameToFString(FoundSocialAnimationTable->OnlyTargetFXPath), MyCharacter, nullptr);
		if (FoundSocialAnimationTable->OnlyTargetMessage.IsNone() == false)
		{
			FString TargetMessageKey = WRUIUtility::ConvertFNameToFString(FoundSocialAnimationTable->OnlyTargetMessage);
			FString OnlyTargetMessage = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, TargetMessageKey).ToString();
			OnlyTargetMessage.ReplaceInline(TEXT("<name>"), *WRUIUtility::ConvertFNameToFString(PlayerCharacter->GetCharacterName()));
			WRNoticeManager::Get()->AddNotice(OnlyTargetMessage);
		}
	}	
	else
	{
		AWRCharacter* TargetCharacter = WRCharacterManager::Get()->FindbyAID(Packet.targetAID);
		if (TargetCharacter)
		{
			if (FoundSocialAnimationTable->TargetFXPath.IsNone() == false)
				WRVFXManager::Get()->Spawn(WRUIUtility::ConvertFNameToFString(FoundSocialAnimationTable->TargetFXPath), TargetCharacter, TargetCharacter);
		}
	}
}

void IMMessageReceiver::OnInf_Revive_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_Revive_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.aid);
	if (pChar)
	{
		pChar->OnInformRevive(Packet);
	}
}

//====================================================================================
// Vehicle
//====================================================================================

void IMMessageReceiver::OnInf_FloatingBoardAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_FloatingBoardAppear_G2U);

	WREnvInteractionManager::Get()->SpawnFloatingBoard(Packet);
}

void IMMessageReceiver::OnInf_ElevatorAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ElevatorAppear_G2U);

	WREnvInteractionManager::Get()->SpawnElevator(Packet);
}

void IMMessageReceiver::OnInf_TardisAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_TardisAppear_G2U);

	WREnvInteractionManager::Get()->SpawnTardis(Packet);
}

void IMMessageReceiver::OnInf_ChangePreset_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ChangePreset_G2U);

	if (WRCharacterManager::Get()->GetMyAID() != Packet.playerAID)
	{
		WRCharacterManager::Get()->ChangePreset(Packet.playerAID, Packet.costumeTID);
	}
}

void IMMessageReceiver::OnInf_SuggestCheers_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_SuggestCheers_G2U);

	WRCheersManager::Get()->Inf_SuggestCheers(Packet.SuggestAID, Packet.sugeestCheersCondition, Packet.AnswerAID, Packet.answerCheersCondition);
}

void IMMessageReceiver::OnInf_AnswerCheers_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_AnswerCheers_G2U);

	WRCheersManager::Get()->Inf_AnswerCheers(Packet.SuggestAID, Packet.sugeestCheersCondition, Packet.AnswerAID, Packet.answerCheersCondition, Packet.bAccept);
}

void IMMessageReceiver::OnInf_BeginCheers_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_BeginCheers_G2U);

	WRCheersManager::Get()->Inf_BeginCheers(Packet.SuggestAID, Packet.sugeestCheersCondition, Packet.AnswerAID, Packet.answerCheersCondition);
}

void IMMessageReceiver::OnInf_CancelCheers_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_CancelCheers_G2U);

	WRCheersManager::Get()->Inf_CancelCheers(Packet.cancelAID, Packet.cancelCheersCondition, Packet.cnacelTargetAID, Packet.cnacelTargetCheersCondition, Packet.suggestAID);
}

void IMMessageReceiver::OnInf_MicAnimation_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_MicAnimation_G2U);

	AWRCharacterPlayer* FoundPlayer = WRCharacterManager::Get()->FindbyAID_Casted<AWRCharacterPlayer>(Packet.aid);
	if (FoundPlayer)
	{
		UWRActorComponentMic* MicComponent = FoundPlayer->GetComponent<UWRActorComponentMic>();
		if (MicComponent)
		{
			MicComponent->OnAnimation(Packet.animationTID);
		}
	}
}

void IMMessageReceiver::OnInf_APAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_APAppear_G2U);

	WRCharacterManager::Get()->SpawnAP(Packet);
}

void IMMessageReceiver::OnInf_ChangeAPState_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ChangeAPState_G2U);

	WRCharacterManager::Get()->OnChangeAPState(Packet);
}

void IMMessageReceiver::OnInf_BroadcastTestPacket_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_BroadcastTestPacket_G2U);

	
}

void IMMessageReceiver::OnInf_TransportState_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_TransportState_G2U);

	WREnvInteractionManager::Get()->OnInf_VehicleState(Packet);
}

void IMMessageReceiver::OnInf_GetOnTransport_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GetOnTransport_G2U);

	WREnvInteractionManager::Get()->OnInf_GetOn(Packet);
}

void IMMessageReceiver::OnInf_GetOffTransport_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GetOffTransport_G2U);

	WREnvInteractionManager::Get()->OnInf_GetOff(Packet);
}

void IMMessageReceiver::OnInf_GroupWarpTimerCount_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GroupWarpTimerCount_G2U);

	WREnvInteractionManager::Get()->OnInf_GroupWarpTimerCount_G2U(Packet);
}

void IMMessageReceiver::OnInf_GroupWarpStop_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GroupWarpStop_G2U);

	WREnvInteractionManager::Get()->OnInf_GroupWarpStop_G2U(Packet);
}

void IMMessageReceiver::OnInf_VehicleAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_VehicleAppear_G2U);

	WRVehicleManager::Get()->OnInformSpawnVehicle(Packet);
}

void IMMessageReceiver::OnInf_GetOnVehicle_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GetOnVehicle_G2U);

	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfGetOnVehicle(Packet);
	}
}

void IMMessageReceiver::OnInf_GetOffVehicle_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GetOffVehicle_G2U);

	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfGetOffVehicle(Packet);
	}
}

void IMMessageReceiver::OnInf_BeginBrake_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_BeginBrake_G2U);
	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfBeginBrake(Packet);
	}
}

void IMMessageReceiver::OnInf_EndBrake_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_EndBrake_G2U);
	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfEndBrake(Packet);
	}
}

void IMMessageReceiver::OnInf_Acceleration_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_Acceleration_G2U);

	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfAcceleration(Packet);
	}
}

void IMMessageReceiver::OnInf_Steering_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_Steering_G2U);

	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfSteering(Packet);
	}
}

void IMMessageReceiver::OnInf_BeHorning_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_BeHorning_G2U);

	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfHorning(Packet);
	}
}

void IMMessageReceiver::OnInf_UpdateExterior_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_UpdateExterior_G2U);

	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfUpdateExterior(Packet);
	}
}

void IMMessageReceiver::OnInf_CancelOnBoard_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_CancelOnBoard_G2U);
	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnInfCancelOnBoard(Packet);
	}
}

void IMMessageReceiver::OnInf_ChangeDanceStep_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ChangeDanceStep_G2U);

	AWRCharacter* Character = WRCharacterManager::Get()->FindbyAID(Packet.AID);
	if (Character == nullptr)
		return;

	UWRActorComponentDanceMode* DanceModeComponent = Character->GetComponent<UWRActorComponentDanceMode>();
	if (DanceModeComponent == nullptr)
		return;

	DanceModeComponent->OnInformChangeDanceStepReceived(Packet.danceGroupTID, Packet.danceTID);
}

// [ 2020-3-10 : magedoga ] Sync HMD
void IMMessageReceiver::OnInf_SyncHMD_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_SyncHMD_G2U);

	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.ActorID));
	if (Player)
	{
		if (Player->IsMe() == false)
		{
			Player->OnInformSyncHMD(Packet);
		}
	}
	else
	{

	}
}

void IMMessageReceiver::OnInf_SpeechBubble_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_SpeechBubble_G2U);

	WRChatBubbleManager::Get()->OnInformSpeechBubbleReceived(Packet.playerAID, EWRLanguageType::en(Packet.language), (Packet.msg).c_str());
	WRVoiceContentsManager::Get()->GetVideoChatController().OnInformSpeechBubbleReceived(Packet.playerAID, EWRLanguageType::en(Packet.language), (Packet.msg).c_str());
}

void IMMessageReceiver::OnInf_SetLanguageType_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_SetLanguageType_G2U);

	AWRCharacterPlayer* PlayerCharacter = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.playerAID));
	if (PlayerCharacter)
		PlayerCharacter->SetLanguageType(EWRLanguageType::en(Packet.languageType));
}

void IMMessageReceiver::OnInf_GetOnMW_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GetOnMW_G2U);
	
	WREnvInteractionManager::Get()->OnInf_GetOnMW_G2U(Packet);
}

void IMMessageReceiver::OnInf_GetOffMW_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_GetOffMW_G2U);

	WREnvInteractionManager::Get()->OnInf_GetOffMW_G2U(Packet);
}

void IMMessageReceiver::OnInf_VoiceChat_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_VoiceChat_G2U);

#if PLATFORM_PS4
	//WRInputSystemManager::Get()->OnInformVoiceChat(Packet.playerAID, (Packet.voiceChatMsg).c_str(), Packet.length, Packet.focusedAID);
	//WRVoiceContentsManager::Get()->OnInformVoiceChat(Packet.playerAID, Packet.focusedAID);
#endif //PLATFORM_PS4
}

void IMMessageReceiver::OnInf_ActorMoveStopInMW_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_ActorMoveStopInMW_G2U);
	// [20-06-28 - ljh]현재 미구현 추후 액터도 무빙워크 사용을 위해
}

//====================================================================================
// Item
//====================================================================================

void IMMessageReceiver::OnInf_EntryInfo_Item_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_EntryInfo_Item_G2U);

	WRItemManager::Get()->OnEntryItemList(&Packet.listItem, Packet.bFinish);
}

void IMMessageReceiver::OnInf_ChangeEquipment_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Inventory::Inf_ChangeEquipment_G2U);

	AWRCharacter* FoundCharacter = WRCharacterManager::Get()->FindbyAID(Packet.AID);
	if (FoundCharacter)
	{
		FoundCharacter->SetEquippedWeaponItemTID(Packet.rightWeaponTID, Packet.leftWeaponTID, Packet.temporaryTID);
	}
}

void IMMessageReceiver::OnInf_ChangeCostume_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Inventory::Inf_ChangeCostume_G2U);

	AWRCharacter* FoundCharacter = WRCharacterManager::Get()->FindbyAID(Packet.AID);
	if (FoundCharacter)
	{
		FoundCharacter->OnInformChangeCostume(Packet);
	}
}

void IMMessageReceiver::OnInf_AddItem_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Inventory::Inf_AddItem_G2U);

	WRItemManager::Get()->OnAddItemReceived(&Packet.listItem);	
	WRInventoryManager::Get()->OnAddItemReceived(&Packet.listItem);		
	WRCharacterManager::Get()->OnAddItemReceived();
}

void IMMessageReceiver::OnInf_UpdateItemCount_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Inventory::Inf_UpdateItemCount_G2U);

	WRItemManager::Get()->OnUpdateItemQuantityReceived(&Packet.listItem);
	WRInventoryManager::Get()->OnUpdateItemCountReceived();
	WRCharacterManager::Get()->OnUpdateItemCountReceived();
}

void IMMessageReceiver::OnInf_DeleteItem_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Inventory::Inf_DeleteItem_G2U);

	WRItemManager::Get()->OnDeleteItemReceived(&Packet.listItem);
	WRInventoryManager::Get()->OnDeleteItemReceived();
	WRCharacterManager::Get()->OnDeleteItemReceived();
}

//====================================================================================
// Buff
//====================================================================================

void IMMessageReceiver::OnInf_BeginBuff_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_BeginBuff_G2U);

	WRBuffManager::Get()->AddBuff(Packet.ownerAID, Packet.buffInfo);
}

void IMMessageReceiver::OnInf_EndBuff_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_EndBuff_G2U);

	WRBuffManager::Get()->RemoveBuff(Packet.ownerAID, Packet.buffTID);
}

void IMMessageReceiver::OnInf_DotBuff_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_DotBuff_G2U);

	AWRCharacter* pTarget = WRCharacterManager::Get()->FindbyAID(Packet.ownerAID);
	if (pTarget)
	{
		// [ 2020-1-3 : kyu ] 일단 처리 안함
	}
}

//====================================================================================
// Move
//====================================================================================

void IMMessageReceiver::OnInf_PlayerMove_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_PlayerMove_G2U);

	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.playerAID));
	if (Player)
	{
		if (Player->IsMe() == false)
		{
			Player->OnInformPlayerMove(Packet);
		}
	}
	else
	{
		WRCharacterManager::Get()->ModifyPlayerLocation(Packet.playerAID, FVector(Packet.posX, Packet.posY, Packet.posZ));
	}
}

void IMMessageReceiver::OnInf_PlayerStop_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_PlayerStop_G2U);

	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.playerAID));
	if (Player)
	{
		if (Player->IsMe() == false)
			Player->OnInformPlayerMoveStop(Packet);
	}
	else
	{
		WRCharacterManager::Get()->ModifyPlayerLocation(Packet.playerAID, FVector(Packet.posX, Packet.posY, Packet.posZ));
	}
}

void IMMessageReceiver::OnInf_PlayerMoveInTransport_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_PlayerMoveInTransport_G2U);

	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.playerAID));
	if (Player)
	{
		if (Player->IsMe() == false)
		{
			Player->OnInformPlayerMoveInVehicle(Packet);
		}
	}
	else
	{
		WRCharacterManager::Get()->ModifyPlayerOffsetTransformInVehicle(Packet.playerAID, FVector(Packet.localPosX, Packet.localPosY, Packet.localPosZ), Packet.localAngle);
	}
}

void IMMessageReceiver::OnInf_PlayerStopInTransport_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_PlayerStopInTransport_G2U);

	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.playerAID));
	if (Player)
	{
		if (Player->IsMe() == false)
		{
			Player->OnInformPlayerStopInVehicle(Packet);
		}
	}
	else
	{
		WRCharacterManager::Get()->ModifyPlayerOffsetTransformInVehicle(Packet.playerAID, FVector(Packet.localPosX, Packet.localPosY, Packet.localPosZ), Packet.localAngle);
	}
}

void IMMessageReceiver::OnInf_PlayerMoveInMW_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_PlayerMoveInMW_G2U);
	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.playerAID));
	if (Player)
	{
		if (Player->IsMe() == false)
		{
			Player->OnInformPlayerMoveInMW(Packet);
		}
	}
	else
	{
		WRCharacterManager::Get()->ModifyPlayerOffsetTransformInVehicle(Packet.playerAID, FVector(Packet.localPosX, Packet.localPosY, Packet.localPosZ), Packet.localAngle);
	}
}

void IMMessageReceiver::OnInf_PlayerStopInMW_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Inf_PlayerStopInMW_G2U);
	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(Packet.playerAID));
	if (Player)
	{
		if (Player->IsMe() == false)
		{
			Player->OnInformPlayerStopInMW(Packet);
		}
	}
	else
	{
		WRCharacterManager::Get()->ModifyPlayerOffsetTransformInVehicle(Packet.playerAID, FVector(Packet.localPosX, Packet.localPosY, Packet.localPosZ), Packet.localAngle);
	}
}

void IMMessageReceiver::OnInf_ActorMoveToPos_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_ActorMoveToPos_G2U);

	// 케릭터 우선
	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.actorID);
	if (pChar != nullptr)
	{
		pChar->OnInformAIMoveTo(Packet);

		WRActorUtility::DrawDebug(pChar, EWRDrawDebugType::AIMoveTo, FVector(Packet.target_posX, Packet.target_posY, Packet.target_posZ));
	}
	else if(WRCharacterManager::Get()->ModifyNpcAIMoveToPosition(Packet))
	{
		// Npc
	}
	else if (WREnvInteractionManager::Get()->OnInf_ActorMoveToPos_G2U(Packet))
	{
		// Spawned Env
	}
}

void IMMessageReceiver::OnInf_ActorMoveStop_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_ActorMoveStop_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.actorID);
	if (pChar != nullptr)
	{
		if (20000007 == pChar->GetCharacterTableID())
		{
			int32 aa = 0;
		}

		pChar->OnInformAIMoveStop(Packet);

		WRActorUtility::DrawDebug(pChar, EWRDrawDebugType::StopMove);
	}
	else if (WREnvInteractionManager::Get()->OnInf_ActorMoveStop_G2U(Packet))
	{
		// Spawned Env
	}
}

void IMMessageReceiver::OnInf_ActorWarp_G2U(const FIMInformParams &IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_ActorWarp_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.actorID);
	if (nullptr == pChar)
	{
		return;
	}

	if (20000007 == pChar->GetCharacterTableID())
	{
		int32 aa = 0;
	}

	pChar->OnInformAIWarp(Packet);
	pChar->MakeWarpDisplay();

	WRActorUtility::DrawDebug(pChar, EWRDrawDebugType::WarpTo);
}

void IMMessageReceiver::OnInf_BeginFall_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_BeginFall_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.fallAID);
	if (pChar)
	{
		pChar->OnInformBeginFall(Packet);
	}
}

void IMMessageReceiver::OnInf_EndFallen_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_EndFallen_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.fallAID);
	if (pChar)
	{
		pChar->OnInformEndFall(Packet);
	}

}

void IMMessageReceiver::OnInf_BattleState_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_BattleState_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.ActorID);
	if (pChar)
	{
		pChar->SetBattleState(Packet.BattleState);
	}
}

//
void IMMessageReceiver::OnInf_UseSkill_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_UseSkill_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.casterAID);
	if (pChar)
	{
		pChar->OnInformUseSkill(Packet);
	}

	WRActorUtility::DrawDebug(pChar, EWRDrawDebugType::Skill, FVector(Packet.casterPosX, Packet.casterPosY, Packet.casterPosZ));
}

void IMMessageReceiver::OnInf_BeginSkill_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_BeginSkill_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.casterAID);
	if (pChar)
	{
		pChar->OnInformBeginSkill(Packet);
	}

	WRActorUtility::DrawDebug(pChar, EWRDrawDebugType::Skill, FVector(Packet.casterPosX, Packet.casterPosY, Packet.casterPosZ));
}

void IMMessageReceiver::OnInf_EndSkill_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_EndSkill_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.casterAID);
	if (pChar)
	{
		pChar->OnInformEndSkill(Packet);
	}
}

void IMMessageReceiver::OnInf_SetTarget_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_SetTarget_G2U);

	AWRCharacterNPC* pChar = Cast<AWRCharacterNPC>(WRCharacterManager::Get()->FindbyAID(Packet.casterAID));
	if (pChar)
	{
		pChar->SetTargetAID(Packet.targetAID);
	}
}

void IMMessageReceiver::OnInf_BeginNpcHook_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_BeginNpcHook_G2U);
}

void IMMessageReceiver::OnInf_EndNpcHook_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_EndNpcHook_G2U);
}

void IMMessageReceiver::OnInf_GuardBreak_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_GuardBreak_G2U);

	AWRCharacter* pChar = Cast<AWRCharacter>(WRCharacterManager::Get()->FindbyAID(Packet.targetAID));
	if (pChar)
	{
		pChar->OnInformGuardBreak(Packet);
	}
}

void IMMessageReceiver::OnInf_SuccessToShield_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_SuccessToShield_G2U);

	AWRCharacter* pChar = Cast<AWRCharacter>(WRCharacterManager::Get()->FindbyAID(Packet.casterAID));
	if (pChar)
	{
		pChar->OnInformSuccessToShield(Packet);
	}
}

void IMMessageReceiver::OnInf_SuccessToAvoid_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_SuccessToAvoid_G2U);

	AWRCharacter* pActor = WRCharacterManager::Get()->FindbyAID(Packet.casterAID);
	if (pActor)
	{
		pActor->OnInformSuccessAvoid(Packet);
	}
}

void IMMessageReceiver::OnInf_SyncDynamicMoveLocation_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_SyncDynamicMoveLocation_G2U);

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(Packet.actorID);
	if (pChar && pChar->IsMe() == false)
	{
		pChar->OnInformSyncDynamicMoveLocation(Packet);
	}

	WRActorUtility::DrawDebug(pChar, EWRDrawDebugType::Reaction, FVector(Packet.posX, Packet.posY, Packet.posZ));
}

void IMMessageReceiver::OnInf_DynamicMove_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_DynamicMove_G2U);

	AWRCharacter* pActor = WRCharacterManager::Get()->FindbyAID(Packet.casterAID);
	if (pActor)
	{
		pActor->OnInformDynamicMove(Packet);
	}
}

void IMMessageReceiver::OnInf_HitResult_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_HitResult_G2U);

	//히트 당하는 대상
	AWRCharacter* pActor = WRCharacterManager::Get()->FindbyAID(Packet.targetAID);
	if (pActor)
	{
		pActor->OnInformHitResult(Packet);
	}
	else
	{
		WRCharacterManager::Get()->ModifyHitResult(Packet);
	}
}

void IMMessageReceiver::OnInf_ClientDirectSync_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ClientDirectSync_G2U);

	WRTriggerManager::Get()->RunActiveTrigger(Packet.triggerNodeTID);
}

void IMMessageReceiver::OnInf_TriggerDisable_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_TriggerDisable_G2U);

	WRTriggerManager::Get()->RunDisableTrigger(Packet.triggerNodeTID);
}

void IMMessageReceiver::OnInf_UsedTriggerList_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_UsedTriggerList_G2U);

	WRTriggerManager::Get()->SetUsedTriggerList(Packet.listUsedTrigger);
}

void IMMessageReceiver::OnInf_PlayerLevelActionSequence_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_PlayerLevelActionSequence_G2U);

	WRSequenceManager::Get()->PlaySequence(Packet.levelSequenceName.c_str());
}

void IMMessageReceiver::OnInf_CompleteQuestTask_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Quest::Inf_CompleteQuestTask_G2U);

	WRQuestManager::Get()->OnInformCompletedQuestTaskReceived(Packet.TID, Packet.taskTID);
}

void IMMessageReceiver::OnInf_UpdateQuestTask_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Quest::Inf_UpdateQuestTask_G2U);

	//WRQuestManager::Get()->OnInformUpdateQuestTaskReceived(Packet.TID, Packet.taskTID, Packet.taskValue);
}

void IMMessageReceiver::OnInf_ForcedQuestComplete_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Quest::Inf_ForcedQuestComplete_G2U);

	WRQuestManager::Get()->OnInformForcedQuestCompleteReceived(Packet.TID);
}

void IMMessageReceiver::OnInf_ForcedQuestGiveUp_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Quest::Inf_ForcedQuestGiveUp_G2U);

	WRQuestManager::Get()->OnInformForcedQuestGiveUpReceived(Packet.TID);
}

void IMMessageReceiver::OnInf_BeginInteraction_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_BeginInteraction_G2U);

	WRObjectInteractionManager::Get()->OnInf_BeginInteraction(Packet.casterAID, Packet.targetAID, (EWRIOInteractionType)Packet.interactionType);
}

void IMMessageReceiver::OnInf_StopInteraction_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_StopInteraction_G2U);

	WRObjectInteractionManager::Get()->OnInf_StopInteraction(Packet.casterAID, Packet.targetAID);
}

void IMMessageReceiver::OnInf_EndInteraction_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_EndInteraction_G2U);

	WRObjectInteractionManager::Get()->OnInf_EndInteraction(Packet.casterAID, Packet.targetAID, Packet.retainCount, Packet.interactionType);
}

void IMMessageReceiver::OnInf_VoiceMessage_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_VoiceMessage_G2U);

	FString msg = Packet.voiceMsg.c_str();
	WRInputSystemManager::Get()->OnInf_VoiceMessage(Packet.playerAID, msg);
	WRVoiceContentsManager::Get()->OnInformVoiceMessage(Packet.playerAID);
}

void IMMessageReceiver::OnInf_SetFocusedAID_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_SetFocusedAID_G2U);

	WRVoiceContentsManager::Get()->OnInformSetFocusedAID(Packet.playerAID);
}

void IMMessageReceiver::OnInf_UpdateCurrency_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User_Currency::Inf_UpdateCurrency_G2U);

	WRCurrencyManager::Get()->SetCurrencyValue(EWRCurrencyType(Packet.type), Packet.value);
	WRInventoryManager::Get()->OnUpdateCurrency((EWRCurrencyType)Packet.type);
}

void IMMessageReceiver::OnInf_SecretDialogAppear_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_SecretDialogAppear_G2U);
	if (Packet.groupID == INVALID_CHAT_GROUP_ID)
		return;

	TArray<AID_t> InAidList(Packet.vecPlayer.data(), Packet.vecPlayer.size());
	FWRSecretDialogInfo InInfo;
	InInfo.GroupID = Packet.groupID;
	InInfo.PortalLocation = FVector(Packet.posX, Packet.posY, Packet.posZ);
	//InInfo.TalkPlayerList = TArray<AID_t>(Packet.vecPlayer.data(), Packet.vecPlayer.size());
	InInfo.PortalAID = Packet.SecretDialogAID;
	
	// 뒤늦게 접속시 OnAgree를 받을 수 없어 추가하는 부분 추가
	WRVoiceContentsManager::Get()->AddGroupChat(Packet.groupID, InAidList);
	
	//[2020-08-25 EZ] 포탈 삭제 피드백 대응
	//WREnvInteractionManager::Get()->SpawnSecretDialogPortal(InInfo);
}

void IMMessageReceiver::OnInf_AgreeToSecretDialog_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_AgreeToSecretDialog_G2U);
	if (Packet.groupID == INVALID_CHAT_GROUP_ID)
		return;

	TArray<AID_t> AIDs;
	AIDs.Empty(Packet.lstMember.size());
	for (AID_t InAID : Packet.lstMember)
	{
		AIDs.Add(InAID);
	}

	WRVoiceContentsManager::Get()->OnAgreeToSecretDialog(AIDs, Packet.groupID, Packet.agree);
	
	// cmd 상태에서 받은 groupID와 Inf로 돌아오는 GroupID가 같으면 Dome생성
	//else GourpID만 세팅
	//WRVoiceContentsManager::Get()->OnAgreeToSecretDialog(Packet.partnerAID, Packet.groupID, Packet.agree);
}

void IMMessageReceiver::OnInf_EndOfSecretDialog_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_EndOfSecretDialog_G2U);
	if (Packet.groupID == INVALID_CHAT_GROUP_ID)
		return;

	WRVoiceContentsManager::Get()->OnEndSecretDialog(Packet.groupID);	
}

void IMMessageReceiver::OnInf_RegistSpeaker_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_RegistSpeaker_G2U);

	WRVoiceContentsManager::Get()->OnInformRegistSpeakerReceived(Packet.listenerAID);
}

void IMMessageReceiver::OnInf_DeregistSpeaker_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_DeregistSpeaker_G2U);

	WRVoiceContentsManager::Get()->OnInformDeregistSpeakerReceived(Packet.listenerAID);
}

void IMMessageReceiver::OnInf_ReleaseListener_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_ReleaseListener_G2U);

	WRVoiceContentsManager::Get()->OnInformReleaseListenerReceived(Packet.speakerAID);
}

void IMMessageReceiver::OnInf_BeginVideoChat_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_BeginVideoChat_G2U);

	WRVoiceContentsManager::Get()->GetVideoChatController().OnInfBeginVideoChatReceived(Packet.requester);
}

void IMMessageReceiver::OnInf_EndVideoChat_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_EndVideoChat_G2U);

	WRVoiceContentsManager::Get()->GetVideoChatController().OnInfEndVideoChatReceived();
}


void IMMessageReceiver::OnInf_Agree2BeginVideoChat_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Inf_Agree2BeginVideoChat_G2U);

	WRVoiceContentsManager::Get()->GetVideoChatController().OnInfAgree2BeginVideoChat(Packet.bAgree, Packet.aidPartner);
}

void IMMessageReceiver::OnInf_SekaiItemInfo_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_SekaiItemInfo_G2U);

	WRInventoryManager::Get()->OnSekaiItemInfoReceived(&Packet);
	
}

void IMMessageReceiver::OnInf_SekaiCurrencyInfo_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_User::Inf_SekaiCurrencyInfo_G2U);
	WRInventoryManager::Get()->OnSekaiCurrencyInfoReceived(&Packet);
}

//void IMMessageReceiver::OnInf_VideoChatMessage_G2U(const FIMInformParams& IN InputParams)
//{
//	READ_INF_PACKET(pt_Game_Actor::Inf_VideoChatMessage_G2U);
//	//std::wstring voiceMsg; // 음성 대화 내용
//	//bool bFirst; // 음성 대화 시작 패킷
//	//bool bLast; // 음성 대화 끝 패킷
//	//WRVoiceContentsManager::Get()->GetVideoChatController().OnInfEndVideoChatReceived();
//}


void IMMessageReceiver::OnCmd_AgreeToBoard_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Vehicle::Cmd_AgreeToBoard_G2U);

	AWRVehicle* pVehicle = WRVehicleManager::Get()->FindVehicleByAID(Packet.vehicleAID);
	if (pVehicle)
	{
		pVehicle->OnCmdAgreeToBoard(Packet);
	}
}

void IMMessageReceiver::OnCmd_AgreeToSecretDialog_G2U(const FIMInformParams& IN InputParams)
{
	READ_INF_PACKET(pt_Game_Actor::Cmd_AgreeToSecretDialog_G2U);

	if (Packet.groupID == INVALID_CHAT_GROUP_ID)
		return;

	WRVoiceContentsManager::Get()->OnCmdAgreeToSecretDialog(Packet.partnerAID, Packet.groupID);
}

void IMMessageReceiver::OnError(unsigned short result)
{
}