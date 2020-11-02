// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WRUIStereoLayerComponent.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "TextureResource.h"
#include "Engine/Texture.h"
#include "IStereoLayers.h"
#include "StereoRendering.h"
#include "RHIResources.h"



UWRUIStereoLayerComponent::UWRUIStereoLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}


void UWRUIStereoLayerComponent::BeginDestroy()
{
	if (Shape != nullptr)
	{
		Shape->RemoveFromRoot();
		Shape->ConditionalBeginDestroy();
		Shape = nullptr;
		StereoLayerShapeType = EStereoLayerShape::SLSH_MAX;
	}

	Super::BeginDestroy();
}

void UWRUIStereoLayerComponent::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	if (bNewActive == false)
	{
		IStereoLayers* StereoLayers;
		if (!GEngine->StereoRenderingDevice.IsValid() || (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) == nullptr)
		{
			return;
		}

		bLastVisible = false;
		IStereoLayers::FLayerDesc LayerDsec;
		LayerDsec.Flags |= IStereoLayers::LAYER_FLAG_HIDDEN;
		LayerDsec.Texture = nullptr;
		StereoLayers->SetLayerDesc(LayerId, LayerDsec);

		/*IStereoLayers* StereoLayers;
		if (LayerId && GEngine->StereoRenderingDevice.IsValid() && (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) != nullptr)
		{
			StereoLayers->DestroyLayer(LayerId);
			LayerId = 0;
		}*/

		if (Shape != nullptr)
		{
			Shape->RemoveFromRoot();
			Shape->ConditionalBeginDestroy();
			Shape = nullptr;
			StereoLayerShapeType = EStereoLayerShape::SLSH_MAX;
		}
	}
}


void UWRUIStereoLayerComponent::SetStereoLayerShape(const EStereoLayerShape InType)
{
	if (StereoLayerShapeType == InType)
		return;

	if (Shape != nullptr)
	{
		Shape->RemoveFromRoot();
		Shape->ConditionalBeginDestroy();
		Shape = nullptr;
		StereoLayerShapeType = EStereoLayerShape::SLSH_MAX;
	}

	UCopyStereoLayerShape* StereoLayerShpae = CreateStereoLayerShpae(InType);
	if (StereoLayerShpae == nullptr)
		return;

	StereoLayerShapeType = InType;
	Shape = StereoLayerShpae;
	Shape->AddToRoot();
}

void UWRUIStereoLayerComponent::SetCylinderOverlayArc(float IN InCylinderOverlayArc)
{
	if (GetStereoLayerShape() != EStereoLayerShape::SLSH_CylinderLayer)
		return;

	if (Shape == nullptr)
		return;

	UCopyStereoLayerShapeCylinder* ShapeCylinder = Cast<UCopyStereoLayerShapeCylinder>(Shape);
	if (ShapeCylinder == nullptr)
		return;

	ShapeCylinder->SetOverlayArc(InCylinderOverlayArc);
}
void UWRUIStereoLayerComponent::SetCylinderRadius(const float IN InCylinderRadius)
{
	if (GetStereoLayerShape() != EStereoLayerShape::SLSH_CylinderLayer)
		return;

	if (Shape == nullptr)
		return;

	UCopyStereoLayerShapeCylinder* ShapeCylinder = Cast<UCopyStereoLayerShapeCylinder>(Shape);
	if (ShapeCylinder == nullptr)
		return;

	ShapeCylinder->SetRadius(InCylinderRadius);
}
void UWRUIStereoLayerComponent::SetCylinderHeight(const int32 IN InCylinderHeight)
{
	if (GetStereoLayerShape() != EStereoLayerShape::SLSH_CylinderLayer)
		return;

	if (Shape == nullptr)
		return;

	UCopyStereoLayerShapeCylinder* ShapeCylinder = Cast<UCopyStereoLayerShapeCylinder>(Shape);
	if (ShapeCylinder == nullptr)
		return;

	ShapeCylinder->SetHeight(InCylinderHeight);
}

