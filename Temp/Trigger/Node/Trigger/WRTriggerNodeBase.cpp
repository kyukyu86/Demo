#include "WRTriggerNodeBase.h"
#include "../WRTriggerBaseNode.h"
#include "../Transition/WRTriggerTransitionNodeBase.h"
#include "Utility/WRTriggerUtility.h"
#include "Utility/WRTriggerUtility.h"
#include "Manager/WRTriggerManager.h"

#include "../../WRTriggerSystem.h"


WRTriggerNodeBase::WRTriggerNodeBase()
{
	TriggerTargetActoreUID = 0;
}

WRTriggerNodeBase::~WRTriggerNodeBase()
{
	Transitions.Empty();
}

void WRTriggerNodeBase::OnEtc(const uint64 InUniqueID/* = -1*/)
{
	if(InUniqueID != -1)
		TriggerTargetActoreUID = InUniqueID;

	GenerateSignal(EWRTriggerTransitionType::Etc);
}

void WRTriggerNodeBase::OnEnter(const uint64 InUniqueID/* = -1*/)
{
	if (InUniqueID != -1)
		TriggerTargetActoreUID = InUniqueID;

	GenerateSignal(EWRTriggerTransitionType::Enter);
}

void WRTriggerNodeBase::OnExit(const uint64 InUniqueID/* = -1*/)
{
	if (InUniqueID != -1)
		TriggerTargetActoreUID = InUniqueID;

	GenerateSignal(EWRTriggerTransitionType::Exit);
}

void WRTriggerNodeBase::OnActive(const uint64 InUniqueID/* = -1*/)
{
	if (bEnable == false)
	{
		return;
	}
		

#ifdef DEF_ADD_TRIGGER_CONDITION_CHECK_BY_HAN_190103
	if (WRTriggerManager::Get()->CheckCondition(this) == false)
	{
		return;
	}
#endif //DEF_ADD_TRIGGER_CONDITION_CHECK_BY_HAN_190103

	if (InUniqueID != -1)
		TriggerTargetActoreUID = InUniqueID;

	GenerateSignal(EWRTriggerTransitionType::Active);
}

void WRTriggerNodeBase::OnDeactive(const uint64 InUniqueID/* = -1*/)
{
	if (bEnable == false)
		return;

	if (InUniqueID != -1)
		TriggerTargetActoreUID = InUniqueID;

	GenerateSignal(EWRTriggerTransitionType::DeActive);
}

void WRTriggerNodeBase::OnEnable()
{
	EnteringActor = nullptr;
	bEnable = true;
}

void WRTriggerNodeBase::OnDisable()
{
	TriggerTargetActoreUID = 0;
	EnteringActor = nullptr;
	bEnable = false;
	
	TriggerSystem->RemoveActivedTransitionNodeByCaller(this);
}

bool WRTriggerNodeBase::SetTransition(const EWRTriggerTransitionType::en Type, WRTriggerTransitionNodeBase* InTransition)
{
	if (Transitions.Contains(Type) == true)
		return false;

	Transitions.Add(Type, InTransition);
	return true;
}


bool WRTriggerNodeBase::IsEnteringActor(AWRCharacter* InCharacter)
{
	if (EnteringActor == nullptr)
		return false;

	return EnteringActor == InCharacter;
}

bool WRTriggerNodeBase::RemoveFollowerID(int32 nActorID)
{
	if (FollowIDs.Contains(nActorID))
	{
		FollowIDs.Remove(nActorID);
		return true;
	}
	return false;
}

bool WRTriggerNodeBase::IsContainsTransition(const EWRTriggerTransitionType::en Type)
{
	if (Transitions.Contains(Type) == false)
		return false;

	return true;
}

void WRTriggerNodeBase::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerBaseNode::ImportDataFromJson(InJsonObject);

	FString DataBuffer;
	if ((*InJsonObject)->TryGetStringField("position", DataBuffer) == true)
	{
		WRTriggerUtility::InitFromString(DataBuffer, Position);
		DataBuffer.Empty();
	}

	Rotation = FRotator::ZeroRotator;
	double RotationYaw = 0.f;
	if ((*InJsonObject)->TryGetNumberField("rotation", RotationYaw) == true)
	{
		Rotation.Yaw = RotationYaw;
// 		WRTriggerUtility::InitFromString(DataBuffer, Rotation);
// 		DataBuffer.Empty();
	}

	if ((*InJsonObject)->TryGetStringField("scale", DataBuffer) == true)
	{
		WRTriggerUtility::InitFromString(DataBuffer, Scale);
	}

	if ((*InJsonObject)->TryGetNumberField("trigger_group_id", GroupID) == false)
	{
		GroupID = 0;
	}

	(*InJsonObject)->TryGetBoolField("enable", bEnable);
	(*InJsonObject)->TryGetBoolField("client_only", bClientOnly);
	
}



bool WRTriggerNodeBase::GenerateSignal(const EWRTriggerTransitionType::en Type)
{
	if (Transitions.Contains(Type) == false)
		return false;

	WRTriggerTransitionNodeBase* TransitionNode = Transitions[Type];
	if (TransitionNode == nullptr)
		return false;

	TransitionNode->Signal(this/*, nullptr*/);
	return true;
}
