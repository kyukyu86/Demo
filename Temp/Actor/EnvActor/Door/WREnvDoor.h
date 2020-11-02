// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../WREnvActor.h"
#include "WREnvDoor.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWREnvDoor : public AWREnvActor
{
	GENERATED_BODY()
//protected:
	//	// Called when the game starts or when spawned
private:
	virtual void BeginPlay() override;
public:

	virtual void Tick(float DeltaTime) override;

	/** called when something enters the sphere component */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	class UBoxComponent* InBoxComp = nullptr;

	bool bIsOpened = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DoorInfo)
		TSoftObjectPtr<USoundCue> OpenSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpiderWebInfo)
		TSoftObjectPtr<USoundCue> CloseSound = nullptr;
};
