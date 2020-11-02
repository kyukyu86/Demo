#pragma once
#include "../Enum/EWRTriggerEnum.h"

class WR_API WRTriggerSystem
{
public:
	WRTriggerSystem() : Name(""){}

	WRTriggerSystem(const FString& InName);
	~WRTriggerSystem();

	void Tick(float DeltaTime);

	//bool CallTriggerNode(const int32 TriggerNodeUid, EWRTriggerNodeType::en TransitionType);

	// [2017-1-11 : magedoga] BaseNode
	class WRTriggerBaseNode* GetNodeByUid(const int32& InUid);

	// [2017-1-12 : magedoga] Name
	const FString& GetName() const { return Name; }
	void SetName(const FString& InName) { Name = InName; }

	// [2017-1-12 : magedoga] CollisionActor
	void SetCollisionActor(class AWRCharacter* Actor);
	void RemoveCollisionActor(class AWRCharacter* Actor);

	void SetGroupCollisionActor(class AWRCharacter* Actor, int TriggerGroupID);
	void RemoveGroupCollisionActor(class AWRCharacter* Actor, int TriggerGroupID);

	// [2017-1-13 : magedoga] Active Transition
	void RegisterActiveTransition(class WRTriggerTransitionNode_Active* InActiveTransitionNode, class WRTriggerNodeBase* Caller);
	void UnRegisterActiveTransition(class WRTriggerTransitionNode_Active* InActiveTransitionNode, class WRTriggerNodeBase* Caller);
	void RemoveActivedTransitionNodeByCaller(class WRTriggerBaseNode* Caller);

	void RemoveFollowerID(int32 nActorID);
	void SetClientTriggerSystem() { bClientTriggerSystem = true; }


	bool IsValid();
	bool LoadFromJson(const FString& FileName, bool bBuild = true);

	int32 GetAreaID() {	return CurrentAreaID; }

	

#ifdef WITH_EDITOR
	void DrawTriggerShape();
#endif // WITH_EDITOR

	const TArray<class WRTriggerActionNodeBase*>* GetSpawnActionNodes() { return &SpawnActionNodes; }
	const TArray<class WRTriggerNodeBase*>* GetTriggerNodes() { return &TriggerNodes; }
	const TArray<class WRTriggerActionNodeBase*>* GetActionNodes() { return &ActionNodes; }
	const TArray<class WRTriggerTransitionNodeBase*>* GetTransitionNodes() { return &TransitionNodes; }
	const TArray<class WRTriggerSpecialNodeBase*>* GetSpecialNodes() { return &SpecialNodes; }
	const TMap<int32, class WRTriggerNode_MainAreaBox*>* GetMainAreaTriggerNodes() { return &MainAreaTriggerNodes;	}
	
private:
	bool BuildTriggerSystem();
	bool ContainsCollisionActor(class AWRCharacter* InCharacter);
	bool ContainsGroupCollisionActor(class AWRCharacter* InCharacter, int TriggerGroupID);

	void MainTriggerTickProcess(float DeltaTime);
	void AreaTriggerTickProcess(float DeltaTime);
	void GroupTriggerTickProcess();

	void ImmediatelySpawn();

private:
	// [2017-1-12 : magedoga] SystemName (Key)
	FString Name;

	// [2017-1-12 : magedoga] Nodes
	TMap<int32, class WRTriggerBaseNode*> Nodes;
	TArray<class WRTriggerNodeBase*> TriggerNodes;
	TArray<class WRTriggerActionNodeBase*> ActionNodes;
	TArray<class WRTriggerActionNodeBase*> SpawnActionNodes;
	TArray<class WRTriggerTransitionNodeBase*>  TransitionNodes;
	TArray<class WRTriggerSpecialNodeBase*>  SpecialNodes;
	TMap<int32, TArray<class WRTriggerNodeBase*>> GroupTriggerNodes;

	//AreaBox
	TMap<int32, class WRTriggerNode_MainAreaBox*>			MainAreaTriggerNodes;
	TMap<int32, TArray<class WRTriggerNode_SubAreaBox*>>	SubAreaTriggerNodes;

	// [2017-1-12 : magedoga] Apply Collision Actors
	TDoubleLinkedList<class AWRCharacter*>				CollisionActors;
	TMap<int32, TDoubleLinkedList<class AWRCharacter*>*>	GroupCollisionActors;

	// [2017-1-13 : magedoga] Active Transition
	TMap<class WRTriggerBaseNode*, TDoubleLinkedList<class WRTriggerTransitionNode_Active*>*> ActiveTransitionNodes;

	// [ 2020-6-25 : hansang88 ] Loop Transition
	TMap<class WRTriggerBaseNode*, TDoubleLinkedList<class WRTriggerTransitionNode_Loop*>*> LoopTransitionNodes;

	class WRTriggerNodeBase*	StartNode			= nullptr;
	bool						bStartTriggerRun = false;
	
	int32						CurrentAreaID = 0;
	bool						bClientTriggerSystem = false;

	
};