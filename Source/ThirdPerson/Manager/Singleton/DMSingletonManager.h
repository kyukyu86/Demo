#pragma once

#include "CoreMinimal.h"
#include "Containers/List.h"


#define DECLARE_SINGLETON(T)	\
T::CreateInstance();			\

class DMSingletonManager
{
public:
	static void Start();

	static void ShutDown();

	static void LoadLevelStart();

	static void LoadLevelComplete();
	
	static void Add(class DMSingletonObject* IN InSingletonObject);

	static void DestroySingletons();

private:
	static TDoubleLinkedList<class DMSingletonObject*> SingletonList;
};