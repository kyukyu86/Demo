// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#pragma once

#include "Define/WRTypeDefine.h"
#include "Share/Define/Define_Type.h"
#include "Share/Define/Define_BluePrint.h"
#include "Share/Define/Define_Language.h"
#include "Network/WRProtocolHeaders.h"

class AWRCharacter;
class AWRVehicle;
struct UseSkillParam
{
public:
	friend class IMMessageSender;

public:
	UseSkillParam() : Caster(nullptr), CollisionType(EWRCollisionType::None), AttackHeightType(EWRAttackHeightType::None), SkillTID(INVALID_TABLE_TID)
	{
	}

public:
	class AWRCharacter* Caster;
	TArray<AID_t> TargetList;
	EWRCollisionType CollisionType;
	EWRAttackHeightType AttackHeightType;

private:
	WRTableID SkillTID;

public:
	void SetSkillTID(WRTableID InTID, FString InFileName);
};

class IMMessageSender
{
public:
	// -------------------------------------------------- REQ --------------------------------------------------
	// Login
	static bool RequestLogin();
	static void RequestReserveUser();
	static void RequestEnterGame();
	static void RequestMoveToLab();
	static void RequestSelectPlayer();
	static void RequestPlayerList();
	static void RequestSelectPlayerName();
	static void RequestCreatePlayer(const FString& IN InName, const WRTableID IN InCharacterPresetTID, std::string& IN InCustomizingNoneItemData, const TArray<WRTableID>& IN InCostumeItemArray);
	static void RequestDeletePlayer(const PSN_t IN InPSN);
	static void RequestReserveMap(const WRTableID IN InMapTID);
	static void RequestEnterMap();
	static void RequestExitMap();
	static void RequestReadyToPlay();
	static void RequestLogout();

	// actor
	static void ReuqestRevive(PSN_t InPSN);
	static void RequestBeginTalking(const AID_t IN InAID, const bool IN bInRoamingPossible);
	static void RequestEndTalking(const AID_t IN InAID);
	static void RequestSocialAction(const WRTableID IN InSocialAnimationTID, const AID_t IN InTargetAID);
	static void RequestChangePreset(const WRTableID IN InTID);
	static void RequestUnoccupyIO();
	static void RequestSuggestCheers(const AID_t IN InAID);
	static void RequestAnswerCheers(const AID_t IN InAID, const bool IN InAnswer);
	static void RequestBeginCheers(const AID_t IN InAID);
	static void RequestCancelCheers(const AID_t IN InAID);
	static void RequestMicAnimation(const TID_t IN InAnimationTID);
	static void RequestBrodcastTest(FVector IN LinearVelocity, FVector IN AngularVelocity, FVector IN Location, FRotator IN Rotation, bool bSimulatedPhysicSleep, bool bRepPhysics, uint8 LocationQuantizationLevel, uint8 VelocityQuantizationLevel, uint8 RotationQuantizationLevel);

	// move
	static void RequestPlayerMove(class AWRCharacter* InActor);
	static void RequestPlayerMoveStop(class AWRCharacter* InActor);
	static void RequestPlayerMoveInTransportation(class AWRCharacterPlayer* InActor);
	static void RequestPlayerStopInTransportation(class AWRCharacterPlayer* InActor);
	static void RequestPlayerMoveInMW(class AWRCharacterPlayer* InActor);
	static void RequestPlayerStopInMW(class AWRCharacterPlayer* InActor);
	static void RequestCallADA(const AID_t IN InAdaAID, const FVector IN InPos);
	static void RequestPlayerWarp(const WRTableID IN InCoordinateTID);
	static void RequestBeginFall(AWRCharacter* InActor);
	static void RequestEndFall(AWRCharacter* InActor);
	static void ReqeustPlayerAIMoveTo(AWRCharacter* InActor, FVector InDestLocation);

	// combat
	static void RequestUseSkill(const UseSkillParam& InParam);
	static void RequestUseSkillBegin(const UseSkillParam& InParam);

