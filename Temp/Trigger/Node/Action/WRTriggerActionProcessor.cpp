#include "WRTriggerActionProcessor.h"

#include "../Trigger/WRTriggerNodeBase.h"
#include "../../WRTriggerSystem.h"

#include "WRTriggerActionNodeBase.h"
#include "Manager/WRCharacterManager.h"
#include "Utility/WRTriggerUtility.h"
#include "Table/WREnpcTable.h"
#include "Table/Base/WRTableManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Table/WRCoordinateTable.h"
#include "Table/WRObjectTable.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Table/WRWeaponTable.h"
#include "Utility/WREnumUtility.h"
#include "../../../Enum/EWRSFX.h"
#include "Manager/WRSequenceManager.h"
#include "Manager/WRSFXManager.h"
#include "Components/AudioComponent.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Manager/WRTODManager.h"
#include "Manager/WRQuestManager.h"
#include <MediaAssets/Public/MediaPlayer.h>
#include <MediaAssets/Public/FileMediaSource.h>
#include "Component/Character/WRActorComponentStereoLayerManager.h"
#include <Kismet/KismetMathLibrary.h>
#include "Manager/WRTutorialManager.h"
#include "Actor/Character/WRCharacterActionProp.h"
#include "Manager/WRDialogueManager.h"
#include "Manager/WRMediaPlayerManager.h"
#include "Enum/EWRUIEnum.h"
#include "../../../CustomAsset/WRPlay360VideoAsset.h"
#include "Network/IMMessageSender.h"
#include "Manager/WRConsoleManager.h"
#include "Component/Character/WRActorComponentInteractionObject.h"
#include "Manager/WRVoiceContentsManager.h"

class WR_API WRTriggerActionProcessor_Active : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Active(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Active() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		TArray<int32> OutputNodeUids;	ActionNode->TryGetArrayField("output", OutputNodeUids);
		WRTriggerSystem* TriggerSystem = ActionNode->GetTriggetSystem();
		if (TriggerSystem == nullptr)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("TriggerSystem is Null => %s"), *(ActionNode->GetNodeName()));
			return;
		}
		if (OutputNodeUids.Num() == 0)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("OutputNodeUids.Num() == 0 => %s"), *(ActionNode->GetNodeName()));
			return;
		}

		for (int i = 0; i < OutputNodeUids.Num(); ++i)
		{
			WRTriggerBaseNode* BaseNode = TriggerSystem->GetNodeByUid(OutputNodeUids[i]);
			if (BaseNode == nullptr)
			{
				UE_LOG(WRTriggerLog, Error, TEXT("WRTriggerBaseNode is Null => WRTriggerBaseNode Tid : %d"), OutputNodeUids[i]);
				continue;
			}

			if (EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == false)
			{
				UE_LOG(WRTriggerLog, Error, TEXT("Node type is Not a Trigger => WRTriggerBaseNode Tid : %d"), OutputNodeUids[i]);
				continue;
			}

			WRTriggerNodeBase* TriggerNode = (WRTriggerNodeBase*)BaseNode;
			//WRTriggerNodeBase* WRTriggerNode = WRTriggerManager::Get()->FindTriggerNodeFromCurrentMapTrigger(OutPinUid);

			TriggerNode->OnActive(Caller != nullptr ? Caller->TriggerTargetActoreUID : -1);
		}

	}
};

class WR_API WRTriggerActionProcessor_Deactive : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Deactive(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Deactive() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		//int32 OutPinUid;	ActionNode->TryGetNumberField("output", OutPinUid);
		//WRTriggerNodeBase* WRTriggerNode = WRTriggerManager::Get()->FindTriggerNodeFromCurrentMapTrigger(OutPinUid);

		TArray<int32> OutputNodeUids;	ActionNode->TryGetArrayField("output", OutputNodeUids);
		WRTriggerSystem* TriggerSystem = ActionNode->GetTriggetSystem();
		if (TriggerSystem == nullptr || Caller == nullptr)
			return;

		for (int i = 0; i < OutputNodeUids.Num(); ++i)
		{
			WRTriggerBaseNode* BaseNode = TriggerSystem->GetNodeByUid(OutputNodeUids[i]);
			if (BaseNode == nullptr)
				continue;

			if (EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == false)
				continue;

			WRTriggerNodeBase* TriggerNode = (WRTriggerNodeBase*)BaseNode;
			TriggerNode->OnDeactive(Caller != nullptr ? Caller->TriggerTargetActoreUID : -1);
		}
	}
};

