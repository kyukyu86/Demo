#include "WRTriggerSystem.h"
#include "Helper/WRTriggerHelper.h"
#include "Enum/EWRTriggerEnum.h"

#include "Node/Trigger/WRTriggerNode_MainAreaBox.h"
#include "Node/Special/WRTriggerSpecialNodeBase.h"
#include "Node/Trigger/WRTriggerNodeBase.h"

#include "Manager/WRTriggerManager.h"
#include <JsonSerializer.h>
#include "MessageDialog.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Network/IMMessageSender.h"


WRTriggerSystem::WRTriggerSystem(const FString& InName)
	: Name(InName)
{
}

WRTriggerSystem::~WRTriggerSystem()
{
	CollisionActors.Empty();

	for (auto It = Nodes.CreateIterator(); It; ++It)
	{
		delete It.Value();
	}

	Nodes.Empty();

	for (auto It = GroupTriggerNodes.CreateIterator(); It; ++It)
	{
		It.Value().Empty();
	}

	GroupTriggerNodes.Empty();

	TriggerNodes.Empty();
	TransitionNodes.Empty();
	ActionNodes.Empty();
	MainAreaTriggerNodes.Empty();
	SubAreaTriggerNodes.Empty();
	LoopTransitionNodes.Empty();

	for (auto It = ActiveTransitionNodes.CreateIterator(); It; ++It)
	{
		It.Value()->Empty();
		delete It.Value();
	}

	ActiveTransitionNodes.Empty();

	for (auto It = GroupCollisionActors.CreateIterator(); It; ++It)
	{
		It.Value()->Empty();
		delete It.Value();
	}

	GroupCollisionActors.Empty();

	StartNode = nullptr;
}

void WRTriggerSystem::Tick(float DeltaTime)
{
	if (bClientTriggerSystem)
	{
		MainTriggerTickProcess(DeltaTime);
		AreaTriggerTickProcess(DeltaTime);
		GroupTriggerTickProcess();
	}
}


// bool WRTriggerSystem::CallTriggerNode(const int32 TriggerNodeUid, EWRTriggerNodeType::en TransitionType)
// {
// 	if (Nodes.Contains(TriggerNodeUid) == false)
// 		return false;
// 
// 	WRTriggerBaseNode* FoundNode = Nodes[TriggerNodeUid];
// 	if (FoundNode == nullptr || EWRTriggerNodeType::IsTriggerNode(FoundNode->GetNodeType()) == false)
// 		return false;
// 
// 	if (EWRTriggerNodeType::IsTransitionNode(TransitionType) == false)
// 		return false;
// 
// 	WRTriggerNodeBase* FoundTriggerNode = (WRTriggerNodeBase*)FoundNode;
// 	switch (TransitionType)
// 	{
// 		case EWRTriggerNodeType::Transition_Active:		{ FoundTriggerNode->OnActive(); } break;
// 		case EWRTriggerNodeType::Transition_Deactive:	{ FoundTriggerNode->OnDeactive(); } break;
// 		case EWRTriggerNodeType::Transition_Enter:		{ FoundTriggerNode->OnEnter(); } break;
// 		case EWRTriggerNodeType::Transition_Exit:		{ FoundTriggerNode->OnExit(); } break;
// 		default: { return false; }
// 	}
// 
// 	return true;
// }

class WRTriggerBaseNode* WRTriggerSystem::GetNodeByUid(const int32& InUid)
{
	if (Nodes.Contains(InUid) == false)
		return nullptr;

	return Nodes[InUid];
}



bool WRTriggerSystem::IsValid()
{
	// [2017-1-11 : magedoga] 노드 개수 체크
// 	if (ActionNodes.Num() == 0 || TransitionNodes.Num() == 0 || TriggerNodes.Num() == 0)
// 		return false;
	if (Nodes.Num() == 0)
	{
		FString Message = FString::Printf(TEXT("Nodes.Num() == 0"));
		FText Title = FText::FromString(TEXT("New Trigger Load Error"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message), &Title);
		return false;
	}

	// [2017-1-11 : magedoga] TransitionNode에 링크 된 노드들 체크
	for (int32 i = 0; i < TransitionNodes.Num(); ++i)
	{
		// [2017-1-11 : magedoga] InPin
		int32 TempUid = TransitionNodes[i]->GetInPinNodeUid();
		WRTriggerBaseNode* BaseNode = this->GetNodeByUid(TempUid);
		if (BaseNode == nullptr)
		{
			FString Message = FString::Printf(TEXT("TriggerNode that do not exist : tid => %d"), TempUid);
			FText Title = FText::FromString(TEXT("New Trigger Load Error"));
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message), &Title);
			return false;
		}
		
		// [2017-1-11 : magedoga] OutPin
