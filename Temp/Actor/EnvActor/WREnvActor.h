// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Character/WRCharacter.h"
#include "Network/Share/Define/Define_Type.h"
#include "Define/WRTypeDefine.h"
#include "WREnvActor.generated.h"

UCLASS()
class WR_API AWREnvActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWREnvActor();
	AWREnvActor(EWREnvType InEnvType);

	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	AID_t AID = INVALID_AID;
	WRTableID TID = INVALID_TABLE_TID;
	AID_t OwnerAID = INVALID_AID;
	EWREnvType EnvType = EWREnvType::None;

public:
	FORCEINLINE void SetTID(const WRTableID& InTID) { TID = InTID; }
	FORCEINLINE WRTableID GetTID() { return TID; }

	FORCEINLINE void SetAID(const AID_t IN InAID) { AID = InAID; }
	FORCEINLINE AID_t GetAID() { return AID; }

	FORCEINLINE EWREnvType GetEnvType() { return EnvType; }
	
	FORCEINLINE void SetMyOwner(AWRCharacter* InOwnerActor) { OwnerAID = InOwnerActor->GetActorID(); }
	FORCEINLINE AID_t& GetMyOwner() { return OwnerAID; }
	
	FORCEINLINE virtual bool IsVehicle() { return false; }

public:
	virtual void BeginInteraction() {};
};
