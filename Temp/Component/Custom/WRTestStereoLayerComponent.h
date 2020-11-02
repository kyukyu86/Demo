// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/SceneComponent.h"
#include "IStereoLayers.h"
#include "Components/StereoLayerComponent.h"

#include "WRTestStereoLayerComponent.generated.h"

class UTexture;


UCLASS(ClassGroup = "HeadMountedDisplay", hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "WR Test Stereo Layer", BlueprintSpawnableComponent))
 class WR_API UWRTestStereoLayerComponent : public USceneComponent
 {
	GENERATED_UCLASS_BODY()
	//friend class FStereoLayerComponentVisualizer;


public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;

	//~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void SetActive(bool bNewActive, bool bReset = false);
	/**
	 * Change the texture displayed on the stereo layer.
	 *
	 * If stereoscopic layer textures are supported on the platform and LeftTexture is set, this property controls the texture for the right eye.
	 * @param	InTexture: new Texture2D
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		void SetTexture(UTexture* InTexture);

	/**
	 * Change the texture displayed on the stereo layer for left eye, if stereoscopic layer textures are supported on the platform.
	 * @param	InTexture: new Texture2D
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		void SetLeftTexture(UTexture* InTexture);

	// @return the texture mapped to the stereo layer.
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		UTexture* GetTexture() const { return Texture; }

	// @return the texture mapped to the stereo layer for left eye, if stereoscopic layer textures are supported on the platform.
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		UTexture* GetLeftTexture() const { return LeftTexture; }

	/**
	 * Change the quad size. This is the unscaled height and width, before component scale is applied.
	 * @param	InQuadSize: new quad size.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		void SetQuadSize(FVector2D InQuadSize);

	// @return the height and width of the rendered quad
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		FVector2D GetQuadSize() const { return QuadSize; }

	/**
	 * Change the UV coordinates mapped to the quad face
	 * @param	InUVRect: Min and Max UV coordinates
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		void SetUVRect(FBox2D InUVRect);

	// @return the UV coordinates mapped to the quad face
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		FBox2D GetUVRect() const { return UVRect; }

	/**
	 * Set Equirect layer properties: UVRect, Scale, and Bias
	 * @param	LeftScale: Scale for left eye
	 * @param	LeftBias: Bias for left eye
	 * @param	RightScale: Scale for right eye
	 * @param	RightBias: Bias for right eye
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		void SetEquirectProps(FEquirectProps InScaleBiases);

	/**
	 * Change the layer's render priority, higher priorities render on top of lower priorities
	 * @param	InPriority: Priority value
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		void SetPriority(int32 InPriority);

	// @return the render priority
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		int32 GetPriority() const { return Priority; }

	// Manually mark the stereo layer texture for updating
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
		void MarkTextureForUpdate();

	/** True if the stereo layer texture needs to update itself every frame(scene capture, video, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoLayer")
		uint32 bLiveTexture : 1;

	/** True if the stereo layer needs to support depth intersections with the scene geometry, if available on the platform */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoLayer")
		uint32 bSupportsDepth : 1;

	/** True if the texture should not use its own alpha channel (1.0 will be substituted) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoLayer")
		uint32 bNoAlphaChannel : 1;

protected:
	/** Texture displayed on the stereo layer (is stereocopic textures are supported on the platfrom and more than one texture is provided, this will be the right eye) **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StereoLayer")
		class UTexture* Texture;

	/** Texture displayed on the stereo layer for left eye, if stereoscopic textures are supported on the platform **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StereoLayer | Cubemap Overlay Properties")
		class UTexture* LeftTexture;

public:
	/** True if the quad should internally set it's Y value based on the set texture's dimensions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoLayer")
		uint32 bQuadPreserveTextureRatio : 1;

protected:
	/** Size of the rendered stereo layer quad **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "StereoLayer | Quad Overlay Properties")
		FVector2D QuadSize;

	/** UV coordinates mapped to the quad face **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "StereoLayer | Quad Overlay Properties")
		FBox2D UVRect;

	UE_DEPRECATED(4.25, "float CylinderRadius is deprecated. Use the corresponding property of the UStereoLayerShapeCylinder subobject instead.")
		UPROPERTY()
		float CylinderRadius_DEPRECATED;

	UE_DEPRECATED(4.25, "float CylinderOverlayArc is deprecated. Use the corresponding property of the UStereoLayerShapeCylinder subobject instead.")
		UPROPERTY()
		float CylinderOverlayArc_DEPRECATED;

	UE_DEPRECATED(4.25, "int CylinderHeight is deprecated. Use the corresponding property of the UStereoLayerShapeCylinder subobject instead.")
		UPROPERTY()
		int CylinderHeight_DEPRECATED;

	UE_DEPRECATED(4.25, "FEquirectProps EquirectProps is deprecated. Use the corresponding properties of the UStereoLayerShapeEquirect subobject instead.")
		UPROPERTY()
		FEquirectProps EquirectProps_DEPRECATED;

	/** Specifies how and where the quad is rendered to the screen **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "StereoLayer")
		TEnumAsByte<enum EStereoLayerType> StereoLayerType;

	UE_DEPRECATED(4.25, "TEnumAsByte<enum EStereoLayerShape> StereoLayerShape is deprecated. Use Shape instead.")
		UPROPERTY()
		TEnumAsByte<enum EStereoLayerShape> StereoLayerShape_DEPRECATED;

	/** Specifies which shape of layer it is.  Note that some shapes will be supported only on certain platforms! **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, NoClear, Instanced, Category = "StereoLayer", DisplayName = "Stereo Layer Shape")
		UStereoLayerShape* Shape;

	/** Render priority among all stereo layers, higher priority render on top of lower priority **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "StereoLayer")
		int32 Priority;

private:
	/** Dirty state determines whether the stereo layer needs updating **/
	bool bIsDirty;

	/** Texture needs to be marked for update **/
	bool bTextureNeedsUpdate;

	/** IStereoLayer id, 0 is unassigned **/
	uint32 LayerId;

	/** Last transform is cached to determine if the new frames transform has changed **/
	FTransform LastTransform;

	/** Last frames visiblity state **/
	bool bLastVisible;


 public:
#if UE_4_25
	 void SetStereoLayerShape(EStereoLayerShape InType) { }
	 void SetStereoLayerType(EStereoLayerType InType) {  }
	 void SetStereoLayerPriority(int32 InPriority) {  }
#else // UE_4_25
	 void SetStereoLayerShape(EStereoLayerShape InType) { StereoLayerShape = InType; }
	 void SetStereoLayerType(EStereoLayerType InType) { StereoLayerType = InType; }
	 void SetStereoLayerPriority(int32 InPriority) { Priority = InPriority; }
#endif // UE_4_25
};