// 		const TArray<int32>& OutPinNodesUid = TransitionNodes[i]->GetOutPinNodeUid();
// 		if (OutPinNodesUid.Num() == 0)
// 			return false;
// 
// 		TempUid = -1;
// 		for (int32 j = 0; j < OutPinNodesUid.Num(); ++j)
// 		{
// 			TempUid = OutPinNodesUid[j];
// 			BaseNode = this->GetNodeByUid(TempUid);
// 			if (BaseNode == nullptr)
// 				return false;
// 		}
	}
	
	return true;
}

bool WRTriggerSystem::LoadFromJson(const FString& FileName, bool bBuild )
{
	TSharedRef<TJsonReader<TCHAR>> Reader = WRTriggerParseHelper::CreateJsonReader(FileName);

	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) == false)
	{
		FString Message = FString::Printf(TEXT("JsonSerializer Fail => FileName : (%s)"), *FileName);
		FText Title = FText::FromString(TEXT("New Trigger Load Error"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message), &Title);
		return false;
	}
		

	TArray<WRTriggerBaseNode*> CreatedNodes;

	// [2017-1-10 : magedoga] Create Node And Fill Value
	auto ParseAndCreateNode = [&JsonObject, &CreatedNodes](const FString& FieldName)
	{
		int32 Count = 0;
		while (true)
		{
			// [2017-1-5 : magedoga] GetNode Info From Json
			const TSharedPtr<FJsonObject>* NodeObject = nullptr;
			//if (WRTriggerParseHelper::TryGetNodeFromJsonObject(JsonObject, FieldName + FString::FromInt(Count++), NodeObject) == false)
			FString ResultFieldName = FieldName + FString::FromInt(Count++);
			JsonObject->TryGetObjectField(ResultFieldName, NodeObject);
			if(NodeObject == nullptr)
				break;

			// [2017-1-5 : magedoga] CreateNode
			WRTriggerBaseNode* BaseNode = WRTriggerNodeFactory::CreateNodeFromJsonObject(NodeObject);
			if (BaseNode == nullptr)
				continue;

			CreatedNodes.Add(BaseNode);
		}
	};

	ParseAndCreateNode("TriggerNode");
	ParseAndCreateNode("TransitionNode");
	ParseAndCreateNode("SpecialNode");
	ParseAndCreateNode("ActionNode");
	ParseAndCreateNode("SpawnAction");


	if (CreatedNodes.Num() == 0)
	{
// 		FString Message = FString::Printf(TEXT("CreatedNodes.Num == 0, => FileName : (%s)"), *FileName);
// 		FText Title = FText::FromString(TEXT("New Trigger Load Error"));
// 		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message), &Title);
		return false;
	}


	for (int32 i = 0; i < CreatedNodes.Num(); ++i)
	{
		WRTriggerBaseNode* BaseNode = CreatedNodes[i];

		BaseNode->SetTriggerSystem(this);

		// [2017-1-10 : magedoga] Divide Nodes
		// NodeType
		if (EWRTriggerNodeType::IsActionNode(BaseNode->GetNodeType()) == true)
		{
			if(EWRTriggerNodeType::IsSpawnActionNode(BaseNode->GetNodeType()) == true)
				SpawnActionNodes.Add((WRTriggerActionNodeBase*)BaseNode);
			else
				ActionNodes.Add((WRTriggerActionNodeBase*)BaseNode);

		}
		else if (EWRTriggerNodeType::IsTransitionNode(BaseNode->GetNodeType()) == true)
		{
			TransitionNodes.Add((WRTriggerTransitionNodeBase*)BaseNode);
			if (BaseNode->GetNodeType() == EWRTriggerNodeType::Transition_Loop)
			{
				LoopTransitionNodes.Add((WRTriggerTransitionNode_Loop*)BaseNode);
			}
		}
		else if(EWRTriggerNodeType::IsTriggerNode(BaseNode->GetNodeType()) == true)
		{
			WRTriggerNodeBase* NodeBase = (WRTriggerNodeBase*)BaseNode;

			if (NodeBase->GetGroupID() != 0)
			{
				if (GroupTriggerNodes.Contains(NodeBase->GetGroupID()))
				{
					GroupTriggerNodes[NodeBase->GetGroupID()].Add(NodeBase);
				}
				else
				{
					TArray<class WRTriggerNodeBase*> TempArray;
					TempArray.Add(NodeBase);
					GroupTriggerNodes.Add(NodeBase->GetGroupID(), TempArray);
				}
			}
			else
			{
				if (BaseNode->GetNodeType() == EWRTriggerNodeType::Trigger_MainAreaBox)
				{
					WRTriggerNode_MainAreaBox* MainAreaBoxNode = static_cast<WRTriggerNode_MainAreaBox*>(BaseNode);
					if (MainAreaBoxNode)
					{
						MainAreaTriggerNodes.Add(MainAreaBoxNode->GetAreaGroupID(), MainAreaBoxNode);
					}
				}
				else if (BaseNode->GetNodeType() == EWRTriggerNodeType::Trigger_SubAreaBox)
				{
					WRTriggerNode_SubAreaBox* SubAreaBoxNode = static_cast<WRTriggerNode_SubAreaBox*>(BaseNode);
					if (SubAreaBoxNode)
					{
						int32 AreaGroupID = SubAreaBoxNode->GetAreaGroupID();
						if (SubAreaTriggerNodes.Contains(AreaGroupID))
						{
							SubAreaTriggerNodes[AreaGroupID].Add(SubAreaBoxNode);
						}
						else
						{
							TArray<WRTriggerNode_SubAreaBox*> TempArray;
							TempArray.Add(SubAreaBoxNode);
							SubAreaTriggerNodes.Add(AreaGroupID, TempArray);
						}
					}
				}
				else
				{
					if (NodeBase->IsStartTrigger())
						StartNode = NodeBase;

					TriggerNodes.Add(NodeBase);
				}
			}
		}
		else
		{
			SpecialNodes.Add((WRTriggerSpecialNodeBase*)BaseNode);
		}

		
		// Uid
		if (Nodes.Contains(BaseNode->GetUid()) == true)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerSystem] Contains Uid Already [ %d ]"), (int32)BaseNode->GetUid());
			continue;
		}

		Nodes.Add(BaseNode->GetUid(), BaseNode);
	}

	if(bBuild)
		return this->BuildTriggerSystem();

	return true;
}

