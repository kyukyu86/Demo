#pragma once

#include "CoreMinimal.h"


class IDMSingleton
{
public:
	virtual void OnInit() = 0;
	virtual void OnShutdown() = 0;	
};

class DMSingletonObject : public IDMSingleton
{
public:
	FORCEINLINE DMSingletonObject() {}
	FORCEINLINE virtual ~DMSingletonObject() {}

	void Init();
	void Shutdown();
	void DestroyInstance();

	virtual void OnDestroyInstance() {}
};

