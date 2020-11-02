// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Enum/EWREnvObject.h"
#include "Struct/WREnvStruct.h"
#include "WRSecretDialogPortal.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRSecretDialogPortal : public AWREnvActor
{
	GENERATED_BODY()
		virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;
	
	void OnCompleteSpawn(const FWRSecretDialogInfo& InAreaInfo);
	void StartSecretDialog();
private:
	void PortalProcess(float DeltaTime);

	void SeedProcess(float DeltaTime);
	void OpeningProcess(float DeltaTime);
	void OpenedProcess(float DeltaTime);
	void ClosingProcess(float DeltaTime);
	void ShowUIUpdate();
	void ShowOnTargetUI();
	void ShowOffTargetUI();
public:
	void BeginDestroyPortal();
	
	FORCEINLINE void SetPortalState(const EWRSecretDialogPortalStateType InPortalState) { CurrentState = InPortalState; }
	FORCEINLINE const EWRSecretDialogPortalStateType& GetPortalState() { return CurrentState; }
	UFUNCTION()
	void OnBeginOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	FORCEINLINE CHAT_GROUP_ID_t GetGroupID() { return GroupID; }

	FORCEINLINE bool GetIsReserveDestroy() { return IsReserveDestroy; }
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecretDialogPortalInfo", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* EffectTimeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SecretDialogPortalInfo", meta = (AllowPrivateAccess = "true"))
		FName PortalOpenParamName = FName("WPO");

	class USceneComponent* UISocket = nullptr;
	class USphereComponent* TargetArea = nullptr;
	class USphereComponent* InteractionArea = nullptr;
	class UParticleSystemComponent* ParticleSystem = nullptr;
	class UWRWidgetComponentBase* InWidgetComp = nullptr;
	float CurrentDeltaTime = 0.f;

	float LeaveDistance = 500.f;

	EWRSecretDialogPortalStateType CurrentState = EWRSecretDialogPortalStateType::None;
	
	bool IsUIOpen = false;
	bool IsReserveDestroy = false;

	CHAT_GROUP_ID_t GroupID = INVALID_CHAT_GROUP_ID;
};
