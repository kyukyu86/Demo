
#include "DMSingletonManager.h"
#include "DMSingletonObject.h"
#include "../DMAsyncLoadManager.h"
#include "../DMPathManager.h"
#include "../DMPreviewManager.h"
#include "../DMUIManager.h"
#include "../DMCharacterManager.h"
#include "../DMFSMManager.h"
#include "../DMTargetingManager.h"
#include "../DMDebugManager.h"
#include "../DMInputManager.h"

TDoubleLinkedList<class DMSingletonObject*> DMSingletonManager::SingletonList;
FDelegateHandle DMSingletonManager::TickHandle;

void DMSingletonManager::Start()
{
	// from : GameInstance::Init

	TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateStatic(DMSingletonManager::Tick));

	DECLARE_SINGLETON(DMAsyncLoadManager);
	DECLARE_SINGLETON(DMPathManager);
	DECLARE_SINGLETON(DMPreviewManager);
	DECLARE_SINGLETON(DMUIManager);
	DECLARE_SINGLETON(DMCharacterManager);
	DECLARE_SINGLETON(DMFSMManager);
	DECLARE_SINGLETON(DMInputManager);
	DECLARE_SINGLETON(DMTargetingManager);
	DECLARE_SINGLETON(DMDebugManager);


	TDoubleLinkedList<DMSingletonObject*>::TDoubleLinkedListNode* Node = SingletonList.GetTail();
	for (; Node != nullptr; )
	{
		Node->GetValue()->Init();
		Node = Node->GetPrevNode();
	}
}

void DMSingletonManager::ShutDown()
{
	// from : GameInstance::Shutdown

	if (TickHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(TickHandle);
	}

	TDoubleLinkedList<DMSingletonObject*>::TDoubleLinkedListNode* Node = SingletonList.GetTail();
	for (; Node != nullptr; )
	{
		Node->GetValue()->Shutdown();
		Node = Node->GetPrevNode();
	}	

	DMSingletonManager::DestroySingletons();
}

bool DMSingletonManager::Tick(float InDeltaTime)
{
	TDoubleLinkedList<DMSingletonObject*>::TDoubleLinkedListNode* Node = SingletonList.GetTail();
	for (; Node != nullptr; )
	{
		Node->GetValue()->OnTick(InDeltaTime);
		Node = Node->GetPrevNode();
	}

	return true;
}

void DMSingletonManager::LoadLevelStart()
{

}

void DMSingletonManager::LoadLevelComplete()
{

}

void DMSingletonManager::Add(class DMSingletonObject* IN InSingletonObject)
{
	if (SingletonList.Contains(InSingletonObject) == true)
		return;

	SingletonList.AddTail(InSingletonObject);
}

void DMSingletonManager::DestroySingletons()
{
	TDoubleLinkedList<DMSingletonObject*>::TDoubleLinkedListNode* Node = SingletonList.GetTail();
	for (; Node != nullptr; )
	{
		Node->GetValue()->DestroyInstance();
		Node = Node->GetPrevNode();
	}
	SingletonList.Empty();
}
