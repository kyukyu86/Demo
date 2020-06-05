// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMPreviewActor.h"
#include "DMPreviewActorStatic.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ADMPreviewActorStatic : public ADMPreviewActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent = nullptr;

public:
	ADMPreviewActorStatic();

protected:
	virtual void BeginPlay() override;
	virtual class UMeshComponent* GetMeshComponent() override { return StaticMeshComponent; }

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetStaticMesh(class UStaticMesh* InMesh) override;
};
