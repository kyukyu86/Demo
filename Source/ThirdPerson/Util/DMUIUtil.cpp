// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIUtil.h"
#include "Engine/Texture2D.h"
#include "../GameInstance/DMGameInstance.h"
#include <GameFramework/PlayerController.h>
#include <Blueprint/WidgetLayoutLibrary.h>

DMUIUtil::DMUIUtil()
{
}

DMUIUtil::~DMUIUtil()
{
}

void DMUIUtil::UEDebug(FString IN InLog)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, InLog);
}

UObject* DMUIUtil::LoadImageObject(FString InImageFullPath)
{
	if (InImageFullPath.IsEmpty() == true)
		return nullptr;

	FStringAssetReference AssetReference(InImageFullPath);
	UObject* ImageObject = AssetReference.TryLoad();
	if (ImageObject == nullptr)
	{
//		UE_LOG(IMError, Error, TEXT("Load Failed at path = %s"), *TextureFullPath);
	}
	return ImageObject;
}

bool DMUIUtil::SetResourceObject(UImage* const IN InImage, const FString& IN InTextureFileFullPath, const bool IN bInMatchSize /*= false*/)
{
	if (InImage == nullptr)
		return false;

	UObject* Texture = DMUIUtil::LoadImageObject(InTextureFileFullPath);
	return DMUIUtil::SetResourceObject(InImage, Texture, bInMatchSize);
}

bool DMUIUtil::SetResourceObject(UImage* const IN InImage, UObject* const IN InResourceObject, const bool IN bInMatchSize /*= false*/)
{
	if (InImage == nullptr)
		return false;

	if (InResourceObject == nullptr)
		return false;

	InImage->Brush.SetResourceObject(InResourceObject);

	if (bInMatchSize)
	{
		UTexture2D* CastedTexture = Cast<UTexture2D>(InResourceObject);
		if (CastedTexture != nullptr)
		{
			FVector2D vSize(CastedTexture->GetSizeX(), CastedTexture->GetSizeY());
			InImage->SetBrushSize(vSize);
		}
	}
	return true;
}

UMaterialInstanceDynamic* DMUIUtil::GetDynamicMaterial(UImage* const IN InImage)
{
	if (InImage == nullptr || InImage->IsValidLowLevel() == false)
	{
		ensureMsgf(false, TEXT("InImage is nullptr"));
		return nullptr;
	}
	UObject* Resource = InImage->Brush.GetResourceObject();
	if (Resource == nullptr || Resource->IsValidLowLevel() == false)
	{
		ensureMsgf(false, TEXT("Image's Brush ResourceObject is nullptr"));
		return nullptr;
	}

	// If we already have a dynamic material, return it.
	if (UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Resource))
	{
		return DynamicMaterial;
	}
	// If the resource has a material interface we'll just update the brush to have a dynamic material.
	else if (UMaterialInterface* Material = Cast<UMaterialInterface>(Resource))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
		InImage->Brush.SetResourceObject(DynamicMaterial);

		return DynamicMaterial;
	}

	ensureMsgf(false, TEXT("Failed to get 'Dynamic Mateiral Instance' of 'Image'"));

	return nullptr;
}

bool DMUIUtil::GetMouseScreenPosition(FVector2D& OUT OutMouseScreenPosition)
{
	APlayerController* PlayerController = UDMGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return false;

	FVector2D MousePosition;

	//====================================================================================
	// Get Mouse Position
	//====================================================================================

	// ¾ÈµÊ
// 	PlayerController->GetMousePosition(OutMouseScreenPosition.X, OutMouseScreenPosition.Y);
// 	GEngine->GameViewport->GetMousePosition(OutMouseScreenPosition);
// 	OutMouseScreenPosition = UWidgetLayoutLibrary::GetMousePositionOnPlatform();

	// µÊ
	UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PlayerController, OutMouseScreenPosition.X, OutMouseScreenPosition.Y);

	//====================================================================================
	// Get DPI Scale
	//====================================================================================

	// µÊ
// 	float fDPIScale = GEngine->GameViewport->GetDPIScale();
// 	fDPIScale = GEngine->GameViewport->GetDPIDerivedResolutionFraction();

	// ¾ÈµÊ
//  FVector2D viewportSize;
//  GEngine->GameViewport->GetViewportSize(viewportSize);
//  int32 X = FGenericPlatformMath::FloorToInt(viewportSize.X);
//  int32 Y = FGenericPlatformMath::FloorToInt(viewportSize.Y);
//	FVector2D DPIScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(X, Y));

	//====================================================================================
	// ÃÖÁ¾
	//====================================================================================

//	OutMouseScreenPosition *= fDPIScale;

	return true;
}

void DMUIUtil::GetMouseWorldTransform(FVector& OUT OutMouseWorldLocation, FVector& OUT OutMouseWorldDirection)
{
	APlayerController* PlayerController = UDMGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	// ¸ÂÀ½
	PlayerController->DeprojectMousePositionToWorld(OutMouseWorldLocation, OutMouseWorldDirection);

	// ¾È¸ÂÀ½
//	FVector2D MousePosition;
// 	UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PlayerController, MousePosition.X, MousePosition.Y);
//	UGameplayStatics::DeprojectScreenToWorld(PlayerController, MousePosition, OutMouseWorldLocation, OutMouseWorldDirection);

	// ¾È¸ÂÀ½
// 	ULocalPlayer* LocalPlayer = GEngine->GetLocalPlayerFromControllerId(GetWorld(), 0);
// 	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
// 		LocalPlayer->ViewportClient->Viewport,
// 		GetWorld()->Scene,
// 		LocalPlayer->ViewportClient->EngineShowFlags)
// 		.SetRealtimeUpdate(true));
// 	FVector OutViewLocation;
// 	FRotator OutViewRotation;
// 	FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, OutViewLocation, OutViewRotation, LocalPlayer->ViewportClient->Viewport);
// 	if (SceneView != NULL)
// 	{
// 		SceneView->DeprojectFVector2D(MousePosition, /*out*/ OutMouseWorldLocation, /*out*/ OutMouseWorldDirection);
// 	}
}
