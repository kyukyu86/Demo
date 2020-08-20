
#include "DMSingletonObject.h"

void DMSingletonObject::Init()
{
	OnInit();
}

void DMSingletonObject::Shutdown()
{
	OnShutdown();
}

void DMSingletonObject::DestroyInstance()
{
	OnDestroyInstance();
}
