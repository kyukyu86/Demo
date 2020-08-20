// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameInstance.h"
#include "../Manager/Singleton/DMSingletonManager.h"

UDMGameInstance* UDMGameInstance::Instance = nullptr;

void UDMGameInstance::Init()
{
	Super::Init();

	DMSingletonManager::Start();
	Instance = this;
}

void UDMGameInstance::Shutdown()
{
	DMSingletonManager::ShutDown();


	Super::Shutdown();
}
