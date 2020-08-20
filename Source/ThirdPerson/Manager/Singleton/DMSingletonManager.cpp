
#include "DMSingletonManager.h"
#include "DMSingletonObject.h"
#include "../DMAsyncLoadManager.h"
#include "../DMPathManager.h"
#include "../DMPreviewManager.h"
#include "../DMUIManager.h"

TDoubleLinkedList<class DMSingletonObject*> DMSingletonManager::SingletonList;

void DMSingletonManager::Start()
{
	// from : GameInstance::Init

	DECLARE_SINGLETON(DMAsyncLoadManager);
	DECLARE_SINGLETON(DMPathManager);
	DECLARE_SINGLETON(DMPreviewManager);
	DECLARE_SINGLETON(DMUIManager);


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

	TDoubleLinkedList<DMSingletonObject*>::TDoubleLinkedListNode* Node = SingletonList.GetTail();
	for (; Node != nullptr; )
	{
		Node->GetValue()->Shutdown();
		Node = Node->GetPrevNode();
	}	

	DMSingletonManager::DestroySingletons();
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
