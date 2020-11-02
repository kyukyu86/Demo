// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WRGameModeBase.h"
#include "WRGameModeStart.generated.h"


#define DEF_HMD_GUIDE_ON_FOR_PROTOTYPE
/**
 * 
 */
UCLASS()
class WR_API AWRGameModeStart : public AWRGameModeBase
{
	GENERATED_BODY()

public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	virtual EWRGameMode GetGameModeType() { return EWRGameMode::Lab; }

	// [2020-06-10 LJH HMD Guide용 정식 런칭 후 없어질 예정]
private:
	void InitializeHmdGuide();
};
