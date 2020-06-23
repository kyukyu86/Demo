// Fill out your copyright notice in the Description page of Project Settings.


#include "DMGameInstance.h"

UDMGameInstance* UDMGameInstance::Instance = nullptr;

void UDMGameInstance::Init()
{
	Super::Init();

	Instance = this;
}

void UDMGameInstance::Shutdown()
{



	Super::Shutdown();
}