class WR_API WRTriggerActionProcessor_Disable : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Disable(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Disable() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		//int32 OutPinUid;	ActionNode->TryGetNumberField("output", OutPinUid);
		//WRTriggerNodeBase* WRTriggerNode = WRTriggerManager::Get()->FindTriggerNodeFromCurrentMapTrigger(OutPinUid);

		TArray<int32> OutputNodeUids;	ActionNode->TryGetArrayField("output", OutputNodeUids);
		WRTriggerSystem* TriggerSystem = ActionNode->GetTriggetSystem();
		if (TriggerSystem == nullptr)
			return;

		for (int i = 0; i < OutputNodeUids.Num(); ++i)
		{
			WRTriggerBaseNode* BaseNode = TriggerSystem->GetNodeByUid(OutputNodeUids[i]);
			if (BaseNode == nullptr)
				continue;

			if (EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == false)
				continue;

			WRTriggerNodeBase* TriggerNode = (WRTriggerNodeBase*)BaseNode;
			TriggerNode->OnDisable();
		}
	}
};

class WR_API WRTriggerActionProcessor_Enable : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Enable(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Enable() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		//int32 OutPinUid;	ActionNode->TryGetNumberField("output", OutPinUid);
		//WRTriggerNodeBase* WRTriggerNode = WRTriggerManager::Get()->FindTriggerNodeFromCurrentMapTrigger(OutPinUid);

		TArray<int32> OutputNodeUids;	ActionNode->TryGetArrayField("output", OutputNodeUids);
		WRTriggerSystem* TriggerSystem = ActionNode->GetTriggetSystem();
		if (TriggerSystem == nullptr)
			return;

		for (int i = 0; i < OutputNodeUids.Num(); ++i)
		{
			WRTriggerBaseNode* BaseNode = TriggerSystem->GetNodeByUid(OutputNodeUids[i]);
			if (BaseNode == nullptr)
				continue;

			if (EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == false)
				continue;

			WRTriggerNodeBase* TriggerNode = (WRTriggerNodeBase*)BaseNode;
			TriggerNode->OnEnable();
		}
	}
};