bool WRTriggerSystem::BuildTriggerSystem()
{
	if (this->IsValid() == false)
		return false;

	// [2017-1-11 : magedoga] Transition InPin의 Node들에게 TransitionNode 세팅
	for (int32 i = 0; i < TransitionNodes.Num(); ++i)
	{
		WRTriggerTransitionNodeBase* TransitionNode = TransitionNodes[i];
		if(TransitionNode == nullptr)
			continue;

		int32 InPinUid = TransitionNode->GetInPinNodeUid();
		WRTriggerBaseNode** BaseNode = Nodes.Find(InPinUid);
 		EWRTriggerNodeType::en NodeType = (*BaseNode)->GetNodeType();

		switch (TransitionNode->GetNodeType())
		{
			case EWRTriggerNodeType::Transition_Active:
			{
				((WRTriggerNodeBase*)(*BaseNode))->SetTransition(EWRTriggerTransitionType::Active, TransitionNode);
			}break;
			case EWRTriggerNodeType::Transition_Deactive:
			{
				((WRTriggerNodeBase*)(*BaseNode))->SetTransition(EWRTriggerTransitionType::DeActive, TransitionNode);
			}break;
			case EWRTriggerNodeType::Transition_Enter:
			{
				((WRTriggerNodeBase*)(*BaseNode))->SetTransition(EWRTriggerTransitionType::Enter, TransitionNode);
			}break;
			case EWRTriggerNodeType::Transition_Exit:
			{
				((WRTriggerNodeBase*)(*BaseNode))->SetTransition(EWRTriggerTransitionType::Exit, TransitionNode);
			}break;
			case EWRTriggerNodeType::Transition_Loop:
			{
				((WRTriggerNodeBase*)(*BaseNode))->SetTransition(EWRTriggerTransitionType::Loop, TransitionNode);
			}break;
			case EWRTriggerNodeType::Transition_Etc:
			{
				if (EWRTriggerNodeType::IsTriggerNode((*BaseNode)->GetNodeType()))
				{
					((WRTriggerNodeBase*)(*BaseNode))->SetTransition(EWRTriggerTransitionType::Etc, TransitionNode);
				}
				else // Special
				{
					((WRTriggerSpecialNodeBase*)(*BaseNode))->SetTransition(EWRTriggerTransitionType::Etc, TransitionNode);
				}
			} break;
			default:
			{
				FString Message = FString::Printf(TEXT("UnKnown TransitionNodeType"));
				FText Title = FText::FromString(TEXT("New Trigger Load Error"));
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message), &Title);
				UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerSystem] BuildTriggerSystem() Error NodeType (%d)"), (int32)NodeType);
				return false;
			}
		}
	}
	
	
	this->SetCollisionActor(WRCharacterManager::Get()->GetMyCharacter());

	for (int32 i = 0; i < ActionNodes.Num(); ++i)
	{
		ActionNodes[i]->CreateActionProcessor();
	}

	for (int32 i = 0; i < SpawnActionNodes.Num(); ++i)
	{
		SpawnActionNodes[i]->CreateActionProcessor();
	}

	return true;
}