	static void RequestUseSkillEnd(class AWRCharacter* InCaster, SKILL_SEQ_t InSeqID);
	static void RequestGuardBreak(class AWRCharacter* InCaster, class AWRCharacter* InTarget, SKILL_SEQ_t InSeqID);
	static void RequestSuccessToShield(class AWRCharacter* InAttacker, class AWRCharacter* InBlocker, SKILL_SEQ_t InSeqID);
	static void RequestAvoid(class AWRCharacter* InCaster, SKILL_SEQ_t InSeqID, uint8 AvoidDir);
	static void ReqestHitResult(class AWRCharacter* InCaster, class AWRCharacter* InTarget, SKILL_SEQ_t InSeqID, EWRCollisionType InCollisionType);
	static void ReqestProjectileHitResult(class AWRCharacter* InCaster, FVector InLocation, SKILL_SEQ_t InSeqID);
	static void RequestDynamicMove(class AWRCharacter* InActor, FVector InDestLocation, WRTableID InDynamicTID);
	static void RequestSyncDynamicMoveLocation(class AWRCharacter* InCaster);

	// [ 2019-11-13 : hansang88 ] Trigger 관련
	static void RequestTriggerEvent(const TID_t TriggerTID, const uint32 TransitionType, FVector Pos, float Yaw);
	static void RequestSkipSequence();

	// Quest
	static void RequestProgressQuestList();
	static void RequestCompleteQuestList();
	static void RequestAcceptQuest(const WRTableID IN InQuestTID);
	static void RequestCompletableQuest(const WRTableID IN InQuestTID);
	static void RequestCompleteQuest(const WRTableID IN InQuestTID);
	static void RequestCompleteQuestTask(const WRTableID IN InQuestTID, const WRTableID IN InQuestTaskTID);

