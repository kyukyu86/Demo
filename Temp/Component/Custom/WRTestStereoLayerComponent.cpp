// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WRTestStereoLayerComponent.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "TextureResource.h"
#include "Engine/Texture.h"
#include "IStereoLayers.h"
#include "StereoRendering.h"
#include "RHIResources.h"

UWRTestStereoLayerComponent::UWRTestStereoLayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bLiveTexture(false)
	, bSupportsDepth(false)
	, bNoAlphaChannel(false)
	, Texture(nullptr)
	, LeftTexture(nullptr)
	, bQuadPreserveTextureRatio(false)
	, QuadSize(FVector2D(100.0f, 100.0f))
	, UVRect(FBox2D(FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f)))
#if UE_4_25
#else // UE_4_25
	, CylinderRadius(100)
	, CylinderOverlayArc(100)
	, CylinderHeight(50)
	, EquirectProps()
	, StereoLayerType(SLT_FaceLocked)
	, StereoLayerShape(SLSH_QuadLayer)
#endif // UE_4_25
	, Priority(0)
	, bIsDirty(true)
	, bTextureNeedsUpdate(false)
	, LayerId(0)
	, LastTransform(FTransform::Identity)
	, bLastVisible(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UWRTestStereoLayerComponent::BeginDestroy()
{
	Super::BeginDestroy();

	IStereoLayers* StereoLayers;
	if (LayerId && GEngine->StereoRenderingDevice.IsValid() && (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) != nullptr)
	{
		StereoLayers->DestroyLayer(LayerId);
		LayerId = 0;
	}
}

void UWRTestStereoLayerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
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
			LayerDesc.Texture = Texture->Resource->TextureRHI;
			LayerDesc.Flags |= (Texture->GetMaterialType() == MCT_TextureExternal) ? IStereoLayers::LAYER_FLAG_TEX_EXTERNAL : 0;
		}
		if (LeftTexture)
		{
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

#if UE_4_25
		// Set the correct layer shape and apply any shape-specific properties
		if (Shape == nullptr)
		{
			Shape = NewObject<UStereoLayerShapeQuad>(this);
		}
		Shape->ApplyShape(LayerDesc);
#else // UE_4_25
		switch (StereoLayerShape)
		{
		case SLSH_QuadLayer:
			LayerDsec.ShapeType = IStereoLayers::QuadLayer;
			break;

		case SLSH_CylinderLayer:
			LayerDsec.ShapeType = IStereoLayers::CylinderLayer;
			LayerDsec.CylinderRadius = CylinderRadius;
			LayerDsec.CylinderOverlayArc = CylinderOverlayArc;
			LayerDsec.CylinderHeight = CylinderHeight;
			break;

		case SLSH_CubemapLayer:
			LayerDsec.ShapeType = IStereoLayers::CubemapLayer;
			break;

		case SLSH_EquirectLayer:
			LayerDsec.ShapeType = IStereoLayers::EquirectLayer;
			LayerDsec.EquirectProps = { EquirectProps.LeftUVRect, EquirectProps.RightUVRect, EquirectProps.LeftScale, EquirectProps.RightScale, EquirectProps.LeftBias, EquirectProps.RightBias };
			break;

		default:
			break;
		}
#endif // UE_4_25


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


void UWRTestStereoLayerComponent::SetActive(bool bNewActive, bool bReset /*= false*/)
{
	Super::SetActive(bNewActive, bReset);

	if (bNewActive == false)
	{
	/*	IStereoLayers* StereoLayers;
		if (!GEngine->StereoRenderingDevice.IsValid() || (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) == nullptr)
		{
			return;
		}
		
		bLastVisible = false;
		IStereoLayers::FLayerDesc LayerDsec;
		LayerDsec.Flags |= IStereoLayers::LAYER_FLAG_HIDDEN;
		LayerDsec.Texture = nullptr;
		StereoLayers->SetLayerDesc(LayerId, LayerDsec);*/

		/*IStereoLayers* StereoLayers;
		if (LayerId && GEngine->StereoRenderingDevice.IsValid() && (StereoLayers = GEngine->StereoRenderingDevice->GetStereoLayers()) != nullptr)
		{
			StereoLayers->DestroyLayer(LayerId);
			LayerId = 0;
		}*/
	}
}

void UWRTestStereoLayerComponent::SetTexture(UTexture* InTexture)
{
	if (Texture == InTexture)
	{
		return;
	}

	Texture = InTexture;
	bIsDirty = true;
}

void UWRTestStereoLayerComponent::SetLeftTexture(UTexture* InTexture)
{
	if (LeftTexture == InTexture)
	{
		return;
	}

	LeftTexture = InTexture;
	bIsDirty = true;
}

void UWRTestStereoLayerComponent::SetQuadSize(FVector2D InQuadSize)
{
	if (QuadSize == InQuadSize)
	{
		return;
	}

	QuadSize = InQuadSize;
	bIsDirty = true;
}

void UWRTestStereoLayerComponent::SetUVRect(FBox2D InUVRect)
{
	if (UVRect == InUVRect)
	{
		return;
	}

	UVRect = InUVRect;
	bIsDirty = true;
}

void UWRTestStereoLayerComponent::SetEquirectProps(FEquirectProps InEquirectProps)
{
#if UE_4_25
	return;
#else // UE_4_25
	if (EquirectProps == InEquirectProps)
	{
		return;
	}
	EquirectProps = InEquirectProps;
	bIsDirty = true;
#endif // UE_4_25
}

void UWRTestStereoLayerComponent::SetPriority(int32 InPriority)
{
	if (Priority == InPriority)
	{
		return;
	}

	Priority = InPriority;
	bIsDirty = true;
}

void UWRTestStereoLayerComponent::MarkTextureForUpdate()
{
	bTextureNeedsUpdate = true;
}

//// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
//
//#include "WRTestStereoLayerComponent.h"
//#include "SVirtualWindow.h"
//#include "UserWidget.h"
//#include "SWidget.h"
//#include "WidgetRenderer.h"
//#include "Engine/GameViewportClient.h"
//#include "Engine/GameInstance.h"
//#include "Engine/World.h"
//#include "SViewport.h"
//#include "Manager/WRUIManager.h"
//
//
//void UWRTestStereoLayerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	UpdateWidget();
//
//	if (Widget == nullptr )
//	{
//		return;
//	}
//
//	DrawWidgetToRenderTarget(DeltaTime);
//}
//
//void UWRTestStereoLayerComponent::OnRegister()
//{
//	Super::OnRegister();
//
//	if (WidgetRenderer == nullptr)
//	{
//		WidgetRenderer = new FWidgetRenderer(true);
//	}
//	
//	SetQuadSize(FVector2D(782, 404));
//	if (WRUIManager::IsValid())
//	{
//		FString UIPath = TEXT("Panel/Panel_Debug_WB.Panel_Debug_WB_C");
//		SetWidget(WRUIManager::Get()->CreateUISync(UIPath));
//	}
//}
//
//void UWRTestStereoLayerComponent::SetWidget(UUserWidget* IN InWidget)
//{
//	/*if (Widget)
//	{
//		RemoveWidgetFromScreen();
//	}*/
//
//	Widget = InWidget;
//
//	UpdateWidget();
//}
//
//void UWRTestStereoLayerComponent::DrawWidgetToRenderTarget(const float IN InDeltaTime)
//{
//	if (!SlateWindow.IsValid())
//	{
//		return;
//	}
//
//	if (WidgetRenderer == nullptr)
//		return;
//
//	const float DrawScale = 1.0f;
//
//	/*if (bDrawAtDesiredSize)
//	{
//		SlateWindow->SlatePrepass(DrawScale);
//
//		FVector2D DesiredSize = SlateWindow->GetDesiredSize();
//		DesiredSize.X = FMath::RoundToInt(DesiredSize.X);
//		DesiredSize.Y = FMath::RoundToInt(DesiredSize.Y);
//		CurrentDrawSize = DesiredSize.IntPoint();
//
//		WidgetRenderer->SetIsPrepassNeeded(false);
//	}
//	else
//	{
//		WidgetRenderer->SetIsPrepassNeeded(true);
//	}*/
//
//	WidgetRenderer->SetIsPrepassNeeded(true);
//
//	/*if (CurrentDrawSize != PreviousDrawSize)
//	{
//		UpdateBodySetup(true);
//		RecreatePhysicsState();
//	}*/
//
//	UpdateRenderTarget(QuadSize.IntPoint());
//
//
//	/*TSharedPtr<SWidget>					StereoSlateWidget;
//	StereoSlateWidget = Widget->TakeWidget();
//	RenderTarget = WidgetRenderer->DrawWidget(StereoSlateWidget.ToSharedRef(), QuadSize);
//	MarkTextureForUpdate();
//	if (RenderTarget != nullptr)
//	{
//		UTexture* pTexure = static_cast<UTexture*>(RenderTarget);
//		if (pTexure != nullptr)
//		{
//			SetTexture(pTexure);
//		}
//	}*/
//
//	// The render target could be null if the current draw size is zero
//	if (RenderTarget)
//	{
//		//bRedrawRequested = false;
//
//		WidgetRenderer->DrawWindow(
//			RenderTarget,
//			SlateWindow->GetHittestGrid(),
//			SlateWindow.ToSharedRef(),
//			DrawScale,
//			QuadSize.IntPoint(),
//			InDeltaTime);
//
//		//LastWidgetRenderTime = GetCurrentTime();
//	}
//}
//
//void UWRTestStereoLayerComponent::UpdateWidget()
//{
//	if (Widget == nullptr)
//		return;
//
//	TSharedPtr<SWidget> NewSlateWidget;
//	
//	NewSlateWidget = Widget->TakeWidget();
//	if (NewSlateWidget.IsValid() == false)
//		return;
//
//	bool bNeededNewWindow = false;
//	if (!SlateWindow.IsValid())
//	{
//		//UpdateMaterialInstance();	// ºÒÇÊ¿ä
//		
//		//SlateWindow = SNew(SVirtualWindow).Size(CurrentDrawSize);
//		//SlateWindow->SetIsFocusable(bWindowFocusable);
//		//SlateWindow->SetVisibility(ConvertWindowVisibilityToVisibility(WindowVisibility));
//		SlateWindow = SNew(SVirtualWindow).Size(QuadSize);
//		SlateWindow->SetIsFocusable(true);
//		SlateWindow->SetVisibility(EVisibility::SelfHitTestInvisible);
//		RegisterWindow();
//
//		bNeededNewWindow = true;
//	}
//
//	SlateWindow->Resize(QuadSize);
//
//	bool bWidgetChanged = false;
//	if (NewSlateWidget != CurrentSlateWidget || bNeededNewWindow)
//	{
//		CurrentSlateWidget = NewSlateWidget;
//		SlateWindow->SetContent(NewSlateWidget.ToSharedRef());
//		bWidgetChanged = true;
//	}
//
//	if (bNeededNewWindow || bWidgetChanged)
//	{
//		MarkRenderStateDirty();
//	}
//	/*else
//	{
//		UnregisterWindow();
//	}*/
//}
//
//void UWRTestStereoLayerComponent::UpdateRenderTarget(FIntPoint DesiredRenderTargetSize)
//{
//	bool bWidgetRenderStateDirty = false;
//	bool bClearColorChanged = false;
//
//	FLinearColor ActualBackgroundColor(FLinearColor::Transparent);
//	/*switch (BlendMode)
//	{
//	case EWidgetBlendMode::Opaque:
//		ActualBackgroundColor.A = 1.0f;
//		break;
//	case EWidgetBlendMode::Masked:
//		ActualBackgroundColor.A = 0.0f;
//		break;
//	}*/
//
//	if (DesiredRenderTargetSize.X != 0 && DesiredRenderTargetSize.Y != 0)
//	{
//		if (RenderTarget == nullptr)
//		{
//			RenderTarget = NewObject<UTextureRenderTarget2D>(this);
//			RenderTarget->ClearColor = ActualBackgroundColor;
//
//			bClearColorChanged = bWidgetRenderStateDirty = true;
//
//			RenderTarget->InitCustomFormat(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y, PF_B8G8R8A8, false);
//
//			/*if (MaterialInstance)
//			{
//				MaterialInstance->SetTextureParameterValue("SlateUI", RenderTarget);
//			}*/
//			SetTexture(RenderTarget);
//		}
//		else
//		{
//			// Update the format
//			if (RenderTarget->SizeX != DesiredRenderTargetSize.X || RenderTarget->SizeY != DesiredRenderTargetSize.Y)
//			{
//				RenderTarget->InitCustomFormat(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y, PF_B8G8R8A8, false);
//				bWidgetRenderStateDirty = true;
//			}
//
//			// Update the clear color
//			if (RenderTarget->ClearColor != ActualBackgroundColor)
//			{
//				RenderTarget->ClearColor = ActualBackgroundColor;
//				bClearColorChanged = bWidgetRenderStateDirty = true;
//			}
//
//			if (bWidgetRenderStateDirty)
//			{
//				RenderTarget->UpdateResourceImmediate();
//			}
//		}
//	}
//
//	if (RenderTarget)
//	{
//		// If the clear color of the render target changed, update the BackColor of the material to match
//		/*if (bClearColorChanged && MaterialInstance)
//		{
//			MaterialInstance->SetVectorParameterValue("BackColor", RenderTarget->ClearColor);
//		}*/
//
//		if (bWidgetRenderStateDirty)
//		{
//			MarkRenderStateDirty();
//		}
//	}
//}
//
//void UWRTestStereoLayerComponent::RegisterWindow()
//{
//	if (SlateWindow.IsValid())
//	{
//		if (Widget && !Widget->IsDesignTime())
//		{
//			if (UWorld* LocalWorld = GetWorld())
//			{
//				if (LocalWorld->IsGameWorld())
//				{
//					UGameInstance* GameInstance = LocalWorld->GetGameInstance();
//					check(GameInstance);
//
//					UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient();
//					if (GameViewportClient)
//					{
//						SlateWindow->AssignParentWidget(GameViewportClient->GetGameViewportWidget());
//					}
//				}
//			}
//		}
//	}
//}
//
//void UWRTestStereoLayerComponent::UnregisterWindow()
//{
//	if (SlateWindow.IsValid())
//	{
//		SlateWindow.Reset();
//	}
//}
