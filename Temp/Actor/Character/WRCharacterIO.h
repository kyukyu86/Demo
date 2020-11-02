// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"

#include "Actor/Character/WRCharacter.h"
#include "Table/WRObjectTable.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Component/Character/WRActorComponentInteractionObject.h"

#include "WRCharacterObjectBase.h"
#include "Manager/WRObjectInteractionManager.h"
#include "Enum/EWRCharacter.h"

#include "WRCharacterIO.generated.h"


//====================================================================================

UCLASS()
class WR_API AWRCharacterIO : public AWRCharacterObjectBase
{
	GENERATED_BODY()

	friend class WRTriggerActionProcessor_PlayObjectAnim;
	friend class UWRActorComponentInteractionObject;
public:
	AWRCharacterIO();
	AWRCharacterIO(const FObjectInitializer& ObjectInitializer);

private:
	void UpdateOptimization();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
	virtual void MakeDebugString(TArray<FString>& OUT OutString) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnPostSpawn() override;
	virtual void PostInitializeComponents() override;
	virtual void OnPreDestroy() override;
	virtual void BuildCustomComponents() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#else//WITH_EDITOR
#endif//WITH_EDITOR

	virtual bool IsIO() override { return true; }
	virtual bool IsInteracting() override;
	virtual void OnAnimEnded(UAnimSequenceBase* AnimSeq) override;
	
	// 직접 상호작용 거리
public:
	bool IsOverlappingInteractionableArea();
protected:
	UFUNCTION()
		void OnBeginOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlap_InteractionableArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UFUNCTION()
		void OnBeginOverlapIOForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlapIOForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	FORCEINLINE void SetCheatedObject(const bool IN InSet) { bCheatedObject = InSet; }
	FORCEINLINE bool IsCheatedObject() { return bCheatedObject; }
	void OnSpawnComplete(FWRInteractionObjectTable* IN InTableData, EWRInteractionState::en IN InState);
	void OnSpawnComplete(const pt_Game_Actor::Inf_IOAppear_G2U& InInform, FWRInteractionObjectTable* IN InTableData, const bool IN InManualSpawn = false);
	// Collision Set (for Debug)
	void SetShowCollisions(const bool IN InShow);

public:
	// Target
	virtual void OnTargeted(const bool IN InMulti = false, const bool IN InByMe = true) override;
	virtual void ReleasedTarget(const bool IN InMulti = false, const bool IN InByMe = true) override;
	// Input
	virtual bool OnPressedEvent(const EWRInput InInputType) override;	// 입력 시 여기를 먼저 들어온다.
	virtual bool OnReleasedEvent(const EWRInput InInputType) override;
	virtual void OnPadEvent(const EWRInput IN InEvent) override;
	// Widget
	virtual FTransform GetWidgetTransform() override;
	virtual EWRUIWidgetCreationBitFlag GetWidgetComponentShowType() override;
	// Hook
	virtual const EWRHookType GetHookType() override;
	virtual bool IsHookableState() override;
	virtual void OnHookArrival(const EWRConnectCableEvent IN InEvent, const FVector IN InImpactLocation, AWRCharacter* IN InHookCaster) override;
	virtual void OnHookCutted() override;
	virtual void OnHookEventToTarget(AWRCharacter* IN InHookCaster, EWRCollisionType IN InConnectedCollisionType) override;
	// Etc.
	virtual FTransform GetInteractionTransform() override;
	virtual void SetHighLight(EWRObjectHighLight IN InHighLightType) override;
	USceneComponent* GetInteractionComponent();
	USceneComponent* GetOccupationEndComponent();
	void SetTargetAreaEnable(const bool IN InEnable);
	FORCEINLINE bool IsBodyLock() { return BodyLock; }
	FORCEINLINE const FVector& GetMarkOffset() { return MarkOffset; }
	FORCEINLINE const FVector& GetCameraOffset() { return CameraOffset; }
	FORCEINLINE int32 GetResultAttachMyID() { return ResultAttachMyID; }
	FORCEINLINE int32 GetResultAttachTargetID() { return ResultAttachTargetID; }
	FORCEINLINE FName GetResultAttachSocketName() { return ResultAttachSocketName; }
	FORCEINLINE float GetInteractionMinDist() { return InteractionMinDist; }
	FORCEINLINE EWRIdleType GetIdleType() { return IdleType; }

	// Occupation
	bool IsDirectOccupiable();
	bool IsDirectOccupiedState();
	FORCEINLINE void SetOccupiedPlayer(class AWRCharacterPlayer* IN InPlayer) { OccupiedPlayer = InPlayer; }
	FORCEINLINE class AWRCharacterPlayer* GetOccupiedPlayer() { return OccupiedPlayer; }

	// Zipline
	int32 GetZiplineUniqueKey();
	void ArrivalZiplineEndPoint();

	ISN_t GetISN();
	bool CompareISN(ISN_t InISN);
	FWRInteractionObjectTable* GetIOTable() { return pTableData; }

public:
	void RemoveDisplay();

	bool IsCompletedRemove() {
		return IsCompleted;
	}

private:
	void CompleteDisplay();

private:
	bool IsCompleted = false;

private:
	FWRInteractionObjectTable* pTableData = nullptr;
	bool bCheatedObject = false;
	class AWRCharacterPlayer* OccupiedPlayer = nullptr;
	ISN_t RelatedISN = INVALID_ISN;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "UI Attach Type", Category = "IO Setting")
		EWRObjectUIType ObjectUIType;

	// All Directions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Direstions", Category = "IO Setting")
		TMap<EWRIODirectionType, FWRIODirection> Directions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Gyro Step", Category = "IO Setting")
		TArray<FWRIOGyro> GyroStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Mark", Category = "IO Setting")
		FVector MarkOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* IO_InteractionableArea;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		float InteractionMinDist = 0.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		int32 ResultAttachMyID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		int32 ResultAttachTargetID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		FName ResultAttachSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		bool BodyLock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		bool MovableIO = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		FVector CameraOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IO Setting")
		EWRIdleType IdleType = EWRIdleType::None;

};