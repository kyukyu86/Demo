// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "DMPreviewActor.generated.h"


UENUM(BlueprintType)
enum class EDMPreviewType : uint8
{
	Static,
	Dynamic,
	CustomActor,
};


UCLASS()
class THIRDPERSON_API ADMPreviewActor : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent*			MeshParent		= nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D*	SceneCapture	= nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector		DesiredExtendMesh	= FVector(30.f, 30.f, 30.f);
	FVector		CurrentExtendMesh	= FVector::ZeroVector;
	bool		bRotate				= false;
	FVector		InputStartLocation	= FVector::ZeroVector;

public:	
	ADMPreviewActor();

protected:
	virtual void BeginPlay() override;	
	virtual void UpdateScale();

	virtual class UMeshComponent* GetMeshComponent() { return nullptr; }

public:	
	virtual void Tick(float DeltaTime) override;	

	FORCEINLINE void SetDesiredExtendSize(const FVector IN InSize) { DesiredExtendMesh = InSize; }

	UFUNCTION(BlueprintCallable)
	void SetMesh(const EDMPreviewType InType, UObject* InObject, class UAnimationAsset* InAnim = nullptr);
	UFUNCTION(BlueprintCallable)
	void OnInputStart(const FVector InCurrentLocation);
	UFUNCTION(BlueprintCallable)
	void OnInputMove(const FVector InCurrentLocation);
	UFUNCTION(BlueprintCallable)
	void OnInputEnd();
	
	UFUNCTION(BlueprintCallable)
	virtual void SetCustomActor(class AActor* InActor, class UAnimationAsset* InAnim) {}
	UFUNCTION(BlueprintCallable)
	virtual void SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim) {}
	UFUNCTION(BlueprintCallable)
	virtual void SetStaticMesh(class UStaticMesh* InMesh) {}
};
