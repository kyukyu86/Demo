// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#include "IMMessageSender.h"
#include "IMNetwork.h"
#include "WRProtocolHeaders.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/EnvActor/WREnvVehicleBase.h"
#include "Actor/EnvActor/MovingWalkway/WREnvMovingWalkway.h"
#include "Actor/Vehicle/WRVehicle.h"
#include "Utility/WRActorUtility.h"

#include "CombatSystem/Combat/WRCombatInfo.h"

#include "Manager/WRInputSystemManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WREnvInteractionManager.h"
#include "Manager/WRAccountManager.h"
#include "Manager/WRMapManager.h"

#include "Share/Define/Define_BluePrint.h"

#include "Table/WRSkillTable.h"
#include "Table/Base/WRTableManager.h"
#include "Enum/EWRCharacter.h"

//DEFINE_LOG_CATEGORY(WRNetwork)

#define SEND_TO_GAME_SERVER()																	\
	IMNetwork::Get()->SendToGameServer(&Packet);												
	//FString _SendMessage = __FUNCTION__;														\
	//UE_LOG(WRNetwork, Display, TEXT("[%s] : sent."), *_SendMessage);

#define SEND_TO_LOGIN_SERVER()																	\
	IMNetwork::Get()->SendToLoginServer(&Packet);												
	//FString _SendMessage = __FUNCTION__;														\
	//UE_LOG(WRNetwork, Display, TEXT("[%s] : sent."), *_SendMessage);


void UseSkillParam::SetSkillTID(WRTableID InTID, FString InFileName)
{
	SkillTID = InTID;
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InTID, InFileName);
	if (SkillTable && SkillTable->IsNoneHeightFlag() == false)
	{
		AttackHeightType = WRInputSystemManager::Get()->GetAttackHeightType();
	}
}


bool IMMessageSender::RequestLogin()
{
	pt_Login_User::Req_LoginUser_U2L Packet;

	if (IMNetwork::Get()->IsConnected(L"Login"))
	{
		Packet.userid = WRAccountManager::Get()->GetUserID();
		Packet.protocolversion = WXR_PROTOCOL_VERSION;
		SEND_TO_LOGIN_SERVER();
		return true;
	}
	{
		ensureMsgf(false, TEXT("Server is not connected yet."));
		return false;
	}
}

void IMMessageSender::RequestReserveUser()
{
	pt_Login_User::Req_ReserveUser_U2L Packet;

	Packet.userid = WRAccountManager::Get()->GetUserID();
	Packet.userdkey = WRAccountManager::Get()->GetDKey();

	SEND_TO_LOGIN_SERVER();
}

