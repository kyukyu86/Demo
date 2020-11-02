// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRGameModeBase.h"
#include "Manager/SingletonClass/WRSingletonManager.h"
#include "Engine/World.h"


AWRGameModeBase::AWRGameModeBase(const FObjectInitializer& ObjectInitializer)
	: AGameModeBase(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("Sprite")))
{
	DefaultPawnClass = nullptr;
// 	PlayerControllerClass = APlayerController::StaticClass();
// 	PlayerStateClass = APlayerState::StaticClass();
// 	GameStateClass = AGameStateBase::StaticClass();
// 	HUDClass = AHUD::StaticClass();
// 	GameSessionClass = AGameSession::StaticClass();
	SpectatorClass = nullptr;
// 	ReplaySpectatorPlayerControllerClass = APlayerController::StaticClass();
// 	ServerStatReplicatorClass = AServerStatReplicator::StaticClass();
}

void AWRGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	WRSingletonManager::LoadLevelComplete();
}

void AWRGameModeBase::StartPlay()
{
	Super::StartPlay();

#if WITH_EDITOR
	UWorld* World = GetWorld();
	if (World->IsValidLowLevel() == true)
	{
		World->GetFirstPlayerController()->bShowMouseCursor = true;
	}
#endif // WITH_EDITOR
}