//====================================================================================================================
// [2017-1-10 : magedoga] Action Spawn
class WR_API WRTriggerActionProcessor_Spawn : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Spawn(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Spawn() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		FString Buffer;
		FVector Position = FVector::ZeroVector;
		FRotator Rotation = FRotator::ZeroRotator;
		WRTableID CharacterTID;	ActionNode->TryGetNumberField("objecttid", CharacterTID);

		if (ActionNode->GetNodeType() == EWRTriggerNodeType::Action_AOSpawn ||
			ActionNode->GetNodeType() == EWRTriggerNodeType::Action_SOSpawn ||
			ActionNode->GetNodeType() == EWRTriggerNodeType::Action_IOSpawn ||
			ActionNode->GetNodeType() == EWRTriggerNodeType::Action_ENpcSpawn)
		{
			TArray<FVector>		Positions;		ActionNode->TryGetArrayField("position", Positions);
			TArray<float>		Yaws;			ActionNode->TryGetArrayField("direction", Yaws);
			int PosRandomIndex = UKismetMathLibrary::RandomInteger(Positions.Num());
			int RotRandomIndex = UKismetMathLibrary::RandomInteger(Yaws.Num());

			if (PosRandomIndex < Positions.Num())
				Position = Positions[PosRandomIndex];

			if (RotRandomIndex < Yaws.Num())
				Rotation.Yaw = Yaws[RotRandomIndex];
		}
		else
		{
			ActionNode->TryGetStringField("position", Buffer);	WRTriggerUtility::InitFromString(Buffer, Position);	Buffer.Empty();
			ActionNode->TryGetNumberField("direction", Rotation.Yaw);
		}

		switch (ActionNode->GetNodeType())
		{
		case EWRTriggerNodeType::Action_IOSpawn:
		{
			FWRInteractionObjectTable* OutTable = WRTableManager::Get()->FindRow<FWRInteractionObjectTable>(CharacterTID);
			if (OutTable)
			{
				FWRCharacterSpawnParams SpawnParam(0, CharacterTID, OutTable->MainBPPath, Position, Rotation, FVector::OneVector, ActionNode->GetUid());
				AWRCharacterIO* SpawnIO = WRCharacterManager::Get()->Spawn<AWRCharacterIO>(SpawnParam);
				if (SpawnIO != nullptr)
				{
					SpawnIO->OnSpawnComplete(OutTable, EWRInteractionState::Usable);
					if (Caller)
					{
						Caller->AddFollowerID(ActionNode->GetUid());
					}
				}
			}
		}
		break;
		case EWRTriggerNodeType::Action_ActionPropSpawn:
		{
			FWRActionPropTable* OutTable = WRTableManager::Get()->FindRow<FWRActionPropTable>(CharacterTID);
			if (OutTable)
			{
				FWRCharacterSpawnParams SpawnParam(0, CharacterTID, OutTable->MainBPPath, Position, Rotation, FVector::OneVector, ActionNode->GetUid());
				AWRCharacterActionProp* SpawnChar = WRCharacterManager::Get()->Spawn<AWRCharacterActionProp>(SpawnParam);
				if (Caller)
				{
					Caller->AddFollowerID(ActionNode->GetUid());
				}
			}
		}
		break;
		case EWRTriggerNodeType::Action_BrickSpawn:
		{
			FWRBrickTable* OutTable = WRTableManager::Get()->FindRow<FWRBrickTable>(CharacterTID);
			if (OutTable)
			{
				FWRCharacterSpawnParams SpawnParam(0, CharacterTID, OutTable->MainBPPath, Position, Rotation, FVector::OneVector, ActionNode->GetUid());
				AWRCharacterObjectBase* SpawnObject = WRCharacterManager::Get()->Spawn<AWRCharacterObjectBase>(SpawnParam);
				if (Caller)
				{
					Caller->AddFollowerID(ActionNode->GetUid());
				}
			}
		}
		break;
		case EWRTriggerNodeType::Action_ENpcSpawn:
		case EWRTriggerNodeType::Action_NpcSpawn:
		{
			FWRNPCBaseTable* OutTable = nullptr;
			if (ActionNode->GetNodeType() == EWRTriggerNodeType::Action_ENpcSpawn)
				OutTable = WRTableManager::Get()->FindRow<FWREnpcTable>(CharacterTID);
			else if (ActionNode->GetNodeType() == EWRTriggerNodeType::Action_NpcSpawn)
				OutTable = WRTableManager::Get()->FindRow<FWRNpcTable>(CharacterTID);

			if (OutTable)
			{
				FWRCharacterSpawnParams SpawnParam(0, CharacterTID, OutTable->BlueprintID, Position, Rotation, FVector::OneVector, ActionNode->GetUid());
				AWRCharacter* SpawnChar = WRCharacterManager::Get()->Spawn<AWRCharacter>(SpawnParam);
				if (SpawnChar != nullptr)
				{
					if (Caller)
					{
						Caller->AddFollowerID(ActionNode->GetUid());
					}
				}
			}
		}
		break;
		}
// 		if (ActionNode->GetNodeType() == EWRTriggerNodeType::Action_IOSpawn)
// 		{
// 			FWRInteractionObjectTable* OutTable = WRTableManager::Get()->FindRow<FWRInteractionObjectTable>(CharacterTID);
// 			if (OutTable)
// 			{
// 				FWRCharacterSpawnParams SpawnParam(0, CharacterTID, OutTable->MainBPPath.ToString(), Position, Rotation, FVector::OneVector, ActionNode->GetUid());
// 				AWRCharacterIO* SpawnIO = WRCharacterManager::Get()->Spawn<AWRCharacterIO>(SpawnParam);
// 				if (SpawnIO != nullptr)
// 				{
// 					SpawnIO->OnSpawnComplete(OutTable, EWRInteractionState::Usable);
// 					if (Caller)
// 					{
// 						Caller->AddFollowerID(ActionNode->GetUid());
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			FWRNPCBaseTable* OutTable = nullptr;
// 			if(ActionNode->GetNodeType() == EWRTriggerNodeType::Action_ENpcSpawn)
// 				OutTable = WRTableManager::Get()->FindRow<FWREnpcTable>(CharacterTID);
// 			else if (ActionNode->GetNodeType() == EWRTriggerNodeType::Action_NpcSpawn)
// 				OutTable = WRTableManager::Get()->FindRow<FWRNpcTable>(CharacterTID);
// 
// 			if (OutTable)
// 			{
// 				FWRCharacterSpawnParams SpawnParam(0, CharacterTID, OutTable->BlueprintID.ToString(), Position, Rotation, FVector::OneVector, ActionNode->GetUid());
// 				AWRCharacter* SpawnChar = WRCharacterManager::Get()->Spawn<AWRCharacter>(SpawnParam);
// 				if (SpawnChar != nullptr)
// 				{
// 					if (Caller)
// 					{
// 						Caller->AddFollowerID(ActionNode->GetUid());
// 					}
// 				}
// 			}
// 		}
	}
};

class WR_API WRTriggerActionProcessor_UnSpawn : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_UnSpawn(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_UnSpawn() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		int32 ActorTUid;	ActionNode->TryGetNumberField("actor_tuid", ActorTUid);

		if (ActorTUid != 0)
		{
			AWRCharacter* UnSpawnChar = WRCharacterManager::Get()->FindCharacterBySpawnTid(ActorTUid);
			if (UnSpawnChar != nullptr)
				WRCharacterManager::Get()->Remove(UnSpawnChar);
		}
	}
};

class WR_API WRTriggerActionProcessor_Warp : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Warp(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Warp() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32 CoordinateTid;	ActionNode->TryGetNumberField("coordinate_tid", CoordinateTid);
		FWRCoordinateTable*	CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(CoordinateTid);
		if (CoordinateTable)
		{
			AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
			if (MyChar)
			{
				// [ 2019-10-4 : hansang88 ] 일단 맵정보가 없어서 그냥 True 처리 해놈 추후 => 현재 맵과 같은곳(CoordinateTable->MapTID)인지? 체크 하도록 수정 필요
				if (true)
				{
					// [ 2019-10-4 : hansang88 ] 워프
					FRotator MyRot = FRotator::ZeroRotator;
					MyRot.Yaw = CoordinateTable->Angle;
					MyChar->SetLocation(CoordinateTable->Position);
					MyChar->SetRotation(MyRot);
				}
				else
				{
					// [ 2019-10-4 : hansang88 ] 맵이동
				}
			}
		}
	}
};

