// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMPreviewActor.generated.h"

UCLASS()
class THIRDPERSON_API ADMPreviewActor : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent*	MeshParent	= nullptr;
	FVector		CurrentExtendMesh	= FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector		DesiredExtendMesh	= FVector(30.f, 30.f, 30.f);
	bool		bRotate				= false;
	FVector		InputStartLocation	= FVector::ZeroVector;

public:	
	ADMPreviewActor();

protected:
	virtual void BeginPlay() override;
	virtual class UMeshComponent* GetMeshComponent() { return nullptr; }

public:	
	virtual void Tick(float DeltaTime) override;
	
	void SetDesiredExtendSize(const FVector IN InSize) { DesiredExtendMesh = InSize; }

	UFUNCTION(BlueprintCallable)
	void OnInputStart(const FVector InCurrentLocation);
	UFUNCTION(BlueprintCallable)
	void OnInputMove(const FVector InCurrentLocation);
	UFUNCTION(BlueprintCallable)
	void OnInputEnd();
	UFUNCTION(BlueprintCallable)
	void UpdateScale();

	UFUNCTION(BlueprintCallable)
	virtual void SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim) {}
	UFUNCTION(BlueprintCallable)
	virtual void SetStaticMesh(class UStaticMesh* InMesh) {}
};
