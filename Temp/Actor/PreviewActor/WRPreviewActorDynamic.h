// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRPreviewStudio.h"
#include "WRPreviewActorDynamic.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRPreviewActorDynamic : public AWRPreviewStudio
{
	GENERATED_BODY()

private:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
// 	class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

public:
	AWRPreviewActorDynamic();

protected:
	virtual void BeginPlay() override;
	virtual class UMeshComponent* GetMeshComponent() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim) override;
};