class WR_API WRTriggerActionProcessor_WalkSpeedRate : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_WalkSpeedRate(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_WalkSpeedRate() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		float WalkSpeedRate;	ActionNode->TryGetNumberField("walk_speed_rate", WalkSpeedRate);
		AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		if (MyChar)
		{
			UCharacterMovementComponent* MyMoveComponent = MyChar->GetCharacterMovement();
			if (MyMoveComponent)
			{
				MyMoveComponent->MaxWalkSpeed *= WalkSpeedRate;
			}
		}
	}
};

class WR_API WRTriggerActionProcessor_NPCSocketSwap : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_NPCSocketSwap(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_NPCSocketSwap() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32 WeaponTid;			ActionNode->TryGetNumberField("weapon_tid", WeaponTid);
		int32 SpawnTriggerTid;		ActionNode->TryGetNumberField("actor_tuid", SpawnTriggerTid);

		FWRWeaponTable* WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(WeaponTid);

		if (WeaponTable != nullptr)
		{
			AWRCharacter* TargetChar = WRCharacterManager::Get()->FindCharacterBySpawnTid(SpawnTriggerTid);
			if (TargetChar != nullptr)
			{
				UWRActorComponentEquipmentLook* EquipmentLookComponent = TargetChar->GetComponent<UWRActorComponentEquipmentLook>();
				if (EquipmentLookComponent)
				{
					EquipmentLookComponent->Equip(WeaponTable->WeaponLookTid);
				}
			}
		}
	}
};

class WR_API WRTriggerActionProcessor_ControllerVibrationStart : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ControllerVibrationStart(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ControllerVibrationStart() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32 ForceFeedBackTid;	ActionNode->TryGetNumberField("force_feed_back_tid", ForceFeedBackTid);

		WRInputSystemManager::Get()->ClientPlayForceFeedback(ForceFeedBackTid);
	}
};

class WR_API WRTriggerActionProcessor_ControllerVibrationEnd : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ControllerVibrationEnd(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ControllerVibrationEnd() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32 ForceFeedBackTid;	ActionNode->TryGetNumberField("force_feed_back_tid", ForceFeedBackTid);

		WRInputSystemManager::Get()->ClientStopForceFeedback(ForceFeedBackTid);
	}
};

class WR_API WRTriggerActionProcessor_HideUI : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_HideUI(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_HideUI() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		FString Buffer;			ActionNode->TryGetStringField("ui_kind", Buffer);
		EWRUIPanelKind Type = (EWRUIPanelKind)WREnumUtility::StringToEnum("EWRUIPanelKind", Buffer);

		WRUIManager::Get()->CloseUI(Type);
	}
};

class WR_API WRTriggerActionProcessor_ShowUI : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ShowUI(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ShowUI() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		FString Buffer;			ActionNode->TryGetStringField("ui_kind", Buffer);
		EWRUIPanelKind Type = (EWRUIPanelKind)WREnumUtility::StringToEnum("EWRUIPanelKind", Buffer);

		int32 CustomParam;		ActionNode->TryGetNumberField("custom_param", CustomParam);
		FTransform Transform;	ActionNode->TryGetStringField("transform", Buffer); WRTriggerUtility::InitFromString(Buffer, Transform);

		if (IsMultiCreatableUI(Type))
			return;

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				if (CustomParam != 0)
					InCombinedPanelData.UIPanel->ThrowParam(CustomParam);
			}
		});

		if (Type == EWRUIPanelKind::EquipNavigator)
		{
			FWRWidgetInfo WidgetInfo(Type, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
			//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(Type, Transform, EWRUIWidgetCreationBitFlag::None, &Delegate);
		}
		else
		{
			FWRWidgetInfo WidgetInfo(Type, EWRUIWidgetCreationBitFlag::None, &Delegate);
			WidgetInfo.SetTransform(Transform);
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
			//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(Type, Transform, EWRUIWidgetCreationBitFlag::None, &Delegate);
		}
	}
};

class WR_API WRTriggerActionProcessor_PlayAnim : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_PlayAnim(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_PlayAnim() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32		TargetActorTUid;		ActionNode->TryGetNumberField("actor_tuid", TargetActorTUid);
		int32		MontageID;				ActionNode->TryGetNumberField("montage_id", MontageID);
		FString		StartSectionName;		ActionNode->TryGetStringField("start_section_name", StartSectionName);

		AWRCharacter* TargetChar = WRCharacterManager::Get()->FindCharacterBySpawnTid(TargetActorTUid);
		if (TargetChar->IsValidLowLevel())
		{
			TargetChar->PlayAnimationMontage(MontageID, StartSectionName);
		}
	}
};

