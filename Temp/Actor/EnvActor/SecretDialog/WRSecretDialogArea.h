// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Struct/WREnvStruct.h"
#include "WRSecretDialogArea.generated.h"

/**
 * 
 */

UCLASS()
class WR_API AWRSecretDialogArea : public AWREnvActor
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo", meta = (AllowPrivateAccess = "true"))
		float AreaRadius = 500.f;
private:
	void CalcSpringArmRotAndCameraRot();
public:
	void OnSpawnComplete(const FWRSecretDialogInfo& InAreaInfo);

	FORCEINLINE CHAT_GROUP_ID_t& GetChatGroupID() { return GroupID; }

	FORCEINLINE class AWRSecretDialogPortal* GetLinkedPortal() { return PortalPtr; }
	FORCEINLINE void SetActorIDs(const TArray<AID_t>& InAIDs) { ActorIDs = InAIDs; }
	FORCEINLINE TArray<AID_t> GetActorIDs() { return ActorIDs; }
private:
	class USceneCaptureComponent2D* PortalViewCamera;
	class USpringArmComponent* SpringArmComp;

	class AWRSecretDialogPortal* PortalPtr = nullptr;

	CHAT_GROUP_ID_t GroupID = INVALID_CHAT_GROUP_ID;
	TArray<AID_t> ActorIDs;
};
