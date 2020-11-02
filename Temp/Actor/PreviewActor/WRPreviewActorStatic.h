// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRPreviewStudio.h"
#include "WRPreviewActorStatic.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRPreviewActorStatic : public AWRPreviewStudio
{
	GENERATED_BODY()

private:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
// 	class UStaticMeshComponent* StaticMeshComponent = nullptr;

public:
	AWRPreviewActorStatic();

protected:
	virtual void BeginPlay() override;
	virtual class UMeshComponent* GetMeshComponent() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetStaticMesh(class UStaticMesh* InMesh) override;
};
