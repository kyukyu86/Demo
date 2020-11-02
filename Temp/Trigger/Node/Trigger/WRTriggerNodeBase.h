#pragma once
#include "../WRTriggerBaseNode.h"
#include "../../../Enum/EWRTriggerEnum.h"
#include "../Transition/WRTriggerTransitionNodeBase.h"

namespace EWRTriggerRandomPosLockAxis
{
	enum Type
	{
		None,
		XAxis = 0x0001,
		YAxis = 0x0002,
		ZAxis = 0x0004,
		XYAxis = XAxis | YAxis,
		XZAxis = XAxis | ZAxis,
		YZAxis = YAxis | ZAxis,
		MAX,
	};
}

class WR_API WRTriggerNodeBase : public WRTriggerBaseNode
{
public:
	WRTriggerNodeBase();
	~WRTriggerNodeBase();

	// [2017-1-11 : magedoga] Events
	virtual void OnEtc(const uint64 InUniqueID = -1);
	virtual void OnEnter(const uint64 InUniqueID = -1);
	virtual void OnExit(const uint64 InUniqueID = -1);
	virtual void OnActive(const uint64 InUniqueID = -1);
	virtual void OnDeactive(const uint64 InUniqueID = -1);
	virtual void OnEnable();
	virtual void OnDisable();

	bool SetTransition(const EWRTriggerTransitionType::en Type, class WRTriggerTransitionNodeBase* InTransition);
	
	const FVector& GetPosition() { return Position; }
	const FRotator& GetRotation() { return Rotation; }
	const FVector& GetScale() { return Scale; }
	const bool GetEnable() { return bEnable; }
	int GetGroupID() { return GroupID; }
	int32 GetQuestTid() { return QuestTid; }

	virtual FVector GetRandomPositionFromShape(const EWRTriggerRandomPosLockAxis::Type& LockAxis) { return Position; }

	virtual bool IsIntersect(AWRCharacter* InCharacter) { return false; }
	bool IsEnteringActor(AWRCharacter* InCharacter);

	bool IsMyFollowIDsEmpty() { return (FollowIDs.Num() == 0); }
	void AddFollowerID(int32 nActorID) { FollowIDs.Add(nActorID); }
	bool RemoveFollowerID(int32 nActorID);
	bool IsContainsTransition(const EWRTriggerTransitionType::en Type);
	bool IsClientOnlyData() { return bClientOnly; }
	virtual bool IsSelfDisable() { return false; }
	virtual bool IsStartTrigger() { return false; }
	virtual int32 GetSaveNumber() { return 0; }
	virtual void EnteringActorReset(bool bForced = false) {}
	virtual void EnteringActorResetCheckUsed() {}

	// [2017-1-12 : magedoga] ImportData
	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject);

	void SetEnteringActor(AWRCharacter* InActor) { EnteringActor = InActor; }

	EWRTriggerConditionType::en GetConditionType() { return ConditionType; }
	int32 GetConditionValue() { return ConditionValue; }

#ifdef WITH_EDITOR
	virtual void DrawTriggerShape() {}
#endif // WITH_EDITOR

private:
	bool GenerateSignal(const EWRTriggerTransitionType::en Type);

protected:
	FVector Position;
	FRotator Rotation;
	FVector Scale;

	bool bEnable = true;
	bool bClientOnly = false;
	class AWRCharacter* EnteringActor = nullptr;
	int			GroupID;
	int32		QuestTid = 0;
	TArray<int32>	FollowIDs;

	EWRTriggerConditionType::en		ConditionType = EWRTriggerConditionType::None;
	int32							ConditionValue = 0;

private:
	TMap<EWRTriggerTransitionType::en, class WRTriggerTransitionNodeBase*> Transitions;

public:
	uint64		TriggerTargetActoreUID;
};