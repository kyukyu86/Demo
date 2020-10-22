// Fill out your copyright notice in the Description page of Project Settings.


#include "DMInputManager.h"


DMInputManager::DMInputManager()
{
	
}

DMInputManager::~DMInputManager()
{

}

void DMInputManager::OnInit()
{

}

void DMInputManager::OnShutdown()
{
	
}

bool DMInputManager::OnTick(float InDeltaTime)
{	
	

	return true;
}

FDMInputDelegate& DMInputManager::GetInputDelegate(const EDMInput IN InInputType)
{
	FDMInputDelegate& FoundInputDelegate = InputDelegateList.FindOrAdd(InInputType);
	return FoundInputDelegate;
}

void DMInputManager::UnBindInputEvent(const EDMInput IN InInputType, FDelegateHandle& IN InDelegateHandle)
{
	FDMInputDelegate* FoundInputDelegate = InputDelegateList.Find(InInputType);
	if (FoundInputDelegate)
	{
		FoundInputDelegate->Remove(InDelegateHandle);
	}
}