class WR_API WRTriggerActionProcessor_PlayLevelSequence : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_PlayLevelSequence(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_PlayLevelSequence() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		FString SequencePath;		ActionNode->TryGetStringField("level_sequence_name", SequencePath);

		TArray<int32> OutputNodeUids;
		if (ActionNode->TryGetArrayField("output", OutputNodeUids) == false)
		{
			OutputNodeUids.Empty();
		}

		WRSequenceManager::Get()->PlaySequence(SequencePath);
		WRSequenceManager::Get()->SetPostTriggerIDs(SequencePath, OutputNodeUids);
	}
};
#define SPAWN_DUMMY_VALUE -1.f
class WR_API WRTriggerActionProcessor_PlaySound : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_PlaySound(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_PlaySound() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		FString Buffer;			ActionNode->TryGetStringField("sound_type", Buffer);
		FString SoundPath;		ActionNode->TryGetStringField("sound_path", SoundPath);
		EWRSoundClassKind Type = (EWRSoundClassKind)WREnumUtility::StringToEnum("EWRSoundClassKind", Buffer);
		float	FadeDuration;	ActionNode->TryGetNumberField("fade_duration", FadeDuration);

		WRSFXManager::Get()->SpawnBGM(SoundPath, FadeDuration);
	}
};

class WR_API WRTriggerActionProcessor_QuestComplete : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_QuestComplete(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_QuestComplete() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		WRQuestManager::Get()->OnQuestCompleteTriggered(ActionNode->GetUid());
	}
};



class WR_API WRTriggerActionProcessor_SoundSpawn : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_SoundSpawn(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_SoundSpawn() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		FString Buffer;
		FVector SpawnLocation;	ActionNode->TryGetStringField("spawn_position", Buffer); WRTriggerUtility::InitFromString(Buffer, SpawnLocation);
		FString SoundPath;		ActionNode->TryGetStringField("sound", SoundPath);
		int32	ActorTUid;		ActionNode->TryGetNumberField("attach_actor_tuid", ActorTUid);
		float	FadeDuration;	ActionNode->TryGetNumberField("fade_duration", FadeDuration);

		AWRCharacter* AttchTargetChar = WRCharacterManager::Get()->FindCharacterBySpawnTid(ActorTUid);
		
		if (AttchTargetChar->IsValidLowLevel() == true)
		{
			USceneComponent* TempSceneComp = AttchTargetChar->GetRootComponent();

			if (TempSceneComp->IsValidLowLevel() == true)
				WRSFXManager::Get()->SpawnAttached(SoundPath, AttchTargetChar->GetRootComponent(), SPAWN_DUMMY_VALUE, SPAWN_DUMMY_VALUE, ActionNode->GetUid());
		}
		else
		{
			WRSFXManager::Get()->SpawnAtLocation(SoundPath, SpawnLocation, SPAWN_DUMMY_VALUE, SPAWN_DUMMY_VALUE, ActionNode->GetUid());
		}
	}
};

class WR_API WRTriggerActionProcessor_TODControl : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_TODControl(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_TODControl() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32	Priority;		ActionNode->TryGetNumberField("priority", Priority);
		float	Duration;		ActionNode->TryGetNumberField("duration", Duration);
		float	DayCycle;		ActionNode->TryGetNumberField("day_cycle", DayCycle);
		//bool	bIsRaining;		ActionNode->TryGetBoolField("is_raining", bIsRaining);
		//bool	bIsHeavyRaining;ActionNode->TryGetBoolField("is_heavy_raining", bIsHeavyRaining);
		//bool	bIsThundering;	ActionNode->TryGetBoolField("is_thundering", bIsThundering);
		//bool	bIsLightning;	ActionNode->TryGetBoolField("is_lightning", bIsLightning);
		float Raining;			ActionNode->TryGetNumberField("raining", Raining);
		float Thundering;		ActionNode->TryGetNumberField("thundering", Thundering);
		float Lightning;		ActionNode->TryGetNumberField("lightning", Lightning);

		FWRWind LocalWind;

		FWRTODETC LocalETC;
		LocalETC.Raining = Raining;
		LocalETC.Thundering = Thundering;
		LocalETC.Lightning = Lightning;

		WRTODManager::Get()->SpawnTOD(DayCycle, Priority, Duration, LocalETC, LocalWind);
	}
};

class WR_API WRTriggerActionProcessor_PlayMovie : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_PlayMovie(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_PlayMovie() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32	MovieTid;	ActionNode->TryGetNumberField("movie_tid", MovieTid);

		WRMediaPlayerManager::Get()->PlayTheaterMovie(MovieTid);

		//TArray<int32> OutputNodeUids;	ActionNode->TryGetArrayField("output", OutputNodeUids);

//  		AWRCharacter* MyChar = WRCharacterManager::Get()->GetMyCharacter();
//  		if (MyChar)
//  		{
// 			UWRActorComponentStereoLayerManager* StereoLayerManager = MyChar->GetComponent<UWRActorComponentStereoLayerManager>();
// 			if (StereoLayerManager->IsValidLowLevel() == true)
// 			{
// 				StereoLayerManager->PlayStereoLayerMovie(MovieTid, OutputNodeUids);
// 			}
//  		}
	}
};

