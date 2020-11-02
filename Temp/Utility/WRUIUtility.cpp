// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIUtility.h"

#include "TextBlock.h"
#include "Widget.h"
#include "Image.h"
#include "SlateWrapperTypes.h"
#include "SlateColor.h"
#include "RetainerBox.h"
#include "RichTextBlock.h"
#include "UI/CustomWidget/WRTextBlock.h"

#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"

#include "Manager/WRPathManager.h"
#include "Manager/WRInputSystemManager.h"

#include "Actor/Character/WRCharacter.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "Enum/EWRCharacter.h"
#include "UI/Panel/WRUIPanel_ChatBubble.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRConsoleManager.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/Public/EngineUtils.h"




//====================================================================================
//	Resource
//====================================================================================

UObject* WRUIUtility::LoadImageObject(FString InImageFullPath)
{
	if (InImageFullPath.IsEmpty() == true)
		return nullptr;

// 	if (IMPathUtility::IsValidPackageName(TextureFullPath) == false)
// 		return nullptr;

	FStringAssetReference AssetReference(InImageFullPath);
	UObject* ImageObject = AssetReference.TryLoad();
	if (ImageObject == nullptr)
	{
//		UE_LOG(IMError, Error, TEXT("Load Failed at path = %s"), *TextureFullPath);
	}
	return ImageObject;
}

bool WRUIUtility::SetResourceObject(UImage* const IN InImage, const FString& IN InTextureFileFullPath, const bool IN bInMatchSize /*= false*/)
{
	if (InImage == nullptr)
		return false;

	UObject* Texture = WRUIUtility::LoadImageObject(InTextureFileFullPath);
	return WRUIUtility::SetResourceObject(InImage, Texture, bInMatchSize);
}

bool WRUIUtility::SetResourceObject(UImage* const IN InImage, UObject* const IN InResourceObject, const bool IN bInMatchSize /*= false*/)
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

//====================================================================================
//	Safe
//====================================================================================

bool WRUIUtility::SetTextSafe(UTextBlock* IN InTextBlock, const FString IN InText)
{
	if (InTextBlock != nullptr && InTextBlock->IsValidLowLevel())
	{
		InTextBlock->SetText(FText::FromString(InText));
		return true;
	}
	return false;
}

bool WRUIUtility::SetTextSafe(URichTextBlock* IN InTextBlock, const FString IN InText)
{
	if (InTextBlock != nullptr && InTextBlock->IsValidLowLevel())
	{
		InTextBlock->SetText(FText::FromString(InText));
		return true;
	}
	return false;
}

bool WRUIUtility::SetTextSafe(UWRTextBlock* IN InWRTextBlock, const FString& IN InText)
{
	if (InWRTextBlock != nullptr && InWRTextBlock->IsValidLowLevel())
	{
		InWRTextBlock->SetText(FText::FromString(InText));
		return true;
	}
	return false;
}

bool WRUIUtility::SetTextSafe(UWRTextBlock* IN InWRTextBlock, const FText& IN InText)
{
	if (InWRTextBlock != nullptr && InWRTextBlock->IsValidLowLevel())
	{
		InWRTextBlock->SetText(InText);
		return true;
	}
	return false;
}

bool WRUIUtility::SetTextSafe(UWRTextBlock* IN InWRTextBlock, const FName& IN InText)
{
	if (InWRTextBlock != nullptr && InWRTextBlock->IsValidLowLevel())
	{
		InWRTextBlock->SetText(InText);
		return true;
	}
	return false;
}

bool WRUIUtility::SetVisibilitySafe(UWidget* IN InWidget, const ESlateVisibility IN InVisibility)
{
	if (InWidget != nullptr && InWidget->IsValidLowLevel())
	{
		InWidget->SetVisibility(InVisibility);
		return true;
	}
	return false;
}

bool WRUIUtility::SetImageTintColorSafe(UImage* IN InImage, const FSlateColor IN InSlateColor)
{
	if (InImage != nullptr && InImage->IsValidLowLevel())
	{
		InImage->SetBrushTintColor(InSlateColor);
		return true;
	}
	return false;
}

