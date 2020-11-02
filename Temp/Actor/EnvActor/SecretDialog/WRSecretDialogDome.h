// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "WRSecretDialogDome.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRSecretDialogDome : public AWREnvActor
{
	GENERATED_BODY()
		virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE void SetGroupIDAndRadius(const CHAT_GROUP_ID_t InGroupID, const float InRadius) { GroupID = InGroupID; DomeRadius = InRadius; }
	void ReserveDestroy(bool InReserve);// { IsReserveEndDialog = InReserve; }
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecretDialogDomeInfo", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* EffectTimeOpenCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecretDialogDomeInfo", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* EffectTimeCloseCurve;

	class UStaticMeshComponent* InStaticMeshComp = nullptr;
	class UMaterialInstanceDynamic * InMaterial = nullptr;
	CHAT_GROUP_ID_t GroupID;
	float DomeRadius = 0.f;

	bool IsReserveEndDialog = false;
	bool IsPlayedSpawnsound = false;

	float CurrentDeltaTime = 0.f;
	float MaxDeltaTime = 5.f;

	TArray<AActor*> OverlapedActorList;
};