bool WRTriggerSystem::ContainsCollisionActor(class AWRCharacter* InCharacter)
{
	for (auto It : CollisionActors)
	{
		if (It == InCharacter)
			return true;
	}

	return false;
}

bool WRTriggerSystem::ContainsGroupCollisionActor(class AWRCharacter* InCharacter, int TriggerGroupID)
{
 	if (GroupCollisionActors.Contains(TriggerGroupID))
 	{
 		for (auto It : *(GroupCollisionActors[TriggerGroupID]))
 		{
 			if (It == InCharacter)
 				return true;
 		}
 	}
 		
 	return false;
}

void WRTriggerSystem::MainTriggerTickProcess(float DeltaTime)
{
	if (bStartTriggerRun == false)
	{
		if (StartNode)
			StartNode->OnActive(0);

		ImmediatelySpawn();
		StartNode = nullptr;
		bStartTriggerRun = true;
		WRTriggerManager::Get()->SetStartTriggerRun(bStartTriggerRun);
	}
	else
	{
		// 시퀀스 재생중에는 트리거 발동 안되도록

		AWRCharacter* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		// [2017-1-12 : magedoga] 충돌검사
		for (auto It : CollisionActors)
		{
			for (int32 i = 0; i < TriggerNodes.Num(); ++i)
			{
				if (TriggerNodes[i]->GetEnable() == false)
					continue;

				if (TriggerNodes[i]->IsEnteringActor(It) == false && TriggerNodes[i]->IsIntersect(It) == true)
				{
					if (TriggerNodes[i]->IsContainsTransition(EWRTriggerTransitionType::Enter))
					{
						if (TriggerNodes[i]->IsClientOnlyData())
						{
							TriggerNodes[i]->OnEnter(It->GetActorID());
							if (TriggerNodes[i]->GetSaveNumber() != 0)
							{
								WRTriggerManager::Get()->SaveUsedTriggerData(TriggerNodes[i]->GetSaveNumber());
							}
						}
						else
						{
							IMMessageSender::RequestTriggerEvent(TriggerNodes[i]->GetUid(), EWRTriggerNodeType::Transition_Enter, MyChar->GetActorLocation(), MyChar->GetActorRotation().Yaw);
						}
					}
				}
				else if (TriggerNodes[i]->IsEnteringActor(It) == true && TriggerNodes[i]->IsIntersect(It) == false)
				{
					if (TriggerNodes[i]->IsSelfDisable())
						TriggerNodes[i]->OnDisable();

					if (TriggerNodes[i]->IsContainsTransition(EWRTriggerTransitionType::Exit))
					{
						if (TriggerNodes[i]->IsClientOnlyData())
							TriggerNodes[i]->OnExit(It->GetActorID());
						else
						{
							IMMessageSender::RequestTriggerEvent(TriggerNodes[i]->GetUid(), EWRTriggerNodeType::Transition_Exit, MyChar->GetActorLocation(), MyChar->GetActorRotation().Yaw);
						}
					}
				}
			}
		}
		
		// [2017-1-16 : magedoga] Active Update
		if (ActiveTransitionNodes.Num() > 0)
		{
			for (auto It = ActiveTransitionNodes.CreateIterator(); It; ++It)
			{
				TArray<WRTriggerTransitionNode_Active*> RemoveNodes;
				TDoubleLinkedList<WRTriggerTransitionNode_Active*>* ActiveNodes = It.Value();
				WRTriggerNodeBase* CallFrom = static_cast<WRTriggerNodeBase*>(It.Key());
				for (auto Node : (*ActiveNodes))
				{
					// 마지막 active 실행 1 tick 후에 삭제 및 Deactive 시킨다.
					if (Node->UpdateActive(DeltaTime, CallFrom) == false)
					{
						// 해당 트리거 박스의 Etc 를 찾아서 
						// Deactive 를 호출 해줘야함.
						int InPinUid = Node->GetInPinNodeUid();
						WRTriggerBaseNode** BaseNode = Nodes.Find(InPinUid);
						//EWRTriggerNodeType::en NodeType = (*BaseNode)->GetNodeType();
						((WRTriggerNodeBase*)(*BaseNode))->OnDeactive();

						if (((WRTriggerNodeBase*)(*BaseNode))->GetSaveNumber() != 0)
						{
							WRTriggerManager::Get()->SaveUsedTriggerData(((WRTriggerNodeBase*)(*BaseNode))->GetSaveNumber());
						}

						RemoveNodes.Add(Node);
					}
				}

				for (int32 i = 0; i < RemoveNodes.Num(); ++i)
				{
					ActiveNodes->RemoveNode(RemoveNodes[i]);
				}
			}
		}

		if (LoopTransitionNodes.Num() > 0)
		{
			for (auto It = LoopTransitionNodes.CreateIterator(); It; ++It)
			{
				TDoubleLinkedList<WRTriggerTransitionNode_Loop*>* LoopNodes = It.Value();
				WRTriggerNodeBase* CallFrom = static_cast<WRTriggerNodeBase*>(It.Key());
				for (auto Node : (*LoopNodes))
				{
					Node->UpdateLoop(DeltaTime, CallFrom);
				}
			}
		}
	}
}