class WR_API WRTriggerActionProcessor_PlayObjectAnim : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_PlayObjectAnim(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_PlayObjectAnim() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32	TargetActorTUid;		ActionNode->TryGetNumberField("actor_tuid", TargetActorTUid);

		AWRCharacterIO* TargetChar = Cast<AWRCharacterIO>(WRCharacterManager::Get()->FindCharacterBySpawnTid(TargetActorTUid));
		if (TargetChar->IsValidLowLevel())
		{
			UWRActorComponentInteractionObject* pInteractionComponent = TargetChar->GetComponent<UWRActorComponentInteractionObject>();
			if (pInteractionComponent != nullptr)
			{
				pInteractionComponent->InvokeDirectionGroup(EWRInteractionGroup::DirectInteraction);
			}
		}
	}
};

class WR_API WRTriggerActionProcessor_SoundUnSpawn : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_SoundUnSpawn(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_SoundUnSpawn() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32	SoundTUid;		ActionNode->TryGetNumberField("sound_tuid", SoundTUid);

		WRSFXManager::Get()->Remove(SoundTUid);
	}
};

class WR_API WRTriggerActionProcessor_Tutorial : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Tutorial(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Tutorial() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		int32	TutorialTid;		ActionNode->TryGetNumberField("tutorial_tid", TutorialTid);

		WRTutorialManager::Get()->OnNewTutorial((WRTableID)TutorialTid);
	}
};

class WR_API WRTriggerActionProcessor_TutorialMoveComplete : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_TutorialMoveComplete(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_TutorialMoveComplete() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		if (Caller != nullptr)
		{
			int32 TriggerTid = Caller->GetUid();

			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::MoveToTrigger, (WRTableID)TriggerTid);
		}
	}
};

class WR_API WRTriggerActionProcessor_ShowDialogue : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ShowDialogue(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ShowDialogue() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		TArray<int32> DialogueTid;	ActionNode->TryGetArrayField("dialogue_tid", DialogueTid);
		bool	bLoop;				ActionNode->TryGetBoolField("loop", bLoop);
		float	LoopingDelay;		ActionNode->TryGetNumberField("looping_delay", LoopingDelay);

		if (bLoop)
		{
			WRDialogueManager::Get()->StartLoopDialogue(ActionNode->GetUid(), DialogueTid, LoopingDelay * 0.001f);
		}
		else
		{
			if (DialogueTid.Num() == 0)
				return;

			int32 RandIndex = FMath::Rand() % DialogueTid.Num();
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::TriggerNpcTalk, DialogueTid[RandIndex]);
		}
	}
};

class WR_API WRTriggerActionProcessor_ShowDialogueEnd : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ShowDialogueEnd(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ShowDialogueEnd() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		int32	ShowDialogueTUid;		ActionNode->TryGetNumberField("show_dialogue_tuid", ShowDialogueTUid);

		WRDialogueManager::Get()->StopLoopDialogue(ShowDialogueTUid);
	}
};

class WR_API WRTriggerActionProcessor_StopMovie : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_StopMovie(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_StopMovie() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		int32	MovieTid;	ActionNode->TryGetNumberField("movie_tid", MovieTid);

		WRMediaPlayerManager::Get()->StopTheaterMovie(MovieTid);
	}
};

class WR_API WRTriggerActionProcessor_ShowMarker : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ShowMarker(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ShowMarker() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		int32	MarkerTid;	ActionNode->TryGetNumberField("marker_tid", MarkerTid);

	}
};

class WR_API WRTriggerActionProcessor_HideMarker : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_HideMarker(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_HideMarker() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		int32	MarkerTid;	ActionNode->TryGetNumberField("marker_tid", MarkerTid);

	}
};

class WR_API WRTriggerActionProcessor_ShowFirstPersonCharacter : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ShowFirstPersonCharacter(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ShowFirstPersonCharacter() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter->IsValidLowLevel() == true)
		{
			// [ 2020-4-8 : magedoga ] Offset Trigger Data로 입력받아서 넣어주면 됨
			float OffsetX = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetX);
			float OffsetZ = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetZ);
			FVector OffsetXZ = (FVector::ForwardVector * OffsetX) + (FVector::UpVector * OffsetZ);

			MyCharacter->Show3rdCharacter(false, OffsetXZ);
		}
	}
};

class WR_API WRTriggerActionProcessor_ShowThirdPersonCharacter : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ShowThirdPersonCharacter(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ShowThirdPersonCharacter() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter->IsValidLowLevel() == true)
		{
			// [ 2020-4-8 : magedoga ] Offset Trigger Data로 입력받아서 넣어주면 됨
			float OffsetX = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetX);
			float OffsetZ = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetZ);
			FVector OffsetXZ = (FVector::ForwardVector * OffsetX) + (FVector::UpVector * OffsetZ);

			MyCharacter->Show3rdCharacter(true, OffsetXZ);
		}
	}
};

