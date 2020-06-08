// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMPreviewActor.h"
#include "DMPreviewActorCustomActor.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ADMPreviewActorCustomActor : public ADMPreviewActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* ChildActorComp = nullptr;

public:
	ADMPreviewActorCustomActor();

protected:
	virtual void BeginPlay() override;
	virtual class UMeshComponent* GetMeshComponent() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetCustomActor(class AActor* InActor, class UAnimationAsset* InAnim) override;
	void SetAnimation();
	void SetData();
};
