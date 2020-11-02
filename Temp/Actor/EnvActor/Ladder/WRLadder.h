// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Manager/WRObjectInteractionManager.h"
#include "Enum/EWRCharacter.h"
#include "WRLadder.generated.h"

#define DEF_INVALID_LADDER_HANDLE_INDEX -1

enum class EWRLadderGuideUI : uint8
{
	None,
	Left,
	Right,
};

/**
 * 
 */
UCLASS()
class WR_API AWRLadder : public AWREnvActor
{
	GENERATED_BODY()

public:
	AWRLadder();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:	
	float GetHandleOffset() { return HandleOffset; }
	float GetHandleMeshHeight() { return HandleMeshHeight; }
	float GetCharAttachOffset() { return CharAttachOffset; }
	int32 GetHandleCount() { return HandleCount; }
	int32 GetTopStartHandleNumber() { return LadderHandles.Num() - 1; }
	int32 GetBottomStartHandleNumber() { return BottomStartingHandleNumber; }
	int32 GetHandleIndex(FWRLadderHandle* IN InHandle);
	FTransform GetTopStartPosition();
	FTransform GetTopEndPosition();
	FTransform GetBottomPosition();
	FWRLadderHandle* GetHandle(const int32 IN InIndex);
	FWRLadderHandle* GetTopHandle();
	FWRLadderHandle* GetBottomStartHandle();
	FWRLadderHandle* GetNearestHandle(class AWRCharacterPlayer* IN pPlayer);
	
	float GetCalculatedCharLocationZ(FWRLadderHandle* IN InHandle);

	bool IsTopHandle(const int32 IN InHandleIndex);
	bool IsTopHandle(FWRLadderHandle* IN InComp);
	bool IsBottomHandle(const int32 IN InHandleIndex);

	// for Hand Guide Widget
	void UpdateHandGuideOnlyForMe(const int32 IN InCurrentHandleIndex);
	void SetHandGuideVisibility(const bool IN InVisible);

	void ReleaseFromMe(class AWRCharacterPlayer* IN InMe);

	// for Debug
	void SetHandleIndex(const int32 IN InIndex) { HandleIndex = InIndex; }

private:
	EWRLadderHand GetHandType(const int32 IN InIndex);
	// for Hand Guide Widget
	void UpdateHandGuide(const bool IN InForce);
	void AttachHandGuide(FWRLadderHandle* IN InHandle, bool IN InInverse);
	void AttachTopHandleGuide();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		float HandleOffset = 50.0f; // 손잡이 간 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		int32 HandleCount = 1; // 손잡이 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		int32 BottomStartingHandleNumber = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		EWRLadderHand StartHandType = EWRLadderHand::Left;		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		float HandleMeshHeight = 30.0f; // 실제 손잡이 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		float HandGuideOffset = 30.0f; // 손 가이드 위치 조정용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		float CharAttachOffset = 30.0f; // 케릭터 위치 조정용

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* LeftHandComp = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* RightHandComp = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<class UStaticMeshComponent*> LadderHandles;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int32 HandleIndex = 0;
	
private:
	class USceneComponent* TopStartPositionComponent = nullptr;
	class USceneComponent* TopEndPositionComponent = nullptr;
	class USceneComponent* BottomPositionComponent = nullptr;
	class USceneComponent* LeftWidgetComponent = nullptr;
	class USceneComponent* RightWidgetComponent = nullptr;
	class UBoxComponent* InteractionArea = nullptr;
	TArray<FWRLadderHandle*> Handles;

	// Hand Guide Widget (for Me)
	class UWRWidgetComponentBase* WidgetComponentBase = nullptr;
	class UWRUIPanel_ClimbHandGuide* HandGuideWidget = nullptr;
	bool bVisibleGuideWidget = false;
	EWRLadderDirection PrevDirection = EWRLadderDirection::None;
	int32 MyHandleIndex = DEF_INVALID_LADDER_HANDLE_INDEX;
};