UCopyStereoLayerShape* UWRUIStereoLayerComponent::CreateStereoLayerShpae(const EStereoLayerShape InType)
{
	switch (InType)
	{
	case SLSH_QuadLayer:
	{
		return NewObject<UCopyStereoLayerShapeQuad>(this);
	}
	break;
	case SLSH_CubemapLayer:
	{
		return NewObject<UCopyStereoLayerShapeCubemap>(this);
	}
	break;
	case SLSH_CylinderLayer:
	{
		return NewObject<UCopyStereoLayerShapeCylinder>(this);
	}
	break;
	case SLSH_EquirectLayer:
	{
		return NewObject<UCopyStereoLayerShapeEquirect>(this);
	}
	break;
	}

	return nullptr;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copyright Epic Games, Inc. All Rights Reserved.

UCopyStereoLayerComponent::UCopyStereoLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bLiveTexture(false)
	, bSupportsDepth(false)
	, bNoAlphaChannel(false)
	, Texture(nullptr)
	, LeftTexture(nullptr)
	, bQuadPreserveTextureRatio(false)
	, QuadSize(FVector2D(100.0f, 100.0f))
	, UVRect(FBox2D(FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f)))
	, StereoLayerType(SLT_FaceLocked)
	, Priority(0)
	, bIsDirty(true)
	, bTextureNeedsUpdate(false)
	, LayerId(0)
	, LastTransform(FTransform::Identity)
	, bLastVisible(false)
	, bNeedsPostLoadFixup(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	// The following somehow overrides subobjects read through serialization.
	//Shape = ObjectInitializer.CreateDefaultSubobject<UStereoLayerShapeQuad>(this, TEXT("Shape"));
}

void UCopyStereoLayerComponent::BeginDestroy()
{
	Super::BeginDestroy();

	IStereoLayers* StereoLayers;
	if (LayerId && GEngine->StereoRenderingDevice.IsValid() && (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) != nullptr)
	{
		StereoLayers->DestroyLayer(LayerId);
		LayerId = 0;
	}
}

void UCopyStereoLayerComponent::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FVRObjectVersion::GUID);
	Super::Serialize(Ar);
	if (Ar.IsLoading() && Ar.CustomVer(FVRObjectVersion::GUID) < FVRObjectVersion::UseSubobjectForStereoLayerShapeProperties)
	{
		bNeedsPostLoadFixup = true; // Postponing fixups until after load, as we need to modify subobjects.
	}
}

void UCopyStereoLayerComponent::PostLoad()
{
	Super::PostLoad();


	if (Shape == nullptr)
	{
		Shape = NewObject<UCopyStereoLayerShapeQuad>(this);
	}
}

void UCopyStereoLayerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	IStereoLayers* StereoLayers;
	if (!GEngine->StereoRenderingDevice.IsValid() || (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) == nullptr)
	{
		return;
	}

	FTransform Transform;
	if (StereoLayerType == SLT_WorldLocked)
	{
		Transform = GetComponentTransform();
	}
	else
	{
		Transform = GetRelativeTransform();
	}

	// If the transform changed dirty the layer and push the new transform
	if (!bIsDirty && (bLastVisible != GetVisibleFlag() || FMemory::Memcmp(&LastTransform, &Transform, sizeof(Transform)) != 0))
	{
		bIsDirty = true;
	}

	bool bCurrVisible = GetVisibleFlag();
	if (!Texture || !Texture->Resource)
	{
		bCurrVisible = false;
	}

	if (bIsDirty)
	{
		IStereoLayers::FLayerDesc LayerDesc;
		LayerDesc.Priority = Priority;
		LayerDesc.QuadSize = QuadSize;
		LayerDesc.UVRect = UVRect;
		LayerDesc.Transform = Transform;

		if (Texture)
		{
			Texture->SetForceMipLevelsToBeResident(30.0f);
			LayerDesc.Texture = Texture->Resource->TextureRHI;
			LayerDesc.Flags |= (Texture->GetMaterialType() == MCT_TextureExternal) ? IStereoLayers::LAYER_FLAG_TEX_EXTERNAL : 0;
		}
		if (LeftTexture)
		{
			Texture->SetForceMipLevelsToBeResident(30.0f);
			LayerDesc.LeftTexture = LeftTexture->Resource->TextureRHI;
		}

		LayerDesc.Flags |= (bLiveTexture) ? IStereoLayers::LAYER_FLAG_TEX_CONTINUOUS_UPDATE : 0;
		LayerDesc.Flags |= (bNoAlphaChannel) ? IStereoLayers::LAYER_FLAG_TEX_NO_ALPHA_CHANNEL : 0;
		LayerDesc.Flags |= (bQuadPreserveTextureRatio) ? IStereoLayers::LAYER_FLAG_QUAD_PRESERVE_TEX_RATIO : 0;
		LayerDesc.Flags |= (bSupportsDepth) ? IStereoLayers::LAYER_FLAG_SUPPORT_DEPTH : 0;
		LayerDesc.Flags |= (!bCurrVisible) ? IStereoLayers::LAYER_FLAG_HIDDEN : 0;

		switch (StereoLayerType)
		{
		case SLT_WorldLocked:
			LayerDesc.PositionType = IStereoLayers::WorldLocked;
			break;
		case SLT_TrackerLocked:
			LayerDesc.PositionType = IStereoLayers::TrackerLocked;
			break;
		case SLT_FaceLocked:
			LayerDesc.PositionType = IStereoLayers::FaceLocked;
			break;
		}

		// Set the correct layer shape and apply any shape-specific properties
		if (Shape == nullptr)
		{
			Shape = NewObject<UCopyStereoLayerShapeQuad>(this);
		}
		Shape->ApplyShape(LayerDesc);

		if (LayerId)
		{
			StereoLayers->SetLayerDesc(LayerId, LayerDesc);
		}
		else
		{
			LayerId = StereoLayers->CreateLayer(LayerDesc);
		}

		LastTransform = Transform;
		bLastVisible = bCurrVisible;
		bIsDirty = false;
	}

	if (bTextureNeedsUpdate && LayerId)
	{
		StereoLayers->MarkTextureForUpdate(LayerId);
		bTextureNeedsUpdate = false;
	}
}

