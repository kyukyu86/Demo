// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvVehicleBase.h"
#include "WRFloatingBoard.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRFloatingBoard : public AWREnvVehicleBase
{
	GENERATED_BODY()
		
public:
	AWRFloatingBoard();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void MakeDebugString(TArray<FString>& OUT OutString);
	FORCEINLINE virtual bool IsUseOverlapEventControl() override { return false; }							// override AWREnvVehicleBase
	virtual void OnChangeState(const struct pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform) override;	// override AWREnvVehicleBase

public:
	void OnSpawnComplete(const struct pt_Game_Actor::Inf_FloatingBoardAppear_G2U& IN InInform);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		float PushOutValue = 100.f;

};