	// Interaction
	static void RequestBeginInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const uint8 IN InInteractionType);
	static void RequestStopInteraction(const AID_t IN InCharID, const AID_t IN InObjectID);
	static void RequestEndInteraction(const AID_t IN InCharID, const AID_t IN InObjectID, const uint8 IN InInteractionType);

	// Ladder
	static void RequestBeginLadder(const AID_t IN InCharID, const int32 IN InLadderHandleIndex, const int32 IN InHand);
	static void RequestMoveLadder(const AID_t IN InCharID, const int32 IN InLadderHandleIndex, const int32 IN InHand, const int32 IN InLadderDirection);
	static void RequestEndLadder(const AID_t IN InCharID, const int32 IN InLadderHandleIndex);
	static void RequestCancelLadder(const AID_t IN InCharID);

	// Inventory
	static void RequestEntryInfo();
	static void RequestEquipItem(const ISN_t IN InISN);
	static void RequestUnEquipItem(const ISN_t IN InISN);
	static void RequestRegisterEquipmentSlot(const ISN_t IN InISN, const EWRRegistrationSlotType IN InSlotTimeToRegister);
	static void RequestUnRegisterEquipmentSlot(const ISN_t IN InISN);
	static void RequestUseItem(const ISN_t IN InISN, const ITEM_COUNT_t IN InUsedItemCount);
	static void RequestDiscardItem(const ISN_t IN InISN, const ITEM_COUNT_t IN InDiscardedItemCount);
	
	

	// Transportation
	static void RequestGetOnTransportation(const AID_t IN InTransportationAID, const FVector IN InOffset, const float IN InAngle);
	static void RequestGetOffTransportation(const AID_t IN InTransportationAID);

	// Vehicle
	static void RequestSpawnVehicle(AWRCharacter* InVehicleOwner, FVector SpawnPosition, float InAngle, ISN_t InISN, WRTableID InTID);
	static void RequestUnspawnVehicle(AID_t InVehicleAID, ISN_t InVehicleISN);
	static void RequestGetOnVehicle(AWRCharacter* InPassenger, int8 InSeatPosition, AID_t InVehicleAID);
	static void RequestGetOffVehicle(AWRCharacter* InPassenger, AID_t InVehicleAID);
	static void RequestBeginBrake(AWRVehicle* InVehicle);
	static void RequestEndBrake(AWRVehicle* InVehicle);
	static void RequestAcceleration(AWRVehicle* InVehicle, float InValue, float Speed);
	static void RequestSteering(AWRVehicle* InVehicle, float InSteering);
	static void RequestBeHorning(AWRVehicle* InVehicle);
	static void RequestUpdateExterior(AWRVehicle* InVehicle, const stVehicleExteriorInfo& InInfo);
	static void RequestSyncPhysicsForce(AWRVehicle* InVehicle, FVector InForceVector, FVector InImpactLocation);

	static void RequestAgreeToBoard(AID_t InPassengerAID, int8 InSeatIndex, AID_t InVehicleAID);

	// Dance
	static void RequestBeginDance(const WRTableID IN InDanceGroupTID);
	static void RequestEndDance();
	static void RequestChangeDanceStep(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID);

	// [ 2020-3-10 : magedoga ] Sync HMD
	static void RequestSyncHMD(class AWRCharacterPlayer* InCharacter);
	
	// ChatBubble
	static void RequestSpeechBubble(const EWRLanguageType::en IN InLanguageType, const FString& IN InMsg);

	// VoiceCommand
	static void RequestBeginVoiceCommand(const bool IN bInDisplayStartDialogue);
	static void RequestEndVoiceCommand();

	// Cheat
	static void RequestCheat(const FString& IN InCheatStr);

	// MovingWalk
	static void RequestBeginOnMW(const FVector& IN InBasePos);
	static void RequestEndOnMW(const FVector& IN InArrivalPos, const float IN InAngle);


	static void RequestVoiceChat(const FString& IN VoiceBuffer, const uint16 IN Length);

	// chat voice
	static void RequestVoiceMessage(const FString IN voiceData);
	static void RequestSetFocusedAID(const AID_t IN InPlayerAID);

	// LanguageType
	static void RequestSetLanguageType(const EWRLanguageType::en IN InLanguageType);

	// Dialogue
	static void RequestDBSaveAnswerToNPC(const WRTableID IN InDialogueTID);

	// Life Event
	static void RequestLifeEventReward(const WRTableID IN InDialogueTID);
	

	// Secret Dialog
	static void RequestAgreeToSecretDialog(AID_t InPartnerAID); // 대화 허락 요청
	static void RequestEndOfSecretDialog(CHAT_GROUP_ID_t InGroupID); // 대화 종료 요청
	static void RequestSecretDialogInfo(CHAT_GROUP_ID_t InGroupID); // 엿보기 시작 요청

	// Chat Recording(청자기준 번역)
	static void RequestRegistSpeaker(const AID_t IN InPlayerAID);
	static void RequestDeregistSpeaker(const AID_t IN InPlayerAID);
	static void RequestReleaseListener(const AID_t IN InPlayerAID = INVALID_AID);	// INVALID_AID를 넘기면 모든 Listener를 삭제한다.


	// interior
	static void RequestArrangeInteriorItem(ISN_t InISN, FVector InLocation, float InAngle);
	static void RequestRemoveInteriorItem(ISN_t InISN);

	// video chat
	static void RequestVideoChatList();
	static void RequestBeginVideoChat(const AID_t IN InAID);
	static void RequestEndVideoChat(const AID_t IN InAID);
	//static void RequestVideoChatMessage(const FString& IN InvoiceData, const bool IN bInfirst, const bool IN bInLast);

	// -------------------------------------------------- RPT --------------------------------------------------

	static void ReportAgreeToBoard(AID_t InPassengerAID, int8 InSeatIndex, AID_t InVehicleAID, bool bAgree);
	static void ReportAgreeToSecretDialog(AID_t InPartnerAID, CHAT_GROUP_ID_t InGroupID, bool InAgree); // 엿보기 시작 요청

	static void RequestAgree2BeginVideoChat(const bool IN bInAgree, const AID_t IN InRequesterAID);

};