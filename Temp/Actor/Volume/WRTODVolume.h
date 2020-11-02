// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Enum/EWRTOD.h"

#include "CoreMinimal.h"
#include "Actor/Volume/WRVolume.h"
#include "WRTODVolume.generated.h"

/**
 * 
 */
UCLASS(DisplayName="WR TOD Volume")
class WR_API AWRTODVolume : public AWRVolume
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void Tick(float DeltaSeconds) override;
	/** If true, actor is ticked even if TickType==LEVELTICK_ViewportsOnly	 */
	virtual bool ShouldTickIfViewportsOnly() const override;

	/**
	 *	Event when this actor overlaps another actor, for example a player walking into a trigger.
	 *	For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 *	Event when an actor no longer overlaps another actor, and they have separated.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#else // WITH_EDITOR
#endif // WITH_EDITOR

protected:
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRTODVolume", meta = (UIMin = "1"))
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRTODVolume")
	float Duration = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRTODVolume")
	float DayCycle = 1.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WRTODVolume")
	float CurrentDayCycle = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRFXVolume", meta = (UIMin = "0.0", UIMax = "1.0"))
	float Raining = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRFXVolume", meta = (UIMin = "0.0", UIMax = "1.0"))
	float Thundering = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRFXVolume", meta = (UIMin = "0.0", UIMax = "1.0"))
	float Lightning = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWindVolume")
	float Strength = 0.1F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWindVolume")
	float Speed = 0.2F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWindVolume")
	float MinGustAmount = 0.1F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWindVolume")
	float MaxGustAmount = 0.2F;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UArrowComponent* ArrowComponent;
#endif // WITH_EDITORONLY_DATA

private:
	bool IsOverlap = false;
};