class WR_API WRTriggerActionProcessor_Play360Movie : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_Play360Movie(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_Play360Movie() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		FString MoviePath;		ActionNode->TryGetStringField("movie_path", MoviePath);
		bool bUse3D = false;	ActionNode->TryGetBoolField("use_3d", bUse3D);
		
		FStringAssetReference Play360AssetSTR = FStringAssetReference(*MoviePath);
		//UWRPlay360VideoAsset* VideoAsset = Cast<UWRPlay360VideoAsset>(StaticLoadObject(UDataTable::StaticClass(), NULL, *MoviePath));
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter->IsValidLowLevel() == true)
		{
			MyCharacter->Play360Movie(Cast<UWRPlay360VideoAsset>(Play360AssetSTR.TryLoad()), bUse3D);
		}
	}
};

class WR_API WRTriggerActionProcessor_ServerTriggerActive : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_ServerTriggerActive(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_ServerTriggerActive() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);
		TArray<int32> OutputNodeUids;	ActionNode->TryGetArrayField("output", OutputNodeUids);
		
		if (OutputNodeUids.Num() == 0)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("OutputNodeUids.Num() == 0 => %s"), *(ActionNode->GetNodeName()));
			return;
		}

		AWRCharacter* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		if (MyChar->IsValidLowLevel() == true)
		{
			UE_LOG(WRTriggerLog, Display, TEXT("ServerTriggerActive Tid => %d"), OutputNodeUids[0]);
			IMMessageSender::RequestTriggerEvent(OutputNodeUids[0], EWRTriggerNodeType::Transition_Active, MyChar->GetActorLocation(), MyChar->GetActorRotation().Yaw);
		}
	}
};

class WR_API WRTriggerActionProcessor_StartVoiceRecord : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_StartVoiceRecord(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_StartVoiceRecord() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		WRVoiceContentsManager::Get()->StartDefaultVoiceRecordingAtTrigger();
	}
};


class WR_API WRTriggerActionProcessor_SetMidpointPauseDirecting : public WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor_SetMidpointPauseDirecting(WRTriggerActionNodeBase* InActionNode) : WRTriggerActionProcessor(InActionNode) {}
	virtual ~WRTriggerActionProcessor_SetMidpointPauseDirecting() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller) override
	{
		WRTriggerActionProcessor::ProcessAction(Caller);

		bool bEnable;
		ActionNode->TryGetBoolField("enable", bEnable);

		WRMapManager::Get()->SetMidpointPauseWhenDirecting(bEnable);
		UE_LOG(WRTriggerLog, Display, TEXT("WRTriggerActionProcessor_SetMidpointPauseDirecting::ProcessAction - %s"), bEnable == true ? TEXT("Enable") : TEXT("Disable"));
	}
};

AWRCharacter* WRTriggerActionProcessor::FindCharacter(const uint64& InUID, bool bSpawnTid/* = true*/)
{
	// 내 캐릭터를 의미하는가?
// 	if (MY_CHARACTER_UID == InUID)
// 	{
// 		return IMCharacterManager::Get()->FindCharacterByAUID(WRCharacterManager::Get()->GetMyCharacterUID());
// 	}
// 	// 내 펠로우를 의미하는가?
// 	else if (MY_FELLOW_SKILL_UID == InUID)
// 	{
// 		UIMActorComponentRide* RideComponent = WRCharacterManager::Get()->GetMyCharacter()->GetCastedComponent<UIMActorComponentRide>(EIMCharacterComponentType::Ride);
// 		if (RideComponent)
// 		{
// 			return IMCharacterManager::Get()->FindCharacterByAUID(RideComponent->GetSlaveUID());
// 		}
// 	}
// 
// 	if (bSpawnTid)
// 	{
// 		return IMCharacterManager::Get()->FindCharacterBySpawnTid(InUID);
// 	}
// 	else
// 	{
// 		return IMCharacterManager::Get()->FindCharacterByAUID(InUID);
// 	}
	return nullptr;
}

