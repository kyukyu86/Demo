// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Actor/Volume/WRVolume.h"
#include "WRDanceVolume.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRDanceVolume : public AWRVolume
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void Tick(float DeltaSeconds) override;
	
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

protected:
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay();

public:
	bool bEnter = false;	//  댄스 볼륨에 들어와 있는가??
	bool OnStartDanceButtonClicked();
};
