// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "WREnvPresetCharacter.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWREnvPresetCharacter : public AWREnvActor
{
	GENERATED_BODY()

private:
	bool EventOccurred = false;
	bool IsInInterationArea = false;
	class AWRCharacter* EnvPresetCharacter;
	class USceneComponent* TopStartPositionComponent = nullptr;
	class UBoxComponent* BoxComponent = nullptr;
	FDelegateHandle ProduceTickHandle;
	float ProduceTime;
	bool bIsProducing = false;

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
	TArray<uint64> AIDs;
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DefaultPresetTID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 LastPresetTID;
	UPROPERTY(VisibleAnywhere, Category = "WREnvPresetCharacter")
		UCurveFloat* CurveEndEffect;
	UPROPERTY(VisibleAnywhere, Category = "WREnvPresetCharacter")
		UCurveFloat* CurveStartEffect;
	
private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void VerifyCharacterEquip();								// [ 2020-4-23 : vuvle ] : Equip을 다시 해야 연출이 정상적으로 발생되므로 재장착을 하기 위한 함수.
	bool TickProduceStartEffect(float InDeltaTime);
	bool TickProduceEndEffect(float InDeltaTime);
	void LoadEffectCurve();
	void UnloadEffectCurve();
	void SetMaterialParameterValue(const float InValue);
	AWRCharacter* SpawnPresetCharacter();
	AActor*		  GetLookAtActor();
	void OnPresetCharacterTargeted();

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
	void OnCompleteDirection(AActor* InActor, EWREquipPart InPart);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

public:
	void SetDefaultPresetTID(const int32 IN InDefaultPresetTID) { DefaultPresetTID = InDefaultPresetTID; }
	WRTableID GetLastPresetTID() { return (WRTableID)LastPresetTID; }
	void SwitchPresetCharacter();
	bool IsProducing() { return bIsProducing; }
};