void WRTriggerSystem::AreaTriggerTickProcess(float DeltaTime)
{
	if(bStartTriggerRun)
	{
		AWRCharacter* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		for (auto MainAreaNodeIter : MainAreaTriggerNodes)
		{
			WRTriggerNode_MainAreaBox* MainAreaNode = MainAreaNodeIter.Value;
			if (MainAreaNode->GetEnable() == false)
				continue;

			if (MainAreaNode->IsEnteringActor(MyChar) == false && MainAreaNode->IsIntersect(MyChar) == true)
			{
				if (CurrentAreaID != MainAreaNodeIter.Key && MainAreaNode->IsContainsTransition(EWRTriggerTransitionType::Enter))
				{
					MainAreaNode->OnEnter(MyChar->GetActorID());
					CurrentAreaID = MainAreaNodeIter.Key;
				}
			}
			else if (MainAreaNode->IsEnteringActor(MyChar) == true && MainAreaNode->IsIntersect(MyChar) == false)
			{
				if (MainAreaNode->IsSelfDisable())
					MainAreaNode->OnDisable();
			}
		}

		for (auto SubAreaNodeIter : SubAreaTriggerNodes)
		{
			TArray<class WRTriggerNode_SubAreaBox*> SubAreaNodes = SubAreaNodeIter.Value;
			for (int i = 0; i < SubAreaNodes.Num(); ++i)
			{
				if (SubAreaNodes[i]->GetEnable() == false)
					continue;

				if (SubAreaNodes[i]->IsEnteringActor(MyChar) == false && SubAreaNodes[i]->IsIntersect(MyChar) == true)
				{
					if (CurrentAreaID != SubAreaNodeIter.Key)
					{
						WRTriggerNode_MainAreaBox** MainAreaNode = MainAreaTriggerNodes.Find(SubAreaNodeIter.Key);
						if(MainAreaNode)
							(*MainAreaNode)->OnEnter(MyChar->GetActorID());

						CurrentAreaID = SubAreaNodeIter.Key;
					}
				}
				else if (SubAreaNodes[i]->IsEnteringActor(MyChar) == true && SubAreaNodes[i]->IsIntersect(MyChar) == false)
				{
					if (SubAreaNodes[i]->IsSelfDisable())
						SubAreaNodes[i]->OnDisable();
				}
			}
		}
	}
}