void IMMessageSender::RequestEnterGame()
{
	pt_Game_User::Req_EnterGame_U2G Packet;

	Packet.userid = WRAccountManager::Get()->GetUserID();
	Packet.userdkey = WRAccountManager::Get()->GetDKey();
	Packet.protocolversion = WXR_PROTOCOL_VERSION;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestMoveToLab()
{
	pt_Game_User::Req_MoveToLab_U2G Packet;

	Packet.usn = WRAccountManager::Get()->GetUSN();

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSelectPlayer()
{
	pt_Game_User::Req_SelectPlayer_U2G Packet;

	Packet.psn = WRAccountManager::Get()->GetPSN();

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestPlayerList()
{
	pt_Game_User::Req_PlayerList_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSelectPlayerName()
{
	pt_Game_User::Req_SelectPlayerName_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCreatePlayer(const FString& IN InName, const WRTableID IN InCharacterPresetTID, std::string& IN InCustomizingNoneItemData, const TArray<WRTableID>& IN InCostumeItemArray)
{
	pt_Game_User::Req_CreatePlayer_U2G Packet;

	Packet.playername = *InName;
	Packet.costumeTID = InCharacterPresetTID;
	
	stCustomizeInfo TempData;
	TempData.customize = InCustomizingNoneItemData;
	TempData.customizeSize = InCustomizingNoneItemData.size();
	Packet.customizeInfo = TempData;

	for (int32 i = 0; i < InCostumeItemArray.Num(); ++i)
	{
		Packet.listCostumeItem.push_back(TID_t(InCostumeItemArray[i]));
	}

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestDeletePlayer(const PSN_t IN InPSN)
{
	pt_Game_User::Req_DeletePlayer_U2G Packet;

	Packet.psn = InPSN;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestReserveMap(const WRTableID IN InMapTID)
{
	if (WRMapManager::Get()->IsMapReserved())
	{
		UE_LOG(WRNetwork, Display, TEXT("Reserved map already exist."));
		return;
	}

	pt_Game_User::Req_ReserveMap_U2G Packet;

	Packet.psn = WRAccountManager::Get()->GetPSN();
	if (InMapTID == WRMapManager::Get()->GetCurrentMapTID())
	{
		FString ErrorMessage = __FUNCTION__;
		UE_LOG(WRNetwork, Error, TEXT("[%s] : Reserving same map TID."), *ErrorMessage);
		return;
	}
	Packet.mapTID = InMapTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEnterMap()
{
	pt_Game_User::Req_EnterMap_U2G Packet;
	Packet.psn = WRAccountManager::Get()->GetPSN();
	Packet.mapTID = WRMapManager::Get()->GetReservedMapTID();
	Packet.mapIndex = WRMapManager::Get()->GetReservedMapIndex();
	Packet.coordinateTID = WRAccountManager::Get()->GetMySpawnCoordinateTID();

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestExitMap()
{
	pt_Game_User::Req_ExitMap_U2G Packet;
	Packet.psn = WRAccountManager::Get()->GetPSN();
	Packet.mapTID = WRMapManager::Get()->GetCurrentMapTID();
	Packet.mapIndex = WRMapManager::Get()->GetCurrentMapIndex();

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestReadyToPlay()
{
	pt_Game_User::Req_ReadyToPlay_U2G Packet;

	Packet.psn = WRAccountManager::Get()->GetPSN();
	
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestLogout()
{
	pt_Game_User::Req_LogoutUser_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::ReuqestRevive(PSN_t InPSN)
{
	pt_Game_Actor::Req_Revive_U2G Packet;

	Packet.psn = InPSN;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginTalking(const AID_t IN InAID, const bool IN bInRoamingPossible)
{
	pt_Game_Actor::Req_BeginTalking_U2G Packet;

	Packet.npcAID = InAID;
	Packet.bRoaming = bInRoamingPossible;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndTalking(const AID_t IN InAID)
{
	pt_Game_Actor::Req_EndTalking_U2G Packet;

	Packet.npcAID = InAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSocialAction(const WRTableID IN InSocialAnimationTID, const AID_t IN InTargetAID)
{
	pt_Game_Actor::Req_SocialAction_U2G Packet;

	Packet.socialAnimationTID = InSocialAnimationTID;
	Packet.targetAID = InTargetAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestChangePreset(const WRTableID IN InTID)
{
	pt_Game_Actor::Req_ChangePreset_U2G Packet;

	Packet.costumeTID = InTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUnoccupyIO()
{
	pt_Game_Actor::Req_Unoccupy_IO_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSuggestCheers(const AID_t IN InAID)
{
	pt_Game_Actor::Req_SuggestCheers_U2G Packet;

	Packet.AnswerAID = InAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestAnswerCheers(const AID_t IN InAID, const bool IN InAnswer)
{
	pt_Game_Actor::Req_AnswerCheers_U2G Packet;

	Packet.SuggestAID = InAID;
	Packet.bAccept = InAnswer;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginCheers(const AID_t IN InAID)
{
	pt_Game_Actor::Req_BeginCheers_U2G Packet;

	Packet.AnswerAID = InAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCancelCheers(const AID_t IN InAID)
{
	pt_Game_Actor::Req_CancelCheers_U2G Packet;

	Packet.cnacelTargetAID = InAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestMicAnimation(const TID_t IN InAnimationTID)
{
	pt_Game_Actor::Req_MicAnimation_U2G Packet;

	Packet.animationTID = InAnimationTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBrodcastTest(FVector IN LinearVelocity, FVector IN AngularVelocity, FVector IN Location, FRotator IN Rotation, bool bSimulatedPhysicSleep, bool bRepPhysics, uint8 LocationQuantizationLevel, uint8 VelocityQuantizationLevel, uint8 RotationQuantizationLevel)
{
	pt_Game_Actor::Req_BroadcastTestPacket_U2G Packet;
	Packet.LinearVelocity = LinearVelocity;
	Packet.AngularVelocity = AngularVelocity;
	Packet.Location = Location;
	Packet.Rotation = Rotation;
	Packet.bSimulatedPhysicSleep = bSimulatedPhysicSleep;
	Packet.bRepPhysics = bRepPhysics;
	Packet.LocationQuantizationLevel = LocationQuantizationLevel;
	Packet.VelocityQuantizationLevel = VelocityQuantizationLevel;
	Packet.RotationQuantizationLevel = RotationQuantizationLevel;
}

void IMMessageSender::RequestPlayerMove(class AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return;

	pt_Game_User::Req_PlayerMove_U2G Packet;
	Packet.playerAID = InActor->GetActorID();
	Packet.posX = InActor->GetActorLocation().X;
	Packet.posY = InActor->GetActorLocation().Y;
	Packet.posZ = InActor->GetActorLocation().Z;
	Packet.forwardScale = InActor->MoveForwardValue;
	Packet.rightScale = InActor->MoveRightValue;
	Packet.rotate = InActor->GetActorRotation().Yaw;

	FVector LastInputVec = InActor->GetLastMovementInputVector();
	Packet.angle_for_server = LastInputVec.Rotation().Yaw;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestPlayerMoveStop(class AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return;

	InActor->ClearAIMoveTo();

	if (InActor->IsMe())
	{
		if (InActor->GetCurrentFSMType() == EWRFSM::Move)
		{
			InActor->DetermineFSMSignal(EWRFSM::Idle);
		}
	}

	pt_Game_User::Req_PlayerStop_U2G Packet;
	Packet.playerAID = InActor->GetActorID();
	Packet.posX = InActor->GetActorLocation().X;
	Packet.posY = InActor->GetActorLocation().Y;
	Packet.posZ = InActor->GetActorLocation().Z;
	Packet.rotate = InActor->GetActorRotation().Yaw;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestPlayerMoveInTransportation(class AWRCharacterPlayer* InPlayer)
{
	if (nullptr == InPlayer)
		return;

	AID_t EnvVehicleAID = InPlayer->GetEnvVehicleAID();
	if (EnvVehicleAID == INVALID_AID)
		return;

	AWREnvVehicleBase* FoundEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(EnvVehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	FVector vOffset = InPlayer->GetActorLocation() - FoundEnvVehicle->GetActorLocation();

	pt_Game_Vehicle::Req_PlayerMoveInTransport_U2G Packet;
	Packet.localPosX = vOffset.X;
	Packet.localPosY = vOffset.Y;
	Packet.localPosZ = vOffset.Z;
	Packet.forwardScale = InPlayer->MoveForwardValue;
	Packet.rightScale = InPlayer->MoveRightValue;
	Packet.rotate = InPlayer->GetActorRotation().Yaw;

// 	FVector LastInputVec = InPlayer->GetLastMovementInputVector();
// 	Packet.angle_for_server = LastInputVec.Rotation().Yaw;
	Packet.localAngle = WRActorUtility::GetDegreesBetween2Vectors(FoundEnvVehicle->GetActorForwardVector(), InPlayer->GetActorForwardVector());

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestPlayerStopInTransportation(class AWRCharacterPlayer* InPlayer)
{
	if (nullptr == InPlayer)
		return;

	AID_t EnvVehicleAID = InPlayer->GetEnvVehicleAID();
	if (EnvVehicleAID == INVALID_AID)
		return;

	AWREnvVehicleBase* FoundEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(EnvVehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	FVector vOffset = InPlayer->GetActorLocation() - FoundEnvVehicle->GetActorLocation();

	pt_Game_Vehicle::Req_PlayerStopInTransport_U2G Packet;
	Packet.localPosX = vOffset.X;
	Packet.localPosY = vOffset.Y;
	Packet.localPosZ = vOffset.Z;
//	Packet.rotate = InPlayer->GetActorRotation().Yaw;
	Packet.localAngle = WRActorUtility::GetDegreesBetween2Vectors(FoundEnvVehicle->GetActorForwardVector(), InPlayer->GetActorForwardVector());

	SEND_TO_GAME_SERVER();
}


void IMMessageSender::RequestPlayerMoveInMW(AWRCharacterPlayer* InPlayer)
{
	if (nullptr == InPlayer)
		return;
	
	AWREnvMovingWalkway* BoardingMovingWalkway = WREnvInteractionManager::Get()->FindOverlappedMWByCharacterAID(InPlayer->GetActorID());
	if (BoardingMovingWalkway == nullptr)
		return;

	FVector vOffset = InPlayer->GetActorLocation() - BoardingMovingWalkway->GetActorLocation();

	pt_Game_Vehicle::Req_PlayerMoveInMW_U2G Packet;
	Packet.localPosX = vOffset.X;
	Packet.localPosY = vOffset.Y;
	Packet.localPosZ = vOffset.Z;
	Packet.forwardScale = InPlayer->MoveForwardValue;
	Packet.rightScale = InPlayer->MoveRightValue;
	Packet.rotate = InPlayer->GetActorRotation().Yaw;
	// 	FVector LastInputVec = InPlayer->GetLastMovementInputVector();
	// 	Packet.angle_for_server = LastInputVec.Rotation().Yaw;
	Packet.localAngle = WRActorUtility::GetDegreesBetween2Vectors(BoardingMovingWalkway->GetActorForwardVector(), InPlayer->GetActorForwardVector());

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestPlayerStopInMW(AWRCharacterPlayer* InPlayer)
{
	AWREnvMovingWalkway* BoardingMovingWalkway = WREnvInteractionManager::Get()->FindOverlappedMWByCharacterAID(InPlayer->GetActorID());
	if (BoardingMovingWalkway == nullptr)
		return;

	FVector vOffset = InPlayer->GetActorLocation() - BoardingMovingWalkway->GetActorLocation();

	pt_Game_Vehicle::Req_PlayerStopInMW_U2G Packet;

	Packet.localPosX = vOffset.X;
	Packet.localPosY = vOffset.Y;
	Packet.localPosZ = vOffset.Z;

	// 	FVector LastInputVec = InPlayer->GetLastMovementInputVector();
	// 	Packet.angle_for_server = LastInputVec.Rotation().Yaw;
	Packet.localAngle = WRActorUtility::GetDegreesBetween2Vectors(BoardingMovingWalkway->GetActorForwardVector(), InPlayer->GetActorForwardVector());
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCallADA(const AID_t IN InAdaAID, const FVector IN InPos)
{
	pt_Game_User::Req_CallADA_U2G Packet;

	Packet.adaAID = InAdaAID;
	Packet.casterPosX = InPos.X;
	Packet.casterPosY = InPos.Y;
	Packet.casterPosZ = InPos.Z;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestPlayerWarp(const WRTableID IN InCoordinateTID)
{
	pt_Game_User::Req_PlayerWarp_U2G Packet;

	Packet.coordinateTID = InCoordinateTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginFall(AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return;

	pt_Game_User::Req_BeginFall_U2G Packet;

	Packet.PosX = InActor->GetActorLocation().X;
	Packet.PosY = InActor->GetActorLocation().Y;
	Packet.PosZ = InActor->GetActorLocation().Z;

	WRActorUtility::ModifyZHeightForServer(InActor, InActor->GetActorLocation(), Packet.PosZ);

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndFall(AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return;

	pt_Game_User::Req_EndFallen_U2G Packet;

	Packet.PosX = InActor->GetActorLocation().X;
	Packet.PosY = InActor->GetActorLocation().Y;
	Packet.PosZ = InActor->GetActorLocation().Z;

	WRActorUtility::ModifyZHeightForServer(InActor, InActor->GetActorLocation(), Packet.PosZ);

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::ReqeustPlayerAIMoveTo(AWRCharacter* InActor, FVector InDestLocation)
{
	if (nullptr == InActor)
		return;

	pt_Game_User::Req_ActorMoveToPos_U2G Packet;
	Packet.angle = InActor->GetActorRotation().Yaw; // 각도
	Packet.start_posX = InActor->GetActorLocation().X; // 시작
	Packet.start_posY = InActor->GetActorLocation().Y; // 시작
	Packet.start_posZ = InActor->GetActorLocation().Z; // 시작
	Packet.target_posX = InDestLocation.X; // 목표
	Packet.target_posY = InDestLocation.Y; // 목표
	Packet.target_posZ = InDestLocation.Z; // 목표
	//Packet.actionType; // 이동 타입 참조\Global\Share\Define\Define_Actor.h => EWRActionType
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUseSkill(const UseSkillParam& InParam)
{
	if (nullptr == InParam.Caster)
		return;

	pt_Game_User::Req_UseSkill_U2G Packet;
	Packet.skillTID = InParam.SkillTID;
	Packet.casterAID = InParam.Caster->GetActorID();
	
	for (int32 Index = 0; Index < InParam.TargetList.Num(); ++Index)
	{		
		Packet.lstTarget.push_back(InParam.TargetList[Index]);
	}
	Packet.casterPosX = InParam.Caster->GetActorLocation().X;
	Packet.casterPosY = InParam.Caster->GetActorLocation().Y;
	Packet.casterPosZ = InParam.Caster->GetActorLocation().Z;
	Packet.rotate = InParam.Caster->GetActorRotation().Yaw;
	Packet.attackHeight = (uint8)InParam.AttackHeightType;

	if (InParam.Caster->IsMe())
	{
		WRCombatInfo* LastCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InParam.Caster->GetActorID());
		if (LastCombatInfo)
		{
			Packet.tempSeq = LastCombatInfo->GetCombatSeqID();
		}
	}

	SEND_TO_GAME_SERVER();
	//IMNetwork::Get()->SendToGameServer(&Packet);
}

void IMMessageSender::RequestUseSkillBegin(const UseSkillParam& InParam)
{
	if (nullptr == InParam.Caster)
		return;

	pt_Game_User::Req_BeginSkill_U2G Packet;
	Packet.skillTID = InParam.SkillTID;
	Packet.casterAID = InParam.Caster->GetActorID();
	for (int32 Index = 0; Index < InParam.TargetList.Num(); ++Index)
	{
		Packet.lstTarget.push_back(InParam.TargetList[Index]);
	}

	Packet.casterPosX = InParam.Caster->GetActorLocation().X;
	Packet.casterPosY = InParam.Caster->GetActorLocation().Y;
	Packet.casterPosZ = InParam.Caster->GetActorLocation().Z;
	Packet.casterRotate = InParam.Caster->GetActorRotation().Yaw;
	Packet.attackHeight = (uint8)InParam.AttackHeightType;

	if (InParam.Caster->IsMe())
	{
		WRCombatInfo* LastCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InParam.Caster->GetActorID());
		if (LastCombatInfo)
		{
			Packet.tempSeq = LastCombatInfo->GetCombatSeqID();
		}
	}

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUseSkillEnd(class AWRCharacter* InCaster, SKILL_SEQ_t InSeqID)
{
	if (nullptr == InCaster)
		return;

	pt_Game_User::Req_EndSkill_U2G Packet;
	Packet.skillSeq = InSeqID;
	Packet.casterAID = InCaster->GetActorID();
	Packet.casterPosX = InCaster->GetActorLocation().X;
	Packet.casterPosY = InCaster->GetActorLocation().Y;
	Packet.casterPosZ = InCaster->GetActorLocation().Z;
	//Packet.rotate = InCaster->GetActorRotation().Yaw;

	SEND_TO_GAME_SERVER();
	//IMNetwork::Get()->SendToGameServer(&Packet);
}

void IMMessageSender::RequestGuardBreak(class AWRCharacter* InCaster, class AWRCharacter* InTarget, SKILL_SEQ_t InSeqID)
{
	if (nullptr == InCaster || nullptr == InTarget)
		return;

	pt_Game_User::Req_GuardBreak_U2G Packet;
	Packet.casterAID = InCaster->GetActorID();
	Packet.targetAID = InTarget->GetActorID();
	Packet.skillSeq = InSeqID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSuccessToShield(class AWRCharacter* InAttacker, class AWRCharacter* InBlocker, SKILL_SEQ_t InSeqID)
{
	if (nullptr == InBlocker || nullptr == InAttacker)
		return;

	pt_Game_User::Req_SuccessToShield_U2G Packet;
	Packet.casterAID = InAttacker->GetActorID();
	Packet.targetAID = InBlocker->GetActorID();
	Packet.skillSeq = InSeqID;

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InBlocker->GetActorID());
	if (nullptr == CombatInfo)
		return;

	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(CombatInfo->GetCombatSeqParam().SkillTID, InBlocker->GetSkillTableFileName());
	if (nullptr == SkillTable)
		return;

	if (EWRSkillType::Defense != SkillTable->SkillType)
		return;

	Packet.shieldSkillTID = CombatInfo->GetCombatSeqParam().SkillTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestAvoid(class AWRCharacter* InCaster, SKILL_SEQ_t InSeqID, uint8 AvoidDir)
{
	if (nullptr == InCaster)
		return;

	pt_Game_User::Req_SuccessToAvoid_U2G Packet;
	Packet.casterAID = InCaster->GetActorID();
	Packet.skillSeq = InSeqID;
	Packet.avoidDirection = AvoidDir;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::ReqestHitResult(class AWRCharacter* InCaster, class AWRCharacter* InTarget, SKILL_SEQ_t InSeqID, EWRCollisionType InCollisionType)
{
	if (nullptr == InCaster)
		return;

	if (nullptr == InTarget)
		return;

	pt_Game_User::Req_HitResult_U2G Packet;
	Packet.skillSeq = InSeqID;
	Packet.casterAID = InCaster->GetActorID();
	Packet.targetAID = InTarget->GetActorID();
	
	Packet.hitPosX = InTarget->GetActorLocation().X;
	Packet.hitPosY = InTarget->GetActorLocation().Y;
	Packet.hitPosZ = InTarget->GetActorLocation().Z;

	Packet.collisionType = (int32)InCollisionType;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::ReqestProjectileHitResult(class AWRCharacter* InCaster, FVector InLocation, SKILL_SEQ_t InSeqID)
{
	if (nullptr == InCaster)
		return;

	pt_Game_User::Req_HitResult_U2G Packet;
	Packet.skillSeq = InSeqID;
	Packet.casterAID = InCaster->GetActorID();
	Packet.hitPosX = InLocation.X;
	Packet.hitPosY = InLocation.Y;
	Packet.hitPosZ = InLocation.Z;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestDynamicMove(class AWRCharacter* InActor, FVector InDestLocation, WRTableID InDynamicTID)
{
	if (nullptr == InActor)
	{
		return;
	}

	pt_Game_User::Req_DynamicMove_U2G Packet;
	Packet.casterAID = InActor->GetActorID();
	Packet.DestX = InDestLocation.X;
	Packet.DestY = InDestLocation.Y;
	Packet.DestZ = InDestLocation.Z;
	Packet.dynamicMoveTableTID = InDynamicTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSyncDynamicMoveLocation(class AWRCharacter* InCaster)
{
	if (nullptr == InCaster)
	{
		return;
	}

	pt_Game_User::Req_SyncDynamicMoveLocation_U2G Packet;
	Packet.actorID = InCaster->GetActorID();
	Packet.posX = InCaster->GetActorLocation().X;
	Packet.posY = InCaster->GetActorLocation().Y;
	Packet.posZ = InCaster->GetActorLocation().Z;
	Packet.rotate = InCaster->GetActorRotation().Yaw;
	
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestTriggerEvent(const TID_t TriggerTID, const uint32 TransitionType, FVector Pos, float Yaw)
{
	pt_Game_Actor::Req_TriggerEvent_U2G Packet;
	Packet.triggerNodeTID = TriggerTID;
	Packet.triggerNodeType = TransitionType;
	Packet.posX = Pos.X;
	Packet.posY = Pos.Y;
	Packet.posZ = Pos.Z;

	Packet.angle = Yaw;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSkipSequence()
{
	pt_Game_Actor::Req_TriggerEvent_U2G Packet;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestProgressQuestList()
{
	pt_Game_User_Quest::Req_ProgressQuestList_U2G Packet;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCompleteQuestList()
{
	pt_Game_User_Quest::Req_CompleteQuestList_U2G Packet;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestAcceptQuest(const WRTableID IN InQuestTID)
{
	pt_Game_User_Quest::Req_AcceptQuest_U2G Packet;

	Packet.TID = InQuestTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCompletableQuest(const WRTableID IN InQuestTID)
{
	pt_Game_User_Quest::Req_CompletableQuest_U2G Packet;

	Packet.TID = InQuestTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCompleteQuest(const WRTableID IN InQuestTID)
{
	pt_Game_User_Quest::Req_CompleteQuest_U2G Packet;

	Packet.TID = InQuestTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCompleteQuestTask(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID)
{
	pt_Game_User_Quest::Req_CompleteQuestTask_U2G Packet;

	Packet.TID = InQuestTID;
	Packet.taskTID = InQuestTaskTID;

	SEND_TO_GAME_SERVER();
}

// Interaction

void IMMessageSender::RequestBeginInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const uint8 IN InInteractionType)
{
	pt_Game_User::Req_BeginInteraction_U2G Packet;

	Packet.casterAID = InCharID;
	Packet.targetAID = InObjectID;
	Packet.interactionType = InInteractionType;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestStopInteraction(const AID_t IN InCharID, const AID_t IN InObjectID)
{
	pt_Game_User::Req_StopInteraction_U2G Packet;

	Packet.casterAID = InCharID;
	Packet.targetAID = InObjectID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const uint8 IN InInteractionType)
{
	pt_Game_User::Req_EndInteraction_U2G Packet;

	Packet.casterAID = InCharID;
	Packet.targetAID = InObjectID;
	Packet.interactionType = InInteractionType;

	SEND_TO_GAME_SERVER();
}

// Ladder

void IMMessageSender::RequestBeginLadder(const AID_t IN InCharID, const int32 IN InLadderHandleIndex, const int32 IN InHand)
{
	pt_Game_User::Req_BeginLadder_U2G Packet;

	Packet.InHandleIndex = InLadderHandleIndex;
	Packet.InHand = InHand;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestMoveLadder(const AID_t IN InCharID, const int32 IN InLadderHandleIndex, const int32 IN InHand, const int32 IN InLadderDirection)
{
	pt_Game_User::Req_MoveLadder_U2G Packet;

	Packet.InHandleIndex = InLadderHandleIndex;
	Packet.InHand = InHand;
	Packet.InDirection = InLadderDirection;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndLadder(const AID_t IN InCharID, const int32 IN InLadderHandleIndex)
{
	pt_Game_User::Req_EndLadder_U2G Packet;

	Packet.InHandleIndex = InLadderHandleIndex;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCancelLadder(const AID_t IN InCharID)
{
	pt_Game_User::Req_CancelLadder_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEntryInfo()
{
	pt_Game_User::Req_EntryInfo_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUseItem(const ISN_t IN InISN, const ITEM_COUNT_t IN InUsedItemCount)
{
	pt_Game_User_Inventory::Req_UseItem_U2G Packet;

	Packet.ISN = InISN;
	Packet.itemCount = InUsedItemCount;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestDiscardItem(const ISN_t IN InISN, const ITEM_COUNT_t IN InDiscardedItemCount)
{
	pt_Game_User_Inventory::Req_DiscardItem_U2G Packet;

	Packet.ISN = InISN;
	Packet.itemCount = InDiscardedItemCount;

	SEND_TO_GAME_SERVER();
}

// Equip

void IMMessageSender::RequestEquipItem(const ISN_t IN InISN)
{
	// vuvle TODO : TID 제거 필요
	pt_Game_User_Inventory::Req_EquipItem_U2G Packet;

	Packet.ISN = InISN;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUnEquipItem(const ISN_t IN InISN)
{
	pt_Game_User_Inventory::Req_UnEquipItem_U2G Packet;

	Packet.ISN = InISN;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestRegisterEquipmentSlot(const ISN_t IN InISN, const EWRRegistrationSlotType IN InSlotTimeToRegister)
{
	pt_Game_User_Inventory::Req_RegisterEquipmentSlot_U2G Packet;

	Packet.ISN = InISN;
	Packet.slotType = (uint8)InSlotTimeToRegister;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUnRegisterEquipmentSlot(const ISN_t IN InISN)
{
	pt_Game_User_Inventory::Req_UnRegisterEquipmentSlot_U2G Packet;

	Packet.ISN = InISN;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestGetOnTransportation(const AID_t IN InTransportationAID, const FVector IN InOffset, const float IN InAngle)
{
	pt_Game_Vehicle::Req_GetTransport_U2G Packet;

	Packet.vehicleAID = InTransportationAID;
	Packet.localPosX = InOffset.X;
	Packet.localPosY = InOffset.Y;
	Packet.localPosZ = InOffset.Z;
	Packet.localAngle = InAngle;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestGetOffTransportation(const AID_t IN InTransportationAID)
{
	pt_Game_Vehicle::Req_GetOffTransport_U2G Packet;

	Packet.vehicleAID = InTransportationAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSpawnVehicle(AWRCharacter* InVehicleOwner, FVector SpawnPosition, float InAngle, ISN_t InISN, WRTableID InTID)
{
	if (nullptr == InVehicleOwner)
		return;

	pt_Game_Vehicle::Req_SpawnVehicle_U2G Packet;
	Packet.actorID = InVehicleOwner->GetActorID();
	Packet.posX = SpawnPosition.X;
	Packet.posY = SpawnPosition.Y;
	Packet.posZ = SpawnPosition.Z;
	Packet.Angle = InAngle;
	Packet.itemSN = InISN;
	Packet.vehicleTID = InTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUnspawnVehicle(AID_t InVehicleAID, ISN_t InVehicleISN)
{
	pt_Game_Vehicle::Req_UnspawnVehicle_U2G Packet;
	Packet.vehicleAID = InVehicleAID;
	Packet.itemSN = InVehicleISN;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestGetOnVehicle(AWRCharacter* InPassenger, int8 InSeatPosition, AID_t InVehicleAID)
{
	if (nullptr == InPassenger)
		return;

	pt_Game_Vehicle::Req_GetOnVehicle_U2G Packet;
	Packet.actorID = InPassenger->GetActorID();
	Packet.seatIndex = InSeatPosition;
	Packet.vehicleAID = InVehicleAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestGetOffVehicle(AWRCharacter* InPassenger, AID_t InVehicleAID)
{
	if (nullptr == InPassenger)
		return;

	pt_Game_Vehicle::Req_GetOffVehicle_U2G Packet;
	Packet.actorID = InPassenger->GetActorID();
	Packet.vehicleAID = InVehicleAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginBrake(AWRVehicle* InVehicle)
{
	if (nullptr == InVehicle)
		return;

	pt_Game_Vehicle::Req_BeginBrake_U2G Packet;
	Packet.vehicleAID = InVehicle->GetVehicleAID();
	Packet.posX = InVehicle->GetActorLocation().X;
	Packet.posY = InVehicle->GetActorLocation().Y;
	Packet.posZ = InVehicle->GetActorLocation().Z;
	Packet.Angle = InVehicle->GetActorRotation().Yaw;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndBrake(AWRVehicle* InVehicle)
{
	if (nullptr == InVehicle)
		return;

	pt_Game_Vehicle::Req_EndBrake_U2G Packet;
	Packet.vehicleAID = InVehicle->GetVehicleAID();
	Packet.posX = InVehicle->GetActorLocation().X;
	Packet.posY = InVehicle->GetActorLocation().Y;
	Packet.posZ = InVehicle->GetActorLocation().Z;
	Packet.Angle = InVehicle->GetActorRotation().Yaw;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestAcceleration(AWRVehicle* InVehicle, float InValue, float Speed)
{
	if (nullptr == InVehicle)
		return;

	pt_Game_Vehicle::Req_Acceleration_U2G Packet;
	Packet.vehicleAID = InVehicle->GetVehicleAID();
	Packet.InputPower = InValue;

	//위치
// 	Packet.posX = InVehicle->GetActorLocation().X;
// 	Packet.posY = InVehicle->GetActorLocation().Y;
// 	Packet.posZ = InVehicle->GetActorLocation().Z;

	FVector ToNext = InVehicle->GetActorForwardVector() * (VEHICLE_TICK_TIME_TO_SYNC * Speed);
	ToNext += InVehicle->GetActorLocation();
	Packet.posX = ToNext.X;
	Packet.posY = ToNext.Y;
	Packet.posZ = ToNext.Z;

	//회전값
	Packet.Pitch = InVehicle->GetActorRotation().Pitch;
	Packet.Yaw = InVehicle->GetActorRotation().Yaw;
	Packet.Roll = InVehicle->GetActorRotation().Roll;

	//차 속도
	Packet.Speed = Speed;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSteering(AWRVehicle* InVehicle, float InSteering)
{
	if (nullptr == InVehicle)
		return;

	pt_Game_Vehicle::Req_Steering_U2G Packet;
	Packet.vehicleAID = InVehicle->GetVehicleAID();
	Packet.steering = InSteering;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeHorning(AWRVehicle* InVehicle)
{
	if (nullptr == InVehicle)
		return;

	pt_Game_Vehicle::Req_BeHorning_U2G Packet;
	Packet.vehicleAID = InVehicle->GetVehicleAID();
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestUpdateExterior(AWRVehicle* InVehicle, const stVehicleExteriorInfo& InInfo)
{
	if (nullptr == InVehicle)
		return;

	pt_Game_Vehicle::Req_UpdateExterior_U2G Packet;
	Packet.vehicleAID = InVehicle->GetVehicleAID();
	Packet.Exterior = InInfo;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSyncPhysicsForce(AWRVehicle* InVehicle, FVector InForceVector, FVector InImpactLocation)
{
	if (nullptr == InVehicle)
		return;

	//SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestAgreeToBoard(AID_t InPassengerAID, int8 InSeatIndex, AID_t InVehicleAID)
{
	pt_Game_Vehicle::Req_AgreeToBoard_U2G Packet;
	Packet.passengerAID = InPassengerAID;
	Packet.seatIndex = InSeatIndex;
	Packet.vehicleAID = InVehicleAID;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginDance(const WRTableID IN InDanceGroupTID)
{
	pt_Game_Actor::Req_BeginDance_U2G Packet;

	Packet.danceGroupTID = InDanceGroupTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndDance()
{
	pt_Game_Actor::Req_EndDance_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestChangeDanceStep(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID)
{
	pt_Game_Actor::Req_ChangeDanceStep_U2G Packet;

	Packet.danceGroupTID = InDanceGroupTID;
	Packet.danceTID = InDanceTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSyncHMD(class AWRCharacterPlayer* InCharacter)
{
	pt_Game_User::Req_SyncHMD_U2G	Packet;

	Packet.ActorID = InCharacter->GetActorID();

	FRotator HMDRotation = InCharacter->GetHMDRotationToUpdate();
	Packet.RotationX = HMDRotation.Roll;
	Packet.RotationY = HMDRotation.Pitch;
	Packet.RotationZ = HMDRotation.Yaw;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSpeechBubble(const EWRLanguageType::en IN InLanguageType, const FString& IN InMsg)
{
	pt_Game_Actor::Req_SpeechBubble_U2G	Packet;
	
	Packet.language = uint8(InLanguageType);
	Packet.msg = *InMsg;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginVoiceCommand(const bool IN bInDisplayStartDialogue)
{
	pt_Game_User::Req_BeginVoiceCommand_U2G Packet;

	Packet.enableStartDlg = bInDisplayStartDialogue;
	/*AWRCharacterMine* MyCharacter =  WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	Packet.currPosX = MyCharacter->GetActorLocation().X;
	Packet.currPosY = MyCharacter->GetActorLocation().Y;
	Packet.currPosZ = MyCharacter->GetActorLocation().Z;
	Packet.currAngle = MyCharacter->GetActorRotation().Yaw;*/

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndVoiceCommand()
{
	pt_Game_User::Req_EndVoiceCommand_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestCheat(const FString& IN InCheatStr)
{
	pt_Game_Actor::Req_Cheat_U2G Packet;

	if (InCheatStr.IsEmpty())
		return;

	Packet.cheatdata = *InCheatStr;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginOnMW(const FVector& IN InBasePos)
{
	pt_Game_Vehicle::Req_GetOnMW_U2G Packet;
	Packet.basePosX = InBasePos.X;
	Packet.basePosY = InBasePos.Y;
	Packet.basePosZ = InBasePos.Z;
	
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndOnMW(const FVector& IN InArrivalPos, const float IN InAngle)
{
	pt_Game_Vehicle::Req_GetOffMW_U2G Packet;
	Packet.arrivalAngle = InAngle;
	Packet.arrivalPosX = InArrivalPos.X;
	Packet.arrivalPosY = InArrivalPos.Y;
	Packet.arrivalPosZ = InArrivalPos.Z;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestVoiceChat(const FString& IN VoiceBuffer, const uint16 IN Length)
{
// 	pt_Game_Actor::Req_VoiceChat_U2G Packet;
// 
// 	Packet.voiceChatMsg = *VoiceBuffer;
// 	Packet.length = Length;	
// 
// 	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestVoiceMessage(const FString IN voiceData)
{
	pt_Game_Actor::Req_VoiceMessage_U2G Packet;

	Packet.voiceMsg = *voiceData;
	
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSetFocusedAID(const AID_t IN InPlayerAID)
{
	pt_Game_Actor::Req_SetFocusedAID_U2G Packet;

	Packet.focusedAID = InPlayerAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSetLanguageType(const EWRLanguageType::en IN InLanguageType)
{
	pt_Game_Actor::Req_SetLanguageType_U2G Packet;

	Packet.languageType = uint8(InLanguageType);

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestDBSaveAnswerToNPC(const WRTableID IN InDialogueTID)
{
	pt_Game_Actor::Req_DBSaveAnswerToNpc_U2G Packet;

	Packet.dialogueTID = InDialogueTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestLifeEventReward(const WRTableID IN InDialogueTID)
{
	pt_Game_Actor::Req_LifeEventReward_U2G Packet;

	Packet.dialogueTID = InDialogueTID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestAgreeToSecretDialog(AID_t InPartnerAID)
{
	pt_Game_Actor::Req_AgreeToSecretDialog_U2G Packet;
	
	Packet.partnerAID = InPartnerAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndOfSecretDialog(CHAT_GROUP_ID_t InGroupID)
{
	pt_Game_Actor::Req_EndOfSecretDialog_U2G Packet;

	Packet.groupID = InGroupID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestSecretDialogInfo(CHAT_GROUP_ID_t InGroupID)
{
	pt_Game_Actor::Req_SecretDialogInfo_U2G Packet;

	Packet.groupID = InGroupID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestRegistSpeaker(const AID_t IN InPlayerAID)
{
	pt_Game_Actor::Req_RegistSpeaker_U2G Packet;

	Packet.speakerAID = InPlayerAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestDeregistSpeaker(const AID_t IN InPlayerAID)
{
	pt_Game_Actor::Req_DeregistSpeaker_U2G Packet;

	Packet.speakerAID = InPlayerAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestReleaseListener(const AID_t IN InPlayerAID /*= INVALID_AID*/)
{
	pt_Game_Actor::Req_ReleaseListener_U2G Packet;

	Packet.listenerAID = InPlayerAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestArrangeInteriorItem(ISN_t InISN, FVector InLocation, float InAngle)
{
	pt_Game_User_Inventory::Req_ArrangeInteriorItem_U2G Packet;

	Packet.isn = InISN;
	Packet.posX = InLocation.X;
	Packet.posY = InLocation.Y;
	Packet.posZ = InLocation.Z;
	Packet.angle = InAngle;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestRemoveInteriorItem(ISN_t InISN)
{
	pt_Game_User_Inventory::Req_RemoveInteriorItem_U2G Packet;

	Packet.isn = InISN;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestVideoChatList()
{
	pt_Game_Actor::Req_VideoChatList_U2G Packet;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestBeginVideoChat(const AID_t IN InAID)
{
	pt_Game_Actor::Req_BeginVideoChat_U2G Packet;

	Packet.aidReceiver = InAID;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestEndVideoChat(const AID_t IN InAID)
{
	pt_Game_Actor::Req_EndVideoChat_U2G Packet;

	Packet.aidPartner = InAID;

	SEND_TO_GAME_SERVER();
}

//void IMMessageSender::RequestVideoChatMessage(const FString& IN InvoiceData, const bool IN bInfirst, const bool IN bInLast)
//{
//	pt_Game_Actor::Req_VideoChatMessage_U2G Packet;
//
//	if (WRConsoleManager::Get()->GetBool(EWRConsole::TestYSY2))
//		return;
//
//	Packet.voiceMsg = *InvoiceData;
//	Packet.bFirst = bInfirst;
//	Packet.bLast = bInLast;
//
//	SEND_TO_GAME_SERVER();
//}

void IMMessageSender::ReportAgreeToBoard(AID_t InPassengerAID, int8 InSeatIndex, AID_t InVehicleAID, bool bAgree)
{
	pt_Game_Vehicle::Rpt_AgreeToBoard_U2G Packet;
	Packet.passengerAID = InPassengerAID;
	Packet.seatIndex = InSeatIndex;
	Packet.vehicleAID = InVehicleAID;
	Packet.agree = bAgree;
	SEND_TO_GAME_SERVER();
}

void IMMessageSender::ReportAgreeToSecretDialog(AID_t InPartnerAID, CHAT_GROUP_ID_t InGroupID, bool InAgree)
{
	pt_Game_Actor::Rpt_AgreeToSecretDialog_U2G Packet;

	Packet.partnerAID = InPartnerAID;
	Packet.groupID = InGroupID;
	Packet.agree = InAgree;

	SEND_TO_GAME_SERVER();
}

void IMMessageSender::RequestAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InRequesterAID)
{
	pt_Game_Actor::Req_Agree2BeginVideoChat_U2G Packet;

	Packet.bAgree = bInAgree;
	Packet.aidRequester = InRequesterAID;

	SEND_TO_GAME_SERVER();
}
