// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMPreviewActor.h"
#include "DMPreviewActorDynamic.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ADMPreviewActorDynamic : public ADMPreviewActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

public:
	ADMPreviewActorDynamic();

protected:
	virtual void BeginPlay() override;
	virtual class UMeshComponent* GetMeshComponent() override { return SkeletalMeshComponent; }

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim) override;
};