void WRTriggerSystem::GroupTriggerTickProcess()
{
	for (auto It = GroupCollisionActors.CreateIterator(); It; ++It)
	{
		int& Key = It.Key();
		if (GroupTriggerNodes.Contains(Key) == false)
			continue;

		const TArray<class WRTriggerNodeBase*>& IMGroupTriggerNodes = GroupTriggerNodes[Key];
		TDoubleLinkedList<class AWRCharacter*>* CollisionActorsTemp = It.Value();
		for (auto ActorIt : *(CollisionActorsTemp))
		{
			for (int32 i = 0; i < IMGroupTriggerNodes.Num(); ++i)
			{
				if (IMGroupTriggerNodes[i]->GetEnable() == false)
					continue;

				if (IMGroupTriggerNodes[i]->IsEnteringActor(ActorIt) == false && IMGroupTriggerNodes[i]->IsIntersect(ActorIt) == true)
				{
					if (IMGroupTriggerNodes[i]->IsContainsTransition(EWRTriggerTransitionType::Enter))
					{
						IMGroupTriggerNodes[i]->OnEnter(ActorIt->GetActorID());
					}
				}
				else if (IMGroupTriggerNodes[i]->IsEnteringActor(ActorIt) == true && IMGroupTriggerNodes[i]->IsIntersect(ActorIt) == false)
				{
					if (IMGroupTriggerNodes[i]->IsSelfDisable())
						IMGroupTriggerNodes[i]->OnDisable();

					if (IMGroupTriggerNodes[i]->IsContainsTransition(EWRTriggerTransitionType::Exit))
					{
						IMGroupTriggerNodes[i]->OnExit(ActorIt->GetActorID());
					}
				}
			}
		}
	}
}

void WRTriggerSystem::ImmediatelySpawn()
{
	for (int i = 0; i < SpawnActionNodes.Num(); ++i)
	{
		bool ImmediatelySpawn = false;
		if (SpawnActionNodes[i]->TryGetBoolField("immediately_spawn", ImmediatelySpawn))
		{
			if (ImmediatelySpawn)
			{
				SpawnActionNodes[i]->ProcessAction(nullptr);
			}
		}
	}
}

void WRTriggerSystem::SetCollisionActor(class AWRCharacter* Actor)
{
	if (this->ContainsCollisionActor(Actor) == true)
		return;

	CollisionActors.AddTail(Actor);
}

void WRTriggerSystem::RemoveCollisionActor(class AWRCharacter* Actor)
{
	CollisionActors.RemoveNode(Actor);
}


void WRTriggerSystem::SetGroupCollisionActor(class AWRCharacter* Actor, int TriggerGroupID)
{
	if (this->ContainsGroupCollisionActor(Actor, TriggerGroupID) == true)
		return;

	if (GroupCollisionActors.Contains(TriggerGroupID) == false)
	{
		GroupCollisionActors.Add(TriggerGroupID, new TDoubleLinkedList<class AWRCharacter*>());
	}

	GroupCollisionActors[TriggerGroupID]->AddTail(Actor);
}

void WRTriggerSystem::RemoveGroupCollisionActor(class AWRCharacter* Actor, int TriggerGroupID)
{
	if (GroupCollisionActors.Contains(TriggerGroupID))
	{
		GroupCollisionActors[TriggerGroupID]->RemoveNode(Actor);
		if (GroupCollisionActors[TriggerGroupID]->Num() == 0)
		{
			delete GroupCollisionActors[TriggerGroupID];
			GroupCollisionActors.Remove(TriggerGroupID);
		}
	}
}