void UCopyStereoLayerComponent::SetTexture(UTexture* InTexture)
{
	if (Texture == InTexture)
	{
		return;
	}

	Texture = InTexture;
	MarkStereoLayerDirty();
}

void UCopyStereoLayerComponent::SetLeftTexture(UTexture* InTexture)
{
	if (LeftTexture == InTexture)
	{
		return;
	}

	LeftTexture = InTexture;
	MarkStereoLayerDirty();
}

void UCopyStereoLayerComponent::SetQuadSize(FVector2D InQuadSize)
{
	if (QuadSize == InQuadSize)
	{
		return;
	}

	QuadSize = InQuadSize;
	MarkStereoLayerDirty();
}

void UCopyStereoLayerComponent::SetUVRect(FBox2D InUVRect)
{
	if (UVRect == InUVRect)
	{
		return;
	}

	UVRect = InUVRect;
	MarkStereoLayerDirty();
}

void UCopyStereoLayerComponent::SetEquirectProps(FCopyEquirectProps InEquirectProps)
{
	if (Shape->IsA<UCopyStereoLayerShapeEquirect>())
	{
		Cast<UCopyStereoLayerShapeEquirect>(Shape)->SetEquirectProps(InEquirectProps);
	}
}

void UCopyStereoLayerShapeEquirect::SetEquirectProps(FCopyEquirectProps InEquirectProps)
{
	if (InEquirectProps == *this)
	{
		return;
	}
	LeftUVRect = InEquirectProps.LeftUVRect;
	RightUVRect = InEquirectProps.RightUVRect;
	LeftScale = InEquirectProps.LeftScale;
	RightScale = InEquirectProps.RightScale;
	LeftBias = InEquirectProps.LeftBias;
	RightBias = InEquirectProps.RightBias;

	MarkStereoLayerDirty();
}


void UCopyStereoLayerShapeCylinder::SetRadius(float InRadius)
{
	if (Radius == InRadius)
	{
		return;
	}

	Radius = InRadius;
	MarkStereoLayerDirty();
}

void UCopyStereoLayerShapeCylinder::SetOverlayArc(float InOverlayArc)
{
	if (OverlayArc == InOverlayArc)
	{
		return;
	}

	OverlayArc = InOverlayArc;
	MarkStereoLayerDirty();
}

void UCopyStereoLayerShapeCylinder::SetHeight(int InHeight)
{
	if (Height == InHeight)
	{
		return;
	}

	Height = InHeight;
	MarkStereoLayerDirty();
}


void UCopyStereoLayerShape::MarkStereoLayerDirty()
{
	check(GetOuter()->IsA<UCopyStereoLayerComponent>());
	Cast<UCopyStereoLayerComponent>(GetOuter())->MarkStereoLayerDirty();
}

void UCopyStereoLayerShapeCylinder::ApplyShape(IStereoLayers::FLayerDesc& LayerDesc)
{
	LayerDesc.SetShape<FCylinderLayer>(Radius, OverlayArc, Height);
}

void UCopyStereoLayerShapeCubemap::ApplyShape(IStereoLayers::FLayerDesc& LayerDesc)
{
	LayerDesc.SetShape<FCubemapLayer>();
}

