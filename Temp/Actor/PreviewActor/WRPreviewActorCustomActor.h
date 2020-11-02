// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRPreviewStudio.h"
#include "WRPreviewActorCustomActor.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRPreviewActorCustomActor : public AWRPreviewStudio
{
	GENERATED_BODY()
	
private:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
// 	class UChildActorComponent* ChildActorComp = nullptr;

public:
	AWRPreviewActorCustomActor();

protected:
	virtual void BeginPlay() override;
	virtual class UMeshComponent* GetMeshComponent() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetCustomActor(class AActor* InActor, class UAnimationAsset* InAnim) override;
	void SetAnimation();
	void SetData();
};