void WRTriggerSystem::RegisterActiveTransition(class WRTriggerTransitionNode_Active* InActiveTransitionNode, class WRTriggerNodeBase* Caller)
{
	if (ActiveTransitionNodes.Contains(InActiveTransitionNode) == false)
	{
		ActiveTransitionNodes.Add(Caller, new TDoubleLinkedList<WRTriggerTransitionNode_Active*>());
	}

	TDoubleLinkedList<WRTriggerTransitionNode_Active*>* ActiveNodes = ActiveTransitionNodes[Caller];
	if (ActiveNodes == nullptr)
		return;

	for (auto It : (*ActiveNodes))
	{
		if (It == InActiveTransitionNode)
			return;
	}

	ActiveNodes->AddTail(InActiveTransitionNode);
}


void WRTriggerSystem::UnRegisterActiveTransition(class WRTriggerTransitionNode_Active* InActiveTransitionNode, class WRTriggerNodeBase* Caller)
{
	if (ActiveTransitionNodes.Contains(Caller) == false)
		return;

	TDoubleLinkedList<WRTriggerTransitionNode_Active*>* ActiveNodes = ActiveTransitionNodes[Caller];
	if (ActiveNodes == nullptr)
		return;

	ActiveNodes->RemoveNode(InActiveTransitionNode);
}

void WRTriggerSystem::RemoveActivedTransitionNodeByCaller(class WRTriggerBaseNode* Caller)
{
	if (ActiveTransitionNodes.Contains(Caller))
		ActiveTransitionNodes.Remove(Caller);
}

void WRTriggerSystem::RemoveFollowerID(int32 nActorID)
{
	for (int32 i = 0; i < SpecialNodes.Num(); ++i)
	{
		WRTriggerSpecialNodeBase* SpecialNode = SpecialNodes[i];
		if (SpecialNode)
		{
			if(SpecialNode->GetNodeType() != EWRTriggerNodeType::Special_Watch)
				continue;

			((WRTriggerSpecialNode_Watch*)SpecialNode)->RemoveFollowerID(nActorID);
		}
	}
}

#ifdef WITH_EDITOR
void WRTriggerSystem::DrawTriggerShape()
{
	/*TEXT("-1 : Draw All")
	TEXT("0 : Draw off")
	TEXT("1 : Draw Basic Trigger")
	TEXT("2 : Draw Server Trigger")
	TEXT("3 : Draw Group Trigger")
	TEXT("4 : Draw Arae Trigger"),*/
	// [2017-1-12 : magedoga] Trigger

	// [ 2019-9-23 : hansang88 ] 상현임시 주석처리
	int DrawValue = 0;//IMConsoleManager::Get()->GetInt(EIMConsole::Debug_Draw_Trigger);

	if (DrawValue == -1 || DrawValue == 1 || DrawValue == 2)
	{
		for (int32 i = 0; i < TriggerNodes.Num(); ++i)
		{
			if (DrawValue != 2 || TriggerNodes[i]->IsClientOnlyData() == false)
			{
				TriggerNodes[i]->DrawTriggerShape();
			}
		}
	}
	
	if (DrawValue == -1 || DrawValue == 3)
	{
		for (auto It = GroupTriggerNodes.CreateIterator(); It; ++It)
		{
			TArray<class WRTriggerNodeBase*>& GroupTriggers = It.Value();
			for (int i = 0; i < GroupTriggers.Num(); ++i)
			{
				GroupTriggers[i]->DrawTriggerShape();
			}
		}
	}

	if (DrawValue == -1 || DrawValue == 4)
	{
		for (auto MainAreaNodeIter : MainAreaTriggerNodes)
		{
			WRTriggerNode_MainAreaBox* MainAreaNode = MainAreaNodeIter.Value;
			MainAreaNode->DrawTriggerShape();
		}

		for (auto SubAreaNodeIter : SubAreaTriggerNodes)
		{
			TArray<class WRTriggerNode_SubAreaBox*> SubAreaNodes = SubAreaNodeIter.Value;
			for (int i = 0; i < SubAreaNodes.Num(); ++i)
			{
				SubAreaNodes[i]->DrawTriggerShape();
			}
		}
	}
}
#endif // WITH_EDITOR