void UCopyStereoLayerShapeEquirect::ApplyShape(IStereoLayers::FLayerDesc& LayerDesc)
{
	LayerDesc.SetShape<FEquirectLayer>(LeftUVRect, RightUVRect, LeftScale, RightScale, LeftBias, RightBias);
}

void UCopyStereoLayerShapeQuad::ApplyShape(IStereoLayers::FLayerDesc& LayerDesc)
{
	LayerDesc.SetShape<FQuadLayer>();
}

void UCopyStereoLayerShape::ApplyShape(IStereoLayers::FLayerDesc& LayerDesc)
{
}

#if WITH_EDITOR
void UCopyStereoLayerShape::DrawShapeVisualization(const class FSceneView* View, class FPrimitiveDrawInterface* PDI)
{
}

void UCopyStereoLayerShapeQuad::DrawShapeVisualization(const class FSceneView* View, class FPrimitiveDrawInterface* PDI)
{
	FLinearColor YellowColor = FColor(231, 239, 0, 255);
	check(GetOuter()->IsA<UCopyStereoLayerComponent>());

	auto StereoLayerComp = Cast<UCopyStereoLayerComponent>(GetOuter());
	const FVector2D QuadSize = StereoLayerComp->GetQuadSize() / 2.0f;
	const FBox QuadBox(FVector(0.0f, -QuadSize.X, -QuadSize.Y), FVector(0.0f, QuadSize.X, QuadSize.Y));

	DrawWireBox(PDI, StereoLayerComp->GetComponentTransform().ToMatrixWithScale(), QuadBox, YellowColor, 0);
}

void UCopyStereoLayerShapeCylinder::DrawShapeVisualization(const class FSceneView* View, class FPrimitiveDrawInterface* PDI)
{
	FLinearColor YellowColor = FColor(231, 239, 0, 255);
	check(GetOuter()->IsA<UCopyStereoLayerComponent>());

	auto StereoLayerComp = Cast<UCopyStereoLayerComponent>(GetOuter());
	float ArcAngle = OverlayArc * 180 / (Radius * PI);

	FVector X = StereoLayerComp->GetComponentTransform().GetUnitAxis(EAxis::Type::X);
	FVector Y = StereoLayerComp->GetComponentTransform().GetUnitAxis(EAxis::Type::Y);
	FVector Base = StereoLayerComp->GetComponentTransform().GetLocation();
	FVector HalfHeight = FVector(0, 0, Height / 2);

	FVector LeftVertex = Base + Radius * (FMath::Cos(ArcAngle / 2 * (PI / 180.0f)) * X + FMath::Sin(ArcAngle / 2 * (PI / 180.0f)) * Y);
	FVector RightVertex = Base + Radius * (FMath::Cos(-ArcAngle / 2 * (PI / 180.0f)) * X + FMath::Sin(-ArcAngle / 2 * (PI / 180.0f)) * Y);

	DrawArc(PDI, Base + HalfHeight, X, Y, -ArcAngle / 2, ArcAngle / 2, Radius, 10, YellowColor, 0);

	DrawArc(PDI, Base - HalfHeight, X, Y, -ArcAngle / 2, ArcAngle / 2, Radius, 10, YellowColor, 0);

	PDI->DrawLine(LeftVertex - HalfHeight, LeftVertex + HalfHeight, YellowColor, 0);

	PDI->DrawLine(RightVertex - HalfHeight, RightVertex + HalfHeight, YellowColor, 0);
}
#endif

void UCopyStereoLayerComponent::SetPriority(int32 InPriority)
{
	if (Priority == InPriority)
	{
		return;
	}

	Priority = InPriority;
	MarkStereoLayerDirty();
}

void UCopyStereoLayerComponent::MarkTextureForUpdate()
{
	bTextureNeedsUpdate = true;
}

void UCopyStereoLayerComponent::MarkStereoLayerDirty()
{
	bIsDirty = true;
}

bool FCopyEquirectProps::operator==(const class UCopyStereoLayerShapeEquirect& Other) const
{
	return (LeftUVRect == Other.LeftUVRect) && (RightUVRect == Other.RightUVRect) && (LeftScale == Other.LeftScale) && (RightScale == Other.RightScale) && (LeftBias == Other.LeftBias) && (RightBias == Other.RightBias);
}

bool FCopyEquirectProps::operator==(const FCopyEquirectProps& Other) const
{
	return (LeftUVRect == Other.LeftUVRect) && (RightUVRect == Other.RightUVRect) && (LeftScale == Other.LeftScale) && (RightScale == Other.RightScale) && (LeftBias == Other.LeftBias) && (RightBias == Other.RightBias);
}
