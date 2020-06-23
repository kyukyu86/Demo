// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <Kismet/BlueprintPlatformLibrary.h>

#include "DMGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	static UDMGameInstance*	Instance;

public:

	virtual void Init();
	virtual void Shutdown();

	static UDMGameInstance* GetGameInstance() { return Instance; }


};
