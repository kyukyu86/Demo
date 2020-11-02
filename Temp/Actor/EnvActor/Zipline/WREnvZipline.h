// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "WREnvZipline.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWREnvZipline : public AWREnvActor
{
	GENERATED_BODY()

public:
	AWREnvZipline();

protected:
	virtual void BeginPlay() override;

// 	UFUNCTION()
// 	void OnBeginOverlap_StartPoint(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
// 	UFUNCTION()
// 	void OnEndOverlap_StartPoint(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
// 
// 	UFUNCTION()
// 	void OnBeginOverlap_EndPoint(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
// 	UFUNCTION()
// 	void OnEndOverlap_EndPoint(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ArrivalEndPoint();


public:
	virtual void Tick(float DeltaTime) override;

	void AttachZiplineHandle(class AWRCharacterIO* IN InZiplineHandle);
	void ActiveZipline();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		int32 UniqueKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		bool bMovement = false;

private:
	// Spline
	class USplineComponent* Zipline = nullptr;
	class UBoxComponent* StartPoint = nullptr;
	class UBoxComponent* EndPoint = nullptr;
	class USceneComponent* AttachComponent = nullptr;

	// Attached
	class AWRCharacterIO* AttachedHandle = nullptr;
	class AWRCharacterPlayer* AttachedPlayer = nullptr;
};