WRTriggerActionProcessor* WRTriggerActionProcessor::CreateLegalProcessor(WRTriggerActionNodeBase* InActionNode)
{
	if (InActionNode == nullptr)
		return new WRTriggerActionProcessor(InActionNode);

	switch (InActionNode->GetNodeType())
	{
	case EWRTriggerNodeType::Action_Active: return new WRTriggerActionProcessor_Active(InActionNode);
	case EWRTriggerNodeType::Action_Deactive: return new WRTriggerActionProcessor_Deactive(InActionNode);
	case EWRTriggerNodeType::Action_Disable: return new WRTriggerActionProcessor_Disable(InActionNode);
	case EWRTriggerNodeType::Action_Enable: return new WRTriggerActionProcessor_Enable(InActionNode);
	case EWRTriggerNodeType::Action_UnSpawn: return new WRTriggerActionProcessor_UnSpawn(InActionNode);
	case EWRTriggerNodeType::Action_NpcSpawn: return new WRTriggerActionProcessor_Spawn(InActionNode);
	case EWRTriggerNodeType::Action_ENpcSpawn: return new WRTriggerActionProcessor_Spawn(InActionNode);
	case EWRTriggerNodeType::Action_Warp: return new WRTriggerActionProcessor_Warp(InActionNode);
	case EWRTriggerNodeType::Action_WalkSpeedRate: return new WRTriggerActionProcessor_WalkSpeedRate(InActionNode);
	case EWRTriggerNodeType::Action_NPCSocketSwap: return new WRTriggerActionProcessor_NPCSocketSwap(InActionNode);
	case EWRTriggerNodeType::Action_ControllerVibrationStart: return new WRTriggerActionProcessor_ControllerVibrationStart(InActionNode);
	case EWRTriggerNodeType::Action_ControllerVibrationEnd: return new WRTriggerActionProcessor_ControllerVibrationEnd(InActionNode);

	case EWRTriggerNodeType::Action_HideUI: return new WRTriggerActionProcessor_HideUI(InActionNode);
	case EWRTriggerNodeType::Action_ShowUI: return new WRTriggerActionProcessor_ShowUI(InActionNode);
	case EWRTriggerNodeType::Action_PlayAnim: return new WRTriggerActionProcessor_PlayAnim(InActionNode);
	case EWRTriggerNodeType::Action_PlayLevelSequence: return new WRTriggerActionProcessor_PlayLevelSequence(InActionNode);
	case EWRTriggerNodeType::Action_PlaySound: return new WRTriggerActionProcessor_PlaySound(InActionNode);
	case EWRTriggerNodeType::Action_QuestComplete: return new WRTriggerActionProcessor_QuestComplete(InActionNode);
	case EWRTriggerNodeType::Action_SoundSpawn: return new WRTriggerActionProcessor_SoundSpawn(InActionNode);
	case EWRTriggerNodeType::Action_TODControl: return new WRTriggerActionProcessor_TODControl(InActionNode);
	case EWRTriggerNodeType::Action_PlayMovie: return new WRTriggerActionProcessor_PlayMovie(InActionNode);
	case EWRTriggerNodeType::Action_PlayObjectAnim: return new WRTriggerActionProcessor_PlayObjectAnim(InActionNode);
	case EWRTriggerNodeType::Action_SoundUnSpawn: return new WRTriggerActionProcessor_SoundUnSpawn(InActionNode);
	case EWRTriggerNodeType::Action_BrickSpawn: return new WRTriggerActionProcessor_Spawn(InActionNode);
	case EWRTriggerNodeType::Action_ActionPropSpawn: return new WRTriggerActionProcessor_Spawn(InActionNode);
	case EWRTriggerNodeType::Action_Tutorial: return new WRTriggerActionProcessor_Tutorial(InActionNode);
	case EWRTriggerNodeType::Action_TutorialMoveComplete: return new WRTriggerActionProcessor_TutorialMoveComplete(InActionNode);
	case EWRTriggerNodeType::Action_ShowDialogue: return new WRTriggerActionProcessor_ShowDialogue(InActionNode);
	case EWRTriggerNodeType::Action_ShowDialogueEnd: return new WRTriggerActionProcessor_ShowDialogueEnd(InActionNode);
	case EWRTriggerNodeType::Action_StopMovie: return new WRTriggerActionProcessor_StopMovie(InActionNode);
	case EWRTriggerNodeType::Action_ShowMarker: return new WRTriggerActionProcessor_ShowMarker(InActionNode);
	case EWRTriggerNodeType::Action_HideMarker: return new WRTriggerActionProcessor_HideMarker(InActionNode);
	case EWRTriggerNodeType::Action_ShowFirstPersonCharacter: return new WRTriggerActionProcessor_ShowFirstPersonCharacter(InActionNode);
	case EWRTriggerNodeType::Action_ShowThirdPersonCharacter: return new WRTriggerActionProcessor_ShowThirdPersonCharacter(InActionNode);
	case EWRTriggerNodeType::Action_Play360Movie: return new WRTriggerActionProcessor_Play360Movie(InActionNode);
	case EWRTriggerNodeType::Action_ServerTriggerActive: return new WRTriggerActionProcessor_ServerTriggerActive(InActionNode);
	case EWRTriggerNodeType::Action_StartVoiceRecord: return new WRTriggerActionProcessor_StartVoiceRecord(InActionNode);
	case EWRTriggerNodeType::Action_SetMidpointPauseDirecting: return new WRTriggerActionProcessor_SetMidpointPauseDirecting(InActionNode);
	}

	return new WRTriggerActionProcessor(InActionNode);
}