bool WRUIUtility::SetTextColorSafe(UTextBlock* IN InTextBlock, const FLinearColor IN InLinearColor)
{
	if (InTextBlock != nullptr && InTextBlock->IsValidLowLevel())
	{
		InTextBlock->SetColorAndOpacity(InLinearColor);
		return true;
	}
	return false;
}

bool WRUIUtility::SetTextColorSafe(UWRTextBlock* IN InWRTextBlock, const struct FLinearColor IN InLinearColor)
{
	if (InWRTextBlock != nullptr && InWRTextBlock->IsValidLowLevel())
	{
		InWRTextBlock->SetColorAndOpacity(InLinearColor);
		return true;
	}
	return false;
}

UMaterialInstanceDynamic* WRUIUtility::GetDynamicMaterial(UImage* const IN InImage)
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

UMaterialInstanceDynamic* WRUIUtility::GetDynamicMaterial(URetainerBox* const IN InRetainerBox)
{
	if (InRetainerBox == nullptr || InRetainerBox->IsValidLowLevel() == false)
	{
		ensureMsgf(false, TEXT("InRetainerBox is nullptr"));
		return nullptr;
	}

	// If we already have a dynamic material, return it.
	if (UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(InRetainerBox->GetEffectMaterial()))
	{
		return DynamicMaterial;
	}
	// If the resource has a material interface we'll just update the brush to have a dynamic material.
	else if (UMaterialInterface* Material = Cast<UMaterialInterface>(InRetainerBox->GetEffectMaterial()))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
		InRetainerBox->SetEffectMaterial(DynamicMaterial);

		return DynamicMaterial;
	}

	ensureMsgf(false, TEXT("Failed to get 'Dynamic Mateiral Instance' of 'Image'"));

	return nullptr;
}

FText WRUIUtility::ConvertFNameToFText(const FName& IN InContent)
{
	return FText::FromName(InContent);
}

FString WRUIUtility::ConvertFNameToFString(const FName& IN InContent)
{
	return InContent.ToString();
}

FName WRUIUtility::ConvertWStringToFName(const std::wstring& IN InContent)
{
	FString HappyString(InContent.c_str());
	return FName(*HappyString);
}

FText WRUIUtility::ConvertWStringToFText(const std::wstring& IN InContent)
{
	FString HappyString(InContent.c_str());
	return FText::FromString(HappyString);
}

int32 WRUIUtility::ConvertFNameToInt(const FName& IN InNumContent)
{
	return FCString::Atoi(*(InNumContent.ToString()));
}

EWRInput WRUIUtility::ConvertFKeyToEWRInput(const FKey& IN InKey)
{
	const UInputSettings* InputSettings = GetDefault<UInputSettings>();
	FName ActionName;
	const TArray <FInputActionKeyMapping>& ActionMappingArray = InputSettings->GetActionMappings();
	for (FInputActionKeyMapping ActionMapping : ActionMappingArray)
	{
		if(ActionMapping.Key != InKey)
			continue;

		ActionName = ActionMapping.ActionName;
		break;
	}

	if(ActionName.IsNone())
		return EWRInput::None;

	return WRInputSystemManager::Get()->FindInput(ActionName);
}

FString WRUIUtility::ConvertNumberingToKUnit(const int32 IN InNumber)
{
	FString strConvert = FString::FormatAsNumber(InNumber);
	if (InNumber >= 1000)
	{
		strConvert = FString::Printf(TEXT("%sK"), *FString::FormatAsNumber(InNumber / 1000));
	}
	return strConvert;
}

void WRUIUtility::SetDOF(const float IN InValue)
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	{
		APostProcessVolume* PostProcessVolume = *Iter;
		if (PostProcessVolume == nullptr)
			continue;

		FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
		FPostProcessSettings* PostProcessSettings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);

		if (InValue != 0)
		{
			PostProcessSettings->bOverride_DepthOfFieldFocalDistance = true;
			PostProcessSettings->DepthOfFieldFocalDistance = InValue;
		}
		else
		{
			PostProcessSettings->bOverride_DepthOfFieldFocalDistance = false;
			PostProcessSettings->DepthOfFieldFocalDistance = InValue;
		}
	}
}

void WRUIUtility::SlatePrepass(UWidget* IN InWidget)
{
	if (InWidget == nullptr)
		return;

	InWidget->GetCachedWidget()->SlatePrepass();
}
