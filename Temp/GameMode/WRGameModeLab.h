// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WRGameModeBase.h"
#include "WRGameModeLab.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRGameModeLab : public AWRGameModeBase
{
	GENERATED_BODY()

public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;
	virtual void StartToLeaveMap() override;

	virtual EWRGameMode GetGameModeType() { return EWRGameMode::Lab; }

public:
	void OnPressed(const uint32 InPSN);

private:
	UFUNCTION()
	void SpawnMaintainVFX();
	UFUNCTION()
	void OnAppearDirectionCompleted(class UNiagaraComponent* InComponent);
	UFUNCTION()
	void OnDisappearDirectionCompleted(class UNiagaraComponent* InComponent);

	class AWRVFX* CreateVFX(const FString& InPath);

private:
	FString Hash;

	uint32 VFXUID = 0;
	uint32 PSN = 0;
};
