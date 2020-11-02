// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WRGameModeBase.h"
#include "WRGameModeTown.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRGameModeTown : public AWRGameModeBase
{
	GENERATED_BODY()

public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	virtual EWRGameMode GetGameModeType() { return EWRGameMode::Town; }
};
