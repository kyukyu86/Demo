#pragma once

#include "CoreMinimal.h"
#include "DMSingletonObject.h"
#include "DMSingletonManager.h"

template<class T>
class THIRDPERSON_API DMSingleton : public DMSingletonObject
{
public:
	FORCEINLINE DMSingleton() {}
	FORCEINLINE virtual ~DMSingleton() {}

protected:
	static T* Instance;
	
protected:
	virtual void OnDestroyInstance() override
	{
		delete Instance;
		Instance = nullptr;
	}

public:
	static void CreateInstance()
	{
		Instance = new T;
		DMSingletonManager::Add(Instance);
	}
	static T* Get()
	{
		return Instance;
	}
};

template <class T> T* DMSingleton<T>::Instance = nullptr;

