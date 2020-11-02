// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enum/EWRMap.h"
#include "WRGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRGameModeBase : public AGameModeBase
{
	//GENERATED_BODY()
	GENERATED_UCLASS_BODY()

public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
	virtual void StartPlay();

	virtual EWRGameMode GetGameModeType() { ensureMsgf(false, TEXT("Error virtual function called")); return EWRGameMode::Field; }
};
