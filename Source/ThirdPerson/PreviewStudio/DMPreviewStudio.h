// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "../Manager/DMPreviewManager.h"
#include "DMPreviewStudio.generated.h"


UCLASS()
class THIRDPERSON_API ADMPreviewStudio : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UChildActorComponent* ChildActorComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent*			MeshParent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneCaptureComponent2D*	SceneCapture = nullptr;

protected:
	FDMPreviewInfo	PreviewInfo;
	FString			strAsyncKey = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FVector		DesiredExtendMesh = FVector(30.f, 30.f, 30.f);
	FVector		CurrentExtendMesh = FVector::ZeroVector;

	bool		bRotate = false;
	bool		bAuto = false;
	bool		bLeft = false;
	FVector2D	InputStartLocation = FVector2D::ZeroVector;

	bool		bZoomIn = false;
	bool		bZoomOut = false;
	FVector		SrcZoomRelativeLoc = FVector::ZeroVector;
	FVector		ZoomDirection = FVector::ZeroVector;
	float		AccumulatedZoom = 0.f;

	AActor*		CustomActor = nullptr;

public:
	ADMPreviewStudio();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	virtual class UMeshComponent* GetMeshComponent();

	virtual void UpdateScale();
	void UpdateRotate(const float IN InDeltaTime);
	void UpdateZoom(const float IN InDeltaTime);

	void OnPreviewTargetSetted();

	FVector GetSrcCameraOffset();
	FVector GetZoomDirection();
	void ResetZoomDirection();

public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetDesiredExtendSize(const FVector IN InSize) { DesiredExtendMesh = InSize; }

	//========== Input

	UFUNCTION(BlueprintCallable)
		bool OnInputStart(const FVector2D InCurrentLocation);
	UFUNCTION(BlueprintCallable)
		bool OnInputMove(const FVector2D InCurrentLocation);
	UFUNCTION(BlueprintCallable)
		bool OnInputEnd();

	UFUNCTION(BlueprintCallable)
		bool OnRotateStart(const bool InLeft);
	UFUNCTION(BlueprintCallable)
		bool OnRotateEnd();

	UFUNCTION(BlueprintCallable)
		bool OnZoomIn();
	UFUNCTION(BlueprintCallable)
		bool OnZoomOut();
	UFUNCTION(BlueprintCallable)
		bool OnZoomEnd();

	//========== Preview Target

	void SetPreviewTarget(const FDMPreviewInfo& IN InPreivewInfo);

	UFUNCTION(BlueprintCallable)
		virtual void SetStaticMesh(class UStaticMesh* InMesh);
	UFUNCTION(BlueprintCallable)
		virtual void SetSkeletalMesh(class USkeletalMesh* InMesh, class UAnimationAsset* InAnim);
	UFUNCTION(BlueprintCallable)
		virtual void SetDefaultActor(class UClass* InActorClass, class UAnimationAsset* InAnim);
	UFUNCTION(BlueprintCallable)
		virtual void SetCustomActor(class AActor* InActor, class UAnimationAsset* InAnim);

	AActor* GetCustomActor();
};