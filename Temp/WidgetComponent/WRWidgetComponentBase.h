// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "Components/StereoLayerComponent.h"
#include "Define/WRTypeDefine.h"
#include "Enum/EWRUIEnum.h"
#include "WRWidgetComponentBase.generated.h"

/**
 *
 */

class UWRUIPanel;
class AWRCharacter;

UCLASS(Blueprintable, ClassGroup = "UserInterface", hidecategories = (Object, Activation, "Components|Activation", Sockets, Base, Lighting, LOD, Mesh), editinlinenew, meta = (BlueprintSpawnableComponent))
class WR_API UWRWidgetComponentBase : public UWidgetComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void SetActive(bool bNewActive, bool bReset = false) override;
	virtual void SetWidget(UUserWidget* InWidget) override;
public:
	FORCEINLINE const AActor* GetOwnerActor() { return OwnerActor; }

	void SetFixedTransform(const FTransform& IN InTransform);
	void SetOffsetTransform(const FTransform& IN InTransform);
	void SetBillboard(const bool IN bInBillboard);
	void SetOffsetScaleForFixedSize(const float IN InOffsetScaleForFixedSize);
	void SetOwnerActor(const AActor* const IN InOwnerActor);
	void SetBitFlag(const EWRUIWidgetCreationBitFlag IN InWidgetComponentFlag);
	void SetAttachSocketName(const FName& IN InAttachSoccketName);
	void SetAttachToCamera(const bool IN bInAttachToCamera);
	void SetMaterialAsHidden();
	void SetMaterialByBlendMode(const EWidgetBlendMode IN InBlendMode);

	void StartCyberAni();
	void UseStereoLayer(const EStereoLayerShape IN StereoLayerShape = EStereoLayerShape::SLSH_QuadLayer);
	class UWRUIStereoLayerComponent* GetStereoLayer() { return WRStereoLayerComponent; }
private:
	void InitMaterial();
	void InitCyberMaterial();

	void Tick_CyberAni(const float IN InDeltaTime);
	void Tick_StereoLayer(const float IN InDeltaTime);

	void UpdateLocationByActor();
	void UpdateLocationByCamera(const float IN InDeltaTime);	// Attach to Camera
	void UpdateScaleForFixedSize(const float IN InDeltaTime);
	void UpdateRotationByBilboard();

	void OnRenderTargetSizeChanged(const FVector2D& IN InRenderTargetSize);

	FVector CalculateGoAToB(const float IN InDeltaTime, const FVector& IN InAPosition, const FVector& IN InBPosition);
	float SmoothMoving(const float IN InDeltaTime, float IN InCameraYaw);
	FVector GetCameraLocation();
	FRotator GetCameraRotation();

protected:
	const AActor*	OwnerActor				= nullptr;
	EWRUIWidgetCreationBitFlag		WidgetComponentFlag		= EWRUIWidgetCreationBitFlag::None;
	bool			bBillboard				= false;	
	FTransform		OffsetTransform			= FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(0.2f, 0.2f, 0.2f));
	bool			bAttachToActor			= false;
	bool			bAttachToCamera			= false;
	bool			bRotateToMe				= false;	// [ 2019-9-24 : kyu ] 내쪽으로 위치조정 (위치 기반 회전)
	bool			bRotateAxisZToMe		= false;	// [ 2019-10-7 : kyu ] 내쪽으로 위치조정 (Offset 기반 회전)
	float			OffsetScaleForFixedSize = 0.f;
	FName			AttachSocketName;
	float			BeforeYaw				= 0.f;

	float			CyberUIOpacityTime		= 0.f;
	bool			bCyberUI				= false;

	// WRStereoLayer
	class UWRUIStereoLayerComponent * WRStereoLayerComponent;
	
	UPROPERTY()
		class UMaterialInterface* MaterialHideWidgetComponent;

	bool bIgnore = false;
	FTransform TransformForCameraAll;
private:
	void CreateStereoLayerComponent(const EStereoLayerShape IN InStereoLayerShape = EStereoLayerShape::SLSH_QuadLayer);
	void DrawRenderTarget(const float IN InDeltaTime);
	virtual void UpdateRenderTarget(FIntPoint DesiredRenderTargetSize) override;
	
};
